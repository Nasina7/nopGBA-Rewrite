#include <stdint.h>

class gbaRAM
{
    public:
        uint8_t bios[0x4000]; // 0x00000000 - 0x00003FFF
        // 0x00004000 - 0x01FFFFFF UNUSED
        uint8_t onBoard[0x40000]; // 0x02000000 - 0x0203FFFF
        // 0x02040000 - 0x02FFFFFF UNUSED
        uint8_t onChip[0x8000]; // 0x03000000 - 0x03007FFF
        // 0x03008000 - 0x03FFFFFF UNUSED
        // 0x04000000 - 0x040003FE IO REGS (Will redirect writes)
        // 0x04000400 - 0x04FFFFFF UNUSED
        uint8_t bg_ObjPal[0x400]; // 05000000-050003FF
        // 05000400-05FFFFFF UNUSED
        uint8_t video[0x18000]; // 06000000-06017FFF
        // 06018000-06FFFFFF UNUSED
        uint8_t object[0x400]; // 07000000-070003FF
        // 07000400-07FFFFFF UNUSED
        uint8_t rom[0x6000000]; // 0x08000000 - 0x0DFFFFFF

        uint8_t save[0x10000]; // 0x0E000000 - 0x0E00FFFF
        // 0E010000 - FFFFFFFF UNUSED

        bool openBusEnable;
};




class gbaIO {
    public:

        uint16_t lcdControl;
        bool lcdControlUpdated;
        uint16_t dispStat;


        uint32_t dmaStartAddress[4];
        uint32_t dmaEndAddress[4];
        uint16_t dmaWordCount[4];
        uint16_t dmaControl[4];


        uint16_t LY;


        bool IME;
        uint16_t IE;
        uint16_t IF;

        bool HALTCNT;
        bool currentlyHalted;

        uint16_t bgCNT[4];
        uint16_t bgXScroll[4];
        uint16_t bgYScroll[4];
        uint16_t soundBias;

        uint16_t SOUNDCNT_H[4];
        uint16_t SOUNDCNT_HD;

        uint16_t KEYINPUT;

        uint16_t TMCNT_L[4]; // Reload
        uint16_t TMCNT_H[4]; // Control
        uint16_t TMCNT_C[4]; // Counter



        //uint32_t readRAM();
        uint32_t get32bitOpcode(uint32_t location);
        uint32_t readMem(uint32_t location, uint8_t mode);
        void writeMem(uint32_t location, uint8_t mode, uint32_t value);
    private:
};



extern gbaIO io;
extern gbaRAM ram;
