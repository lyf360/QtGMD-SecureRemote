package com.example.myapplication

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.defaultMinSize
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// 全局配置常量
private val BUTTON_ENABLED_BG_COLOR = Color(0xFFBBDEFB)
private val BUTTON_ENABLED_TEXT_COLOR = Color.White
private const val BUTTON_ALPHA = 0.4f
private val BUTTON_DISABLED_BG_COLOR = Color(0xFF616161)
private val BUTTON_DISABLED_TEXT_COLOR = Color(0xFFE0E0E0)
private val OVER_THRESHOLD_COLOR = Color.Red // 超阈值数字颜色（红色）
private val NORMAL_COLOR = Color.White // 正常数字颜色
private val RECORD_BUTTON_COLOR = Color(0xFF2196F3) // 历史记录按钮颜色
private val ORANGE_COLOR = Color(0xFFFFA500) // 记录对话框超温背景色
private val YELLOW_COLOR = Color(0xFFFFFF00) // 记录对话框超湿背景色

/**
 * 设备控制界面
 */
@Composable
fun DeviceControlUi(
    mqttState: String,
    openDoorFeedback: String,
    openDoorStatus: String,
    closeDoorFeedback: String,
    closeDoorStatus: String,
    sensorFeedback: String,
    sensorStatus: String,
    sensorThFeedback: String,
    sensorThData: SensorThData?,
    isTempOverThreshold: Boolean,
    isHumidityOverThreshold: Boolean,
    onSendOpenDoor: () -> Unit,
    onSendCloseDoor: () -> Unit,
    onQuerySensor: () -> Unit,
    onQuerySensorTh: () -> Unit,
    isContinuousQuerying: Boolean,
    onToggleContinuousQuery: () -> Unit,  // 新增这个回调
    isButtonEnabled: Boolean,
    showRecordDialog: Boolean,
    onShowRecordClick: () -> Unit,
    onDismissRecordDialog: () -> Unit,
    sensorRecords: List<SensorRecord>
) {
    MaterialTheme {
        Surface(modifier = Modifier.fillMaxSize()) {
            Box(modifier = Modifier.fillMaxSize()) {
                // 背景图
                Image(
                    painter = painterResource(id = R.drawable.bg_device_control),
                    contentDescription = "界面背景图",
                    modifier = Modifier.fillMaxSize(),
                    contentScale = ContentScale.Crop
                )

                // 半透明遮罩
                Box(
                    modifier = Modifier
                        .fillMaxSize()
                        .background(Color.Black.copy(alpha = 0.5f))
                )

                // 前景内容
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(20.dp),
                    verticalArrangement = androidx.compose.foundation.layout.Arrangement.Top
                ) {
                    // MQTT连接状态
                    Text(
                        text = "📡 MQTT状态：$mqttState",
                        modifier = Modifier.padding(bottom = 20.dp),
                        color = if (mqttState.contains("成功") || mqttState.contains("Connected")) Color.White else Color.Red,
                        style = MaterialTheme.typography.titleMedium
                    )

                    // 1. 开门功能行
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(bottom = 16.dp)
                            .background(
                                color = Color.White.copy(alpha = 0.15f),
                                shape = MaterialTheme.shapes.medium
                            )
                            .padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = androidx.compose.foundation.layout.Arrangement.SpaceBetween
                    ) {
                        Button(
                            onClick = onSendOpenDoor,
                            enabled = isButtonEnabled && !isContinuousQuerying,
                            modifier = Modifier
                                .weight(2f)
                                .defaultMinSize(minHeight = 60.dp),
                            colors = androidx.compose.material3.ButtonDefaults.buttonColors(
                                containerColor = BUTTON_ENABLED_BG_COLOR.copy(alpha = BUTTON_ALPHA),
                                contentColor = BUTTON_ENABLED_TEXT_COLOR,
                                disabledContainerColor = BUTTON_DISABLED_BG_COLOR,
                                disabledContentColor = BUTTON_DISABLED_TEXT_COLOR
                            )
                        ) {
                            Text(text = "open D", fontSize = 18.sp, fontWeight = FontWeight.Medium)
                        }
                        Text(
                            text = "${openDoorFeedback} status：$openDoorStatus",
                            color = when {
                                openDoorFeedback.contains("失败") -> Color.Red
                                openDoorFeedback.contains("正在发送") -> Color.LightGray
                                else -> Color.White
                            },
                            modifier = Modifier.weight(3f).padding(start = 12.dp),
                            style = MaterialTheme.typography.bodyLarge,
                            fontSize = 16.sp
                        )
                    }

                    // 2. 关门功能行
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(bottom = 16.dp)
                            .background(
                                color = Color.White.copy(alpha = 0.15f),
                                shape = MaterialTheme.shapes.medium
                            )
                            .padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = androidx.compose.foundation.layout.Arrangement.SpaceBetween
                    ) {
                        Button(
                            onClick = onSendCloseDoor,
                            enabled = isButtonEnabled && !isContinuousQuerying,
                            modifier = Modifier
                                .weight(2f)
                                .defaultMinSize(minHeight = 60.dp),
                            colors = androidx.compose.material3.ButtonDefaults.buttonColors(
                                containerColor = BUTTON_ENABLED_BG_COLOR.copy(alpha = BUTTON_ALPHA),
                                contentColor = BUTTON_ENABLED_TEXT_COLOR,
                                disabledContainerColor = BUTTON_DISABLED_BG_COLOR,
                                disabledContentColor = BUTTON_DISABLED_TEXT_COLOR
                            )
                        ) {
                            Text(text = "close D", fontSize = 18.sp, fontWeight = FontWeight.Medium)
                        }
                        Text(
                            text = "${closeDoorFeedback} status：$closeDoorStatus",
                            color = when {
                                closeDoorFeedback.contains("失败") -> Color.Red
                                closeDoorFeedback.contains("正在发送") -> Color.LightGray
                                else -> Color.White
                            },
                            modifier = Modifier.weight(3f).padding(start = 12.dp),
                            style = MaterialTheme.typography.bodyLarge,
                            fontSize = 16.sp
                        )
                    }

                    // 3. 温湿度传感器（TH）功能行
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(bottom = 16.dp)
                            .background(
                                color = Color.White.copy(alpha = 0.15f),
                                shape = MaterialTheme.shapes.medium
                            )
                            .padding(horizontal = 12.dp, vertical = 16.dp),
                        verticalAlignment = Alignment.Top,
                        horizontalArrangement = androidx.compose.foundation.layout.Arrangement.SpaceBetween
                    ) {
                        Button(
                            onClick = onQuerySensorTh,
                            enabled = isButtonEnabled && !isContinuousQuerying,
                            modifier = Modifier
                                .weight(2f)
                                .defaultMinSize(minHeight = 90.dp),
                            colors = androidx.compose.material3.ButtonDefaults.buttonColors(
                                containerColor = BUTTON_ENABLED_BG_COLOR.copy(alpha = BUTTON_ALPHA),
                                contentColor = BUTTON_ENABLED_TEXT_COLOR,
                                disabledContainerColor = BUTTON_DISABLED_BG_COLOR,
                                disabledContentColor = BUTTON_DISABLED_TEXT_COLOR
                            )
                        ) {
                            Text(text = "query TH", fontSize = 18.sp, fontWeight = FontWeight.Medium)
                        }

                        Column(
                            modifier = Modifier
                                .weight(3f)
                                .padding(start = 12.dp),
                            verticalArrangement = androidx.compose.foundation.layout.Arrangement.Top
                        ) {
                            Text(
                                text = sensorThFeedback,
                                color = when {
                                    sensorThFeedback.contains("失败") -> Color.Red
                                    sensorThFeedback.contains("正在发送") -> Color.LightGray
                                    else -> Color.White
                                },
                                style = MaterialTheme.typography.bodyLarge,
                                fontSize = 16.sp,
                                modifier = Modifier.padding(bottom = 4.dp)
                            )

                            // 温湿度显示（超阈值变红）
                            sensorThData?.let {
                                Text(
                                    text = "温度 : ${it.temperature}℃",
                                    color = if (isTempOverThreshold) OVER_THRESHOLD_COLOR else NORMAL_COLOR,
                                    style = MaterialTheme.typography.bodyLarge,
                                    fontSize = 16.sp,
                                    fontWeight = if (isTempOverThreshold) FontWeight.Bold else FontWeight.Normal,
                                    modifier = Modifier.padding(bottom = 2.dp)
                                )

                                Text(
                                    text = "湿度 : ${it.humidity}%",
                                    color = if (isHumidityOverThreshold) OVER_THRESHOLD_COLOR else NORMAL_COLOR,
                                    style = MaterialTheme.typography.bodyLarge,
                                    fontSize = 16.sp,
                                    fontWeight = if (isHumidityOverThreshold) FontWeight.Bold else FontWeight.Normal
                                )
                            } ?: run {
                                // 无数据时的默认显示
                                Text(
                                    text = "温度 : —",
                                    color = Color.LightGray,
                                    style = MaterialTheme.typography.bodyLarge,
                                    fontSize = 16.sp,
                                    modifier = Modifier.padding(bottom = 2.dp)
                                )
                                Text(
                                    text = "湿度 : —",
                                    color = Color.LightGray,
                                    style = MaterialTheme.typography.bodyLarge,
                                    fontSize = 16.sp
                                )
                            }
                        }
                    }




                    // 4. 传感器（G）功能行
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(bottom = 24.dp)
                            .background(
                                color = Color.White.copy(alpha = 0.15f),
                                shape = MaterialTheme.shapes.medium
                            )
                            .padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = androidx.compose.foundation.layout.Arrangement.SpaceBetween
                    ) {
                        Button(
                            onClick = onQuerySensor,
                            enabled = isButtonEnabled && !isContinuousQuerying,
                            modifier = Modifier
                                .weight(2f)
                                .defaultMinSize(minHeight = 60.dp),
                            colors = androidx.compose.material3.ButtonDefaults.buttonColors(
                                containerColor = BUTTON_ENABLED_BG_COLOR.copy(alpha = BUTTON_ALPHA),
                                contentColor = BUTTON_ENABLED_TEXT_COLOR,
                                disabledContainerColor = BUTTON_DISABLED_BG_COLOR,
                                disabledContentColor = BUTTON_DISABLED_TEXT_COLOR
                            )
                        ) {
                            Text(text = "query G", fontSize = 18.sp, fontWeight = FontWeight.Medium)
                        }
                        Text(
                            text = "${sensorFeedback} status：$sensorStatus",
                            color = when {
                                sensorFeedback.contains("失败") -> Color.Red
                                sensorFeedback.contains("正在发送") -> Color.LightGray
                                else -> Color.White
                            },
                            modifier = Modifier.weight(3f).padding(start = 12.dp),
                            style = MaterialTheme.typography.bodyLarge,
                            fontSize = 16.sp
                        )
                    }








                    // 5. 连续查询控制行
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(bottom = 16.dp)
                            .background(
                                color = Color.White.copy(alpha = 0.15f),
                                shape = MaterialTheme.shapes.medium
                            )
                            .padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = androidx.compose.foundation.layout.Arrangement.SpaceBetween
                    ) {
                        Button(
                            onClick = onToggleContinuousQuery,  // 使用新的切换回调
                            enabled = isButtonEnabled,  // 只要连接成功就可用
                            modifier = Modifier
                                .weight(2f)  // 占2份宽度
                                .defaultMinSize(minHeight = 60.dp),
                            colors = androidx.compose.material3.ButtonDefaults.buttonColors(
                                // 根据状态切换颜色
                                containerColor = if (isContinuousQuerying)
                                    Color(0xFFF44336).copy(alpha = BUTTON_ALPHA)  // 停止状态-红色
                                else
                                    Color(0xFF4CAF50).copy(alpha = BUTTON_ALPHA),  // 开始状态-绿色
                                contentColor = BUTTON_ENABLED_TEXT_COLOR,
                                disabledContainerColor = BUTTON_DISABLED_BG_COLOR,
                                disabledContentColor = BUTTON_DISABLED_TEXT_COLOR
                            )
                        ) {
                            // 根据状态切换显示文本
                            Text(
                                text = if (isContinuousQuerying) "停止连续查询" else "开启连续查询",
                                fontSize = 16.sp,
                                fontWeight = FontWeight.Medium
                            )
                        }

                        Text(
                            text = if (isContinuousQuerying) "🔄 正在连续查询" else "⏹️ 已停止",
                            color = if (isContinuousQuerying) Color.Green else Color.White,
                            modifier = Modifier.weight(1f).padding(start = 12.dp),
                            style = MaterialTheme.typography.bodyLarge,
                            fontSize = 16.sp
                        )
                    }


                     // // //        // --------------------
                }

                // 右上角：历史记录文字按钮
                HistoryRecordButton(
                    modifier = Modifier
                        .align(Alignment.TopEnd)
                        .padding(20.dp),
                    onClick = onShowRecordClick
                )

                // 超阈值记录对话框
                if (showRecordDialog) {
                    SensorRecordDialog(
                        records = sensorRecords,
                        onDismiss = onDismissRecordDialog
                    )
                }
            }
        }
    }
}

/**
 * 右上角历史记录文字按钮
 */
@Composable
private fun HistoryRecordButton(
    modifier: Modifier = Modifier,
    onClick: () -> Unit
) {
    Box(
        modifier = modifier
            .height(40.dp)
            .background(
                color = BUTTON_ENABLED_BG_COLOR.copy(alpha = BUTTON_ALPHA),
                shape = RoundedCornerShape(8.dp)
            )
            .border(2.dp, Color.White, RoundedCornerShape(8.dp))
            .clickable(onClick = onClick)
            .padding(horizontal = 12.dp),
        contentAlignment = Alignment.Center
    ) {
        Text(
            text = "Record",
            color = BUTTON_ENABLED_TEXT_COLOR, // 使用与其他按钮相同的文字颜色
            fontSize = 14.sp,
            fontWeight = FontWeight.Medium
        )
    }
}

/**
 * 超阈值记录对话框（支持滑动查看）
 */
@Composable
private fun SensorRecordDialog(
    records: List<SensorRecord>,
    onDismiss: () -> Unit
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = {
            Text(
                text = "温湿度超阈值记录",
                style = MaterialTheme.typography.titleLarge,
                color = Color.Black
            )
        },
        text = {
            if (records.isEmpty()) {
                Text(
                    text = "暂无超阈值记录",
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.Gray
                )
            } else {
                // 关键修复：使用maxHeight并配合fillMaxWidth，确保内容可滚动
                LazyColumn(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(300.dp) // 改为maxHeight允许内容在超出时滚动
                        .padding(vertical = 8.dp)
                ) {
                    items(records) { record ->
                        Box(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(vertical = 8.dp)
                                .background(
                                    color = when {
                                        record.isOverTemp && record.isOverHumidity -> Color.Red.copy(alpha = 0.1f)
                                        record.isOverTemp -> ORANGE_COLOR.copy(alpha = 0.1f)
                                        record.isOverHumidity -> YELLOW_COLOR.copy(alpha = 0.1f)
                                        else -> Color.Transparent
                                    },
                                    shape = MaterialTheme.shapes.small
                                )
                                .padding(12.dp)
                        ) {
                            Column {
                                Text(
                                    text = "${record.formattedTime} | ${record.recordDesc}",
                                    style = MaterialTheme.typography.bodyMedium,
                                    color = Color.Black,
                                    fontWeight = FontWeight.Medium
                                )
                                Text(
                                    text = "温度：${record.temperature}℃ | 湿度：${record.humidity}%",
                                    style = MaterialTheme.typography.bodySmall,
                                    color = Color.Gray
                                )
                            }
                        }
                    }
                }
            }
        },
        confirmButton = {
            Button(onClick = onDismiss) {
                Text(text = "关闭")
            }
        },
        containerColor = Color.White,
        shape = MaterialTheme.shapes.medium,
        modifier = Modifier.padding(20.dp)
    )
}