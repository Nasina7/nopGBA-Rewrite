#include "io.hpp"
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
    printf("Attempted to ");
    switch(write)
    {
        case false:
            printf("read ");
        break;

        case true:
            printf("write ");
        break;
    }
    printf("to I/O Reg 0x%X!\n",location);

}

uint32_t gbaIO::readMem(uint32_t location, uint8_t mode)
{
    uint32_t *returnVal;
    uint16_t *returnVal16;
    //uint8_t *returnVal8;
    switch(location)
    {

        case onBoardLocation:
            switch(mode)
            {
                case 0: // 8 Bit
                    location -= 0x02000000; // Align Location
                    return ram.onBoard[location];
                break;

                case 1: // 16 Bit
                    location -= 0x02000000; // Align Location
                    returnVal16 = (uint16_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    return returnVal16[0]; // Return the 32 bit value that is stored
                break;

                case 2: // 32 Bit
                    location -= 0x02000000; // Align Location
                    returnVal = (uint32_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    return returnVal[0]; // Return the 32 bit value that is stored
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
                    returnVal16 = (uint16_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    return returnVal16[0]; // Return the 32 bit value that is stored
                break;

                case 2: // 32 Bit
                    location -= 0x03000000; // Align Location
                    returnVal = (uint32_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    return returnVal[0]; // Return the 32 bit value that is stored
                break;
            }
        break;

        case ioLocaton:
            return handleIORegs(location,mode,0,false);
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
                    returnVal16 = (uint16_t *)(ram.rom + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    return returnVal16[0]; // Return the 32 bit value that is stored
                break;

                case 2: // 32 Bit
                    location -= 0x08000000; // Align Location
                    returnVal = (uint32_t *)(ram.rom + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    return returnVal[0]; // Return the 32 bit value that is stored
                break;
            }
        break;

        default:
            printf("UNIMPLEMENTED LOCATION: 0x%X\n",location);
            while(true)
            {

            }
        break;
    }
}




uint32_t gbaIO::writeMem(uint32_t location, uint8_t mode, uint32_t value)
{
    uint32_t *returnVal;
    uint16_t *returnVal16;
    //uint8_t *returnVal8;
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
                    returnVal16 = (uint16_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value;
                break;

                case 2: // 32 Bit
                    location -= 0x02000000; // Align Location
                    returnVal = (uint32_t *)(ram.onBoard + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal[0] = value;
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
                    returnVal16 = (uint16_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 16 bit pointer, and put the value into another
                                                                    // 16 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal16[0] = value; // Return the 32 bit value that is stored
                break;

                case 2: // 32 Bit
                    location -= 0x03000000; // Align Location
                    returnVal = (uint32_t *)(ram.onChip + location);   // Make a pointer that points to the current 8 bit location,
                                                                    // Cast it as a 32 bit pointer, and put the value into another
                                                                    // 32 bit pointer.  This allows for much faster RAM lookups
                                                                    // at the cost of big-endian compatibility
                    returnVal[0] = value; // Return the 32 bit value that is stored
                break;
            }
        break;

        case ioLocaton:
            handleIORegs(location,mode,value,true);
        break;

        case romLocation:
            printf("ATTEMPTED TO WRITE TO ROM!\n");
        break;

        default:
            printf("UNIMPLEMENTED LOCATION: 0x%X\n",location);
            while(true)
            {

            }
        break;
    }
}
