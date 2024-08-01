package com.example.myapplication

import android.os.Trace

/**
 * bytedance
 * @author zenglw
 * @date   2024/8/1 18:36
 */
class TraceTag {
    companion object {
        @JvmStatic
        fun traceStart(name:String) {
            Trace.beginSection(name)
        }

        @JvmStatic
        fun traceEnd() {
            Trace.endSection()
        }
    }
}