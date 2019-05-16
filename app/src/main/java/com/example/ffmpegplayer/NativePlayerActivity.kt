package com.example.ffmpegplayer

import android.os.Bundle
import android.os.Environment
import android.view.Surface
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_native_player.*
import java.io.File

class NativePlayerActivity : AppCompatActivity(), ISurfaceCallback, IVideoListener {

    private lateinit var nativePlayer: NativePlayer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_native_player)
        surface_view.surfaceCallback = this
    }

    override fun onVideoSizeChanged(width: Int, height: Int) {
        val layoutParams = surface_view.layoutParams
        if (width * 1f / height < surface_view.width * 1f / surface_view.height) {
            layoutParams.width = (width * 1f / height * surface_view.height).toInt()
        } else {
            layoutParams.height = (surface_view.width / (width * 1f / height)).toInt()
        }
        surface_view.layoutParams = layoutParams
    }

    override fun surfaceCreated(surface: Surface) {
        nativePlayer = NativePlayer(applicationContext)
        nativePlayer.videoListener = this
//        val path = "file://${File(Environment.getExternalStorageDirectory().absolutePath, "trailer.mp4")}"
        val path = "file://${File(Environment.getExternalStorageDirectory().absolutePath, "test.mp3")}"
        nativePlayer.setDataSource(path)
        nativePlayer.start()
    }

    override fun onRenderFirstFrame() {

    }

    override fun surfaceDestroyed() {
        nativePlayer.pause()
        nativePlayer.release()
    }

}