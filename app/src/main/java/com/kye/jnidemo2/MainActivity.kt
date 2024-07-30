package com.kye.jnidemo2

import android.Manifest
import android.content.Intent
import android.content.res.Configuration
import android.net.http.SslError
import android.os.Bundle
import android.os.Trace
import android.util.Log
import android.view.View
import android.webkit.ConsoleMessage
import android.webkit.SslErrorHandler
import android.webkit.WebChromeClient
import android.webkit.WebResourceError
import android.webkit.WebResourceRequest
import android.webkit.WebResourceResponse
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.bytedance.shadowhook.ShadowHook
import com.bytedance.shadowhook.ShadowHook.ConfigBuilder
import com.kye.jnidemo2.MyAp.Companion
import com.kye.jnidemo2.databinding.ActivityMainBinding
import com.kye.mylibrary.TestC
import dalvik.system.DexClassLoader
import java.io.File


class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)


        binding = ActivityMainBinding.inflate(layoutInflater)
        intent.putExtra("nihao", "asd")
        setContentView(binding.root)
        binding.webview.apply {
            settings.apply {
                javaScriptEnabled = true
                allowFileAccess = true
                this.useWideViewPort = true
                this.javaScriptCanOpenWindowsAutomatically = true
            }
        }
        binding.webview.webViewClient = object: WebViewClient() {
            override fun shouldOverrideUrlLoading(view: WebView?, url: String?): Boolean {
//                view?.loadUrl("http://kuasheng-res-uat.kyslb.com/app-test/teamIcon/2024/07/09/82b0bad023ec06c627aa2fb73efccdd9_32144127_15811110752437904293.mp4")
//                view?.loadUrl(url ?: "")
//                return false
                return super.shouldOverrideUrlLoading(view, url)
            }

            override fun onReceivedError(
                view: WebView?,
                request: WebResourceRequest?,
                error: WebResourceError?
            ) {
                Log.e(TAG, "onReceivedError: ", )
                super.onReceivedError(view, request, error)
            }

            override fun onReceivedHttpError(
                view: WebView?,
                request: WebResourceRequest?,
                errorResponse: WebResourceResponse?
            ) {
                Log.e(TAG, "onReceivedHttpError: ", )
                super.onReceivedHttpError(view, request, errorResponse)
            }

            override fun onReceivedSslError(
                view: WebView?,
                handler: SslErrorHandler?,
                error: SslError?
            ) {
                Log.e(TAG, "onReceivedSslError: ", )
                super.onReceivedSslError(view, handler, error)
            }
        }
        
        binding.webview.webChromeClient = object: WebChromeClient() {
            override fun onConsoleMessage(consoleMessage: ConsoleMessage?): Boolean {
                Log.e(TAG, consoleMessage?.message() + " -- From line "
                        + consoleMessage?.lineNumber() + " of "
                        + consoleMessage?.sourceId());
                return super.onConsoleMessage(consoleMessage)
            }
        }

        binding.webview.loadUrl("http://kuasheng-res-uat.kyslb.com/app-test/teamIcon/2024/07/09/82b0bad023ec06c627aa2fb73efccdd9_32144127_15811110752437904293.mp4")
        binding.webview.visibility = View.GONE
        requestPermissions(
            arrayOf(
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE
            ), 100
        )
        val plugin1ApkFile = File(filesDir, "plugin1-debug.apk")
        if (!plugin1ApkFile.exists()) {
            val pluginStream = applicationContext.assets.open("plugin1-debug.apk")
            plugin1ApkFile.writeBytes(pluginStream.readBytes())
        }

        binding.loadPlugin.setOnClickListener {
            Trace.beginSection("disableVerify")
            disableVerify()
            Trace.endSection()
            val loader = DexClassLoader(
                File(filesDir, "plugin1-debug.apk").also {
                    it.setReadable(true, false)
                    it.setWritable(false)
                }.absolutePath,
                File(filesDir, "opt").also {
                    if (!it.exists()) {
                        it.mkdirs()
                    }
                }.absolutePath,
                null,
                MainActivity::class.java.classLoader
            )
            val clz = loader.loadClass("com.kye.plugin1.MainActivity")
            Log.e(TAG, "onCreate: class :$clz", )

            Toast.makeText(this@MainActivity, "load plugin", Toast.LENGTH_SHORT).show()
        }

        binding.enableOat.setOnClickListener {
//            enableOat()
            startActivity(Intent(this, MainActivity2::class.java))
//            binding.webview.evaluateJavascript("javascript:console.log('Hello from Android')", null);
        }
        binding.disableOat.setOnClickListener {
            val loader = DexClassLoader(
                File(filesDir, "plugin1-debug.apk").also {
                    it.setReadable(true, false)
                    it.setWritable(false)
                }.absolutePath,
                File(filesDir, "opt").also {
                    if (!it.exists()) {
                        it.mkdirs()
                    }
                }.absolutePath,
                null,
                MainActivity::class.java.classLoader
            )
            val clz = loader.loadClass("com.kye.plugin1.MainActivity")
            Log.e(TAG, "onCreate: verify_clz $clz", )
        }


        TestC().call()
        binding.sampleText.text = stringFromJNI()
    }

    external fun disableVerify()
    external fun enableOat()
    external fun disableOat()

    external fun stringFromJNI(): String

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        Log.e(TAG, "onConfigurationChanged: or :${newConfig.orientation}", )
    }
    override fun onDestroy() {
        super.onDestroy()
        Log.e(TAG, "onDestroy: ", )
    }

    companion object {
        init {
            System.loadLibrary("nativelib")
        }
        val TAG = MainActivity::class.java.simpleName
    }
}