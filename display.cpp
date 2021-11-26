#include "display.hpp"
#include "ui.hpp"
#include "io.hpp"

gbaDisplay screen;

void gbaDisplay::drawScreen()
{
    switch(io.lcdControl & 0x7)
    {
        case 0:
            screen.drawScreenMode0();
        break;

        case 1:
            screen.drawScreenMode0();
        break;

        case 2:
            screen.drawScreenMode0();
        break;

        case 4:
            screen.drawScreenMode4();
        break;

        default:

        break;
    }
    //printf("Successful Return\n");
}

void gbaDisplay::drawSpriteTile(uint16_t tileNum, uint16_t xLoc, uint16_t yLoc, uint8_t mode, uint8_t palNum)
{
    uint8_t xStart, xEnd, yStart, yEnd;
    int xAdd, yAdd;
    switch(mode)
    {
        case 0:
            xStart = 0;
            xEnd = 8;
            xAdd = 1;
            yStart = 0;
            yEnd = 8;
            yAdd = 1;
        break;

        case 1:
            xStart = 7;
            xEnd = 0xFF;
            xAdd = -1;
            yStart = 0;
            yEnd = 8;
            yAdd = 1;
        break;

        case 2:
            xStart = 0;
            xEnd = 8;
            xAdd = 1;
            yStart = 7;
            yEnd = 0xFF;
            yAdd = -1;
        break;

        case 3:
            xStart = 7;
            xEnd = 0xFF;
            xAdd = -1;
            yStart = 7;
            yEnd = 0xFF;
            yAdd = -1;
        break;
    }

    palNum += 0x10;
    for(uint8_t y = yStart; y != yEnd; y += yAdd)
    {
        //printf("y: 0x%X\n",y);
        for(uint8_t x = xStart; x != xEnd; x += xAdd)
        {
            //printf("x: 0x%X\n",x);

            uint32_t curTileDataLocation = 0x06010000 + ((tileNum) * 0x20) + ((y) * 4) + ((x) / 2);

            uint8_t tileDataTrue = io.readMem(curTileDataLocation, 0);

            uint32_t location = 0x05000000 + ((tileDataTrue & 0xF)) + ((tileDataTrue >> 4) * 0x20);

            uint8_t colorIndex;

            switch(x % 2)
            {
                case 0:
                    location = 0x05000000 + ((tileDataTrue & 0xF) * 2);
                    colorIndex = ((tileDataTrue & 0xF));
                break;

                case 1:
                    location = 0x05000000 + (((tileDataTrue >> 4)) * 2);
                    colorIndex = (((tileDataTrue >> 4) & 0xF));
                break;
            }
            //printf("location: 0x%X\n", location);
            //printf("palNum: 0x%X\n", palNum);
            uint16_t color = io.readMem(location + (palNum * 0x20), 1);
            //printf("XWRITE: %i\n",x + (xTile * 8));
            //printf("YWRITE: %i\n",y + (yTile * 8));
            int finalX = x;
            int finalY = y;
            if(colorIndex != 0 && xLoc < 240 && yLoc < 160)
            {
                screenArray[yLoc][xLoc] = (0xFF << 24) |
                       ((((color >> 10) & 0x1F) << 3) << 16) |
                       ((((color >> 5) & 0x1F) << 3) << 8) |
                       (((color & 0x1F) << 3));
            }
            xLoc++;


            //printf("SUCCESSFULLY WROTE\n");
        }
        xLoc -= 8;
        yLoc++;
        //printf("Exit X Loop\n");
    }
}

uint8_t yTileSizeLookup[] =
{
    8,
    8,
    16,
    0, // INVALID
    16,
    8,
    32,
    0, // INVALID
    32,
    16,
    32,
    0, // INVALID
    64,
    32,
    64,
    0, // INVALID
};

uint8_t xTileSizeLookup[] =
{
    8,
    16,
    8,
    0, // INVALID
    16,
    32,
    8,
    0, // INVALID
    32,
    32,
    16,
    0, // INVALID
    64,
    64,
    32,
    0, // INVALID
};

void gbaDisplay::drawSprites()
{
    for(uint8_t sprID = 127; sprID != 0xFF; sprID--)
    {
        uint32_t sprOAMLocate = 0x07000000 + (sprID * 8);
        uint16_t yCoord, att0, xCoord, att1, tileNum, palNum;

        yCoord = io.readMem(sprOAMLocate, 0);
        att0 = io.readMem(sprOAMLocate + 1, 0);
        xCoord = io.readMem(sprOAMLocate + 2, 1) & 0x1FF;
        att1 = io.readMem(sprOAMLocate + 3, 0) & 0xFE;
        tileNum = io.readMem(sprOAMLocate + 4, 1) & 0x3FF;
        palNum = io.readMem(sprOAMLocate + 5, 0) >> 4;
        palNum &= 0xF;

        uint8_t objSize = att1 >> 6;
        objSize &= 0x3;
        //printf("objSize: 0x%X\n", objSize);
        uint8_t objShape = att0 >> 6;
        objShape &= 0x3;
        //printf("objShape: 0x%X\n", objShape);


        uint8_t objMode = att1 >> 4;
        objMode &= 0x3;

        uint8_t trueSize = (objSize & 0x3) << 2 | (objShape & 0x3);
        //printf("PAL1: 0x%X\n",palNum);

        //drawSpriteTile(tileNum, xCoord, yCoord, 0, palNum);
        //printf("trueSize: 0x%X\n", trueSize);
        for(int yTile = 0; yTile < yTileSizeLookup[trueSize] / 8; yTile++)
        {
            for(int xTile = 0; xTile < xTileSizeLookup[trueSize] / 8; xTile++)
            {
                //printf("RenderTile 0x%X\n",xTile);
                switch((((io.lcdControl >> 6) & 0x1) << 2) | objMode)
                {
                    case 0:
                        drawSpriteTile(tileNum + (xTile) + (yTile * 0x20),
                                       xCoord + (xTile * 8),
                                       yCoord + (yTile * 8),
                                       objMode,
                                       palNum);
                    break;


                    case 1:
                        drawSpriteTile(tileNum + (xTile) + (yTile * 0x20),
                                       ((xTileSizeLookup[trueSize] + xCoord) - 8) - (xTile * 8),
                                       yCoord + (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                    case 2:
                        drawSpriteTile(tileNum + (xTile) + (yTile * 0x20),
                                       xCoord + (xTile * 8),
                                       ((yTileSizeLookup[trueSize] + yCoord) - 8) - (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                    case 3:
                        drawSpriteTile(tileNum + (xTile) + (yTile * 0x20),
                                       ((xTileSizeLookup[trueSize] + xCoord) - 8) - (xTile * 8),
                                       ((yTileSizeLookup[trueSize] + yCoord) - 8) - (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                    case 4:
                        drawSpriteTile(tileNum + (xTile) + (yTile * (xTileSizeLookup[trueSize] / 8)),
                                       xCoord + (xTile * 8),
                                       yCoord + (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                    case 5:
                        drawSpriteTile(tileNum + (xTile) + (yTile * (xTileSizeLookup[trueSize] / 8)),
                                       ((xTileSizeLookup[trueSize] + xCoord) - 8) - (xTile * 8),
                                       yCoord + (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                    case 6:
                        drawSpriteTile(tileNum + (xTile) + (yTile * (xTileSizeLookup[trueSize] / 8)),
                                       xCoord + (xTile * 8),
                                       ((yTileSizeLookup[trueSize] + yCoord) - 8) - (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                    case 7:
                        drawSpriteTile(tileNum + (xTile) + (yTile * (xTileSizeLookup[trueSize] / 8)),
                                       ((xTileSizeLookup[trueSize] + xCoord) - 8) - (xTile * 8),
                                       ((yTileSizeLookup[trueSize] + yCoord) - 8) - (yTile * 8),
                                       objMode,
                                       palNum);
                    break;

                }
            }
        }

    }
}

void gbaDisplay::drawBGMode0(uint8_t bgNum)
{
    if(bgNum == 0xFF)
    {

        uint16_t color = ram.bg_ObjPal[0x1] << 8 | ram.bg_ObjPal[0];
        //printf("CLR\n");
        for(int y = 0; y < 160; y++)
        {
            for(int x = 0; x < 240; x++)
            {
                screenArray[y][x] = (0xFF << 24) |
                                    ((((color >> 10) & 0x1F) << 3) << 16) |
                                    ((((color >> 5) & 0x1F) << 3) << 8) |
                                    (((color & 0x1F) << 3));
            }
        }
        return;
    }
    if( ( ( io.lcdControl >> (8 + bgNum) ) & 0x1 ) == 0)
    {
        if(forceDrawBGs == false)
        {
            return;
        }

    }
    if(forceDrawBGs == true && forceDrawBg[bgNum] == false)
    {
        return;
    }

    uint8_t bg0priority = ((io.bgCNT[bgNum]) & 0x3);
    uint8_t bg0charBlock = ((io.bgCNT[bgNum] >> 2) & 0x3);
    bool bg0bpp = ((io.bgCNT[bgNum] >> 7) & 1);
    uint8_t bg0mapBlock = ((io.bgCNT[bgNum] >> 8) & 0x1F);

    uint32_t bg0mapLocation = 0x06000000 + (bg0mapBlock * 0x800);

    uint32_t bg0tileLocation = 0x06000000 + (bg0charBlock * 0x4000);

    uint8_t xMod, yMod;
    switch(io.bgCNT[bgNum] >> 14)
    {
        case 0:
            xMod = 0x20;
            yMod = 0x20;
        break;

        case 1:
            xMod = 0x40;
            yMod = 0x20;
        break;

        case 2:
            xMod = 0x20;
            yMod = 0x40;
        break;

        case 3:
            xMod = 0x40;
            yMod = 0x40;
        break;
    }
    //printf("bg0tileLocation: 0x%X\n",bg0tileLocation);


    uint8_t fineX, fineY;
    fineX = io.bgXScroll[bgNum] & 0x7;
    fineY = io.bgYScroll[bgNum] & 0x7;

    for(int yTile = 0; yTile < 0x15; yTile++)
    {
        //printf("yTile: 0x%X\n",yTile);
        for(int xTile = 0; xTile < 0x1F; xTile++)
        {
            //printf("xTile: 0x%X\n",xTile);
            uint32_t xAmount = (xTile * 2) + ((((io.bgXScroll[bgNum] / 8)) * 2));
            xAmount = xTile + ((io.bgXScroll[bgNum] / 8));
            xAmount %= 0x40;

            if(xAmount >= 0x20)
            {
                xAmount %= 0x20;
                switch(io.bgCNT[bgNum] >> 14)
                {
                    case 1:
                        xAmount += 0x400;
                    break;

                    case 3:
                        xAmount += 0x400;
                    break;
                }
            }

            xAmount *= 2;

            uint32_t yAmount = (yTile * 0x40) + (((io.bgYScroll[bgNum] / 8) * 0x40));
            yAmount = yTile + (io.bgYScroll[bgNum] / 8);
            yAmount %= 0x40;
            if(yAmount >= 0x20)
            {
                yAmount %= 0x20;
                switch(io.bgCNT[bgNum] >> 14)
                {
                    case 2:
                        yAmount += 0x800;
                    break;

                    case 3:
                        yAmount += 0x800;
                    break;
                }
            }
            yAmount *= 0x40;
            //xAmount %= (xMod * 4);
            //yAmount %= (yMod * 0x80);
            uint32_t curTileLocation = bg0mapLocation + xAmount + yAmount;
            //(yTile * 0x400) + (xTile * 0x40) + (y * 8) + (x * 1);
            uint16_t tileData = io.readMem(curTileLocation, 1);
            uint16_t prevTileData = tileData;
            //curTileLocation &= 0x17FFE;


            //tileData = ram.video[curTileLocation + 1] << 8 | ram.video[curTileLocation];

            //if(prevTileData != tileData)
            //{
                //printf("curTileLocation: 0x%X  Correct: 0x%X  Broken: 0x%X  BGMAP: 0x%X\nSomeones being a little sussy...", curTileLocation, prevTileData, tileData, bg0mapBlock);

            //}
            //tileData = ((uint16_t *)(ram.video + curTileLocation ))[0];


            for(int y = 0; y != 8; y += 1)
            {
                //printf("y: 0x%X\n",y);
                for(int x = 0; x != 8; x += 1)
                {
                    //printf("x: 0x%X\n",x);

                    uint32_t curTileDataLocation = bg0tileLocation + ((tileData & 0x3FF) * 0x20) + ((y) * 4) + ((x) / 2);
                    uint8_t palNum = (tileData >> 12) & 0xF;

                    uint8_t tileDataTrue = io.readMem(curTileDataLocation, 0);

                    uint32_t location = 0x05000000 + ((tileDataTrue & 0xF) * 2) + ((tileDataTrue >> 4) * 0x20);

                    uint8_t colorIndex;

                    switch(x % 2)
                    {
                        case 0:
                            location = 0x05000000 + ((tileDataTrue & 0xF) * 2);
                            colorIndex = ((tileDataTrue & 0xF) * 2);
                        break;

                        case 1:
                            location = 0x05000000 + (((tileDataTrue >> 4) & 0xF) * 2);
                            colorIndex = (((tileDataTrue >> 4) & 0xF) * 2);
                        break;
                    }

                    uint16_t color = io.readMem(location + (palNum * 0x20), 1);
                    //printf("XWRITE: %i\n",x + (xTile * 8));
                    //printf("YWRITE: %i\n",y + (yTile * 8));
                    int finalX = x;
                    int finalY = y;
                    if(colorIndex != 0)
                    {
                        switch((tileData >> 10) & 0x3)
                        {
                            case 0:
                                if((finalX + (xTile * 8)) - fineX < 0 || (finalX + (xTile * 8)) - fineX >= 240)
                                {
                                    break;
                                }
                                if(((finalY) + (yTile * 8)) - fineY < 0 || ((finalY) + (yTile * 8)) - fineY >= 160)
                                {
                                    break;
                                }
                                screenArray[(finalY) + (yTile * 8) - fineY][finalX + (xTile * 8) - fineX] = (0xFF << 24) |
                                       ((((color >> 10) & 0x1F) << 3) << 16) |
                                       ((((color >> 5) & 0x1F) << 3) << 8) |
                                       (((color & 0x1F) << 3));
                            break;

                            case 1:
                                if(((7 - finalX) + (xTile * 8)) - fineX < 0 || ((7 - finalX) + (xTile * 8)) - fineX >= 240)
                                {
                                    break;
                                }
                                if(((finalY) + (yTile * 8)) - fineY < 0 || ((finalY) + (yTile * 8)) - fineY >= 160)
                                {
                                    break;
                                }
                                screenArray[(finalY) + (yTile * 8) - fineY][(7 - finalX) + (xTile * 8) - fineX] = (0xFF << 24) |
                                       ((((color >> 10) & 0x1F) << 3) << 16) |
                                       ((((color >> 5) & 0x1F) << 3) << 8) |
                                       (((color & 0x1F) << 3));
                            break;

                            case 2:
                                if((finalX + (xTile * 8)) - fineX < 0 || (finalX + (xTile * 8)) - fineX >= 240)
                                {
                                    break;
                                }
                                if(((7 - finalY) + (yTile * 8)) - fineY < 0 || ((7 - finalY) + (yTile * 8)) - fineY >= 160)
                                {
                                    break;
                                }
                                screenArray[(7 - finalY) + (yTile * 8) - fineY][finalX + (xTile * 8) - fineX] = (0xFF << 24) |
                                       ((((color >> 10) & 0x1F) << 3) << 16) |
                                       ((((color >> 5) & 0x1F) << 3) << 8) |
                                       (((color & 0x1F) << 3));
                            break;

                            case 3:
                                if(((7 - finalX) + (xTile * 8)) - fineX < 0 || ((7 - finalX) + (xTile * 8)) - fineX >= 240)
                                {
                                    break;
                                }
                                if(((7 - finalY) + (yTile * 8)) - fineY < 0 || ((7 - finalY) + (yTile * 8)) - fineY >= 160)
                                {
                                    break;
                                }
                                screenArray[(7 - finalY) + (yTile * 8) - fineY][(7 - finalX) + (xTile * 8) - fineX] = (0xFF << 24) |
                                       ((((color >> 10) & 0x1F) << 3) << 16) |
                                       ((((color >> 5) & 0x1F) << 3) << 8) |
                                       (((color & 0x1F) << 3));
                            break;
                        }
                    }

                    //printf("SUCCESSFULLY WROTE\n");
                }
                //printf("Exit X Loop\n");
            }
            //printf("Exit Y Loop\n");
        }
        //printf("Exit XTILE Loop\n");
        //printf("YTILE: 0x%X\n",yTile);
    }
}

void gbaDisplay::drawScreenMode0()
{
    drawBGMode0(0xFF);
    drawBGMode0(3);
    drawBGMode0(2);
    drawBGMode0(1);
    drawBGMode0(0);
    drawSprites();
    //printf("BG: 0x%X\n", io.bgXScroll[2] / 8);
    //printf("Successful Exit\n");
}

void gbaDisplay::drawScreenMode4()
{
    for(int y = 0; y < 160; y++)
    {
        for(int x = 0; x < 240; x++)
        {
            /*
                Get screen color of current pixel
                Decode screen color
                Write it
            */

            uint32_t location = 0x06000000 + (x) + (y * 240);

            uint8_t colorLocate = io.readMem(location, 0);

            location = 0x05000000 + (colorLocate * 2);
            uint16_t color = io.readMem(location, 1);
            screenArray[y][x] = (0xFF << 24) |
                               ((((color >> 10) & 0x1F) << 3) << 16) |
                               ((((color >> 5) & 0x1F) << 3) << 8) |
                               (((color & 0x1F) << 3));
        }
    }
}
