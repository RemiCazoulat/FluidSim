//
// Created by Rémi on 28/10/2024.
//

#ifndef FLUIDSIM_SOUNDCONVERTOR_H
#define FLUIDSIM_SOUNDCONVERTOR_H

#include "libraries.h"

class SoundConvertor {
    size_t sample_rate;
    float duration_ms;
    size_t sample_count;
    size_t hz_limit;
    char* wavFilePath = nullptr;

    float* sample_buffer;
    int buffer_index = 0;
    std::vector<float> amplitude_buffer;


    ma_device device;
    ma_device_config deviceConfig;
    ma_decoder decoder;
    ma_decoder_config decoderConfig;
    ma_result result;
    void process_audio_callback(const void *pInput, ma_uint32 frameCount);
    void process_fft();


public:

    explicit SoundConvertor(size_t sample_rate = 44100, float duration_ms = 10, int hz_limit = 5000);
    void set_new_rate_and_duration(size_t new_rate, float new_duration, int new_hz_l);
    ~SoundConvertor();
    static void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    void init_device();
    void start_device();
    void uninit_device();

    void read_wav_file(const char* filePath);

};


#endif //FLUIDSIM_SOUNDCONVERTOR_H
