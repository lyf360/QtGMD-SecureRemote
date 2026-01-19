package com.example.myapplication

import android.content.Context
import android.media.MediaPlayer
import android.util.Log

class AudioPlayer(private val context: Context) {
    private var mediaPlayer: MediaPlayer? = null
    private val TAG = "AudioPlayer"

    // 播放提示音（请将音频文件放在res/raw目录下，这里假设文件名为alert_sound.mp3）
    fun playAlertSound() {
        try {
            if (mediaPlayer == null) {
                mediaPlayer = MediaPlayer.create(context, R.raw.alert_sound)
                mediaPlayer?.isLooping = true // 循环播放
            }

            if (mediaPlayer?.isPlaying == false) {
                mediaPlayer?.start()
                Log.d(TAG, "开始播放提示音")
            }
        } catch (e: Exception) {
            Log.e(TAG, "播放提示音失败: ${e.message}")
            stopAndRelease() // 关键修正：将 release() 改为 stopAndRelease()
        }
    }

    // 停止播放并释放资源
    fun stopAndRelease() {
        try {
            if (mediaPlayer?.isPlaying == true) {
                mediaPlayer?.stop()
            }
            mediaPlayer?.release()
            mediaPlayer = null
            Log.d(TAG, "停止播放提示音")
        } catch (e: Exception) {
            Log.e(TAG, "停止播放失败: ${e.message}")
        }
    }
}