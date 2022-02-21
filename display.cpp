#include "display.hpp"
#include "ui.hpp"
#include "io.hpp"

gbaDisplay screen;

void gbaDisplay::drawScreenScanline()
{
    switch(io.lcdControl & 0x7)
    {
        case 0:
            screen.drawScreenMode0Scanline();
        break;

        case 1:
            screen.drawScreenMode0Scanline();
        break;

        case 2:
            screen.drawScreenMode0Scanline();
        break;

        case 4:
            //screen.drawScreenMode4();
        break;

        default:

        break;
    }
    if(io.VCOUNT == 160)
    {
        memcpy(screen.screenArray, screen.drawingArray, sizeof(screen.drawingArray));
    }
}

void gbaDisplay::drawScreenMode0Scanline()
{
    drawBGMode0Scanline(0xFF);
    for(int i = 3; i != -1; i--)
    {
        for(int x = 3; x != -1; x--)
        {
            if((io.bgCNT[x] & 0x3) == i)
            {
                drawBGMode0Scanline(x);
            }
        }
    }
    //drawBGMode0(0xFF);
    //drawBGMode0(3);
    //drawBGMode0(2);
    //drawBGMode0(1);
    //drawBGMode0(0);

    // THIS WILL NEED CHANGED LATER
    if(io.VCOUNT == 160)
    {
        drawSprites();
    }

    //printf("BG: 0x%X\n", io.bgXScroll[2] / 8);
    //printf("Successful Exit\n");
}

void gbaDisplay::drawBGMode0Scanline(uint8_t bgNum)
{
    if(bgNum == 0xFF)
    {
        uint16_t color = ram.bg_ObjPal[0x1] << 8 | ram.bg_ObjPal[0];
        //printf("CLR\n");
        if(io.VCOUNT > 159)
        {
            return;
        }
        for(int x = 0; x < 240; x++)
        {
            drawingArray[io.VCOUNT][x] = (0xFF << 24) |
                                ((((color >> 10) & 0x1F) << 3) << 16) |
                                ((((color >> 5) & 0x1F) << 3) << 8) |
                                (((color & 0x1F) << 3));
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


    uint8_t fineX, fineY, coarseX, coarseY;
    fineX = io.bgXScroll[bgNum] & 0x7;
    fineY = io.bgYScroll[bgNum] & 0x7;
    coarseX = io.bgXScroll[bgNum] >> 3;
    coarseY = io.bgYScroll[bgNum] >> 3;












    for(int xTile = 0; xTile < 0x1F; xTile++)
    {
        //printf("xTile: 0x%X\n",xTile);
        uint32_t xAmount = (xTile << 1);
        xAmount = xTile;
        xAmount += coarseX;
        xAmount &= 0x3F;

        if(xAmount >= 0x20)
        {
            xAmount &= 0x1F;
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

        xAmount = xAmount << 1;

        uint32_t yAmount = ((io.VCOUNT / 8) * 0x40);
        yAmount = (io.VCOUNT / 8);
        yAmount += coarseY;
        if((io.VCOUNT % 8) + fineY >= 8)
        {
            yAmount += 1;
        }
        yAmount & 0x3F;
        if(yAmount >= 0x20)
        {
            yAmount &= 0x1F;
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
        // By this point, we have the tile index

        //xAmount %= (xMod * 4);
        //yAmount %= (yMod * 0x80);
        uint8_t trueFineY = (io.VCOUNT % 8);
        //trueFineY %= 8;
        uint32_t curTileLocation = bg0mapLocation + xAmount + yAmount;
       // printf("Chosen Tile: 0x%X\n", curTileLocation);
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


            for(int x = 0; x != 8; x += 1)
            {
                //printf("x: 0x%X\n",x);

                uint32_t curTileDataLocation;
                uint8_t palNum;

                uint8_t tileDataTrue;

                uint32_t location;

                uint8_t colorIndex;

                uint8_t trueY = (io.VCOUNT % 8) + fineY;
                trueY %= 8;

                if((io.bgCNT[bgNum] & 0x80) == 0x00)
                {
                    curTileDataLocation = bg0tileLocation + ((tileData & 0x3FF) << 5) + (((trueY)) << 2) + ((x) >> 1);
                    if((tileData >> 11) & 0x1 == 1) // Y flipping
                    {
                        curTileDataLocation = bg0tileLocation + ((tileData & 0x3FF) << 5) + ((7 - (trueY)) << 2) + ((x) >> 1);
                    }
                    palNum = (tileData >> 12) & 0xF;

                    tileDataTrue = io.readMem(curTileDataLocation, 0);

                    switch(x & 0x1)
                    {
                        case 0:
                            colorIndex = ((tileDataTrue & 0xF) << 1);
                        break;

                        case 1:
                            colorIndex = (((tileDataTrue >> 4) & 0xF) << 1);
                        break;
                    }
                    location = 0x05000000 | colorIndex;
                }
                else
                {
                    curTileDataLocation = bg0tileLocation + (((tileData * 2) & 0x3FF) * 0x20) + ((trueY) * 8) + ((x));
                    if((tileData >> 11) & 0x1 == 1) // Y flipping
                    {
                        curTileDataLocation = bg0tileLocation + (((tileData * 2) & 0x3FF) * 0x20) + ((7 - (trueY)) * 8) + ((x));
                    }
                    palNum = (tileData >> 12) & 0xF;

                    tileDataTrue = io.readMem(curTileDataLocation, 0);

                    location = 0x05000000 + ((tileDataTrue & 0xF) * 2) + ((tileDataTrue >> 4) * 0x20);

                    location = 0x05000000 + ((tileDataTrue & 0xFF) * 2);
                    colorIndex = ((tileDataTrue & 0xFF) * 2);
                }
                //printf("Chosen Data Location: 0x%X\n", curTileDataLocation);

                uint16_t colorRGB = io.readMem(location + (palNum * 0x20), 1);

                uint32_t color = (0xFF000000) |
                                   ((((colorRGB) & 0x7C00) << 9)) |
                                   ((((colorRGB) & 0x3E0) << 6)) |
                                   (((colorRGB & 0x1F) << 3));
                //printf("XWRITE: %i\n",x + (xTile * 8));
                //printf("YWRITE: %i\n",y + (yTile * 8));
                int finalX = x;
                int finalY = (io.VCOUNT);
                if(colorIndex != 0)
                {
                    switch((tileData >> 10) & 0x1)
                    {
                        case 0:
                        {
                            uint16_t xWrite = (finalX + (xTile * 8)) - fineX;
                            uint16_t yWrite = finalY;
                            if(xWrite >= 240)
                            {
                                break;
                            }
                            if(yWrite >= 160)
                            {
                                break;
                            }
                            drawingArray[yWrite][xWrite] = color;
                        }
                        break;

                        case 1:
                        {
                            uint16_t xWrite = ((7 - finalX) + (xTile * 8)) - fineX;
                            uint16_t yWrite = (finalY);
                            if(xWrite >= 240)
                            {
                                break;
                            }
                            if(yWrite >= 160)
                            {
                                break;
                            }
                            drawingArray[yWrite][xWrite] = color;
                        }
                        break;
                    }
                }

                //printf("SUCCESSFULLY WROTE\n");
            }
            //printf("Exit X Loop\n");
        //printf("Exit Y Loop\n");
    }

}

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

        case 3:
            screen.drawScreenMode3();
        break;

        case 4:
            screen.drawScreenMode4();
        break;

        case 5:
            screen.drawScreenMode4();
        break;

        default:
            screen.drawScreenMode0();
        break;
    }
    memcpy(screen.screenArray, screen.drawingArray, sizeof(screen.drawingArray));
    //printf("Successful Return\n");
}

void gbaDisplay::drawSpriteTile(uint16_t tileNum, uint16_t xLoc, uint8_t yLoc, uint8_t mode, uint8_t palNum)
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
            xLoc &= 0x1FF;
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
                drawingArray[yLoc][xLoc] = (0xFF << 24) |
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
        uint16_t att0, xCoord, att1, tileNum, palNum;
        uint8_t yCoord;

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
                drawingArray[y][x] = (0xFF << 24) |
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
            uint32_t xAmount = (xTile << 1) + ((io.bgXScroll[bgNum] >> 2));
            xAmount = xTile + ((io.bgXScroll[bgNum] >> 3));
            xAmount &= 0x3F;

            if(xAmount >= 0x20)
            {
                xAmount &= 0x1F;
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

            xAmount = xAmount << 1;

            uint32_t yAmount = (yTile * 0x40) + (((io.bgYScroll[bgNum] / 8) * 0x40));
            yAmount = yTile + (io.bgYScroll[bgNum] / 8);
            yAmount & 0x3F;
            if(yAmount >= 0x20)
            {
                yAmount &= 0x1F;
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

                    uint32_t curTileDataLocation;
                    uint8_t palNum;

                    uint8_t tileDataTrue;

                    uint32_t location;

                    uint8_t colorIndex;

                    if((io.bgCNT[bgNum] & 0x80) == 0x00)
                    {
                        curTileDataLocation = bg0tileLocation + ((tileData & 0x3FF) << 5) + ((y) << 2) + ((x) >> 1);
                        palNum = (tileData >> 12) & 0xF;

                        tileDataTrue = io.readMem(curTileDataLocation, 0);

                        //location = 0x05000000 + ((tileDataTrue & 0xF) << 2);

                        switch(x & 0x1)
                        {
                            case 0:
                                colorIndex = ((tileDataTrue & 0xF) << 1);
                            break;

                            case 1:
                                colorIndex = (((tileDataTrue >> 4) & 0xF) << 1);
                            break;
                        }
                        location = 0x05000000 | colorIndex;
                    }
                    else
                    {
                        curTileDataLocation = bg0tileLocation + (((tileData * 2) & 0x3FF) * 0x20) + ((y) * 8) + ((x));
                        palNum = (tileData >> 12) & 0xF;

                        tileDataTrue = io.readMem(curTileDataLocation, 0);

                        location = 0x05000000 + ((tileDataTrue & 0xF) * 2) + ((tileDataTrue >> 4) * 0x20);

                        location = 0x05000000 + ((tileDataTrue & 0xFF) * 2);
                        colorIndex = ((tileDataTrue & 0xFF) * 2);
                    }

                    uint16_t colorRGB = io.readMem(location + (palNum * 0x20), 1);

                    uint32_t color = (0xFF000000) |
                                       ((((colorRGB) & 0x7C00) << 9)) |
                                       ((((colorRGB) & 0x3E0) << 6)) |
                                       (((colorRGB & 0x1F) << 3));
                    //printf("XWRITE: %i\n",x + (xTile * 8));
                    //printf("YWRITE: %i\n",y + (yTile * 8));
                    int finalX = x;
                    int finalY = y;
                    if(colorIndex != 0)
                    {
                        switch((tileData >> 10) & 0x3)
                        {
                            case 0:
                            {
                                uint16_t xWrite = (finalX + (xTile * 8)) - fineX;
                                uint16_t yWrite = ((finalY) + (yTile * 8)) - fineY;
                                if(xWrite >= 240)
                                {
                                    break;
                                }
                                if(yWrite >= 160)
                                {
                                    break;
                                }
                                drawingArray[yWrite][xWrite] = color;
                            }
                            break;

                            case 1:
                            {
                                uint16_t xWrite = ((7 - finalX) + (xTile * 8)) - fineX;
                                uint16_t yWrite = ((finalY) + (yTile * 8)) - fineY;
                                if(xWrite >= 240)
                                {
                                    break;
                                }
                                if(yWrite >= 160)
                                {
                                    break;
                                }
                                drawingArray[yWrite][xWrite] =  color;
                            }
                            break;

                            case 2:
                            {
                                uint16_t xWrite = (finalX + (xTile * 8)) - fineX;
                                uint16_t yWrite = ((7 - finalY) + (yTile * 8)) - fineY;
                                if(xWrite >= 240)
                                {
                                    break;
                                }
                                if(yWrite >= 160)
                                {
                                    break;
                                }
                                drawingArray[yWrite][xWrite] =  color;
                            }
                            break;

                            case 3:
                            {
                                uint16_t xWrite = ((7 - finalX) + (xTile * 8)) - fineX;
                                uint16_t yWrite = ((7 - finalY) + (yTile * 8)) - fineY;
                                if(xWrite >= 240)
                                {
                                    break;
                                }
                                if(yWrite >= 160)
                                {
                                    break;
                                }
                                drawingArray[yWrite][xWrite] = color;
                            }
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
    for(int i = 3; i != -1; i--)
    {
        for(int x = 3; x != -1; x--)
        {
            if((io.bgCNT[x] & 0x3) == i)
            {
                drawBGMode0(x);
            }
        }
    }
    //drawBGMode0(0xFF);
    //drawBGMode0(3);
    //drawBGMode0(2);
    //drawBGMode0(1);
    //drawBGMode0(0);
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
            drawingArray[y][x] = (0xFF << 24) |
                               ((((color >> 10) & 0x1F) << 3) << 16) |
                               ((((color >> 5) & 0x1F) << 3) << 8) |
                               (((color & 0x1F) << 3));
        }
    }
}

void gbaDisplay::drawScreenMode3()
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

            uint32_t location = 0x06000000 + (x * 2) + (y * 480);

            uint8_t colorLocate = io.readMem(location, 1);

            //location = 0x05000000 + (colorLocate * 2);
            uint16_t color = colorLocate;
            drawingArray[y][x] = (0xFF << 24) |
                               ((((color & 0x1F)) << 3) << 16) |
                               ((((color >> 5) & 0x1F) << 3) << 8) |
                               ((((color >> 10) & 0x1F) << 3));
        }
    }
}
