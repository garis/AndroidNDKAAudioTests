//
// Created by riccardo on 23/07/18.
//

#ifndef AUDIOTEST_AUDIO_FILE_H
#define AUDIOTEST_AUDIO_FILE_H


#include <sstream>

#ifdef BYTE_ORDER
#if BYTE_ORDER == BIG_ENDIAN
        #define _BIG_ENDIAN_
    #endif
#endif

class AudioInFile {
protected:
    uint numSamples     = 0;
    uint sampleRate     = 0;
    uint numChannels    = 0;
    uint numBits        = 0;

    void setNumSamples(uint numSamples);
    void setSampleRate(uint sampleRate);
    void setNumChannels(uint numChannels);
    void setNumBits(uint numBits);
    double getConv();

#ifdef _BIG_ENDIAN_
    static int _swap32(int &dwData) {
            dwData = ((dwData >> 24) & 0x000000FF) |
                   ((dwData >> 8)  & 0x0000FF00) |
                   ((dwData << 8)  & 0x00FF0000) |
                   ((dwData << 24) & 0xFF000000);
            return dwData;
        }

        static short _swap16(short &wData) {
            wData = ((wData >> 8) & 0x00FF) |
                    ((wData << 8) & 0xFF00);
            return wData;
        }

        static void _swap16Buffer(short *pData, int numWords) {
            int i;

            for (i = 0; i < numWords; i ++) {
                pData[i] = _swap16(pData[i]);
            }
        }
#else
    static int _swap32(int &dwData) {
        return dwData;
    }

    static short _swap16(short &wData) {
        return wData;
    }

    static void _swap16Buffer(short *pData, int numBytes) {
    }
#endif

public:
    virtual ~AudioInFile() { };

    virtual int eof() const = 0;
    virtual int read(float *buf, int len) = 0;
    virtual void rewind() = 0;

    virtual uint getNumChannels() const;
    virtual uint getSampleRate() const;
    virtual uint getBytesPerSample() const;
    virtual uint getNumSamples() const;
    virtual uint getNumBits() const;
};

class AudioFileFactory {
public:
    static AudioInFile *createAudioInFile(const char *cFilename);
};


#endif //AUDIOTEST_AUDIO_FILE_H
