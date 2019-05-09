package com.example.ffmpegplayer.view

import android.view.Surface

interface ISurfaceCallback {

    fun surfaceCreated(surface: Surface) {

    }

    fun surfaceChanged(width: Int, height: Int) {

    }

    fun surfaceDestroyed() {

    }

}