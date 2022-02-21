#include <iostream>

class gbaDisplay {
    public:
        void drawScreenScanline();
        void drawScreenMode0Scanline();


        void drawScreen();
        void drawScreenMode0();
        void drawScreenMode3();
        void drawScreenMode4();
        uint32_t screenArray[160][240];
        uint32_t drawingArray[160][240];

        bool forceDrawBGs;
        bool forceDrawBg[4];

    private:
        void drawBGMode0Scanline(uint8_t bgNum);

        void drawBGMode0(uint8_t bgNum);
        void drawSprites();
        void drawSpriteTile(uint16_t tileNum, uint16_t xLoc, uint8_t yLoc, uint8_t mode, uint8_t palNum);
};

extern gbaDisplay screen;
