//
// Created by Rémi on 28/10/2024.
//

#include "../include/SoundConvertor.h"



SoundConvertor::SoundConvertor(size_t sample_rate, float duration_ms, int hz_limit) {
    this->sample_rate = sample_rate;
    this->duration_ms = duration_ms;
    this->sample_count = size_t((float)sample_rate * duration_ms / 1000);
    this->hz_limit = hz_limit;
    sample_buffer = new float[sample_count];
    init_device();
}

void SoundConvertor::set_new_rate_and_duration(size_t new_rate, float new_duration, int new_hz_l){
    sample_rate = new_rate;
    duration_ms = new_duration;
    sample_count = size_t((float)sample_rate * duration_ms / 1000);
    hz_limit = new_hz_l;
    delete[] sample_buffer;
    sample_buffer = new float[sample_count];
    init_device();
}



void SoundConvertor::process_fft() {
    amplitude_buffer.clear();
    kiss_fft_cfg cfg = kiss_fft_alloc((int)sample_count, 0, nullptr, nullptr);
    auto* in = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * sample_count);
    auto* out = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * sample_count);
    for (int i = 0; i < sample_count; ++i) {
        in[i].r = sample_buffer[i];
        in[i].i = 0.0;
    }
    kiss_fft(cfg, in, out);
    for (int i = 0; i < sample_count / 2; ++i) {
        float frequency = (float)i * (float)sample_rate / (float)sample_count;
        if (frequency > (float)hz_limit) break;
        float amplitude = sqrt(out[i].r * out[i].r + out[i].i * out[i].i);
        amplitude_buffer.push_back(amplitude);
    }
    free(in);
    free(out);
    free(cfg);
}


void SoundConvertor::process_audio_callback(const void *pInput, ma_uint32 frameCount) {
    const auto* input_samples = (const float*)pInput;

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        sample_buffer[buffer_index++] = input_samples[i];

        if (i >= sample_count) {
            process_fft();
            buffer_index = 0;
        }
    }
}

void SoundConvertor::audio_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    auto* self = (SoundConvertor*)pDevice->pUserData;
    if (self) {
        self->process_audio_callback(pInput, frameCount);
    }
}


SoundConvertor::~SoundConvertor() {
    delete[] sample_buffer;
}

void SoundConvertor::init_device() {
    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 1;
    deviceConfig.sampleRate = sample_rate;
    deviceConfig.dataCallback = audio_callback;
    deviceConfig.pUserData = this;

    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        printf("device init failed.\n");
        return;
    }


}

void SoundConvertor::start_device() {
    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("device starting failed.\n");
        ma_device_uninit(&device);
        return;
    }
}

void SoundConvertor::uninit_device() {
    ma_device_uninit(&device);
}


void SoundConvertor::read_wav_file(const char* filePath) {

}




