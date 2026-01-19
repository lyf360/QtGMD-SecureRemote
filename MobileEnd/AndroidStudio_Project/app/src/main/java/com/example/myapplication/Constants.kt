package com.example.myapplication

/**
 * 全局常量配置 + 指令-设备映射
 */
object Constants {
    // MQTT服务器配置
    const val MQTT_SERVER = "tcp://fuji.lmq.cloudamqp.com:1883"
    const val MQTT_TOPIC_SUB = "/iot/smartdevice/doorlock/ack/json"
    const val MQTT_TOPIC_PUB = "/iot/smarthome/doorlock1"


    // 新增：MQTT认证信息（从Connection Details获取）
    const val MQTT_USERNAME = "xxxxxxxxxxxxxxx"
    const val MQTT_PASSWORD = "xxxxxxxxxxxxxxx"

    // 温湿度阈值宏定义（可根据需求修改）
    const val TEMPERATURE_THRESHOLD = 30.0 // 温度阈值：30℃
    const val HUMIDITY_THRESHOLD = 60.0    // 湿度阈值：60%


    // 开门指令JSON
    val OPEN_DOOR_JSON = """
        {
          "opration": "doorlock",
          "command": "open",
          "deviceType": "0x01",
          "deviceNumber": "0x00"
        }
    """.trimIndent()
    // 开门指令对应的设备标识（与应答包匹配）
    const val OPEN_DOOR_DEVICE_ADDR = "doorlock"
    const val OPEN_DOOR_DEVICE_NUM = 1 // 对应发送的"0x00"

    val CLOSE_DOOR_JSON = """
        {
          "opration": "doorlock",
          "command": "close",
          "deviceType": "0x01",
          "deviceNumber": "0x00"
        }
    """.trimIndent()
    // 开门指令对应的设备标识（与应答包匹配）
    const val CLOSE_DOOR_DEVICE_ADDR = "doorlock"
    const val CLOSE_DOOR_DEVICE_NUM = 1 // "deviceNum": "0x00",

    // 传感器查询指令JSON
    val QUERY_SENSOR_JSON = """
        {
          "opration": "sensor",
          "command": "query",
          "deviceType": "0x00",
          "deviceNumber": "0x00"
        }
    """.trimIndent()
    // 传感器指令对应的设备标识（与应答包匹配）
    const val SENSOR_DEVICE_ADDR = "sensor"
    const val SENSOR_DEVICE_NUM = 0 // 对应发送的"0x00"


    // 传感器查询指令JSON
    val QUERY_SENSOR_TH_JSON = """
        {
          "opration": "sensor",
          "command": "query",
          "deviceType": "0x01",
          "deviceNumber": "0x00"
        }
    """.trimIndent()
    // 传感器指令对应的设备标识（与应答包匹配）
    const val SENSOR_DEVICE_TH_ADDR = "sensor"
    const val SENSOR_DEVICE_TH_NUM = 1 // 对应发送的"0x00"
}