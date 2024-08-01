package com.kye.jnidemo2

import android.app.Application
import android.content.Context
import android.content.res.Configuration
import android.os.Trace
import android.util.Log
import com.bytedance.shadowhook.ShadowHook
import com.bytedance.shadowhook.ShadowHook.ConfigBuilder
import dalvik.system.DexFile


/**
 * bytedance
 * @author zenglw
 * @date   2024/7/8 16:18
 */
class MyAp: Application() {
    override fun attachBaseContext(base: Context?) {
        super.attachBaseContext(base)
        Trace.beginSection("attachBaseContext")
        ShadowHook.init(
            ConfigBuilder()
                .setMode(ShadowHook.Mode.UNIQUE)
                .build()
        )

    }
    override fun onCreate() {
        super.onCreate()
        val app = Class.forName("android.app.ActivityThread")
            .getMethod("currentApplication")
            .invoke(null) as Application

        Log.e(TAG, "attachBaseContext: this = ${hashCode()}, app = ${app.hashCode()}")
    }

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        Log.e(TAG, "onConfigurationChanged: or :${newConfig.orientation}")
    }
    external fun disableVerify()

    companion object {
        val TAG = MyAp::class.java.simpleName
        init {
            System.loadLibrary("nativelib")
        }
    }
}