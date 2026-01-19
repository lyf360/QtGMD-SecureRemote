package com.example.myapplication

import android.util.Log
import com.google.gson.Gson
import org.eclipse.paho.client.mqttv3.*
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence

/**
 * MQTT通信管理类
 */
class MqttManager(
    private val onStateChanged: (String) -> Unit,
    private val onMessageReceived: (String, String?, String, Pair<Float?, Float?>?) -> Unit
) {
    private val TAG = "MqttManager"
    private lateinit var mqttClient: MqttClient
    private val gson = Gson()

    // 暂存发送指令时的设备标识 + 操作类型
    private data class SendDeviceInfo(
        val operationType: String, // openDoor/querySensor/querySensorTh
        val deviceAddr: String,    // 待校验的deviceAddr
        val deviceNum: Int         // 待校验的deviceNum
    )
    private var currentSendDeviceInfo: SendDeviceInfo? = null

    // 初始化MQTT连接
    fun initMqtt() {
        val clientId = "Android-Client-" + System.currentTimeMillis()
        val persistence = MemoryPersistence()

        try {
            mqttClient = MqttClient(Constants.MQTT_SERVER, clientId, persistence)
            val connectOptions = MqttConnectOptions().apply {
                isCleanSession = true
                connectionTimeout = 10
                keepAliveInterval = 20
                userName = Constants.MQTT_USERNAME
                password = Constants.MQTT_PASSWORD.toCharArray()
            }

            mqttClient.connect(connectOptions)
            Log.d(TAG, "MQTT连接成功")
            onStateChanged("连接成功")
            setMqttCallback()
            subscribeTopic()
        } catch (e: MqttException) {
            Log.e(TAG, "MQTT连接失败：${e.message}")
            onStateChanged("连接失败：${e.message}")
            onMessageReceived("MQTT未连接，无法发送指令", null, "", null)
        }
    }

    // 设置MQTT回调
    private fun setMqttCallback() {
        mqttClient.setCallback(object : MqttCallback {
            override fun connectionLost(cause: Throwable?) {
                Log.w(TAG, "MQTT连接断开：${cause?.message}")
                onStateChanged("连接断开")
                onMessageReceived("MQTT连接断开，无法接收数据", null, "", null)
            }

            override fun messageArrived(topic: String?, message: MqttMessage?) {
                val receiveJson = message?.payload?.toString(Charsets.UTF_8) ?: ""
                Log.d(TAG, "收到应答包：$receiveJson")
                parseReceivedJson(receiveJson)
            }

            override fun deliveryComplete(token: IMqttDeliveryToken?) {
                Log.d(TAG, "指令发送完成")
            }
        })
    }

    // 订阅主题
    private fun subscribeTopic() {
        if (::mqttClient.isInitialized && mqttClient.isConnected) {
            try {
                mqttClient.subscribe(Constants.MQTT_TOPIC_SUB, 0)
                Log.d(TAG, "订阅主题成功：${Constants.MQTT_TOPIC_SUB}")
            } catch (e: MqttException) {
                Log.e(TAG, "订阅失败：${e.message}")
                onMessageReceived("MQTT订阅失败，无法接收返回数据", null, "", null)
            }
        }
    }

    // 发布指令
    fun publishJson(
        jsonStr: String,
        operationName: String,
        operationType: String,
        deviceAddr: String,
        deviceNum: Int
    ) {
        currentSendDeviceInfo = SendDeviceInfo(operationType, deviceAddr, deviceNum)

        if (!::mqttClient.isInitialized || !mqttClient.isConnected) {
            onMessageReceived("❌ MQTT未连接，$operationName 失败", null, operationType, null)
            return
        }

        onMessageReceived("⏳ 正在发送$operationName 指令...", null, operationType, null)
        try {
            val mqttMsg = MqttMessage(jsonStr.toByteArray()).apply {
                qos = 0
                setRetained(false)
            }
            mqttClient.publish(Constants.MQTT_TOPIC_PUB, mqttMsg)
            Log.d(TAG, "$operationName 指令已发送：$jsonStr")
        } catch (e: MqttException) {
            Log.e(TAG, "$operationName 发送失败：${e.message}")
            onMessageReceived("❌ $operationName 失败：${e.message}", null, operationType, null)
        }
    }

    // 解析应答包
    private fun parseReceivedJson(json: String) {
        val sendInfo = currentSendDeviceInfo ?: run {
            Log.w(TAG, "无暂存的发送设备信息，跳过解析")
            return
        }

        try {
            // 解析应答包到实体类
            val response = gson.fromJson(json, DeviceResponse::class.java)
            val respDeviceAddr = response.deviceAddr
            val respDeviceNum = response.deviceNum
            val deviceData = response.devicedata
            val status = deviceData.status
            val errmsg = deviceData.errmsg
            val humidity = deviceData.humidity
            val temperature = deviceData.temperature

            // 校验设备标识
            if (respDeviceAddr != sendInfo.deviceAddr || respDeviceNum != sendInfo.deviceNum) {
                Log.w(TAG, "设备标识不匹配，跳过更新：" +
                        "发送[addr:${sendInfo.deviceAddr}, num:${sendInfo.deviceNum}] " +
                        "应答[addr:$respDeviceAddr, num:$respDeviceNum]")
                onMessageReceived("❌ 应答设备标识不匹配", null, sendInfo.operationType, null)
                return
            }

            // 错误处理
            if (errmsg.isNotEmpty()) {
                onMessageReceived("❌ 操作失败：$errmsg", null, sendInfo.operationType, null)
                Log.w(TAG, "设备$respDeviceAddr 应答失败：$errmsg")
                return
            }

            // 处理温湿度查询
            if (sendInfo.operationType == "querySensorTh") {
                // 优先用humidity/temperature字段
                if (humidity != null && temperature != null) {
                    onMessageReceived(
                        "✅ 温湿度查询成功",
                        status,
                        sendInfo.operationType,
                        Pair(humidity, temperature)
                    )
                    Log.d(TAG, "温湿度解析成功：湿度=$humidity%，温度=$temperature℃")
                } else {
                    // 兼容模式：从status字符串拆分温湿度
                    status?.let { statusStr ->
                        val parts = statusStr.split(",")
                        if (parts.size == 2) {
                            val humidityVal = parts[0].toFloatOrNull()
                            val temperatureVal = parts[1].toFloatOrNull()
                            if (humidityVal != null && temperatureVal != null) {
                                onMessageReceived(
                                    "✅ 温湿度查询成功",
                                    statusStr,
                                    sendInfo.operationType,
                                    Pair(humidityVal, temperatureVal)
                                )
                                Log.d(TAG, "兼容模式解析成功：湿度=$humidityVal%，温度=$temperatureVal℃")
                                return
                            }
                        }
                    }
                    onMessageReceived("❌ 温湿度数据缺失或格式错误", status, sendInfo.operationType, null)
                }
            } else {
                // 其他操作（开门/关门/普通传感器）
                onMessageReceived("✅ 设备$respDeviceAddr 操作成功", status, sendInfo.operationType, null)
            }

        } catch (e: Exception) {
            Log.e(TAG, "解析失败：${e.message}，原始JSON：$json")
            onMessageReceived("❌ JSON解析失败：${e.message}", null, sendInfo.operationType, null)
        } finally {
            currentSendDeviceInfo = null
        }
    }

    // 断开MQTT连接
    fun disconnect() {
        if (::mqttClient.isInitialized) {
            try {
                if (mqttClient.isConnected) {
                    mqttClient.disconnect()
                    Log.d(TAG, "MQTT已断开")
                }
            } catch (e: MqttException) {
                Log.e(TAG, "断开失败：${e.message}")
            }
        }
    }
}