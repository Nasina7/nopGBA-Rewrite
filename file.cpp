#include "file.hpp"
#include "cpu.hpp"
#include "io.hpp"
#include <stdint.h>
#include <cstring>
#include "save.hpp"

fileLoadClass fileLoad;

bool fileLoadClass::loadBIOS() // Function to Load BIOS
{
    bios = fopen("bios/bios.rom", "rb");
    // TODO: Ensure Proper Size for BIOS
    if(bios == NULL)
    {
        return false;
    }
    fseek (bios, 0, SEEK_END);
    uint64_t rSize = ftell(bios);
    rewind(bios);
    size_t dummy = fread(ram.bios,rSize,1,bios);
    fclose(bios);
    return true;
}

bool fileLoadClass::loadROM(const char* filename)
{
    rom = fopen(filename,"rb");
    if(rom == NULL)
    {
        return false;
    }
    fseek (rom, 0, SEEK_END);
    uint64_t rSize = ftell(rom);
    rewind(rom);
    size_t dummy = fread(ram.rom,rSize,1,rom);
    fclose(rom);

    save.saveName = std::string((char*)ram.rom + 0xA0, 12);

    save.currentSaveType = 0;
    for(int i = 0; i < rSize; i += 4)
    {
        uint8_t tempBuffer[16];
        memcpy(tempBuffer, ram.rom + i, 16);
        std::string temp((char*)tempBuffer, 16);
        std::size_t pos = 0, old = 0;

        pos = temp.find("EEPROM_", 0);
        if(pos != std::string::npos)
        {
            // Save type is one of the EEPROM types
            save.currentSaveType = 1;
            //return true;
        }
        pos = temp.find("SRAM_", 0);
        if(pos != std::string::npos)
        {
            // Save type is one of the SRAM types
            save.currentSaveType = 2;
            //return true;
        }
        pos = temp.find("FLASH_", 0);
        if(pos != std::string::npos)
        {
            // Save type is one of the FLASH types
            save.currentSaveType = 3;
            //return true;
        }
        pos = temp.find("FLASH512_", 0);
        if(pos != std::string::npos)
        {
            // Save type is one of the FLASH types
            save.currentSaveType = 4;
            //return true;
        }
        pos = temp.find("FLASH1M_", 0);
        if(pos != std::string::npos)
        {
            // Save type is one of the FLASH types
            save.currentSaveType = 5;
            //return true;
        }
    }

    save.loadData();
    return true;
}
