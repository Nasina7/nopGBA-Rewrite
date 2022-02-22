#include "save.hpp"
#include <fstream>
#include <sstream>
#include <time.h>
#include "io.hpp"

gbaSave save;

void gbaSave::saveData()
{
    // Save every 5 seconds
    currentTime = time(NULL);
    if(lastSaveTime + 5 > currentTime)
    {
        return;
    }
    switch(currentSaveType)
    {
        case 0:
            return;
        break;

        case 2: // SRAM
            saveSRAM();
        break;

        case 3:
        case 4:
        case 5:
            saveFLASH();
        break;

        default:
            return;
        break;

    }
}

void gbaSave::saveSRAM()
{
    std::stringstream fileNameSetup;
    fileNameSetup << "save/" << saveName << ".sav";
    std::string fileNameSave = fileNameSetup.str();
    std::ofstream saveFile;
    saveFile.open(fileNameSave, std::ios::binary | std::ios::trunc);
    if(saveFile.is_open())
    {
        saveFile.write((char*)ram.save, sizeof(ram.save));
        saveFile.close();
    }

    lastSaveTime = time(NULL);
}

void gbaSave::saveFLASH()
{
    std::stringstream fileNameSetup;
    fileNameSetup << "save/" << saveName << ".sav";
    std::string fileNameSave = fileNameSetup.str();
    std::ofstream saveFile;
    saveFile.open(fileNameSave, std::ios::binary | std::ios::trunc);
    if(saveFile.is_open())
    {
        saveFile.write((char*)ram.save, sizeof(ram.save));
        if(currentSaveType == 5)
        {
            saveFile.write((char*)save.otherFlashPage, sizeof(save.otherFlashPage));
        }
        saveFile.close();
    }
    lastSaveTime = time(NULL);
}

void gbaSave::loadData()
{
    saveWasLoaded = false;
    switch(currentSaveType)
    {
        case 0:
            return;
        break;

        case 2: // SRAM
            loadSRAM();
        break;

        case 3:
        case 4:
        case 5:
            loadFLASH();
        break;

        default:
            return;
        break;

    }
}

void gbaSave::loadSRAM()
{
    std::stringstream fileNameSetup;
    fileNameSetup << "save/" << saveName << ".sav";
    std::string fileNameSave = fileNameSetup.str();
    std::ifstream saveFile;
    saveFile.open(fileNameSave, std::ios::binary | std::ios::in);
    if(saveFile.is_open())
    {
        saveFile.read((char*)ram.save, sizeof(ram.save));
        saveFile.close();
        saveWasLoaded = true;
    }
    else
    {
        saveWasLoaded = false;
    }
}

void gbaSave::loadFLASH()
{
    std::stringstream fileNameSetup;
    fileNameSetup << "save/" << saveName << ".sav";
    std::string fileNameSave = fileNameSetup.str();
    std::ifstream saveFile;
    saveFile.open(fileNameSave, std::ios::binary | std::ios::in);
    if(saveFile.is_open())
    {
        saveFile.read((char*)ram.save, sizeof(ram.save));
        if(currentSaveType == 5)
        {
            saveFile.read((char*)save.otherFlashPage, sizeof(save.otherFlashPage));
        }
        saveFile.close();
        saveWasLoaded = true;
    }
    else
    {
        saveWasLoaded = false;
    }
}

uint8_t gbaSave::accessSRAM(uint32_t location, bool write, uint8_t value)
{
    switch(save.currentSaveType)
    {
        case 0:
            if(write)
            {
                return 0xFF;
            }
            else
            {
                // Return OPEN BUS?
                return 0xFF;
            }
        break;

        case 2:
            if(write)
            {
                ram.save[location - 0x0E000000] = value;
            }
            else
            {
                // Return OPEN BUS?
                return ram.save[location - 0x0E000000];
            }
        break;

        case 3:
        case 4:
        case 5:
            if(write)
            {
                writeFLASH(location, value);
            }
            else
            {
                return readFLASH(location);
            }
        break;

        default:
            // Unimplemented Save Type
            if(write)
            {
                return 0xFF;
            }
            else
            {
                return 0xFF;
            }
        break;
    }
    return 0xFF;
}

void gbaSave::writeFLASH(uint32_t location, uint8_t value)
{
    /*
        Flash State List:
        0x80: Prepare for Erase

        0x90: Chip ID Mode

        0xA0: Allow Write Byte

        0xB0: Change Memory Page

        ...
        0xFF: Init
    */
    if(location == 0x0E005555)
    {
        if(value == 0xF0)
        {
            flashState = 0xFF;
        }
    }
    if(flashState != 0xA0)
    {
        if(location == 0x0E005555)
        {
            if(value == 0xAA && issueCommand == 0)
            {
                issueCommand = 0x1;
                return;
            }
            if(value == 0xF0)
            {
                flashState = 0xFF;
            }
            if(issueCommand == 0x2)
            {
                if(flashState == 0x90 || flashState == 0xFF)
                {
                    switch(value)
                    {
                        case 0x90: // Enter Chip Identification Mode
                            flashState = 0x90;
                            if(currentSaveType == 3 || currentSaveType == 4)
                            {
                                chipID = 0x1B32;
                            }
                            else if(currentSaveType == 5)
                            {
                                chipID = 0x1362;
                            }
                        break;

                        case 0xF0:
                            flashState = 0xFF;
                            chipID = 0xFFFF;
                        break;

                        case 0x80:
                            flashState = 0x80;
                        break;

                        case 0xA0:
                            flashState = 0xA0;
                        break;

                        case 0xB0:
                            flashState = 0xB0;
                        break;
                    }
                    issueCommand = 0;
                }
            }
            if(flashState == 0x80 && issueCommand == 2)
            {
                switch(value)
                {
                    case 0x10:
                        for(int i = 0; i < 0x20000; i++)
                        {
                            if(i < 0x10000)
                            {
                                ram.save[i] = 0xFF;
                            }
                            else
                            {
                                save.otherFlashPage[i & 0xFFFF] = 0xFF;
                            }
                        }
                        issueCommand = 0;
                    break;
                }
            }

        }
        else if(location == 0x0E002AAA)
        {
            if(issueCommand == 0x1 && value == 0x55)
            {
                issueCommand = 0x2;
                return;
            }
        }
        else if((location & 0xFFFF0FFF) == 0x0E000000 && flashState == 0x80 && issueCommand == 0x2)
        {
            if(value == 0x30)
            {
                for(int i = 0; i < 0x1000; i++)
                {
                    if(currentFlashPage == 0)
                    {
                        ram.save[(location & 0x0000F000) + i] = 0xFF;
                    }
                    else if(currentFlashPage == 1)
                    {
                        otherFlashPage[(location & 0x0000F000) + i] = 0xFF;
                    }
                }
                issueCommand = 0;
                flashState = 0xFF;
            }
        }
        else if(location == 0x0E000000 && flashState == 0xB0)
        {
            if(currentSaveType == 5)
            {
                currentFlashPage = (value & 0x1);
            }
            flashState = 0xFF;
            issueCommand = 0;
        }
    }
    else
    {
        if(currentFlashPage == 0)
        {
            ram.save[location & 0x0000FFFF] = value;
        }
        else
        {
            otherFlashPage[location & 0x0000FFFF] = value;
        }
        flashState = 0xFF;
        issueCommand = 0;
    }
}

uint8_t gbaSave::readFLASH(uint32_t location)
{
    if(chipID == 0xFFFF)
    {
        if(currentFlashPage == 0)
        {
            return ram.save[location & 0x0000FFFF];
        }
        else
        {
            return save.otherFlashPage[location & 0x0000FFFF];
        }
    }
    if(chipID != 0xFFFF && location > 0x0E000001)
    {
        if(currentFlashPage == 0)
        {
            return ram.save[location & 0x0000FFFF];
        }
        else
        {
            return save.otherFlashPage[location & 0x0000FFFF];
        }
    }
    if(chipID != 0xFFFF && location <= 0x0E000001)
    {
        return (chipID >> ((location & 0x1) * 8));
    }
}
