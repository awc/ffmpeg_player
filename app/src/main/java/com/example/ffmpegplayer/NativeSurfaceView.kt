package com.example.ffmpegplayer

import android.content.Context
import android.os.Environment
import android.util.AttributeSet
import android.view.Choreographer
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import java.io.File

class NativeSurfaceView : SurfaceView, SurfaceHolder.Callback, Choreographer.FrameCallback{

    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr)

    private lateinit var nativePlayer: NativePlayer

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        if (holder != null) {
            nativeSurfaceCreated(holder.surface)
            nativePlayer = NativePlayer()
            val path = "file://${File(Environment.getExternalStorageDirectory().absolutePath, "trailer.mp4")}"
            nativePlayer.setDataSource(path)
            nativePlayer.start()
            Choreographer.getInstance().postFrameCallback(this)
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        if (holder != null) {
            nativeSurfaceChanged(width, height)
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        if (holder != null) {
            nativePlayer.pause()
            nativePlayer.release()
            nativeDestroyed()
            Choreographer.getInstance().removeFrameCallback(this)
        }
    }

    var i = 0
    override fun doFrame(frameTimeNanos: Long) {
        if (i == 2) {
            nativeDoFrame(frameTimeNanos)
        }
        i++
        if (i== 3) {
            i = 0
        }
        Choreographer.getInstance().postFrameCallback(this)
    }

    private external fun nativeSurfaceCreated(surface: Surface)
    private external fun nativeSurfaceChanged(width: Int, height: Int)
    private external fun nativeDestroyed()
    private external fun nativeDoFrame(frameTimeNanos: Long)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}