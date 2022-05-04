//
// Created by liuxiaobing on 2021/1/15.
//

#include <jni.h>
#include "utils/LogUtils.h"

const char *CLASS_NAME = "come/live/player/MicroPlayer";
static JavaVM *javaVM = NULL;

void MicroPlayer_init(JNIEnv *env) {
    LOGD("mediaPlayer so 开始初始化");
    jclass clazz = env->FindClass(CLASS_NAME);
    if (clazz == NULL) {
        return;
    }
//    fields.context = env->GetFieldID(clazz, "mNativeContext", "J");
//    if (fields.context == NULL) {
//        return;
//    }
//
//    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
//                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
//    if (fields.post_event == NULL) {
//        return;
//    }

    env->DeleteLocalRef(clazz);
}

static const JNINativeMethod gMethods[] = {
        {"native_init", "()V", (void *)MicroPlayer_init}
};

/**
 * 注册方法
 * @param env
 * @return
 */
static int register_come_live_player_MircroPlayer(JNIEnv *env) {
    int numMethods = (sizeof(gMethods) / sizeof( (gMethods)[0]));
    jclass clazz = env->FindClass(CLASS_NAME);
    if (clazz == NULL) {
        LOGD("Native registration unable to find class '%s'", CLASS_NAME);
        return JNI_ERR;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGD("Native registration unable to find class '%s'", CLASS_NAME);
        return JNI_ERR;
    }
    env->DeleteLocalRef(clazz);

    return JNI_OK;
}


extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    //av_jni_set_java_vm(vm, NULL);
    LOGD("mediaPlayer so JNI_OnLoad");
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    if (register_come_live_player_MircroPlayer(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}