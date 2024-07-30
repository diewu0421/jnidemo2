
#include <android/log.h>
#include "include/jni_hook.h"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "custom_tag", __VA_ARGS__)

static int jni_entrance_index = -1;

static void **get_art_method(JNIEnv *env, jobject foo) {
    void **fooArtMethod;
    if (android_get_device_api_level() >= 30) {
        jclass Executable = (*env)->FindClass(env, "java/lang/reflect/Executable");
        jfieldID artMethodField = (*env)->GetFieldID(env, Executable, "artMethod", "J");
        fooArtMethod = (void **) (*env)->GetLongField(env, foo, artMethodField);
    } else {
        fooArtMethod = (void **) (*env)->FromReflectedMethod(env, foo);
    }
    return fooArtMethod;
}

static void init_jni_hook(JNIEnv *env, jobject foo, void *fooJNI) {
    void **fooArtMethod = get_art_method(env, foo);
    for (int i = 0; i < 50; ++i) {
        if (fooArtMethod[i] == fooJNI) {
            jni_entrance_index = i;
            break;
        }
    }
}

int hook_jni(JNIEnv *env, jobject method, void *new_entrance, void **origin_entrance) {
    if (jni_entrance_index == -1) {
        return -1;
    }
    void **target_art_method = get_art_method(env, method);
    if (target_art_method[jni_entrance_index] == new_entrance) {
        return 0;
    }
    *origin_entrance = target_art_method[jni_entrance_index];
    target_art_method[jni_entrance_index] = new_entrance;
    return 1;
}

void unhook_jni(JNIEnv *env, jobject method, void *origin_entrance) {
    void **target_art_method = get_art_method(env, method);
    if (target_art_method[jni_entrance_index] == origin_entrance) {
        return;
    }
    target_art_method[jni_entrance_index] = origin_entrance;
}


//JNIEXPORT void JNICALL
//Java_com_pika_jnihook_JniHook_jniPlaceHolder(JNIEnv *env, jclass clazz) {
//}
//
//JNIEXPORT void JNICALL
//Java_com_pika_jnihook_JniHook_jniHookInitByHolder(JNIEnv *env, jobject thiz,
//                                                  jobject native_place_holder) {
//    init_jni_hook(env, native_place_holder,
//                  (void *) Java_com_pika_jnihook_JniHook_jniPlaceHolder);
//}
//


void test_zlw(void* arg1, void** arg2) {
    LOGE("test_zlw");
}


JNIEXPORT void JNICALL
Java_com_kye_mylibrary_TestC_test(JNIEnv *env, jobject thiz) {
    LOGE("n1111111111ihao");
}

JNIEXPORT jlong JNICALL
Java_com_kye_mylibrary_TestC_holder1(JNIEnv *env, jobject thiz) {
    return (jlong)0;
}

JNIEXPORT jlong JNICALL
Java_com_kye_mylibrary_TestC_holder2(JNIEnv *env, jobject thiz) {
    return (jlong)0;
}

JNIEXPORT jlong JNICALL
Java_com_kye_mylibrary_TestC_getMethodArtMethod(JNIEnv *env, jobject thiz, jobject method) {
    void **artMethod = get_art_method(env, method);
//    ptrdiff_t diff =
    LOGE("artMethod pointer : %p", artMethod);
    LOGE("artMethod long:%lu", (jlong)artMethod);
    return (jlong) artMethod;
}

JNIEXPORT jlong JNICALL
Java_com_kye_mylibrary_TestC_holder3(JNIEnv *env, jobject thiz) {
    return (jlong)0;
}