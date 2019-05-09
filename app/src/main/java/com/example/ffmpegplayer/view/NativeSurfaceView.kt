package com.example.ffmpegplayer.view

import android.content.Context
import android.util.AttributeSet
import android.view.Choreographer
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class NativeSurfaceView : SurfaceView, SurfaceHolder.Callback, Choreographer.FrameCallback {

    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr)

    var surfaceCallback: ISurfaceCallback? = null

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        if (holder != null) {
            nativeSurfaceCreated(holder.surface)
            surfaceCallback?.surfaceCreated(holder.surface)
            Choreographer.getInstance().postFrameCallback(this)
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        if (holder != null) {
            nativeSurfaceChanged(width, height)
            surfaceCallback?.surfaceChanged(width, height)
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        if (holder != null) {
            surfaceCallback?.surfaceDestroyed()
            nativeDestroyed()
            Choreographer.getInstance().removeFrameCallback(this)
        }
    }

    private var firstDoFrame = true
    private var startTime = 0L
    override fun doFrame(frameTimeNanos: Long) {
        if (firstDoFrame) {
            startTime = frameTimeNanos
            firstDoFrame = false
        }
        nativeDoFrame((frameTimeNanos - startTime) / 1000000)
        Choreographer.getInstance().postFrameCallback(this)
    }

    private external fun nativeSurfaceCreated(surface: Surface)
    private external fun nativeSurfaceChanged(width: Int, height: Int)
    private external fun nativeDestroyed()
    private external fun nativeDoFrame(frameTimeMillis: Long)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}