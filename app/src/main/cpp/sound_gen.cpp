#include "sound_gen.h"

Sound_Gen::Sound_Gen(){
        mFrameRate = 48000;
        mAmplitude = 0.01;
        mFrequency = 0;
        mPhase = 0.0;
        mPhaseIncrement = 440 * M_PI * 2 / mFrameRate;
}

void Sound_Gen::setup(double frequency, double frameRate){
    mFrameRate = frameRate;
    mFrequency=frequency;
    mPhaseIncrement = mFrequency * M_PI * 2 / mFrameRate;
}

void Sound_Gen::setup(double frequency, double frameRate, float amplitude){
    setup(frequency, frameRate);
    mAmplitude = amplitude;
}


void Sound_Gen::render_square_wave(float *buffer, int32_t channelStride, int32_t numFrames) {
    int sampleIndex = 0;
    for (int i = 0; i < numFrames; i++) {
        float sample = (float) (render_sample(mPhase) * mAmplitude);
        buffer[sampleIndex+1] = sample;
        //same signal for all channels
        for (int j = 0; j < channelStride; j++)
            buffer[sampleIndex + j] = buffer[sampleIndex + j] + sample;

        sampleIndex += channelStride;
        advancePhase();
    }
}

float Sound_Gen::render_sample(double mPhase) {
    if (mPhase>=M_PI)
        return 1*mAmplitude;
    else
        return -1*mAmplitude;
}

void Sound_Gen::advancePhase() {
    mPhase += mPhaseIncrement;
    if (mPhase > M_PI * 2) {
        mPhase -= M_PI * 2;
    }
}

double Sound_Gen::getFrequency()
{
    return mFrequency;
}

