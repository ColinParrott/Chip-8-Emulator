#include "Sound.h"
#include <cmath>
#include <iostream>


Sound::Sound()
        : m_sineFreq(500),
          m_sampleFreq(44100),
          m_samplesPerSine(m_sampleFreq / m_sineFreq),
          m_samplePos(0)
{
}

Sound::~Sound()
{
    SDL_CloseAudioDevice(m_device);
}

/**
 * Plays beep
 */
void Sound::play()
{
    SDL_PauseAudioDevice(m_device, 0);
}

/**
 * Stops beep
 */
void Sound::stop()
{
    SDL_PauseAudioDevice(m_device, 1);
}

/**
 * Callback to fill buffer when sound needed
 * @param data Sound class (this class) instance we can access
 * @param raw_buffer Buffer to fill
 * @param bytes Length of buffer in bytes
 */
void Sound::SDLAudioCallback(void *data, Uint8 *raw_buffer, int bytes)
{
    auto *buffer = (Sint16 *) raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int &sample_nr(*(int *) data);

    for (int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double) sample_nr / (double) 44100;
        buffer[i] = (Sint16) (1000 * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}

/**
 * Initialise audio device and spec
 */
void Sound::init()
{
    SDL_AudioSpec wantSpec, haveSpec;

    SDL_zero(wantSpec);
    wantSpec.freq = m_sampleFreq;
    wantSpec.format = AUDIO_S16SYS;
    wantSpec.channels = 2;
    wantSpec.samples = 512;
    wantSpec.callback = SDLAudioCallback;
    wantSpec.userdata = this;

    m_device = SDL_OpenAudioDevice(nullptr, 0, &wantSpec, &haveSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (m_device == 0)
    {
        std::cout << "Failed to open audio: " << SDL_GetError() << std::endl;
    }
}
