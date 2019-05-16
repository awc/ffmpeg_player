package com.example.ffmpegplayer.offScreen

class OffScreenUtil {

    fun startOffScreenTask(path: String) {
        nativeStartOffScreenTask(path)
    }

    private external fun nativeStartOffScreenTask(path: String)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}