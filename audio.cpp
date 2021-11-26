#include "audio.hpp"
#include "cpu.hpp"
#include "io.hpp"

gbaAudio audio;


void audioCallback(void* userdata, Uint8* stream, int len)
{
    static bool firstRun = false;

    if(firstRun == false)
    {
        /*
        FILE* megalo = fopen("mega.raw", "rb");
        fseek (megalo, 0, SEEK_END);
        uint64_t rSize = ftell(megalo);
        rewind(megalo);
        size_t dummy = fread(audio.megalovaniaTest,rSize,1,megalo);
        fclose(megalo);
        audio.megaloTestCount = 0;
        */
    }

    firstRun = true;


    io.dmaControl[1] |= 0x8000;
    if((io.dmaControl[1] & 0x200) == 0)
    {
        //io.lastStartAddr[1] = io.dmaStartAddress[1];
    }
    audio.inAudioCallback = true;
    //cpu.handleDMA();





    //bool dmaWriteMode = ((dmaControl & (0x1 << 10)) != 0);
    //printf("DMALOC: 0x%X\n",startAddrDMA);

    //wordCountDMA = 4;
    //audio.streamCounter = 0;

    uint32_t startAddrDMA = io.lastStartAddr[1];

    for(int i = 0; i < 4; i++)
    {
        for(int x = 0; x < 4; x++)
        {
            audio.samples[((i) * 4) + x] = io.readMem(startAddrDMA, 0);
            //audio.samples[(i * 4) + x] = audio.megalovaniaTest[audio.megaloTestCount];
            audio.megaloTestCount++;
            startAddrDMA++;
        }
    }

    //startAddrDMA += 24;
    io.lastStartAddr[1] = startAddrDMA;






    audio.inAudioCallback = false;
    io.dmaControl[1] |= 0x200;
    //cpu.doAudioDMATransfer(io.dmaStartAddress[1], io.dmaEndAddress[1], io.dmaWordCount[1], io.dmaControl[1]);
    for(int i = 0; i < len; i++)
    {
        //stream[i] = io.readMem(0x03005a20 + (i / 4), 0);
        stream[i] = audio.samples[(i / 2) / 2];
        //stream[i] = audio.megalovaniaTest[((audio.megaloTestCount + len) - i) / 2];

        //audio.frameSamples[audio.frameSampleCount] = audio.samples[i / 2];
        audio.frameSampleCount++;
        //stream[i] = i;
        audio.averageFreqPerFrame++;
        //stream[i] = i;
        //ram.onChip[(io.dmaStartAddress[1] & 0x7FFF) + (len - (i / 4))];
        //io.readMem(0x08000000 + i, 0);
        //stream[i] = (i * 0x10) & 0x80;
        //stream[i] = 0x00;
    }
    //audio.megaloTestCount += len;
}
