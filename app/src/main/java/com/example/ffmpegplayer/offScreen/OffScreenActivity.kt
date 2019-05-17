package com.example.ffmpegplayer.offScreen

import android.os.Bundle
import android.os.Environment
import androidx.appcompat.app.AppCompatActivity
import com.example.ffmpegplayer.R
import kotlinx.android.synthetic.main.activity_offscreen.*
import java.io.File

class OffScreenActivity : AppCompatActivity() {

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
    }

}