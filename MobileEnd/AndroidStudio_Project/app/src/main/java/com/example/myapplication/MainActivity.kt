package com.example.myapplication

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.runtime.remember
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.mutableStateListOf
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import com.example.myapplication.Constants.TEMPERATURE_THRESHOLD
import com.example.myapplication.Constants.HUMIDITY_THRESHOLD

class MainActivity : ComponentActivity() {
    private lateinit var mqttManager: MqttManager

    private lateinit var audioPlayer: AudioPlayer // 音频播放器
    // 状态变量初始化
    private val mqttConnectState = mutableStateOf("MQTT 服务器 连接中...")
    private val openDoorFeedback = mutableStateOf("")
    private val openDoorStatus = mutableStateOf("—")
    private val closeDoorFeedback = mutableStateOf("")
    private val closeDoorStatus = mutableStateOf("—")
    private val sensorFeedback = mutableStateOf("")
    private val sensorStatus = mutableStateOf("—")
    private val sensorThFeedback = mutableStateOf("")
    private val sensorThData = mutableStateOf<SensorThData?>(null)

    // 连续查询相关状态
    private val isContinuousQuerying = mutableStateOf(false)

    // 温湿度监控相关状态
    private val isTempOverThreshold = mutableStateOf(false)
    private val isHumidityOverThreshold = mutableStateOf(false)
    private val sensorRecords = mutableStateListOf<SensorRecord>()
    // 移除1分钟间隔限制，删除lastRecordTime变量

    // 自定义查询项数据类
    private data class QueryItem(
        val jsonStr: String,
        val operationName: String,
        val operationType: String,
        val deviceAddr: String,
        val deviceNum: Int
    )

    // 连续查询列表
    private val continuousQueries = listOf(
//        QueryItem(
//            Constants.QUERY_SENSOR_JSON,
//            "传感器查询",
//            "querySensor",
//            Constants.SENSOR_DEVICE_ADDR,
//            Constants.SENSOR_DEVICE_NUM
//        ),
        QueryItem(
            Constants.QUERY_SENSOR_TH_JSON,
            "温湿度查询",
            "querySensorTh",
            Constants.SENSOR_DEVICE_TH_ADDR,
            Constants.SENSOR_DEVICE_TH_NUM
        )
    )

    private var currentQueryIndex = 0
    private var isWaitingForResponse = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        mqttManager = MqttManager(
            onStateChanged = { state ->
                mqttConnectState.value = state
            },
            onMessageReceived = { feedback, status, operationType, thData ->
                when (operationType) {
                    "openDoor" -> {
                        openDoorFeedback.value = feedback
                        status?.let { openDoorStatus.value = it }
                    }
                    "closeDoor" -> {
                        closeDoorFeedback.value = feedback
                        status?.let { closeDoorStatus.value = it }
                    }
                    "querySensor" -> {
                        sensorFeedback.value = feedback
                        status?.let { sensorStatus.value = it }
                    }
                    "querySensorTh" -> {
                        sensorThFeedback.value = feedback
                        // 温湿度数据处理
                        thData?.let { (humidity, temperature) ->
                            if (humidity != null && temperature != null) {
                                sensorThData.value = SensorThData(humidity, temperature)

                                // 判断是否超阈值
                                val overTemp = temperature > TEMPERATURE_THRESHOLD
                                val overHumidity = humidity > HUMIDITY_THRESHOLD
                                isTempOverThreshold.value = overTemp
                                isHumidityOverThreshold.value = overHumidity

                                // 控制音频播放
                                controlAlertSound(overTemp || overHumidity)

                                // 超阈值记录（只要超标就记录，移除1分钟间隔限制）
                                val currentTime = System.currentTimeMillis()
                                if (overTemp || overHumidity) {
                                    val record = SensorRecord(
                                        timestamp = currentTime,
                                        temperature = temperature,
                                        humidity = humidity,
                                        isOverTemp = overTemp,
                                        isOverHumidity = overHumidity
                                    )
                                    sensorRecords.add(0, record)
                                }
                            }
                        }
                    }
                }

                // 连续查询逻辑
                if (isContinuousQuerying.value && !isWaitingForResponse) {
                    CoroutineScope(Dispatchers.Main).launch {
                        delay(1000)
                        sendNextQuery()
                    }
                } else {
                    isWaitingForResponse = false
                }
            }
        )
        mqttManager.initMqtt()

        audioPlayer = AudioPlayer(this)  // 初始化音频播放器（关键修复）

        setContent {
            // 所有状态变量通过 remember 包装，确保Compose能感知变化
            val connectState = remember { mqttConnectState }
            val odFeedback = remember { openDoorFeedback }
            val odStatus = remember { openDoorStatus }
            val cdFeedback = remember { closeDoorFeedback }
            val cdStatus = remember { closeDoorStatus }
            val sFeedback = remember { sensorFeedback }
            val sStatus = remember { sensorStatus }
            val sThFeedback = remember { sensorThFeedback }
            val sThData = remember { sensorThData }
            val continuousState = remember { isContinuousQuerying }
            val isTempOver = remember { isTempOverThreshold }
            val isHumidityOver = remember { isHumidityOverThreshold }
            val records = remember { sensorRecords }
            val showRecordDialog = remember { mutableStateOf(false) }


            // 关键修复：严格按照DeviceControlUi的参数列表传递，确保名称和类型匹配
            DeviceControlUi(
                mqttState = connectState.value,
                openDoorFeedback = odFeedback.value,
                openDoorStatus = odStatus.value,
                closeDoorFeedback = cdFeedback.value,
                closeDoorStatus = cdStatus.value,
                sensorFeedback = sFeedback.value,
                sensorStatus = sStatus.value,
                sensorThFeedback = sThFeedback.value,
                sensorThData = sThData.value,
                isTempOverThreshold = isTempOver.value,
                isHumidityOverThreshold = isHumidityOver.value,
                onSendOpenDoor = { sendOpenDoor() },
                onSendCloseDoor = { sendCloseDoor() },
                onQuerySensor = { sendQuerySensor() },
                onQuerySensorTh = { sendQuerySensorTh() },
                isContinuousQuerying = continuousState.value,
                onToggleContinuousQuery = {  // 新增这个回调实现
                    if (continuousState.value) {
                        stopContinuousQuery()
                    } else {
                        startContinuousQuery()
                    }
                },
                isButtonEnabled = connectState.value.contains("成功"),
                showRecordDialog = showRecordDialog.value,
                onShowRecordClick = { showRecordDialog.value = true },
                onDismissRecordDialog = { showRecordDialog.value = false },
                sensorRecords = records
            )
        }
    }

    // 控制提示音播放
    private fun controlAlertSound(isOverThreshold: Boolean) {
        if (isContinuousQuerying.value && isOverThreshold) {
            // 连续查询状态且超阈值，播放提示音
            audioPlayer.playAlertSound()
        } else {
            // 不满足条件，停止播放
            audioPlayer.stopAndRelease()
        }
    }

    // 连续查询控制
    private fun startContinuousQuery() {
        if (!isContinuousQuerying.value && mqttConnectState.value.contains("成功")) {
            isContinuousQuerying.value = true
            currentQueryIndex = 0
            sendNextQuery()
        }
    }

    private fun stopContinuousQuery() {
        isContinuousQuerying.value = false
        currentQueryIndex = 0
        isWaitingForResponse = false

        // 停止查询时同时停止音频
        audioPlayer.stopAndRelease()
    }

    private fun sendNextQuery() {
        if (!isContinuousQuerying.value) return

        if (currentQueryIndex >= continuousQueries.size) {
            currentQueryIndex = 0
        }

        val (jsonStr, operationName, operationType, deviceAddr, deviceNum) = continuousQueries[currentQueryIndex]
        isWaitingForResponse = true
        mqttManager.publishJson(
            jsonStr = jsonStr,
            operationName = operationName,
            operationType = operationType,
            deviceAddr = deviceAddr,
            deviceNum = deviceNum
        )
        currentQueryIndex++
    }

    // 指令发送函数
    private fun sendOpenDoor() {
        mqttManager.publishJson(
            jsonStr = Constants.OPEN_DOOR_JSON,
            operationName = "开门",
            operationType = "openDoor",
            deviceAddr = Constants.OPEN_DOOR_DEVICE_ADDR,
            deviceNum = Constants.OPEN_DOOR_DEVICE_NUM
        )
    }

    private fun sendCloseDoor() {
        mqttManager.publishJson(
            jsonStr = Constants.CLOSE_DOOR_JSON,
            operationName = "关门",
            operationType = "closeDoor",
            deviceAddr = Constants.CLOSE_DOOR_DEVICE_ADDR,
            deviceNum = Constants.CLOSE_DOOR_DEVICE_NUM
        )
    }

    private fun sendQuerySensor() {
        mqttManager.publishJson(
            jsonStr = Constants.QUERY_SENSOR_JSON,
            operationName = "传感器查询",
            operationType = "querySensor",
            deviceAddr = Constants.SENSOR_DEVICE_ADDR,
            deviceNum = Constants.SENSOR_DEVICE_NUM
        )
    }

    private fun sendQuerySensorTh() {
        mqttManager.publishJson(
            jsonStr = Constants.QUERY_SENSOR_TH_JSON,
            operationName = "温湿度查询",
            operationType = "querySensorTh",
            deviceAddr = Constants.SENSOR_DEVICE_TH_ADDR,
            deviceNum = Constants.SENSOR_DEVICE_TH_NUM
        )
    }

    override fun onDestroy() {
        super.onDestroy()
        mqttManager.disconnect()

        // 释放音频资源
        audioPlayer.stopAndRelease()
    }
}