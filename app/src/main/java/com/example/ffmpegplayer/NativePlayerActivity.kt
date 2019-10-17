package com.example.ffmpegplayer

import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.Surface
import androidx.appcompat.app.AppCompatActivity
import com.example.ffmpegplayer.server.PlayerCacheServer
import kotlinx.android.synthetic.main.activity_native_player.*
import java.io.File

class NativePlayerActivity : AppCompatActivity(), ISurfaceCallback, IVideoListener {

    private lateinit var nativePlayer: NativePlayer
    private lateinit var playerCacheServer: PlayerCacheServer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        supportActionBar?.hide()
        setContentView(R.layout.activity_native_player)
        surface_view.surfaceCallback = this
        playerCacheServer = PlayerCacheServer.getInstance(applicationContext)
    }

    override fun onVideoSizeChanged(width: Int, height: Int) {
        Log.d(TAG, "onVideoSizeChanged $width $height")
    }

    override fun surfaceCreated(surface: Surface) {
        nativePlayer = NativePlayer(applicationContext)
        nativePlayer.videoListener = this
        val path =
            "file://${File(Environment.getExternalStorageDirectory().absolutePath, "trailer.mp4")}"
//        val path =
//            playerCacheServer.getProxyUrl("https://oimryzjfe.qnssl.com/content/0fcbbe738abf1bf524dc2e7818200cc8.mp4")
        nativePlayer.setDataSource(path)
        nativePlayer.start()
    }

    override fun onRenderFirstFrame() {

    }

    override fun surfaceDestroyed() {
        nativePlayer.pause()
        nativePlayer.release()
    }

    companion object {
        private const val TAG = "NativePlayerActivity"
    }

}