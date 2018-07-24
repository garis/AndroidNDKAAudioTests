//
// Created by riccardo on 23/07/18.
//

#ifndef AUDIOTEST_MP3DECODER_H
#define AUDIOTEST_MP3DECODER_H


#include <iostream>
#include <vector>

#include "audio_file.h"
#include "lame-3.100_libmp3lame/lame.h"

#include <android/asset_manager.h>

#define MP3_SIZE 1024
#define PCM_SIZE MP3_SIZE * 1152

class MP3Decoder : public AudioInFile {
protected:
    const char *filename;

    //FILE *mp3;
    hip_t hip;
    mp3data_struct mp3data;
    std::vector<float> buffer;

    AAsset *asset;
    int fd;

    bool playing;
    bool loadAllFile;

    void readHeaders();
    int decodeChunk();

public:
    MP3Decoder(const char *filename, AAsset *assetVar, bool loadAll);
    ~MP3Decoder();

    int eof() const;
    int read(float *buf, int len);
    void rewind();

    bool isPlaying();

    char* getInfo(int id);

private:
    int readValue;
};


#endif //AUDIOTEST_MP3DECODER_H
