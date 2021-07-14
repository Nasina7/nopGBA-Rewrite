#include "cpu.hpp"
#include "io.hpp"
#include <iostream>


gbaCPU cpu;

#define modeArm 0
#define modeThumb 1



uint32_t gbaCPU::readCPSR(cpsrS value)
{
    return
            value.N << 31 |
            value.Z << 30 |
            value.C << 29 |
            value.V << 28 |
            value.Q << 27 |
            value.I << 7 |
            value.F << 6 |
            value.T << 5 |
            (value.mode & 0x1F);
}

cpsrS gbaCPU::writeCPSR(cpsrS value, uint32_t value2)
{
    value.N = ((value2 >> 31) & 0x1);
    value.Z = ((value2 >> 30) & 0x1);
    value.C = ((value2 >> 29) & 0x1);
    value.V = ((value2 >> 28) & 0x1);
    value.Q = ((value2 >> 27) & 0x1);
    value.I = ((value2 >> 7) & 0x1);
    value.F = ((value2 >> 6) & 0x1);
    value.T = ((value2 >> 5) & 0x1);
    value.mode = (value2 & 0x1F);
    return value;
}



void gbaCPU::doOpcode()
{
    switch(cpu.cpsr.T)
    {
        case modeArm:
            decodeAndRunARM();
        break;

        case modeThumb:

        break;
    }
}
