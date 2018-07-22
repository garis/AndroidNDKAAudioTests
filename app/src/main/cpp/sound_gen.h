//
// Created by riccardo on 22/07/18.
//

#ifndef AUDIOTEST_SOUND_GEN_H
#define AUDIOTEST_SOUND_GEN_H

#include <math.h>
#include <cstdint>

class Sound_Gen {

    double mFrameRate;
    double mAmplitude;
    double mFrequency;
    double mPhaseIncrement;
    double mPhase;

public:
    Sound_Gen();
    ~Sound_Gen() = default;
    void setup(double frequency, double frameRate);
    void setup(double frequency, double frameRate, float amplitude);
    void render_square_wave(float *buffer, int32_t channelStride, int32_t numFrames);
    double getFrequency();

private:
    float render_sample(double mPhase);
    void advancePhase();
};


#endif //AUDIOTEST_SOUND_GEN_H
