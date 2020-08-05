package com.yang.nativegifplay

import android.graphics.Bitmap
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.widget.ImageView

/**
 *
 *
 * @author Yjz
 */
class GifPlay {

    private val TAG = "yang"//this.javaClass.simpleName
    private var nativeGif: Long? = null
    private var updateHandler: UpdateHandler? = null
    private var width: Int? = null
    private var height: Int? = null
    private var imgView: ImageView? = null
    private var currentBitmap: Bitmap? = null

    //fun with() {}//TODO 学习glide自动生命周期释放？？？
    fun load(gifFilePath: String) {
        nativeGif = loadGif(gifFilePath)
        if (nativeGif != null) {
            width = getWidth(nativeGif!!)
            height = getHeight(nativeGif!!)
            Log.d(TAG, "load success width = $width, height = $height")
        }
    }

    fun into(imageView: ImageView) {
        if (updateHandler == null) updateHandler = UpdateHandler()
        currentBitmap = Bitmap.createBitmap(width!!, height!!, Bitmap.Config.ARGB_8888)
        val delayMS = updateFrame(nativeGif!!, currentBitmap!!)
        //imageView.setImageBitmap(currentBitmap)
        Log.d(TAG, "into updateFrame currentBitmap = $currentBitmap")
        this.imgView = imageView
        this.imgView?.setImageBitmap(currentBitmap)
        updateHandler?.sendEmptyMessageDelayed(1, delayMS.toLong())
    }

    fun stop() {
        updateHandler?.removeCallbacksAndMessages(null)
        updateHandler = null
        currentBitmap?.recycle()
        currentBitmap = null
        nativeGif = null
        width = null
        height = null
        imgView = null
    }


    inner class UpdateHandler: Handler(Looper.getMainLooper()) {
        override fun handleMessage(msg: Message) {
            currentBitmap ?: return
            nativeGif ?: return
            val delayMS = updateFrame(nativeGif!!, currentBitmap!!)
            imgView?.setImageBitmap(currentBitmap) //TODO 不需要重复？？？？会导致崩溃？？？
            sendEmptyMessageDelayed(1, delayMS.toLong())
        }
    }


   external fun loadGif(filePath: String): Long
   external fun getWidth(ndkGif: Long): Int
   external fun getHeight(ndkGif: Long): Int
   external fun updateFrame(ndkGif: Long, bitmap: Bitmap): Int

    companion object {
        init {
            System.loadLibrary("gif_hanndle")
        }
    }
}