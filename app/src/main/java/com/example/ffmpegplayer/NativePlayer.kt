package com.example.ffmpegplayer

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.media.AudioManager
import android.os.Build


class NativePlayer(private val context: Context) {

    init {
        nativePlayerInit()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            val myAudioMgr = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
            val sampleRateStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)
            val defaultSampleRate = Integer.parseInt(sampleRateStr)
            val framesPerBurstStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)
            val defaultFramesPerBurst = Integer.parseInt(framesPerBurstStr)
            nativeAudioInit(defaultSampleRate, defaultFramesPerBurst)
        }
    }

    fun setDataSource(url: String) {
        nativePlayerSetDataSource(url, this)
    }

    fun setDataSource(url: String, bgUrl: String) {
        nativePlayerSetDataSources(url, bgUrl, this)
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
    private var mainHandler = Handler(Looper.getMainLooper())

    fun videoSizeChanged(width: Int, height: Int) {
        Log.d(TAG, "width=$width, height=$height")
        mainHandler.post {
            videoListener?.onVideoSizeChanged(width, height)
        }
    }

    private val startTime = System.currentTimeMillis()
    fun onRenderFirstFrame() {
        Log.d(TAG, "onRenderFirstFrame: ${System.currentTimeMillis() - startTime}")
        mainHandler.post {
            videoListener?.onRenderFirstFrame()
        }
    }

    companion object {
        const val TAG = "NativePlayer"
    }

    private external fun nativePlayerInit()
    private external fun nativeAudioInit(defaultSampleRate: Int, defaultFramesPerBurst: Int)
    private external fun nativePlayerSetDataSource(url: String, player: NativePlayer)
    private external fun nativePlayerStart()
    private external fun nativePlayerPause()
    private external fun nativePlayerRelease()
    private external fun nativePlayerSetDataSources(url: String, bgUrl: String, player: NativePlayer)
}