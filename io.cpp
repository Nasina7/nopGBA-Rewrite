#include "io.hpp"
#include "cpu.hpp"
#include "audio.hpp"
#include "ui.hpp"
#include "save.hpp"
#include <stdio.h>

gbaIO io;
gbaRAM ram;

#define biosLocation 0x0000 ... 0x3FFF
#define onBoardLocation 0x02000000 ... 0x0203FFFF
#define onChipLocation 0x03000000 ... 0x03007FFF
#define ioLocaton 0x04000000 ... 0x040003FE
#define palLocation 0x05000000 ... 0x050003FF
#define videoLocation 0x06000000 ... 0x06017FFF
#define objLocation 0x07000000 ... 0x070003FF
#define romLocation 0x08000000 ... 0x0DFFFFFF
#define saveLocation 0x0E000000 ... 0x0E00FFFF

#define biosLocationB 0x0
#define onBoardLocationB 0x2
#define onChipLocationB 0x3
#define ioLocatonB 0x4
#define palLocationB 0x5
#define videoLocationB 0x6
#define objLocationB 0x7
#define romLocationB 0x08 ... 0x0D
#define saveLocationB 0xE
/*
#define biosLocation 0
#define onBoardLocation 2
#define onChipLocation 3
#define ioLocaton 4
#define palLocation 5
#define videoLocation 6
#define objLocation 7
#define romLocation 8 ... 0xD
#define saveLocation 0xE
*/

uint32_t gbaIO::get32bitOpcode(uint32_t location)
{
    uint32_t *returnVal;
    switch(location)
    {
        case romLocation:
            location -= 0x08000000; // Align Location
            returnVal = (uint32_t *)(ram.rom + location); // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster opcode lookups
                                                                    // at the cost of big-endian compatibility
            return returnVal[0]; // Return the 32 bit value that is stored
        break;

        default:
            printf("UNIMPLEMENTED OPCODE LOCATION: 0x%X\n",location);
            return 0xFFFFFFFF;
        break;
    }
}

uint32_t gbaIO::handleIORegs(uint32_t location, uint8_t mode, uint32_t value, bool write)
{

    switch(location)
    {
        case 0x04000000: // LCD CONTROL
            switch(write)
            {
                case false:
                    return io.lcdControl;
                break;

                case true:
                    io.lcdControl = value;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000004:
            switch(write)
            {
                case false: // Read
                    if(mode == 0)
                    {
                        return (io.dispStat & 0xFF);
                    }
                    io.dispStat = io.dispStat ^ 0x1;
                    return io.dispStat;
                break;

                case true: // Write
                    io.dispStat = value;
                break;
            }
            if(mode == 2)
            {
                case false: // Read
                    io.dispStat = io.dispStat ^ 0x1;
                    return io.LY << 8 | io.dispStat;
                break;

                case true: // Write
                    io.dispStat = value;
                break;
            }
        break;

        case 0x04000006:
            //io.LY++;
            //io.LY = io.LY % 228;
            return io.VCOUNT;
        break;

        case 0x04000008: // BG CONTROL 0
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.bgCNT[1] << 8 | io.bgCNT[0];
                    }
                    return io.bgCNT[0];
                break;

                case true:
                    io.bgCNT[0] = value;
                    if(mode == 2)
                    {
                        io.bgCNT[1] = value >> 16;
                    }
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400000A: // BG CONTROL 0
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.bgCNT[2] << 8 | io.bgCNT[1];
                    }
                    return io.bgCNT[1];
                break;

                case true:
                    io.bgCNT[1] = value;
                    if(mode == 2)
                    {
                        io.bgCNT[2] = value >> 16;
                    }
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400000C: // BG CONTROL 0
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.bgCNT[3] << 8 | io.bgCNT[2];
                    }
                    return io.bgCNT[2];
                break;

                case true:
                    io.bgCNT[2] = value;
                    if(mode == 2)
                    {
                        io.bgCNT[3] = value >> 16;
                    }
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400000E: // BG CONTROL 0
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.bgXScroll[0] << 8 | io.bgCNT[3];
                    }
                    return io.bgCNT[3];
                break;

                case true:
                    io.bgCNT[3] = value;
                    if(mode == 2)
                    {
                        io.bgXScroll[0] = value >> 16;
                    }
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000010: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgXScroll[0];
                break;

                case true:
                    io.bgXScroll[0] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000012: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgYScroll[0];
                break;

                case true:
                    io.bgYScroll[0] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000014: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgXScroll[1];
                break;

                case true:
                    io.bgXScroll[1] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000016: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgYScroll[1];
                break;

                case true:
                    io.bgYScroll[1] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000018: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgXScroll[2];
                break;

                case true:
                    io.bgXScroll[2] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400001A: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgYScroll[2];
                break;

                case true:
                    io.bgYScroll[2] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400001C: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgXScroll[3];
                break;

                case true:
                    io.bgXScroll[3] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400001E: // BG Scroll
            switch(write)
            {
                case false:
                    return io.bgYScroll[3];
                break;

                case true:
                    io.bgYScroll[3] = value & 0x1FF;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x04000082: // Sound CNT
            switch(write)
            {
                case false:
                    return io.SOUNDCNT_H;
                break;

                case true:
                    io.SOUNDCNT_H = value;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x040000A0:
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    //printf("sussy baka.\n");
                    //value = 0x12345678;
                    for(int i = 0; i < 4; i++)
                    {
                        audio.samples[audio.streamCounter] = ((value >> (24 - (i * 8))) & 0xFF);
                        audio.streamCounter++;
                        audio.streamCounter %= 16;
                    }
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x040000B0: // DMA0 Source Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaStartAddress[0] = value;
                    if(mode == 2)
                    {
                        io.dmaEndAddress[0] = value >> 16;
                    }
                    if((io.dmaControl[0] & 0x200) == 0)
                    {
                        io.lastStartAddr[0] = io.dmaStartAddress[0];
                        io.lastEndAddr[0] = io.dmaEndAddress[0];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000B4: // DMA0 End Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaEndAddress[0] = value;
                    if(mode == 2)
                    {
                        io.dmaWordCount[0] = value >> 16;
                    }
                    if((io.dmaControl[0] & 0x200) == 0)
                    {
                        io.lastStartAddr[0] = io.dmaStartAddress[0];
                        io.lastEndAddr[0] = io.dmaEndAddress[0];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000B8: // DMA0 Word Count
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaWordCount[0] = value;
                    if(mode == 2)
                    {
                        io.dmaControl[0] = value >> 16;
                    }
                    if((io.dmaControl[0] & 0x200) == 0)
                    {
                        io.lastStartAddr[0] = io.dmaStartAddress[0];
                        io.lastEndAddr[0] = io.dmaEndAddress[0];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000BA: // DMA0 Control
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    return io.dmaControl[0];
                break;

                case true:
                    io.dmaControl[0] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    if((io.dmaControl[0] & 0x200) == 0)
                    {
                        io.lastStartAddr[0] = io.dmaStartAddress[0];
                        io.lastEndAddr[0] = io.dmaEndAddress[0];
                    }
                    cpu.handleDMA();
                break;
            }
        break;



        case 0x040000BC: // DMA0 Source Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaStartAddress[1] = value;
                    if((io.dmaControl[1] & 0x200) == 0)
                    {
                        io.lastStartAddr[1] = io.dmaStartAddress[1];
                        io.lastEndAddr[1] = io.dmaEndAddress[1];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000C0: // DMA0 End Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaEndAddress[1] = value;
                    if((io.dmaControl[1] & 0x200) == 0)
                    {
                        io.lastStartAddr[1] = io.dmaStartAddress[1];
                        io.lastEndAddr[1] = io.dmaEndAddress[1];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000C4: // DMA0 Word Count
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaWordCount[1] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    if((io.dmaControl[1] & 0x200) == 0)
                    {
                        io.lastStartAddr[1] = io.dmaStartAddress[1];
                        io.lastEndAddr[1] = io.dmaEndAddress[1];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000C6: // DMA0 Control
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    return io.dmaControl[1];
                break;

                case true:
                    if((value & 0x8000) == 0x8000)
                    {
                        io.lastStartAddr[1] = io.dmaStartAddress[1];
                        io.lastEndAddr[1] = io.dmaEndAddress[1];
                    }
                    io.dmaControl[1] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    cpu.handleDMA();
                break;
            }
        break;



        case 0x040000C8: // DMA0 Source Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaStartAddress[2] = value;
                    if((io.dmaControl[2] & 0x200) == 0)
                    {
                        io.lastStartAddr[2] = io.dmaStartAddress[2];
                        io.lastEndAddr[2] = io.dmaEndAddress[2];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000CC: // DMA0 End Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaEndAddress[2] = value;
                    if((io.dmaControl[2] & 0x200) == 0)
                    {
                        io.lastStartAddr[2] = io.dmaStartAddress[2];
                        io.lastEndAddr[2] = io.dmaEndAddress[2];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000D0: // DMA0 Word Count
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaWordCount[2] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    if((io.dmaControl[2] & 0x200) == 0)
                    {
                        io.lastStartAddr[2] = io.dmaStartAddress[2];
                        io.lastEndAddr[2] = io.dmaEndAddress[2];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000D2: // DMA0 Control
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    return io.dmaControl[2];
                break;

                case true:
                    io.dmaControl[2] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    if((io.dmaControl[2] & 0x200) == 0)
                    {
                        io.lastStartAddr[2] = io.dmaStartAddress[2];
                        io.lastEndAddr[2] = io.dmaEndAddress[2];
                    }
                    cpu.handleDMA();
                break;
            }
        break;


        case 0x040000D4: // DMA0 Source Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaStartAddress[3] = value;
                    if(mode == 2)
                    {
                        io.dmaEndAddress[3] = value >> 16;
                    }
                    if((io.dmaControl[3] & 0x200) == 0)
                    {
                        io.lastStartAddr[3] = io.dmaStartAddress[3];
                        io.lastEndAddr[3] = io.dmaEndAddress[3];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000D8: // DMA0 End Address
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaEndAddress[3] = value;
                    if(mode == 2)
                    {
                        io.dmaWordCount[3] = value >> 16;
                    }
                    if((io.dmaControl[3] & 0x200) == 0)
                    {
                        io.lastStartAddr[3] = io.dmaStartAddress[3];
                        io.lastEndAddr[3] = io.dmaEndAddress[3];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000DC: // DMA0 Word Count
            switch(write)
            {
                case false:
                    return 0;
                break;

                case true:
                    io.dmaWordCount[3] = value;
                    if(mode == 2)
                    {
                        io.dmaControl[3] = value >> 16;
                    }
                    if((io.dmaControl[3] & 0x200) == 0)
                    {
                        io.lastStartAddr[3] = io.dmaStartAddress[3];
                        io.lastEndAddr[3] = io.dmaEndAddress[3];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x040000DE: // DMA0 Control
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    return io.dmaControl[3];
                break;

                case true:
                    io.dmaControl[3] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    if((io.dmaControl[3] & 0x200) == 0)
                    {
                        io.lastStartAddr[3] = io.dmaStartAddress[3];
                        io.lastEndAddr[3] = io.dmaEndAddress[3];
                    }
                    cpu.handleDMA();
                break;
            }
        break;

        case 0x04000100:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_H[0] << 16 | io.TMCNT_C[0];
                    }
                    return 0x10000 - (cpu.timerScheduler[0] / cpu.timerFreqLookup[io.TMCNT_H[0] & 0x3]);
                break;

                case true:
                    io.TMCNT_L[0] = value;
                    if(mode == 2)
                    {
                        if((io.TMCNT_H[0] & 0x80) == 0 && (value & 0x80) == 0x80)
                        {
                            io.TMCNT_C[0] = value;
                        }
                        io.TMCNT_H[0] = value >> 16;
                    }
                break;
            }
        break;

        case 0x04000102:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_C[1] << 16 | io.TMCNT_H[0];
                    }
                    return io.TMCNT_H[0];
                break;

                case true:
                    if((io.TMCNT_H[0] & 0x80) == 0 && (value & 0x80) == 0x80)
                    {
                        io.TMCNT_C[0] = value;
                    }
                    io.TMCNT_H[0] = value;
                    if(mode == 2)
                    {
                        io.TMCNT_L[1] = value >> 16;
                    }
                break;
            }
        break;

        case 0x04000104:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_H[1] << 16 | io.TMCNT_C[1];
                    }
                    return 0x10000 - (cpu.timerScheduler[1] / cpu.timerFreqLookup[io.TMCNT_H[1] & 0x3]);
                break;

                case true:
                    io.TMCNT_L[1] = value;
                    if(mode == 2)
                    {
                        if((io.TMCNT_H[1] & 0x80) == 0 && (value & 0x80) == 0x80)
                        {
                            io.TMCNT_C[1] = value;
                        }
                        io.TMCNT_H[1] = value >> 16;
                    }
                break;
            }
        break;

        case 0x04000106:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_C[2] << 16 | io.TMCNT_H[1];
                    }
                    return io.TMCNT_H[1];
                break;

                case true:
                    if((io.TMCNT_H[1] & 0x80) == 0 && (value & 0x80) == 0x80)
                    {
                        io.TMCNT_C[1] = value;
                    }
                    io.TMCNT_H[1] = value;
                    if(mode == 2)
                    {
                        io.TMCNT_L[2] = value >> 16;
                    }
                break;
            }
        break;

        case 0x04000108:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_H[2] << 16 | io.TMCNT_C[2];
                    }
                    return 0x10000 - (cpu.timerScheduler[2] / cpu.timerFreqLookup[io.TMCNT_H[2] & 0x3]);
                break;

                case true:
                    io.TMCNT_L[2] = value;
                    if(mode == 2)
                    {
                        if((io.TMCNT_H[2] & 0x80) == 0 && (value & 0x80) == 0x80)
                        {
                            io.TMCNT_C[2] = value;
                        }
                        io.TMCNT_H[2] = value >> 16;
                    }
                break;
            }
        break;

        case 0x0400010A:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_C[3] << 16 | io.TMCNT_H[2];
                    }
                    return io.TMCNT_H[2];
                break;

                case true:
                    if((io.TMCNT_H[2] & 0x80) == 0 && (value & 0x80) == 0x80)
                    {
                        io.TMCNT_C[2] = value;
                    }
                    io.TMCNT_H[2] = value;
                    if(mode == 2)
                    {
                        io.TMCNT_L[3] = value >> 16;
                    }
                break;
            }
        break;

        case 0x0400010C:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        return io.TMCNT_H[3] << 16 | io.TMCNT_C[3];
                    }
                    return 0x10000 - (cpu.timerScheduler[3] / cpu.timerFreqLookup[io.TMCNT_H[3] & 0x3]);
                break;

                case true:
                    io.TMCNT_L[3] = value;
                    if(mode == 2)
                    {
                        if((io.TMCNT_H[3] & 0x80) == 0 && (value & 0x80) == 0x80)
                        {
                            io.TMCNT_C[3] = value;
                        }
                        io.TMCNT_H[3] = value >> 16;
                    }
                break;
            }
        break;

        case 0x0400010E:
            switch(write)
            {
                case false:
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                    return io.TMCNT_H[3];
                break;

                case true:
                    if((io.TMCNT_H[3] & 0x80) == 0 && (value & 0x80) == 0x80)
                    {
                        io.TMCNT_C[3] = value;
                    }
                    io.TMCNT_H[3] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
                break;
            }
        break;

        case 0x04000130:
            switch(write)
            {
                case false: // Read
                    if(mode == 0)
                    {
                        return io.KEYINPUT & 0xFF;
                    }
                    return io.KEYINPUT & 0x3FF;
                break;

                case true: // Write

                break;
            }
            if(mode == 2)
            {
                printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
            }
        break;

        case 0x04000200:
            switch(write)
            {
                case false: // Read
                    if(mode == 0)
                    {
                        return io.IE & 0xFF;
                    }
                    if(mode == 2)
                    {
                        return io.IF << 16 | io.IE;
                    }
                    return io.IE;
                break;

                case true: // Write
                    io.IE = value;
                    if(mode == 2)
                    {
                        io.IF = value >> 16;
                    }
                break;
            }
        break;

        case 0x04000202:
            switch(write)
            {
                case false: // Read
                    if(mode == 0)
                    {
                        return io.IF & 0xFF;
                    }
                    return io.IF;
                break;

                case true: // Write
                    //printf("IRQ 0x%X\n",value);
                    for(int i = 0; i < 15; i++)
                    {
                        if(((value >> i) & 0x1) == 1)
                        {
                            io.IF &= (~(0x1 << i));
                        }
                    }
                    //io.IF = value;
                break;
            }
            if(mode == 2)
            {
                printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
            }
        break;

        case 0x04000204:
            switch(write)
            {
                case false: // Read
                    if(mode == 0)
                    {
                        return io.WAITCNT & 0xFF;
                    }
                    return io.WAITCNT;
                break;

                case true: // Write
                    io.WAITCNT = value;
                break;
            }
            if(mode == 2)
            {
                printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
            }
        break;

        case 0x04000208:
            switch(write)
            {
                case false: // Read
                    return io.IME;
                break;

                case true: // Write
                    io.IME = (value & 0x1);
                break;
            }
            if(mode == 2)
            {
                //printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
            }
        break;

        case 0x04000300: // POSTFLG
            switch(write)
            {
                case false: // Read
                    return 1;
                break;

                case true: // Write
                    // No Effect Currently
                break;
            }
            if(mode == 2)
            {
                printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
            }
        break;

        default:
            //printf("Attempted to ");
            switch(write)
            {
                case false:
                    //printf("read ");
                break;

                case true:
                    //printf("write ");
                break;
            }
            //printf("to I/O Reg 0x%X!\n",location);
            //printf("IO Reg WAS NOT IMPLEMENTED!\n");
        break;
    }
    return 0xFFFFFFFF & andTable[mode];
}

uint8_t badRamAccess;

uint32_t *returnVal;
uint16_t *returnVal16;
uint32_t location2;

uint8_t* mainMem;



uint8_t *memPointArray[0x10] =
{
    ram.bios,
    &badRamAccess,
    ram.onBoard,
    ram.onChip,
    &badRamAccess,
    ram.bg_ObjPal,
    ram.video,
    ram.object,
    ram.rom,
    ram.rom + 0x1000000,
    ram.rom + 0x2000000,
    ram.rom + 0x3000000,
    ram.rom + 0x4000000,
    ram.rom + 0x5000000,
    ram.save,
    &badRamAccess
};

uint32_t memRangeArray[0x10] =
{
    0x4000,
    0x1,
    0x40000,
    0x8000,
    0xFFFFFFFF,
    0x400,
    0x20000,
    0x400,
    0x1000000,
    0x1000000,
    0x1000000,
    0x1000000,
    0x1000000,
    0x1000000,
    0x10000,
    0x1
};

uint32_t andTable[3] = {
    0xFF,
    0xFFFF,
    0xFFFFFFFF
};


uint8_t section;

uint32_t gbaIO::oldReadMem(uint32_t location, uint8_t mode)
{



    //return 0;
    //location2 = location;

    section = location >> 24;
    section &= 0xF;
    ram.accessTest[section]++;

    switch(location)
    {
        case romLocation:
            switch(mode)
            {

                case 1: // 16 Bit
                    location -= 0x08000000; // Align Location
                    //location &= 0x00FFFFFF;
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.rom + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 0: // 8 Bit
                    location -= 0x08000000; // Align Location
                    return ram.rom[location];
                break;

                case 2: // 32 Bit
                    location -= 0x08000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.rom + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    //return returnVal[0];
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case onChipLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    //location -= 0x03000000; // Align Location
                    location &= 0x7FFF;
                    return ram.onChip[location];
                break;

                case 1: // 16 Bit
                    //location -= 0x03000000; // Align Location
                    location &= 0x7FFE;
                    //location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    //location -= 0x03000000; // Align Location
                    location &= 0x7FFC;
                    //location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    //return returnVal[0];
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case biosLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    return ram.bios[location];
                break;

                case 1: // 16 Bit
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.bios + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.bios + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case ioLocaton:
            return handleIORegs(location,mode,0,false);
        break;

        case onBoardLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x02000000; // Align Location
                    return ram.onBoard[location];
                break;

                case 1: // 16 Bit
                    location -= 0x02000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    location -= 0x02000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case 0x03008000 ... 0x03FFFFFF:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0x7FFF;
                    return ram.onChip[location];
                break;

                case 1: // 16 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0x7FFF;
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0x7FFF;
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case palLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x05000000; // Align Location
                    return ram.bg_ObjPal[location];
                break;

                case 1: // 16 Bit
                    location -= 0x05000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.bg_ObjPal + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    location -= 0x05000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.bg_ObjPal + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case videoLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x06000000; // Align Location
                    return ram.video[location];
                break;

                case 1: // 16 Bit
                    location -= 0x06000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.video + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    location -= 0x06000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.video + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case objLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x07000000; // Align Location
                    return ram.object[location];
                break;

                case 1: // 16 Bit
                    location -= 0x07000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.object + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x1)
                    {
                        case 0:
                            return returnVal16[0]; // Return the 16 bit value that is stored
                        break;

                        case 1:
                            return rotr16(returnVal16[0],8);
                        break;
                    }
                break;

                case 2: // 32 Bit
                    location -= 0x07000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.object + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            return returnVal[0]; // Return the 32 bit value that is stored
                        break;

                        case 1:
                            return rotr32(returnVal[0],8);
                        break;

                        case 2:
                            return rotr32(returnVal[0],16);
                        break;

                        case 3:
                            return rotr32(returnVal[0],24);
                        break;
                    }
                break;
            }
        break;

        case saveLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x0E000000; // Align Location
                    return ram.save[location];
                break;

                case 1: // 16 Bit
                    return 0xFFFF;
                break;

                case 2: // 32 Bit
                    return 0xFFFFFFFF;
                break;
            }
        break;

        default:
            return 0;
            printf("UNIMPLEMENTED LOCATION: 0x%X\n",location);
            printf("PC: 0x%X\n",cpu.R[15]);
            while(true)
            {

            }
            switch(ram.openBusEnable)
            {
                case false:
                    printf("UNIMPLEMENTED LOCATION: 0x%X\n",location);
                    printf("PC: 0x%X\n",cpu.R[15]);
                    while(true)
                    {

                    }
                break;

                case true:
                    switch(mode)
                    {
                        case 0: // 8 Bit
                            return readMem(cpu.R[15],0);
                        break;

                        case 1: // 16 Bit
                            return readMem(cpu.R[15],1);
                        break;

                        case 2: // 32 Bit
                            return readMem(cpu.R[15],2);
                        break;
                    }
                break;
            }
        break;
    }
    return 0xFFFFFFFF;
}

uint32_t gbaIO::readMem(uint32_t location, uint8_t mode)
{
    //return oldReadMem(location, mode);
    uint8_t section2 = location >> 24;
    section2 &= 0xF;
    //ram.accessTest[section]++;

    if(section2 == 4)
    {
        return handleIORegs(location, mode, 0, false);
    }
    if(section2 == 1 || section2 == 0xF)
    {
        return 0;
    }
    if(section2 == 6 && (location & (memRangeArray[section2] - 1)) > 0x17FFF)
    {
        return 0;
    }
    if(section2 == 0xE)
    {
        return save.accessSRAM(location, false, 0);
    }
    uint32_t newLocation = location & locationCorrector[mode];
    uint32_t result = ((uint32_t*)(memPointArray[section2] + ((newLocation) & (memRangeArray[section2] - 1))))[0] & andTable[mode];
    if(mode == 1)
    {
        result = rotr32(result, (location & 0x1) * 8);
    }
    if(mode == 2)
    {
        result = rotr32(result, (location & 0x3) * 8);
    }

    return result;
}

void gbaIO::writeMem(uint32_t location, uint8_t mode, uint32_t value)
{
    if(location == 0x080000C4)
    {
        //ui.pauseEmulation = true;
    }
    uint32_t *returnVal;
    uint16_t *returnVal16;
    //uint8_t *returnVal8;
    uint32_t location2 = location;
    switch(location)
    {

        case onBoardLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x02000000; // Align Location
                    ram.onBoard[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x02000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x02000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case onChipLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x03000000; // Align Location
                    ram.onChip[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case 0x03008000 ... 0x03FFFFFF:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0x7FFF;
                    ram.onChip[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0x7FFF;
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x03000000; // Align Location
                    location = location & 0x7FFF;
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case ioLocaton:
            handleIORegs(location,mode,value,true);
        break;

        case palLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x05000000; // Align Location
                    ram.bg_ObjPal[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x05000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.bg_ObjPal + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x05000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.bg_ObjPal + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case videoLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x06000000; // Align Location
                    ram.video[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x06000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.video + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x06000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.video + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case objLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x07000000; // Align Location
                    ram.object[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x07000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.object + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x07000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.object + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case romLocation:
            if(location < 0x0A000000)
            {
                printf("ATTEMPTED TO WRITE TO ROM 0x%X!\n", location);
                break;
            }
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x08000000; // Align Location
                    ram.rom[location] = value;
                break;

                case 1: // 16 Bit
                    location -= 0x08000000; // Align Location
                    location = location & 0xFFFFFFFE;
                    returnVal16 = (uint16_t *)(ram.rom + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x08000000; // Align Location
                    location = location & 0xFFFFFFFC;
                    returnVal = (uint32_t *)(ram.rom + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    switch(location2 & 0x3)
                    {
                        case 0:
                            returnVal[0] = value;
                        break;

                        case 1:
                            returnVal[0] = rotr32(value,8);
                        break;

                        case 2:
                            returnVal[0] = rotr32(value,16);
                        break;

                        case 3:
                            returnVal[0] = rotr32(value,24);
                        break;
                    }
                break;
            }
        break;

        case saveLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    //location -= 0x0E000000; // Align Location
                    //ram.save[location] = value;
                    save.accessSRAM(location, true, value);
                break;
            }
        break;

        default:
            switch(ram.openBusEnable)
            {
                case false:
                    printf("UNIMPLEMENTED LOCATION: 0x%X\n",location);
                    printf("PC: 0x%X\n",cpu.R[15]);
                    while(true)
                    {

                    }
                break;

                case true:

                break;
            }

        break;
    }
}
