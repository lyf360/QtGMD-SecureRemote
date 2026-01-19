package com.example.myapplication

import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

/**
 * 温湿度超阈值记录数据类
 */
data class SensorRecord(
    val timestamp: Long, // 记录时间戳
    val temperature: Float, // 当时温度
    val humidity: Float, // 当时湿度
    val isOverTemp: Boolean, // 是否超温
    val isOverHumidity: Boolean // 是否超湿
) {
    // 格式化时间显示（例如：2025-12-16 14:30:22）
    val formattedTime: String
        get() = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.CHINA)
            .format(Date(timestamp))

    // 记录描述
    val recordDesc: String
        get() = when {
            isOverTemp && isOverHumidity -> "温湿度均超标"
            isOverTemp -> "温度超标"
            isOverHumidity -> "湿度超标"
            else -> "正常记录"
        }
}