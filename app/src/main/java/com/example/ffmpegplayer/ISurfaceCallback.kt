package com.example.ffmpegplayer

import android.view.Surface

interface ISurfaceCallback {

    fun surfaceCreated(surface: Surface) {

    }

    fun surfaceChanged(width: Int, height: Int) {

    }

    fun surfaceDestroyed() {

    }

}