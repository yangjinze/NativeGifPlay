package com.yang.nativegifplay

import android.content.Context
import com.bumptech.glide.GlideBuilder
import com.bumptech.glide.module.AppGlideModule

/**
 *
 * @author Yjz
 */
class MyGlideModule: AppGlideModule() {
    override fun applyOptions(context: Context, builder: GlideBuilder) {
        super.applyOptions(context, builder)
        //builder.setDiskCache(ExternalDiskCacheFactory(context));

    }
}