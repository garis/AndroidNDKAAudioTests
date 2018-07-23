#include <android/asset_manager.h>
#include "MP3Decoder.h"
#include "lame-3.100_libmp3lame/lame.h"

MP3Decoder::MP3Decoder(const char *filename, AAsset *assetVar) {
    this->filename = filename;

    asset=assetVar;

    off_t start, length;
    fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    //AAsset_close(asset);

    //mp3 = fopen(filename, "r");

    hip = hip_decode_init();

    playing=true;
    readHeaders();
}

MP3Decoder::~MP3Decoder() {
    AAsset_close(asset);
    //fclose(mp3);
    hip_decode_exit(hip);
}

bool MP3Decoder::isPlaying(){
    return playing;
}

//int AAsset_read(
//AAsset *asset,
//void *buf,
//        size_t count
//)
//  Attempt to read 'count' bytes of data from the current offset.

//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
//  ptr     Pointer to a block of memory
//  size_t  is an unsigned integral type.
//  size    Size, in bytes, of each element to be read.
//  count   Number of elements, each one with a size of size bytes.
//  stream  Pointer to a FILE object that specifies an input stream.
void MP3Decoder::readHeaders() {
    unsigned char headerBuf[1024];
    int len;

    do {
        //len = fread(headerBuf, sizeof(unsigned char), 1024, mp3);
        len = AAsset_read(asset,headerBuf,1024);

        short pcm_buffer_l[PCM_SIZE];
        short pcm_buffer_r[PCM_SIZE];

        hip_decode1_headers(hip, headerBuf, len, pcm_buffer_l, pcm_buffer_r, &mp3data);
    } while (mp3data.header_parsed == 0 && len > 0);

    if (mp3data.header_parsed == 0) {
        throw std::runtime_error(std::string("Unable to parse mp3 headers for ") + std::string(filename));
    }

    this->setNumSamples((uint) (mp3data.framesize * mp3data.totalframes));
    this->setSampleRate((uint) mp3data.samplerate);
    this->setNumChannels((uint) mp3data.stereo);
    this->setNumBits((uint) 16);
}

int MP3Decoder::eof() const {
    return buffer.empty() && readValue;//feof(mp3);
}

int MP3Decoder::decodeChunk() {
    //int read;
    int decoded;
    unsigned char mp3_buffer[MP3_SIZE];
    short pcm_buffer_l[PCM_SIZE];
    short pcm_buffer_r[PCM_SIZE];

    //read = fread(mp3_buffer, sizeof(unsigned char), MP3_SIZE, mp3);
    readValue = AAsset_read(asset,mp3_buffer,712);//1024

    decoded = hip_decode(hip, mp3_buffer, readValue, pcm_buffer_l, pcm_buffer_r);

    for (int i = 0; i < decoded; i++) {
        buffer.push_back((float) _swap16(pcm_buffer_l[i]) * this->getConv());
        buffer.push_back((float) _swap16(pcm_buffer_r[i]) * this->getConv());
    }

    return decoded;
}

int MP3Decoder::read(float *buf, int len) {
    if (buffer.empty()) {
        if (decodeChunk() == 0) {
            return 0;
        }
    }

    int i;

    for (i = 0; !buffer.empty() && i < len; i++) {
        buf[i] = buffer.at(0)*8;//VOLUME
        buffer.erase(buffer.begin());
    }

    return i;
}

void MP3Decoder::rewind() {
    //fseek(mp3, SEEK_SET, 0);
}

#include "MP3Decoder.h"
