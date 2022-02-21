#include "cpu.hpp"
#include "io.hpp"
#include "ui.hpp"
#include "audio.hpp"
#include <iostream>


gbaCPU cpu;



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

cpsrS gbaCPU::writeCPSR(cpsrS value, uint32_t value2, bool doModeSwitch)
{
    value.N = ((value2 >> 31) & 0x1);
    value.Z = ((value2 >> 30) & 0x1);
    value.C = ((value2 >> 29) & 0x1);
    value.V = ((value2 >> 28) & 0x1);
    value.Q = ((value2 >> 27) & 0x1);
    value.I = ((value2 >> 7) & 0x1);
    value.F = ((value2 >> 6) & 0x1);
    value.T = ((value2 >> 5) & 0x1);
    if(doModeSwitch == true)
    {
        modeSwitch((value2 & 0x1F));
    }
    value.mode = (value2 & 0x1F);
    return value;
}


uint8_t gbaCPU::modeSwitch(uint8_t switchMode)
{
    //printf("Mode Switch 0x%X\n",switchMode);
    if(cpu.cpsr.mode == switchMode)
    {
        return 0;
    }
    if(cpu.cpsr.mode == 0x1F && switchMode == 0x10) // Swap from System to User
    {
        return 0;
    }
    if(cpu.cpsr.mode == 0x10 && switchMode == 0x1F) // Swap from User to System
    {
        return 0;
    }
    uint32_t swapVal;
    cpsrS swapCPSR;
    switch(switchMode)
    {
        case 0x10:
            switch(cpu.cpsr.mode)
            {
                case 0x12: // IRQ
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_irq[0];
                    cpu.R1314_irq[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_irq[1];
                    cpu.R1314_irq[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_irq;
                    cpu.spsr_irq = swapCPSR;
                break;

                case 0x13: // Supervisor
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_svc[0];
                    cpu.R1314_svc[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_svc[1];
                    cpu.R1314_svc[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_svc;
                    cpu.spsr_svc = swapCPSR;
                break;

                default:
                    printf("Unknown Current Mode 0x%X!\n",cpu.cpsr.mode);
                break;
            }
        break;

        case 0x12:
            switch(cpu.cpsr.mode)
            {
                case 0x10: // User
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_irq[0];
                    cpu.R1314_irq[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_irq[1];
                    cpu.R1314_irq[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_irq;
                    cpu.spsr_irq = swapCPSR;
                break;

                case 0x13: // Supervisor
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_irq[0];
                    cpu.R1314_irq[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_irq[1];
                    cpu.R1314_irq[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_irq;
                    cpu.spsr_irq = swapCPSR;
                break;

                case 0x1F: // System
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_irq[0];
                    cpu.R1314_irq[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_irq[1];
                    cpu.R1314_irq[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_irq;
                    cpu.spsr_irq = swapCPSR;
                break;

                default:
                    printf("Unknown Current Mode 0x%X!\n",cpu.cpsr.mode);
                break;
            }
        break;

        case 0x13:
            switch(cpu.cpsr.mode)
            {
                case 0x10: // User COULD CAUSE ISSUES?
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_svc[0];
                    cpu.R1314_svc[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_svc[1];
                    cpu.R1314_svc[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_svc;
                    cpu.spsr_svc = swapCPSR;
                break;

                case 0x12: // IRQ
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_svc[0];
                    cpu.R1314_svc[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_svc[1];
                    cpu.R1314_svc[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_svc;
                    cpu.spsr_svc = swapCPSR;
                break;

                case 0x1F: // System
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_svc[0];
                    cpu.R1314_svc[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_svc[1];
                    cpu.R1314_svc[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_svc;
                    cpu.spsr_svc = swapCPSR;
                break;

                default:
                    printf("Unknown Current Mode 0x%X!\n",cpu.cpsr.mode);
                break;
            }
        break;

        case 0x1F:
            switch(cpu.cpsr.mode)
            {
                case 0x12: // IRQ
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_irq[0];
                    cpu.R1314_irq[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_irq[1];
                    cpu.R1314_irq[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_irq;
                    cpu.spsr_irq = swapCPSR;
                break;

                case 0x13: // Supervisor
                    swapVal = cpu.R[13];
                    cpu.R[13] = cpu.R1314_svc[0];
                    cpu.R1314_svc[0] = swapVal;

                    swapVal = cpu.R[14];
                    cpu.R[14] = cpu.R1314_svc[1];
                    cpu.R1314_svc[1] = swapVal;

                    swapCPSR = cpu.spsr;
                    cpu.spsr = cpu.spsr_svc;
                    cpu.spsr_svc = swapCPSR;
                break;

                default:
                    printf("Unknown Current Mode 0x%X!\n",cpu.cpsr.mode);
                break;
            }
        break;

        default:
            printf("Unknown Mode Switch 0x%X!\n",switchMode);
        break;
    }
    cpu.cpsr.mode = switchMode;
    return 0;
}

uint16_t intToHandle;

void gbaCPU::doInterrupts()
{
    if(io.IME == true && cpu.cpsr.I == 0)
    {
        intToHandle = io.IE & io.IF;
        //if((intToHandle & 0x1) == 1 && (((io.dispStat) & 0x8) == 0))
        //{
            //intToHandle &= 0xFFFE;
        //}
        //if((intToHandle & 0x4) == 4 && (((io.dispStat) & 0x20) == 0))
        //{
            //intToHandle &= 0xFFFB;
        //}

        if(intToHandle != 0x0)
        {
            cpu.R1314_irq[1] = cpu.R[15] + 4;
            cpu.spsr_irq = cpu.cpsr;
            cpu.R[15] = 0x18;
            cpu.modeSwitch(0x12);
            cpu.cpsr.I = 1;
            cpu.cpsr.T = 0;
            //cpu.cpsr.mode = 0x12;

            //printf("PC: 0x%X\n",cpu.R[15]);
            //ui.pauseEmulation = true;
            //ui.runNoStep = false;
            //ui.useRunTo = false;
        }
    }
}

void gbaCPU::doAudioDMATransfer(uint32_t startAddrDMA, uint32_t endAddrDMA, uint32_t wordCountDMA, uint32_t dmaControl)
{
    bool dmaWriteMode = ((dmaControl & (0x1 << 10)) != 0);
    //printf("dmaWRITEMODE: 0x%X\n",dmaWriteMode);
    for(int i = 3; i != -1; i--)
    {
        //io.writeMem(endAddrDMA,2,io.readMem(startAddrDMA,2));
        //audio.samples[i] =
        //io.oldReadMem(startAddrDMA,2);
        uint8_t addrControl = ((dmaControl & (0x1 << 6)) != 0) << 1 | ((dmaControl & (0x1 << 5)) != 0);
        addrControl = ((dmaControl & (0x1 << 8)) != 0) << 1 | ((dmaControl & (0x1 << 7)) != 0);
        switch(addrControl)
        {
            case 0:
                //startAddrDMA += 4;
            break;

            case 1:
                //startAddrDMA -= 4;
            break;

            case 2:
                // Don't change end address
            break;

            default:
                printf("DMA WARNING 3!\n");
            break;
        }
    }
   // printf("DMA FINISH!\n");
}

void doDMATransfer1(uint32_t startAddrDMA, uint32_t endAddrDMA, uint32_t wordCountDMA, uint32_t dmaControl)
{

    bool dmaWriteMode = ((dmaControl & (0x1 << 10)) != 0);
    //printf("DMALOC: 0x%X\n",startAddrDMA);

    wordCountDMA = 4;
    audio.streamCounter = 0;

    for(int i = 0; i < 4; i++)
    {
        for(int x = 0; x < 4; x++)
        {
            audio.samples[((i) * 4) + x] = io.readMem(startAddrDMA + (i * 4) + x, 0);
            //audio.samples[((i) * 4) + x] = audio.megalovaniaTest[audio.megaloTestCount];
            //audio.megaloTestCount++;
        }
    }

    //printf("Before: 0x%X\n", io.lastStartAddr[1]);
    startAddrDMA += 16;
    io.lastStartAddr[1] = startAddrDMA;
    //printf("After: 0x%X\n", io.lastStartAddr[1]);

    /*
    while(wordCountDMA != 0)
    {
        for(int i = 0; i < 4; i++)
        {
            audio.samples[((4 - wordCountDMA) * 4) + (i)] = io.readMem(startAddrDMA + i, 0);
        }
        //io.writeMem(0x040000A0,2,io.readMem(startAddrDMA,2));
        wordCountDMA--;
        uint8_t addrControl = ((dmaControl & (0x1 << 6)) != 0) << 1 | ((dmaControl & (0x1 << 5)) != 0);
        addrControl = ((dmaControl & (0x1 << 8)) != 0) << 1 | ((dmaControl & (0x1 << 7)) != 0);
        switch(addrControl)
        {
            case 0:
                startAddrDMA += 4;
            break;

            case 1:
                startAddrDMA -= 4;
            break;

            case 2:
                // Don't change end address
            break;

            default:
                printf("DMA WARNING 3!\n");
            break;
        }
    }
    */
    //if(io.dmaControl[1] & 0x200 != 0)
    //{
    //}
   // printf("DMA FINISH!\n");
}

void doDMATransfer(uint32_t startAddrDMA, uint32_t endAddrDMA, uint32_t wordCountDMA, uint32_t dmaControl)
{

    bool dmaWriteMode = ((dmaControl & (0x1 << 10)) != 0);
    //printf("dmaWRITEMODE: 0x%X\n",dmaWriteMode);


    switch(dmaWriteMode)
    {
        case 0x0:  // 16-bit
            if(wordCountDMA == 0)
            {
                wordCountDMA = 0x4000;
            }
            while(wordCountDMA != 0)
            {
                io.writeMem(endAddrDMA,1,io.readMem(startAddrDMA,1));
                wordCountDMA--;
                uint8_t addrControl = ((dmaControl & (0x1 << 6)) != 0) << 1 | ((dmaControl & (0x1 << 5)) != 0);
                switch(addrControl)
                {
                    case 0:
                        endAddrDMA += 2;
                    break;

                    case 1:
                        endAddrDMA -= 2;
                    break;

                    case 2:
                        // Don't change end address
                    break;

                    default:
                        printf("DMA WARNING 3!\n");
                    break;
                }
                addrControl = ((dmaControl & (0x1 << 8)) != 0) << 1 | ((dmaControl & (0x1 << 7)) != 0);
                switch(addrControl)
                {
                    case 0:
                        startAddrDMA += 2;
                    break;

                    case 1:
                        startAddrDMA -= 2;
                    break;

                    case 2:
                        // Don't change end address
                    break;

                    default:
                        printf("DMA WARNING 3!\n");
                    break;
                }
            }
        break;

        case 0x1: // 32-bit
            if(wordCountDMA == 0)
            {
                wordCountDMA = 0x10000;
            }
            while(wordCountDMA != 0)
            {
                io.writeMem(endAddrDMA,2,io.readMem(startAddrDMA,2));
                wordCountDMA--;
                uint8_t addrControl = ((dmaControl & (0x1 << 6)) != 0) << 1 | ((dmaControl & (0x1 << 5)) != 0);
                switch(addrControl)
                {
                    case 0:
                        endAddrDMA += 4;
                    break;

                    case 1:
                        endAddrDMA -= 4;
                    break;

                    case 2:
                        // Don't change end address
                    break;

                    default:
                        printf("DMA WARNING 3!\n");
                    break;
                }
                addrControl = ((dmaControl & (0x1 << 8)) != 0) << 1 | ((dmaControl & (0x1 << 7)) != 0);
                switch(addrControl)
                {
                    case 0:
                        startAddrDMA += 4;
                    break;

                    case 1:
                        startAddrDMA -= 4;
                    break;

                    case 2:
                        // Don't change end address
                    break;

                    default:
                        printf("DMA WARNING 3!\n");
                    break;
                }
            }
        break;

        default:
            printf("this should not happen.  DMA ERROR!\n");
        break;
    }
    if(io.dmaControl[1] & 0x200 != 0)
    {
        io.lastStartAddr[1] = startAddrDMA;
    }
   // printf("DMA FINISH!\n");
}



void gbaCPU::handleDMA()
{
    if((io.dmaControl[0] & 0x8000) != 0)
    {
        //printf("DMA 0 WAS ENABLED!\n");
        //startAddrDMA = io.dmaStartAddress[0];
        //endAddrDMA = io.dmaEndAddress[0];
        //wordCountDMA = io.dmaWordCount[0];
        //printf("WORD: 0x%X\n",wordCountDMA);
        uint16_t dmaControl = io.dmaControl[0];
        if(((dmaControl & (0x1 << 13)) != 0) == 0 && ((dmaControl & (0x1 << 12)) != 0) == 0 && io.dmaWordCount[0] != 0)
        {
            // Immediately
            //printf("DMA 0 WAS ENABLED!\n");
            doDMATransfer(io.dmaStartAddress[0], io.dmaEndAddress[0], io.dmaWordCount[0], io.dmaControl[0]);
            io.dmaControl[0] = io.dmaControl[0] & 0x7FFF;
        }
        if(((dmaControl & (0x1 << 13)) != 0) != 0 || ((dmaControl & (0x1 << 12)) != 0) != 0)
        {
            printf("DMA 0 TIMING IS NOT IMPLEMENTED!\n");
        }
    }
    if((io.dmaControl[1] & 0x8000) != 0)
    {
        uint16_t dmaControl = io.dmaControl[1];
        if(((dmaControl & (0x1 << 13)) != 0) != 0 && ((dmaControl & (0x1 << 12)) != 0) != 0)
        {
            // Immediately
            //printf("DMA 1 WAS ENABLED!\n");

            doDMATransfer1(io.lastStartAddr[1], 0x040000A0, 4, io.dmaControl[1] | (0x1 << 10));
            if(io.dmaControl[1] & 0x200 == 0)
            {
                io.dmaControl[1] = io.dmaControl[1] & 0x7FFF;
            }
        }
    }
    if((io.dmaControl[2] & 0x8000) != 0)
    {
        /*
        //printf("DMA 3 WAS ENABLED!\n");
        startAddrDMA = gbaREG.dmaStartAddress[2];
        endAddrDMA = gbaREG.dmaEndAddress[2];
        wordCountDMA = gbaREG.dmaWordCount[2];
        //printf("WORD: 0x%X\n",wordCountDMA);
        dmaControl = gbaREG.dmaControl2.to_ulong();
        if(gbaREG.dmaControl2[13] == 0 && gbaREG.dmaControl2[12] == 0 && wordCountDMA != 0)
        {
            // Immediately
            printf("DMA 2 WAS ENABLED!\n");
            doDMATransfer();
            gbaREG.dmaControl2[15] = 0;
        }
        if(gbaREG.dmaControl2[13] != 0 || gbaREG.dmaControl2[12] != 0)
        {
            doDMATransfer();
            gbaREG.dmaControl2[15] = 0;
        }
        */
    }
    if((io.dmaControl[3] & 0x8000) != 0)
    {
        uint16_t dmaControl = io.dmaControl[3];
        if(((dmaControl & (0x1 << 13)) != 0) == 0 && ((dmaControl & (0x1 << 12)) != 0) == 0 && io.dmaWordCount[3] != 0)
        {
            // Immediately
            //printf("DMA 3 WAS ENABLED!\n");
            doDMATransfer(io.dmaStartAddress[3], io.dmaEndAddress[3], io.dmaWordCount[3], io.dmaControl[3]);
            io.dmaControl[3] = io.dmaControl[3] & 0x7FFF;
        }
        else if(((dmaControl & (0x1 << 13)) != 0) != 0 || ((dmaControl & (0x1 << 12)) != 0) != 0)
        {

            printf("DMA 3 TIMING IS NOT IMPLEMENTED!\n");
        }
    }
}

void gbaCPU::handleTimers()
{
    if(((io.TMCNT_H[0] | io.TMCNT_H[1] | io.TMCNT_H[2] | io.TMCNT_H[3]) & 0x80) == 0)
    {
        return;
    }
    // THIS FUNCTION MUST RUN ONCE PER OPCODE BECAUSE OF THIS.  MUST BE CHANGED WHEN ADDING PROPER TIMING
    for(int i = 0; i < 4; i++)
    {
        if((io.TMCNT_H[i] & 0x80) == 0)
        {
            continue;
        }
        cpu.timerScheduler[i]--;
        if(cpu.timerScheduler[i] <= 0)
        {
            cpu.timerScheduler[i] += (timerFreqLookup[io.TMCNT_H[i] & 0x3]);
            cpu.timerScheduler[i] *= (0x10000 - io.TMCNT_L[i]);

            //uint16_t prevTimer = io.TMCNT_C[i];
            //io.TMCNT_C[i]++;
            //if(prevTimer >= io.TMCNT_C[i])
            //{
                // Overflow
                //io.TMCNT_C[i] = io.TMCNT_L[i];
                if((io.TMCNT_H[i] & 0x40) == 0x40)
                {
                    //printf("OVERFLOW\n");
                    io.IF |= 0x1 << (3 + i);
                }
                if((io.SOUNDCNT_H & 0x400) == 0 && i == 1)
                {
                    //printf("what3: 0x%X\n", io.TMCNT_H[i]);
                    io.dmaControl[1] |= 0x8000;
                }
            //}
        }
    }
}

void gbaCPU::doOpcode()
{
    //runTHUMB(0);
    //return;
    //if(ui.experimentalSpeedup == true)
    //{
    //    if(cpu.R[15] == 0x358 && io.IF == 0)
    //    {
    //        return;
    //    }
    //}
    if(cpu.cpsr.T == modeArm)
    {
        cpu.decodeAndRunARM();
        return;
    }
    else
    {
        cpu.runTHUMB(io.readMem(cpu.R[15], 1));
        return;
    }
    /*
    switch(cpu.cpsr.T)
    {
        case modeArm:

        break;

        case modeThumb:

        break;
    }
    */
}
