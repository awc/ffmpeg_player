package com.example.ffmpegplayer

interface IVideoListener {

    fun onVideoSizeChanged(width: Int, height: Int)

    fun onRenderFirstFrame()
}