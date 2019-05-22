package com.example.ffmpegplayer.offScreen

import android.view.Surface

class OffScreenUtil {

    fun startOffScreenTask(path: String, destPath: String) {
        nativeStartOffScreenTask(path, destPath)
    }

    fun startOnScreenTask(path: String, destPath: String, surface: Surface) {
        nativeStartOnScreenTask(path, destPath, surface)
    }

    private external fun nativeStartOffScreenTask(path: String, destPath: String)

    private external fun nativeStartOnScreenTask(path: String, destPath: String, surface: Surface)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}