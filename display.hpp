#include <iostream>

class gbaDisplay {
    public:
        void drawScreen();
        void drawScreenMode0();
        void drawScreenMode4();
        uint32_t screenArray[160][240];

    private:
        void drawBGMode0(uint8_t bgNum);
        void drawSprites();
        void drawSpriteTile(uint16_t tileNum, uint16_t xLoc, uint16_t yLoc, uint8_t mode, uint8_t palNum);
};

extern gbaDisplay screen;
