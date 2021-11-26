#include <iostream>
#include "file.hpp"
#include "cpu.hpp"
#include "ui.hpp"
#include "io.hpp"
#include "input.hpp"

using namespace std;

time_t seconds2;

uint64_t secondsPassed2 = 0;

int main()
{

    /*
    bool readMemBench = false;
    uint32_t readMemLoc = 0;
    uint64_t readMemAmount = 0;

    while(!readMemBench)
    {
        io.readMem(readMemLoc, 2);
        readMemLoc++;
        readMemAmount++;
        if(readMemAmount == 0xFFFFFFFFFFFFFFFF)
        {
            printf("Overflow!\n");
        }
        if(seconds2 != time(NULL))
        {
            secondsPassed2++;
            if(secondsPassed2 == 5)
            {
                readMemBench = true;
            }
        }
        seconds2 = time(NULL);
    }

    printf("Readmems Sucessfully Ran: %i\n", readMemAmount);

    return 0;
    */
    /*
    if(!fileLoad.loadBIOS())
    {
        cout<<"BIOS COULD NOT BE LOADED!"<<endl;
        return 1;
    }
    if(!fileLoad.loadROM("rom/yoshi.gba"))
    {
        cout<<"ROM COULD NOT BE LOADED!"<<endl;
        return 1;
    }
    */
    cpu.generateThumbLookup();
    cpu.opcodesRan = 0;
    ui.initUI();
    ui.startEmulation = false;
    while(ui.startEmulation == false)
    {
        ui.handleUI();
        input.handleInput();
    }

    //printf("TEST: 0x%X\n", io.readMem(0x080000C0, 2));
    //return 0;
    //cpu.doOpcode();
    //printf("0x%X\n",cpu.R[15]);
    //return 0;

    io.KEYINPUT = 0xFFFF;
    //ui.pauseEmulation = true;
    //ui.runNoStep = false;
    ui.pauseEmulation = true;
    while(!ui.endEmulator) // Main EMU Loop
    {
        cpu.doOpcode();//080010cc
        while(ui.pauseEmulation == true)
        {
            ui.handleUI();
            input.handleInput();
        }
        switch(cpu.cpsr.T)
        {
            case 0:
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            break;

            case 1:
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFE;
            break;
        }
        cpu.opcodesRan++;
        if(cpu.opcodesRan % (opPerFrame / 260) == 0)
        {
            cpu.doInterrupts();
        }
        switch(cpu.cpsr.T)
        {
            case 0:
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            break;

            case 1:
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFE;
            break;
        }
        if(cpu.opcodesRan % opPerFrame == 0)
        {
            SDL_PauseAudio(0);
            if(((io.dispStat) & 0x8) != 0)
            {
                io.IF = io.IF | 0x1;
            }
            ui.handleUI();
            input.handleInput();
        }
        if(ui.runNoStep == false)
        {
            ui.pauseEmulation = true; // Disable this to remove stepping
        }
        if(ui.useRunTo == true)
        {
            if((cpu.R[15]) != ui.runToLocation)
            {
                ui.pauseEmulation = false;
            }
            if((cpu.R[15]) == ui.runToLocation && ui.runToLocation != 0)
            {
                ui.pauseEmulation = true;
                ui.useRunTo = false;
            }
        }
        //printf("CPSR: \nN: 0x%X\nZ: 0x%X\nC: 0x%X\n",cpu.cpsr.N,cpu.cpsr.Z,cpu.cpsr.C);
        //printf("R15: 0x%X\n",cpu.R[15]);
    }
}
