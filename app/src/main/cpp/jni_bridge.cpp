#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "audio_engine.h"
#include "logging_macros.h"

static AudioEngine *engine = nullptr;

extern "C"//without this you will get "java.lang.UnsatisfiedLinkError: No implementation found for..."
{
JNIEXPORT bool JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_create(JNIEnv *env,
                                                       jclass) {
    if (engine == nullptr) {
        engine = new AudioEngine();
    }

    return (engine != nullptr);
}

JNIEXPORT jstring JNICALL
Java_com_example_riccardo_audiotest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_setToneOn(JNIEnv *env,
                                                          jclass,jint toneId, jboolean isToneOn) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return;
    }

    engine->setToneOn(toneId, isToneOn);
}

JNIEXPORT void JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_resetTone(JNIEnv *env,
                                                          jclass,jint toneId, jdouble frequency,
                                                          jdouble amplitude) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return;
    }

    engine->resetTone(toneId, frequency, amplitude);
}

JNIEXPORT jdouble JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_getToneFrequency(JNIEnv *env,
                                                          jclass,jint toneId) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return 0;
    }
    return engine->getToneFrequency(toneId);
}

// create asset audio player
JNIEXPORT jboolean JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_createAssetAudioPlayer(JNIEnv* env, jclass clazz,
        jobject assetManager, jstring filename)
{
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return 0;
    }

    // convert Java string to UTF-8
    const char *utf8 = env->GetStringUTFChars(filename, JNI_FALSE);
    assert(NULL != utf8);

    // use asset manager to open asset by filename
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    assert(NULL != mgr);
    AAsset* asset = AAssetManager_open(mgr, utf8, AASSET_MODE_UNKNOWN);

    // release the Java string and UTF-8
    env->ReleaseStringUTFChars(filename, utf8);

    // the asset might not be found
    if (NULL == asset) {
        return JNI_FALSE;
    }

    const char *nativeString = env->GetStringUTFChars(filename, JNI_FALSE);
    return engine->createAssetAudioPlayer(asset, nativeString);
}

JNIEXPORT jboolean JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_delete(JNIEnv* env, jclass clazz)
{
    delete engine;
}

JNIEXPORT void JNICALL
Java_com_example_riccardo_audiotest_AudioEngine_setBufferSizeInBursts(
        JNIEnv *env, jclass, jint bufferSizeInBursts) {
    if (engine == nullptr) {
        LOGE("Engine is null, you must call createEngine before calling this method");
        return;
    }

    engine->setBufferSizeInBursts(bufferSizeInBursts);
}
}