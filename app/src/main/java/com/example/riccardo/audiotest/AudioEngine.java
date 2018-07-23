package com.example.riccardo.audiotest;

import android.content.res.AssetManager;

public enum AudioEngine {

    INSTANCE;

    // Load native library
    static {
        System.loadLibrary("audio_engine-lib");
        System.loadLibrary("lame_mp3-lib");
    }

    /**
     * All the native methods that are implemented by the 'audioengine-lib' native library,
     * which is packaged with this application.
     */
    static native String stringFromJNI();
    static native boolean create();
    static native void delete();
    static native void setToneOn(int toneId, boolean isToneOn);
    static native void resetTone(int toneId, double frequency, double amplitude);
    static native double getToneFrequency(int toneId);
    static native void setAudioDeviceId(int deviceId);
    static native void setBufferSizeInBursts(int bufferSizeInBursts);
    static native double getCurrentOutputLatencyMillis();

    static native boolean createAssetAudioPlayer(AssetManager assetManager, String filename);
}
