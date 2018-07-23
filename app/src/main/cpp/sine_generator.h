//
// Created by riccardo on 21/07/18.
//

#ifndef AUDIOTEST_SINEGENERATOR_H
#define AUDIOTEST_SINEGENERATOR_H

#include <math.h>
#include <cstdint>

class SineGenerator
{
public:
    SineGenerator() {}
    ~SineGenerator() = default;

    void setup(double frequency, double frameRate) {
        mFrameRate = frameRate;
        mPhaseIncrement = frequency * M_PI * 2 / frameRate;
    }
    void setup(double frequency, double frameRate, float amplitude) {
        setup(frequency, frameRate);
        mAmplitude = amplitude;
    }

    void setSweep(double frequencyLow, double frequencyHigh, double seconds) {
        mPhaseIncrementLow = frequencyLow * M_PI * 2 / mFrameRate;
        mPhaseIncrementHigh = frequencyHigh * M_PI * 2 / mFrameRate;

        double numFrames = seconds * mFrameRate;
        mUpScaler = pow((frequencyHigh / frequencyLow), (1.0 / numFrames));
        mDownScaler = 1.0 / mUpScaler;
        mGoingUp = true;
        mSweeping = true;
    }

    void render(int16_t *buffer, int32_t channelStride, int32_t numFrames) {
        int sampleIndex = 0;
        for (int i = 0; i < numFrames; i++) {
            buffer[sampleIndex] = (int16_t) (32767 * sin(mPhase) * mAmplitude);
            sampleIndex += channelStride;
            advancePhase();
        }
    }
    void render(float *buffer, int32_t channelStride, int32_t numFrames) {
        int sampleIndex = 0;
        for (int i = 0; i < numFrames; i++) {
            buffer[sampleIndex] = (float) (sin(mPhase) * mAmplitude);
            sampleIndex += channelStride;
            advancePhase();
        }
    }

private:
    void advancePhase() {
        mPhase += mPhaseIncrement;
        if (mPhase > M_PI * 2) {
            mPhase -= M_PI * 2;
        }
        if (mSweeping) {
            if (mGoingUp) {
                mPhaseIncrement *= mUpScaler;
                if (mPhaseIncrement > mPhaseIncrementHigh) {
                    mGoingUp = false;
                }
            } else {
                mPhaseIncrement *= mDownScaler;
                if (mPhaseIncrement < mPhaseIncrementLow) {
                    mGoingUp = true;
                }
            }
        }
    }

    double mAmplitude = 0.01;
    double mPhase = 0.0;
    double mPhaseIncrement = 440 * M_PI * 2 / 48000;
    double mFrameRate = 48000;
    double mPhaseIncrementLow;
    double mPhaseIncrementHigh;
    double mUpScaler = 1.0;
    double mDownScaler = 1.0;
    bool   mGoingUp = false;
    bool   mSweeping = false;
};

#endif //AUDIOTEST_SINEGENERATOR_H
