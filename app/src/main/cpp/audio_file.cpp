#include "audio_file.h"
#include "MP3Decoder.h"

void AudioInFile::setNumSamples(uint numSamples) {
    this->numSamples = numSamples;
}

void AudioInFile::setSampleRate(uint sampleRate) {
    this->sampleRate = sampleRate;
}

void AudioInFile::setNumChannels(uint numChannels) {
    this->numChannels = numChannels;
}

void AudioInFile::setNumBits(uint numBits) {
    this->numBits = numBits;
}

uint AudioInFile::getNumChannels() const {
    return numChannels;
}

uint AudioInFile::getSampleRate() const {
    return sampleRate;
}

uint AudioInFile::getBytesPerSample() const {
    return getNumChannels() * getNumBits() / 8;
}

uint AudioInFile::getNumSamples() const {
    return numSamples;
}

uint AudioInFile::getNumBits() const {
    return numBits;
}

double AudioInFile::getConv() {
    switch (this->getNumBits()) {
        case 8:
            return 1.0 / 128.0;
            break;
        case 16:
            return 1.0 / 32768.0;
            break;
        case 24:
            return 1.0 / 2147483648.0;
            break;
        default:
            throw std::runtime_error(std::string("getConv(): Unsupported bit depth of ") + std::to_string(this->getNumBits()));
            break;
    }
}

AudioInFile *AudioFileFactory::createAudioInFile(const char *cFilename) {
    std::string filename(cFilename);
    std::string ext = filename.substr(filename.find_last_of(".") + 1);

    //if (ext == "mp3") {
    //    return new MP3Decoder(cFilename);
    //} else {
    //    throw std::runtime_error(std::string("No decoder found for extension: ") + ext);
    //}

    //if (ext == "wav") {
    //    return new WavInFile(cFilename);
    //} else if (ext == "flac") {
    //    return new FLACDecoder(cFilename);
    //} else if (ext == "aiff") {
    //    return new AIFFDecoder(cFilename);
    //} else if (ext == "mp3") {
    //    return new MP3Decoder(cFilename);
    //} else {
    //    throw std::runtime_error(std::string("No decoder found for extension: ") + ext);
    //}
}
#include "audio_file.h"
