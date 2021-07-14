#include "file.hpp"
#include "cpu.hpp"
#include "io.hpp"
#include <stdint.h>

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
    fread(ram.bios,rSize,1,bios);
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
    fread(ram.rom,rSize,1,rom);
    fclose(rom);
    return true;
}
