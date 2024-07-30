#include <jni.h>
#include <string>
#include <jni_hook.h>
#include "shadowhook.h"
#include <android/log.h>
#include "thread_my.h"
#include <iostream>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "custom_tag", __VA_ARGS__)

void *orig = NULL;
void *stub = NULL;

bool (*Exec_origin) (std::vector<std::string> &arg_vector, std::string *error_msg);

bool Exec_proxy(std::vector<std::string> &arg_vector, std::string *error_msg) {
    LOGE("Exec_proxy args_length:%zu", arg_vector.size());

    for (const auto& arg : arg_vector) {
        LOGE("arg: %s", arg.c_str());
    }
    if (arg_vector[0].find("dex2oat")) {
        LOGE("dex2oat直接拦截返回false");
        return false;
    }
    bool result = Exec_origin(arg_vector, error_msg);
    if (error_msg) {
        LOGE("error_msg: %s", error_msg->c_str());
    }
    LOGE("Original function result: %d", result);
    return result;
}


void do_hook() {
    char* value = static_cast<char *>(malloc(sizeof(char) * 1));
    int security_patch = __system_property_get("ro.hmct.modem.version", value);
    free(value);
    LOGE("security_patch %d, %s", security_patch, value);
    if (android_get_device_api_level())

    stub = shadowhook_hook_sym_name(
            "libart.so",
            "_ZN3art4ExecERNSt3__16vectorINS0_12basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEENS5_IS7_EEEEPS7_",
            (void *) Exec_proxy,
            (void **) &Exec_origin
    );

    if (stub) {
        LOGE("hook success111");
    } else {
        int err_num = shadowhook_get_errno();

        const char *err_msg = shadowhook_to_errmsg(err_num);
        LOGE("hook error %d - %s", err_num, err_msg);
    }
}

void do_unhook() {
    shadowhook_unhook(stub);
    stub = nullptr;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_kye_jnidemo2_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());

}
extern "C"
JNIEXPORT void JNICALL
Java_com_kye_jnidemo2_MainActivity_enableOat(JNIEnv *env, jobject thiz) {
    do_unhook();
}
struct ThreadArgs1 {
    char name[50];
    int value;
    int test;
};

class Animal {

public:
    Animal(const std::string& name) :name(name) {
        LOGE("1111111 %s", name.c_str());
    }

protected:
    std::string name;
};


void* pthread_thread(void* args) {
    // 获取指向结构体的指针
    char* base_ptr = static_cast<char*>(args);

    // 计算value成员的偏移量
    int offset = offsetof(ThreadArgs1, value);
    LOGE("offfset = %d", offset);

    // 通过偏移量访问value成员
    int value = *reinterpret_cast<int*>(base_ptr + offset);
    LOGE("value = %d", value);
    return nullptr;
}

void test_pthread() {
    pthread_t pthread;
//    int* a = static_cast<int *>(malloc(sizeof(int))); // 动态分配内存
//    if (a == nullptr) {
//        perror("Failed to allocate memory");
//        return;
//    }
//    *a = 100;

    auto args = new ThreadArgs;
    args->value = 101;
    char name[50];
    sprintf(args->name, "zenglw_%s", "good");

    LOGE("args pointer:%p, int:%d", args, *((int*)args));

    if (pthread_create(&pthread, nullptr, pthread_thread, args) != 0) {
        perror("Failed to create thread");
//        free(a); // 确保在失败时释放内存
    delete args;
    } else {
        pthread_detach(pthread); // 避免资源泄露，分离线程
    }
}


struct JavaVmExt {
    void *function;
    void *runtime;

};

void (*origin_isverify)(void * self);

void isverify_proxy(void * self){
    origin_isverify(self);
    LOGE("isverify_proxy(void * self %p", self);
//    return ret;
}


void do_hook_verify() {
    if (android_get_device_api_level())

        stub = shadowhook_hook_sym_name(
                "libart.so",
                "_ZN3art7Runtime15DisableVerifierEv",
                (void *) isverify_proxy,
                (void **) &origin_isverify
        );
    LOGE("origin %p", origin_isverify);

    if (stub) {
        LOGE("hook success111");
    } else {
        int err_num = shadowhook_get_errno();

        const char *err_msg = shadowhook_to_errmsg(err_num);
        LOGE("hook error %d - %s", err_num, err_msg);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_kye_jnidemo2_MainActivity_disableOat(JNIEnv *env, jobject thiz) {
    JavaVM  *vm;
    env->GetJavaVM(&vm);
    do_hook_verify();

    if (vm) {

        JavaVmExt *javaVmExt = reinterpret_cast<JavaVmExt *>(vm);
        if (javaVmExt->runtime) {
            //  runtime
//            javaVmExt->runtime
            LOGE("runtime has %p", javaVmExt->runtime);
            void** ptr = static_cast<void **>(javaVmExt->runtime);
            if (origin_isverify) {
                origin_isverify(javaVmExt->runtime);
                LOGE("call disable");
            }
            for (int i =0 ;i<=100; i++) {
                if (ptr[i] == javaVmExt) {
                    LOGE("find1111111 %d %p, %p", i, ptr[i], javaVmExt);
                }
            }
            LOGE("over ");
        }
    }
//    do_hook();
//    test_pthread();
}


extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm , void* reversed) {

    LOGE("JNI_OnLoad");

    JNIEnv  *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_VERSION_1_6;
    }
    if (env->PushLocalFrame(10) == JNI_OK) {
        jstring str = env->NewStringUTF("nihao");
        LOGE("newStringUTF %s", env->GetStringUTFChars(str, nullptr));
        env->PopLocalFrame(nullptr);
    }

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGE("JNI_OnUnload, ");
}
#include <thread>
extern "C"
JNIEXPORT void JNICALL
Java_com_kye_jnidemo2_MyAp_disableOat(JNIEnv *env, jobject thiz) {
    do_hook();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kye_jnidemo2_MyAp_disableVerify(JNIEnv *env, jobject thiz) {
    do_hook_verify();
    JavaVM  *vm;
    env->GetJavaVM(&vm);
    if (vm) {

        JavaVmExt *javaVmExt = reinterpret_cast<JavaVmExt *>(vm);
        if (javaVmExt->runtime) {
            //  runtime
//            javaVmExt->runtime
            LOGE("runtime has %p", javaVmExt->runtime);
            void** ptr = static_cast<void **>(javaVmExt->runtime);
            if (origin_isverify) {
                origin_isverify(javaVmExt->runtime);
                LOGE("call disable");
            }
            for (int i =0 ;i<=100; i++) {
                if (ptr[i] == javaVmExt) {
                    LOGE("find1111111 %d %p, %p", i, ptr[i], javaVmExt);
                }
            }
            LOGE("over ");
        }
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_kye_jnidemo2_MainActivity_disableVerify(JNIEnv *env, jobject thiz) {
    do_hook_verify();
    JavaVM  *vm;
    env->GetJavaVM(&vm);
    if (vm) {

        JavaVmExt *javaVmExt = reinterpret_cast<JavaVmExt *>(vm);
        if (javaVmExt->runtime) {
            //  runtime
//            javaVmExt->runtime
            LOGE("runtime has %p", javaVmExt->runtime);
            void** ptr = static_cast<void **>(javaVmExt->runtime);
            if (origin_isverify) {
                origin_isverify(javaVmExt->runtime);
                LOGE("call disable");
            }
            for (int i =0 ;i<=100; i++) {
                if (ptr[i] == javaVmExt) {
                    LOGE("find1111111 %d %p, %p", i, ptr[i], javaVmExt);
                }
            }
            LOGE("over ");
        }
    }
}