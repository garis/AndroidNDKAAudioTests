#include <jni.h>
#include <string>
#include "audioengine.h"
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