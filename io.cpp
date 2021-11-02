#include "io.hpp"
#include "cpu.hpp"
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

uint32_t handleIORegs(uint32_t location, uint8_t mode, uint32_t value, bool write)
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
                printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
            }
        break;

        case 0x04000006:
            io.LY++;
            io.LY = io.LY % 228;
            return io.LY;
        break;

        case 0x04000008: // BG CONTROL 0
            switch(write)
            {
                case false:
                    return io.bgCNT[0];
                break;

                case true:
                    io.bgCNT[0] = value;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400000A: // BG CONTROL 0
            switch(write)
            {
                case false:
                    return io.bgCNT[1];
                break;

                case true:
                    io.bgCNT[1] = value;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400000C: // BG CONTROL 0
            switch(write)
            {
                case false:
                    return io.bgCNT[2];
                break;

                case true:
                    io.bgCNT[2] = value;
                    //io.lcdControlUpdated = true;
                break;
            }
        break;

        case 0x0400000E: // BG CONTROL 0
            switch(write)
            {
                case false:
                    return io.bgCNT[3];
                break;

                case true:
                    io.bgCNT[3] = value;
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
                    io.bgXScroll[0] = value;
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
                    io.bgYScroll[0] = value;
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
                    io.bgXScroll[1] = value;
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
                    io.bgYScroll[1] = value;
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
                    io.bgXScroll[2] = value;
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
                    io.bgYScroll[2] = value;
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
                    io.bgXScroll[3] = value;
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
                    io.bgYScroll[3] = value;
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
                    io.dmaControl[1] = value;
                    if(mode == 2)
                    {
                        printf("32 Bit access to 0x%X is unimplemented!  This may cause bugs!\n",location);
                    }
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
    return 0xFFFFFFFF;
}

uint8_t badRamAccess;

uint8_t *memPointArray[] =
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
    ram.rom,
    ram.rom,
    ram.rom,
    ram.rom,
    ram.rom,
    ram.save,
    &badRamAccess
};

uint32_t memRangeArray[] =
{
    0x4000,
    0x0,
    0x40000,
    0x8000,
    0xFFFFFFFF,
    0x400,
    0x18000,
    0x400,
    0x1000000,
    0x1000000,
    0x1000000,
    0x1000000,
    0x1000000,
    0x1000000,
    0x10000,
    0x0
};

uint32_t gbaIO::readMem(uint32_t location, uint8_t mode)
{
    if(mode == 0x0)
    {
        //return 0;
    }
    uint32_t *returnVal;
    uint16_t *returnVal16;
    uint8_t lookupVal = (location >> 24) & 0xF;

    uint32_t location2 = location;

    //location &= memRangeArray[lookupVal] - 1;
    //location |= (lookupVal << 24);

    /*
    uint8_t* pointData;
    pointData = memPointArray[lookupVal];
    if(location > 0x0FFFFFFF)
    {
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
    }

    switch(lookupVal)
    {
        case 4:
            return handleIORegs(location, mode, 0, false);
        break;
    }

    location &= 0x00FFFFFF;
    if(location >= memRangeArray[lookupVal])
    {
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
    }
    //return 0;
    //printf("location: 0x%X, 0x%X\n", location, lookupVal);
    switch(mode)
    {
        case 0: // 8 Bit
            return pointData[location];
           // return ram.bios[location];
        break;

        case 1: // 16 Bit
            location = location & 0xFFFFFFFE;
            returnVal16 = (uint16_t *)(pointData + location);   // Make a pointer that points to the current 8 bit location,
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
            returnVal = (uint32_t *)(pointData + location);   // Make a pointer that points to the current 8 bit location,
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


    return 0;
    //uint8_t *returnVal8;
    */
    switch(location)
    {

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

        case onChipLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x03000000; // Align Location
                    return ram.onChip[location];
                break;

                case 1: // 16 Bit
                    location -= 0x03000000; // Align Location
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

        case ioLocaton:
            return handleIORegs(location,mode,0,false);
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

        case romLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x08000000; // Align Location
                    return ram.rom[location];
                break;

                case 1: // 16 Bit
                    location -= 0x08000000; // Align Location
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
            /*
            printf("UNIMPLEMENTED LOCATION: 0x%X\n",location);
            printf("PC: 0x%X\n",cpu.R[15]);
            while(true)
            {

            }
            */
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




void gbaIO::writeMem(uint32_t location, uint8_t mode, uint32_t value)
{
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
                    switch(location2 & 0x1)
                    {
                        case 0:
                            returnVal16[0] = value;
                        break;

                        case 1:
                            returnVal16[0] = rotr16(value,8);
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
                    switch(location2 & 0x1)
                    {
                        case 0:
                            returnVal16[0] = value;
                        break;

                        case 1:
                            returnVal16[0] = rotr16(value,8);
                        break;
                    }
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
                    switch(location2 & 0x1)
                    {
                        case 0:
                            returnVal16[0] = value;
                        break;

                        case 1:
                            returnVal16[0] = rotr16(value,8);
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
                    switch(location2 & 0x1)
                    {
                        case 0:
                            returnVal16[0] = value;
                        break;

                        case 1:
                            returnVal16[0] = rotr16(value,8);
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
                    switch(location2 & 0x1)
                    {
                        case 0:
                            returnVal16[0] = value;
                        break;

                        case 1:
                            returnVal16[0] = rotr16(value,8);
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
                    switch(location2 & 0x1)
                    {
                        case 0:
                            returnVal16[0] = value;
                        break;

                        case 1:
                            returnVal16[0] = rotr16(value,8);
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
            printf("ATTEMPTED TO WRITE TO ROM 0x%X!\n", location);
        break;

        case saveLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x0E000000; // Align Location
                    ram.save[location] = value;
                break;

                case 1: // 16 Bit
                    //return 0xFFFF;
                break;

                case 2: // 32 Bit
                    //return 0xFFFFFFFF;
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
