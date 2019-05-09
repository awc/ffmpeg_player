package com.example.ffmpegplayer

class NativePlayer {

    init {
        nativePlayerInit()
    }

    fun setDataSource(url: String) {
        nativePlayerSetDataSource(url)
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

    fun videoSizeChanged(width: Int, height: Int) {
        videoListener?.onVideoSizeChanged(width, height)
    }

    private external fun nativePlayerInit()
    private external fun nativePlayerSetDataSource(url: String)
    private external fun nativePlayerStart()
    private external fun nativePlayerPause()
    private external fun nativePlayerRelease()
}