#ifndef AUDIOTEST_AUDIOENGINE_H
#define AUDIOTEST_AUDIOENGINE_H

#include <thread>
#include <stdint.h>
#include "audio_common.h"
#include "SineGenerator.h"
#include "sound_gen.h"
#include <vector>

#define BUFFER_SIZE_AUTOMATIC 0

class AudioEngine {

public:
    AudioEngine();

    ~AudioEngine();
    aaudio_data_callback_result_t dataCallback(AAudioStream *stream,
                                               void *audioData,
                                               int32_t numFrames);

    void errorCallback(AAudioStream *stream,
                       aaudio_result_t  __unused error);

    void setToneOn(int toneId, bool isToneOn);

    void setBufferSizeInBursts(int32_t numBursts);

    void resetTone(int toneId, double frequency, float amplitude);

    double getToneFrequency(int toneId);

private:

    int32_t playbackDeviceId_ = AAUDIO_UNSPECIFIED;
    int32_t sampleRate_;
    int16_t sampleChannels_;
    aaudio_format_t sampleFormat_;

    int32_t framesPerBurst_;
    int32_t bufSizeInFrames_;
    int32_t playStreamUnderrunCount_;
    int32_t bufferSizeSelection_ = BUFFER_SIZE_AUTOMATIC;

    AAudioStream *playStream_;

    Sound_Gen *sound_gen1;
    Sound_Gen *sound_gen2;

    bool isTone1On_ = false;
    bool isTone2On_ = false;

    double currentOutputLatencyMillis_ = 0;

private:

    std::mutex restartingLock_;

    void createPlaybackStream();
    void closeOutputStream();

    AAudioStreamBuilder* createStreamBuilder();
    void setupPlaybackStreamParameters(AAudioStreamBuilder *builder);
    void restartStream();

    void prepareSoundGenerators();

    aaudio_result_t calculateCurrentOutputLatencyMillis(AAudioStream *stream, double *latencyMillis);

};

#endif //AUDIOTEST_AUDIOENGINE_H
