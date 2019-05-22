package com.example.ffmpegplayer.offScreen

import android.os.Bundle
import android.os.Environment
import android.view.Surface
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.ffmpegplayer.R
import kotlinx.android.synthetic.main.activity_offscreen.*
import java.io.File

class OffScreenActivity : AppCompatActivity(), SurfaceHolder.Callback {

    private var surface: Surface? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_offscreen)
        offScreenBtn.setOnClickListener {
            val offScreenUtil = OffScreenUtil()
            val path = "file://${File(Environment.getExternalStorageDirectory().absolutePath, "trailer.mp4")}"
            val destPath = "${File(Environment.getExternalStorageDirectory().absolutePath, "ffmpeg_test.h264")}"
            val file = File(destPath)
            if (file.exists()) {
                file.delete()
                file.createNewFile()
            }
            offScreenUtil.startOffScreenTask(path, destPath)
        }

        surface_view.holder.addCallback(this)

        onScreenBtn.setOnClickListener {
            val offScreenUtil = OffScreenUtil()
            val path = "file://${File(Environment.getExternalStorageDirectory().absolutePath, "trailer.mp4")}"
            val destPath = "${File(Environment.getExternalStorageDirectory().absolutePath, "ffmpeg_test.h264")}"
            val file = File(destPath)
            if (file.exists()) {
                file.delete()
                file.createNewFile()
            }
            if (surface != null) {
                offScreenUtil.startOnScreenTask(path, destPath, surface!!)
            }
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        surface = holder?.surface
    }
}