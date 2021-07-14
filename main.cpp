#include <iostream>
#include "file.hpp"
#include "cpu.hpp"

using namespace std;

void emuInit()
{
    cpu.opcodesRan = 0;
    for(int i = 0; i < 13; i++)
    {
        cpu.R[i] = 0;
    }

    cpu.R[13] = 0x03007F00;
    cpu.R[14] = 0x08000000;
    cpu.R1314_svc[0] = 0x03007FE0;
    cpu.R1314_svc[1] = 0x08000000;
    cpu.R1314_irq[0] = 0x03007F00;
    cpu.R1314_irq[1] = 0x08000000;
    cpu.R[15] = cpu.R[14];
    cpu.cpsr.mode = 0x1F;
    cpu.spsr.mode = 0x10;
    //gbaREG.IME = false;
    //gbaREG.IE = 0;
    //gbaREG.IF = 0;
}

int main()
{
    if(!fileLoad.loadBIOS())
    {
        cout<<"BIOS COULD NOT BE LOADED!"<<endl;
        return 1;
    }
    if(!fileLoad.loadROM("rom/armfix.gba"))
    {
        cout<<"ROM COULD NOT BE LOADED!"<<endl;
        return 1;
    }
    cpu.R[15] = 0x08000000;
    //cpu.doOpcode();
    //printf("0x%X\n",cpu.R[15]);
    //return 0;

    while(true) // Main EMU Loop
    {
        cpu.doOpcode();
    }
}
