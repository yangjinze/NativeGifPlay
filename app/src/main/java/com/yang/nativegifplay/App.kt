package com.yang.nativegifplay

import android.app.Application

/**
 *
 * @author Yjz
 */
class App: Application() {
    override fun onLowMemory() {
        super.onLowMemory()
    }

    override fun onTrimMemory(level: Int) {
        super.onTrimMemory(level)
    }

}