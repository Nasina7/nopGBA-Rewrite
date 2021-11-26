#include <SDL2/SDL.h>

class gbaAudio
{
    public:
        SDL_AudioSpec want, have;
        SDL_AudioDeviceID dev;

        Uint8 samples[128];
        float frameSamples[4096];

        bool inAudioCallback;

        int megaloTestCount;
        uint8_t megalovaniaTest[0x4e0aa0];


        int frameSampleCount;
        uint8_t streamCounter;
        uint32_t averageFreqPerFrame;
        uint8_t audioCounter;
};

extern void audioCallback(void* userdata, Uint8* stream, int len);

extern gbaAudio audio;
