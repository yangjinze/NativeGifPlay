package com.yang.nativegifplay

import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.bumptech.glide.Glide
import com.bumptech.glide.load.engine.DiskCacheStrategy
import java.io.File
import java.io.FileOutputStream
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    private val gifName = "demo.gif"
    private val gifFilePath = Environment.getExternalStorageDirectory().absolutePath + File.separator + gifName
    private val TAG = "MainActivity"
    private lateinit var imgGif: ImageView
    private val gifPlay: GifPlay = GifPlay()
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        imgGif = findViewById(R.id.img_gif)
        releaseGifFile()
    }

    fun native(view: View) {
        thread {
            gifPlay.load(gifFilePath)
            runOnUiThread {
                gifPlay.into(imgGif)
            }
        }
    }

    fun glide(view: View) {
        Glide.with(this).load(gifFilePath).into(imgGif)
    }

    private fun releaseGifFile() {
        val gifFile = File(gifFilePath)
        if (!gifFile.exists()) {
            Log.d(TAG, "try release gif")
            val inputStream = resources.assets.open(gifName)
            val outputStream = FileOutputStream(gifFilePath)
            val bytes = ByteArray(1024)
            while (inputStream.read(bytes) !== -1) {
                outputStream.write(bytes)
            }
            inputStream.close()
            outputStream.close()
            Log.d(TAG, "release gif success")
        }
    }
}
