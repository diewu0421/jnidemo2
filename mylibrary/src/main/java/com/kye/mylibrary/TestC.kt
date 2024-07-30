package com.kye.mylibrary

import android.util.Log
import java.lang.reflect.Method

/**
 * bytedance
 * @author zenglw
 * @date   2024/7/3 10:07
 */
class TestC {
    external fun test()

    fun call() {
        val holder1 = getMethodArtMethod(TestC::class.java.getDeclaredMethod("holder1"))
        val holder2 = getMethodArtMethod(TestC::class.java.getDeclaredMethod("holder2"))
        val holder3 = getMethodArtMethod(TestC::class.java.getDeclaredMethod("holder3"))

        Log.e(TAG, "call: diff :${holder2 - holder1}", )
        Log.e(TAG, "call: diff :${holder3 - holder1}", )
    }

    external fun getMethodArtMethod(method: Method): Long

    external fun holder1(): Long
    external fun holder2(): Long
    external fun holder3(): Long

    companion object {
        init {
            System.loadLibrary("mylibrary")
        }
        val TAG = TestC::class.java.simpleName
    }
}