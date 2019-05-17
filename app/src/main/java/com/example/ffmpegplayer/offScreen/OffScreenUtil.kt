package com.example.ffmpegplayer.offScreen

class OffScreenUtil {

    fun startOffScreenTask(path: String, destPath: String) {
        nativeStartOffScreenTask(path, destPath)
    }

    private external fun nativeStartOffScreenTask(path: String, destPath: String)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}