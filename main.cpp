#include <iostream>
#include "file.hpp"
#include "cpu.hpp"
#include "ui.hpp"
#include "io.hpp"
#include "input.hpp"
#include "audio.hpp"
#include "display.hpp"
#include <thread>

using namespace std;

time_t seconds2;

uint64_t secondsPassed2 = 0;

int main()
{

    //FILE* megalo = fopen("mega.raw", "rb");
    //fread(audio.megalovaniaTest, sizeof(uint8_t), sizeof(audio.megalovaniaTest), megalo);
    //fclose(megalo);
    ram.openBusEnable = true;
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
    //ui.initUI();
    ui.startEmulation = false;
    std::thread uiThread (&gbaUI::handleUI, gbaUI());
    while(cpu.R[15] != 0x08000000)
    {
        SDL_Delay(1);
        //ui.handleUI();
        //input.handleInput();
        ui.pauseEmulation = false;
        ui.runNoStep = true;
        //printf("cpu: 0x%X\n", cpu.R[15]);
    }

    //return 0;
    //cpu.doOpcode();
    //printf("0x%X\n",cpu.R[15]);
    //return 0;

    io.KEYINPUT = 0xFFFF;
    //ui.pauseEmulation = true;
    //ui.runNoStep = false;
    while(!ui.endEmulator) // Main EMU Loop
    {
        //io.writeMem(0x030022DC, 0, 1);
        cpu.doOpcode();//080010cc

        ram.save[0] = 0x62;
        ram.save[1] = 0x13;
        if(ui.disableTimers == false)
        {
            cpu.handleTimers();
        }
        //
        //cpu.handleDMA();
        while(ui.pauseEmulation == true)
        {
            SDL_Delay(1);
            if(ui.endEmulator)
            {
                uiThread.join();
            }
            SDL_Delay(1);
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
        if(cpu.opcodesRan % (opPerFrame / 228) == 0)
        {
            //screen.drawScreenScanline();
            io.VCOUNT++;
            io.IF |= 0x2;
            if(io.VCOUNT == 160)
            {
                //io.dispStat |= 0x1;
                //if(((io.dispStat) & 0x8) != 0)
                //{
                    io.IF = io.IF | 0x1;
                //}
            }
            if(io.VCOUNT == (io.dispStat >> 8))
            {
                //io.dispStat |= 0x4;
                if(((io.dispStat) & 0x20) != 0)
                {
                    io.IF = io.IF | 0x4;
                }
            }
        }
        if(io.VCOUNT == 228)
        {
            io.VCOUNT = 0;
            io.dispStat &= 0xFFF8;
            if(io.VCOUNT == (io.dispStat >> 8))
            {
                //io.dispStat |= 0x4;
                if(((io.dispStat) & 0x20) != 0)
                {
                    io.IF = io.IF | 0x4;
                }
            }
            ui.framesPerSecondEMU++;

            SDL_PauseAudio(0);
            //ui.handleUI();
            //input.handleInput();
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
        if((cpu.R[1]) == 0x06000421)
        {
            //ui.pauseEmulation = true;
            //ui.useRunTo = false;
        }
        //printf("CPSR: \nN: 0x%X\nZ: 0x%X\nC: 0x%X\n",cpu.cpsr.N,cpu.cpsr.Z,cpu.cpsr.C);
        //printf("R15: 0x%X\n",cpu.R[15]);
    }
    uiThread.join();
}
