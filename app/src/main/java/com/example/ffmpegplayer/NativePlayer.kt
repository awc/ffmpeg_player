package com.example.ffmpegplayer

import android.os.Handler
import android.os.Looper
import android.util.Log

class NativePlayer {

    init {
        nativePlayerInit()
    }

    fun setDataSource(url: String) {
        nativePlayerSetDataSource(url, this)
    }

    fun start() {
        nativePlayerStart()
    }

    fun pause() {
        nativePlayerPause()
    }

    fun release() {
        nativePlayerRelease()
    }

    var videoListener: IVideoListener? = null
    var mainHandler = Handler(Looper.getMainLooper())

    fun videoSizeChanged(width: Int, height: Int) {
        Log.d(TAG, "width=$width, height=$height")
        mainHandler.post {
            videoListener?.onVideoSizeChanged(width, height)
        }
    }

    companion object {
        const val TAG = "NativePlayer"
    }

    private external fun nativePlayerInit()
    private external fun nativePlayerSetDataSource(url: String, player: NativePlayer)
    private external fun nativePlayerStart()
    private external fun nativePlayerPause()
    private external fun nativePlayerRelease()
}