package com.example.myapplication

data class DeviceResponse(
    val deviceAddr: String,       // 设备地址（doorlock/sensor）
    val deviceNum: Int,           // 设备编号
    val devicedata: DeviceData    // 设备数据体
)

/**
 * devicedata子结构：新增温湿度字段（明确可空）
 */
data class DeviceData(
    val status: String,              // 原有状态字段（保留兼容）
    val errmsg: String,           // 错误信息（空=无错）
    val humidity: Float? = null,  // 可空Float，默认null
    val temperature: Float? = null // 可空Float，默认null
)

// 温湿度实体类（非空，确保创建时已校验）
data class SensorThData(
    val humidity: Float,  // 非空Float
    val temperature: Float // 非空Float
)