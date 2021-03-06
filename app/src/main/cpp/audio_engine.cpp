#include <assert.h>
#include <cstring>
#include <jni.h>
#include "audio_engine.h"
#include "trace.h"
#include "audio_common.h"
#include "logging_macros.h"
#include "sine_generator.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <android/trace.h>

//costruttore
AudioEngine::AudioEngine() {

    // Initialize the trace functions, this enables you to output trace statements without
    // blocking. See https://developer.android.com/studio/profile/systrace-commandline.html
    Trace::initialize();

    mySampleRate_=44100;

    sampleChannels_ = kStereoChannelCount;
    sampleFormat_ = AAUDIO_FORMAT_PCM_FLOAT;

    // Create the output stream. By not specifying an audio device id we are telling AAudio that
    // we want the stream to be created using the default playback audio device.
    createPlaybackStream();

    mp3_file = nullptr;
}

AudioEngine::~AudioEngine(){//It's the destructor, it destroys the instance, frees up memory, etc. etc.

    closeOutputStream();
    //delete sineOsc1_;
    //delete sineOsc2_;
    delete sound_gen1;
    delete sound_gen2;
    delete mp3_file;
}

/**
 * Every time the playback stream requires data this method will be called.
 *
 * @param stream the audio stream which is requesting data, this is the playStream_ object
 * @param userData the context in which the function is being called, in this case it will be the
 * PlayAudioEngine instance
 * @param audioData an empty buffer into which we can write our audio data
 * @param numFrames the number of audio frames which are required
 * @return Either AAUDIO_CALLBACK_RESULT_CONTINUE if the stream should continue requesting data
 * or AAUDIO_CALLBACK_RESULT_STOP if the stream should stop.
 *
 * @see PlayAudioEngine#dataCallback
 */
aaudio_data_callback_result_t dataCallback(AAudioStream *stream, void *userData,
                                           void *audioData, int32_t numFrames) {
    assert(userData && audioData);//if both zero a message is written to the standard error device and abort is called, terminating the program execution
    AudioEngine *audioEngine = reinterpret_cast<AudioEngine *>(userData);//when you call reinterpret_cast the CPU does not invoke any calculations. It just treats a set of bits in the memory like if it had another type
    return audioEngine->dataCallback(stream, audioData, numFrames);
}

/**
 * If there is an error with a stream this function will be called. A common example of an error
 * is when an audio device (such as headphones) is disconnected. In this case you should not
 * restart the stream within the callback, instead use a separate thread to perform the stream
 * recreation and restart.
 *
 * @param stream the stream with the error
 * @param userData the context in which the function is being called, in this case it will be the
 * PlayAudioEngine instance
 * @param error the error which occured, a human readable string can be obtained using
 * AAudio_convertResultToText(error);
 *
 * @see PlayAudioEngine#errorCallback
 */
void errorCallback(AAudioStream *stream,
                   void *userData,
                   aaudio_result_t error) {
    assert(userData);
    AudioEngine *audioEngine = reinterpret_cast<AudioEngine *>(userData);
    audioEngine->errorCallback(stream, error);
}

/**
 * @see errorCallback function at top of this file
 */
void AudioEngine::errorCallback(AAudioStream *stream,
                                    aaudio_result_t error){

    assert(stream == playStream_);
    LOGD("errorCallback result: %s", AAudio_convertResultToText(error));

    aaudio_stream_state_t streamState = AAudioStream_getState(playStream_);
    if (streamState == AAUDIO_STREAM_STATE_DISCONNECTED){

        // Handle stream restart on a separate thread
        std::function<void(void)> restartStream = std::bind(&AudioEngine::restartStream, this);
        std::thread streamRestartThread(restartStream);
        streamRestartThread.detach();
    }
}

void AudioEngine::restartStream(){

    LOGI("Restarting stream");

    if (restartingLock_.try_lock()){
        closeOutputStream();
        createPlaybackStream();
        restartingLock_.unlock();
    } else {
        LOGW("Restart stream operation already in progress - ignoring this request");
        // We were unable to obtain the restarting lock which means the restart operation is currently
        // active. This is probably because we received successive "stream disconnected" events.
        // Internal issue b/63087953
    }
}

void AudioEngine::closeOutputStream(){

    if (playStream_ != nullptr){
        aaudio_result_t result = AAudioStream_requestStop(playStream_);
        if (result != AAUDIO_OK){
            LOGE("Error stopping output stream. %s", AAudio_convertResultToText(result));
        }

        result = AAudioStream_close(playStream_);
        if (result != AAUDIO_OK){
            LOGE("Error closing output stream. %s", AAudio_convertResultToText(result));
        }
    }
}

/**
 * Creates a stream builder which can be used to construct streams
 * @return a new stream builder object
 */
AAudioStreamBuilder* AudioEngine::createStreamBuilder() {

    AAudioStreamBuilder *builder = nullptr;//serves as a universal null pointer literal, replacing the buggy and weakly-typed literal 0 and the infamous NULL macro
    aaudio_result_t result = AAudio_createStreamBuilder(&builder);
    if (result != AAUDIO_OK) {
        LOGE("Error creating stream builder: %s", AAudio_convertResultToText(result));
    }
    return builder;
}

/**
 * Creates an audio stream for playback. The audio device used will depend on playbackDeviceId_.
 */
void AudioEngine::createPlaybackStream(){

    AAudioStreamBuilder* builder = createStreamBuilder();

    if (builder != nullptr){

        setupPlaybackStreamParameters(builder);

        aaudio_result_t result = AAudioStreamBuilder_openStream(builder, &playStream_);

        if (result == AAUDIO_OK && playStream_ != nullptr){

            // check that we got PCM_FLOAT format
            if (sampleFormat_ != AAudioStream_getFormat(playStream_)) {
                LOGW("Sample format is not PCM_FLOAT");
            }

            sampleRate_ = AAudioStream_getSampleRate(playStream_);
            framesPerBurst_ = AAudioStream_getFramesPerBurst(playStream_);

            // Set the buffer size to the burst size - this will give us the minimum possible latency
            AAudioStream_setBufferSizeInFrames(playStream_, framesPerBurst_);
            bufSizeInFrames_ = framesPerBurst_;

            PrintAudioStreamInfo(playStream_);
            prepareSoundGenerators();

            // Start the stream - the dataCallback function will start being called
            result = AAudioStream_requestStart(playStream_);
            if (result != AAUDIO_OK) {
                LOGE("Error starting stream. %s", AAudio_convertResultToText(result));
            }

            // Store the underrun count so we can tune the latency in the dataCallback
            playStreamUnderrunCount_ = AAudioStream_getXRunCount(playStream_);

        } else {
            LOGE("Failed to create stream. Error: %s", AAudio_convertResultToText(result));
        }

        AAudioStreamBuilder_delete(builder);

    } else {
        LOGE("Unable to obtain an AAudioStreamBuilder object");
    }
}

/**
 * Sets the stream parameters which are specific to playback, including device id and the
 * dataCallback function, which must be set for low latency playback.
 * @param builder The playback stream builder
 */
void AudioEngine::setupPlaybackStreamParameters(AAudioStreamBuilder *builder) {
    AAudioStreamBuilder_setDeviceId(builder, playbackDeviceId_);
    AAudioStreamBuilder_setFormat(builder, sampleFormat_);
    AAudioStreamBuilder_setChannelCount(builder, sampleChannels_);

    AAudioStreamBuilder_setSampleRate(builder, mySampleRate_);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_EXCLUSIVE);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setDataCallback(builder, ::dataCallback, this);
    AAudioStreamBuilder_setErrorCallback(builder, ::errorCallback, this);
}

void AudioEngine::prepareSoundGenerators() {
    sound_gen1 = new Sound_Gen();
    sound_gen1 -> setup(3000.0,sampleRate_,0.25);
    sound_gen2 = new Sound_Gen();
    sound_gen2 -> setup(2000.0,sampleRate_,0.25);
}

aaudio_result_t
AudioEngine::calculateCurrentOutputLatencyMillis(AAudioStream *stream, double *latencyMillis) {

    // Get the time that a known audio frame was presented for playing
    int64_t existingFrameIndex;
    int64_t existingFramePresentationTime;
    aaudio_result_t result = AAudioStream_getTimestamp(stream,
                                                       CLOCK_MONOTONIC,
                                                       &existingFrameIndex,
                                                       &existingFramePresentationTime);

    if (result == AAUDIO_OK){

        // Get the write index for the next audio frame
        int64_t writeIndex = AAudioStream_getFramesWritten(stream);

        // Calculate the number of frames between our known frame and the write index
        int64_t frameIndexDelta = writeIndex - existingFrameIndex;

        // Calculate the time which the next frame will be presented
        int64_t frameTimeDelta = (frameIndexDelta * NANOS_PER_SECOND) / sampleRate_;
        int64_t nextFramePresentationTime = existingFramePresentationTime + frameTimeDelta;

        // Assume that the next frame will be written at the current time
        int64_t nextFrameWriteTime = get_time_nanoseconds(CLOCK_MONOTONIC);

        // Calculate the latency
        *latencyMillis = (double) (nextFramePresentationTime - nextFrameWriteTime)
                         / NANOS_PER_MILLISECOND;
    } else {
        LOGE("Error calculating latency: %s", AAudio_convertResultToText(result));
    }

    return result;
}

/**
 * @see dataCallback function at top of this file
 */
aaudio_data_callback_result_t AudioEngine::dataCallback(AAudioStream *stream,
                                                            void *audioData,
                                                            int32_t numFrames) {
    assert(stream == playStream_);

    ATrace_beginSection("myExpensiveFunction");

    //controlla che il buffer non sia mai stato vuoto
    int32_t underrunCount = AAudioStream_getXRunCount(playStream_);
    aaudio_result_t bufferSize = AAudioStream_getBufferSizeInFrames(playStream_);
    bool hasUnderrunCountIncreased = false;
    bool shouldChangeBufferSize = false;

    if (underrunCount > playStreamUnderrunCount_){
        playStreamUnderrunCount_ = underrunCount;
        hasUnderrunCountIncreased = true;
    }

    if (hasUnderrunCountIncreased && bufferSizeSelection_ == BUFFER_SIZE_AUTOMATIC){

        /**
         * This is a buffer size tuning algorithm. If the number of underruns (i.e. instances where
         * we were unable to supply sufficient data to the stream) has increased since the last callback
         * we will try to increase the buffer size by the burst size, which will give us more protection
         * against underruns in future, at the cost of additional latency.
         */
        bufferSize += framesPerBurst_; // Increase buffer size by one burst
        shouldChangeBufferSize = true;
    } else if (bufferSizeSelection_ > 0 && (bufferSizeSelection_ * framesPerBurst_) != bufferSize){

        // If the buffer size selection has changed then update it here
        bufferSize = bufferSizeSelection_ * framesPerBurst_;
        shouldChangeBufferSize = true;
    }

    if (shouldChangeBufferSize){
        LOGD("Setting buffer size to %d", bufferSize);
        bufferSize = AAudioStream_setBufferSizeInFrames(stream, bufferSize);
        if (bufferSize > 0) {
            bufSizeInFrames_ = bufferSize;
        } else {
            LOGE("Error setting buffer size: %s", AAudio_convertResultToText(bufferSize));
        }
    }

    /**
     * The following output can be seen by running a systrace. Tracing is preferable to logging
     * inside the callback since tracing does not block.
     *
     * See https://developer.android.com/studio/profile/systrace-commandline.html
     */
    Trace::beginSection("numFrames %d, Underruns %d, buffer size %d",
                        numFrames, underrunCount, bufferSize);

    int32_t samplesPerFrame = sampleChannels_;

    //zeroing the audio buffer before filling it mixing the sounds
    memset(static_cast<uint8_t *>(audioData), 0,
           sizeof(float) * samplesPerFrame * numFrames);

    if((mp3_file!= NULL) && (mp3_file->isPlaying()))
    {
        int i = mp3_file->read(static_cast<float *>(audioData), (int) numFrames*2);
        //LOGD("%d\tVS %d\tmp3_file->read", i, numFrames);

    }
    //mixing the sounds
    if (isTone1On_) {
        sound_gen1->render_square_wave(static_cast<float *>(audioData), samplesPerFrame, numFrames);
    }
    if (isTone2On_) {
        sound_gen2->render_square_wave(static_cast<float *>(audioData), samplesPerFrame, numFrames);
    }

    calculateCurrentOutputLatencyMillis(stream, &currentOutputLatencyMillis_);

    Trace::endSection();
    ATrace_endSection();
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void AudioEngine::setToneOn(int toneId, bool isToneOn){
    if(toneId==1)
        isTone1On_ = isToneOn;
    if(toneId==2)
        isTone2On_ = isToneOn;
}
void AudioEngine::setBufferSizeInBursts(int32_t numBursts){
    AudioEngine::bufferSizeSelection_ = numBursts;
}

void AudioEngine::resetTone(int toneId, double frequency, float amplitude){
    if(toneId==1)
    {
        delete sound_gen1;
        sound_gen1 = new Sound_Gen();
        sound_gen1 -> setup(frequency,sampleRate_,amplitude);
    }
    if(toneId==2)
    {
        delete sound_gen2;
        sound_gen2 = new Sound_Gen();
        sound_gen2 -> setup(frequency,sampleRate_,amplitude);
    }

}

double AudioEngine::getToneFrequency(int toneId){
    if(toneId==1)
    {
        return sound_gen1->getFrequency();
    }
    if(toneId==2)
    {
        return sound_gen1->getFrequency();
    }
    return 0;
}

bool AudioEngine::createAssetAudioPlayer(AAsset *asset, const char* filename) {
    //load the file
    //bool result=mp3_file_decoder->init(asset);

    // open asset as file descriptor
    //off_t start, length;
    //int fd = AAsset_openFileDescriptor(asset, &start, &length);
    //assert(0 <= fd);
    //AAsset_close(asset);

    if (mp3_file != NULL)
        delete mp3_file;

    mp3_file = new MP3Decoder(filename, asset, true);
    LOGD("INFO:%s", mp3_file->getInfo(0));
    LOGD("INFO:%s", mp3_file->getInfo(1));
    LOGD("INFO:%s", mp3_file->getInfo(2));
    LOGD("INFO:%s", mp3_file->getInfo(3));

    return true;
}