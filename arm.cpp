#include "cpu.hpp"
#include "io.hpp"
#include <stdio.h>

#define condAlways 0xE

void gbaCPU::decodeAndRunARM()
{
    uint32_t opcode = io.get32bitOpcode(cpu.R[15]);
    printf("Opcode: 0x%X\n",opcode);

    uint8_t condition = (opcode >> 28);

    switch(condition)
    {
        case condAlways:
            cpu.runARM(opcode);
        break;

        default:
            printf("Unknown Condition 0x%X!\n",condition);
            cpu.runARM(0xFFFFFFFF);
        break;
    }

}

void gbaCPU::runARM(uint32_t opcode)
{
    switch((opcode & 0x0E000000) >> 25) // Get XXXXYYYXXXXXXXXX
    {
        case 0: // 000
            // TODO:  NEED TO OPTIMISE THIS, THIS IS VERY INNEFFICIENT
            // TODO:  FORMAT THIS BETTER

            if((((opcode & 0x01000000) >> 24)) != 0 &&
              (((opcode & 0x00800000) >> 23)) == 0 &&
              (((opcode & 0x00100000) >> 20)) == 0 &&
              (((opcode & 0x00000010) >> 4)) == 0
              )
            {// Miscellaneous
                //handleMiscInstructions();

                switch((opcode >> 4) & 0xF)
                {
                    case 0x0:
                        switch((opcode >> 21) & 0x1)
                        {
                            case 0:
                                cpu.MISC_MOV_SR_R(opcode);
                            break;

                            case 1:
                                cpu.MISC_MOV_R_SR(opcode);
                            break;
                        }
                    break;

                    case 0x1:
                        switch((opcode >> 22) & 0x1)
                        {
                            case 0:
                                //branchAndExchangeOp();
                                printf("BRANCH EXCHANGE NOT IMPLEMENTED!\n");
                                while(true)
                                {

                                }
                            break;

                            case 1:
                                printf("COUNT LEADING ZEROS NOT IMPLEMENTED!\n");
                                while(true)
                                {

                                }
                            break;
                        }
                    break;

                    default:
                        printf("UNIMPLEMENTED MISC INSTRUCTION 0x%X!\n",(opcode >> 4) & 0xF);
                        while(true)
                        {
                            // Temp
                        }
                    break;
                }



                break; // Break out of 000 case
            }
            if((((opcode & 0x01000000) >> 24)) != 0 &&
              (((opcode & 0x00800000) >> 23)) == 0 &&
              (((opcode & 0x00100000) >> 20)) == 0 &&
              (((opcode & 0x00000010) >> 4)) != 0 &&
              (((opcode & 0x00000080) >> 7)) == 0
              )
            {// Miscellaneous

                switch((opcode >> 4) & 0xF)
                {
                    case 0x0:
                        switch((opcode >> 21) & 0x1)
                        {
                            case 0:
                                cpu.MISC_MOV_SR_R(opcode);
                            break;

                            case 1:
                                cpu.MISC_MOV_R_SR(opcode);
                            break;
                        }
                    break;

                    case 0x1:
                        switch((opcode >> 22) & 0x1)
                        {
                            case 0:
                                //branchAndExchangeOp();
                                printf("BRANCH EXCHANGE NOT IMPLEMENTED!\n");
                                while(true)
                                {

                                }
                            break;

                            case 1:
                                printf("COUNT LEADING ZEROS NOT IMPLEMENTED!\n");
                                while(true)
                                {

                                }
                            break;
                        }
                    break;

                    default:
                        printf("UNIMPLEMENTED MISC INSTRUCTION 0x%X!\n",(opcode >> 4) & 0xF);
                        while(true)
                        {
                            // Temp
                        }
                    break;
                }



                break; // Break out of 000 case
            }
            if((((opcode & 0x00000010) >> 4)) != 0 &&
              (((opcode & 0x00000080) >> 7)) != 0
              )
            {// Multiplies Extra Load Stores
                //handleMultiplyandExtra();
                printf("000 ERROR!\n");
                while(true)
                {

                }
                break;
            }
            /*
            if((((opcode & 0x01000000) >> 24)) != 0 &&
              (((opcode & 0x00800000) >> 23)) == 0 &&
              (((opcode & 0x00100000) >> 20)) != 0
              )
            {
                //printf("DATA!\n");
                dataProcessingORpsrTransfer(currentOpcode);
                break;
            }
            if((((opcode & 0x01000000) >> 24)) == 0 &&
              (((opcode & 0x00800000) >> 23)) != 0
              )
            {
                //printf("DATA!\n");
                dataProcessingORpsrTransfer(currentOpcode);
                break;
            }
            */

            // DATA PROCESSING
            switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
            {
                default:
                    printf("000 Data Processing Immediate 0x%X not implemented!\n",((opcode >> 21) & 0xF));
                    while(1)
                    {
                        // Temporary
                    }
                break;
            }
            break;

            printf("000 ERROR!\n");
            while(true)
            {

            }
        break;

        case 1: // 001
            // Check for SR Transfer
            if(((opcode >> 23) & 0x3) == 0x2 && ((opcode >> 20) & 0x1) == 0x0) // SR Transfer
            {
                printf("Move Immediate to Status Register not implemented!\n");
                while(1)
                {
                    // Temporary
                }

            }
            switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
            {

                case 0xD: // MOV
                    cpu.DPI_MOV(opcode);
                break;

                default:
                    printf("Data Processing Immediate 0x%X not implemented!\n",((opcode >> 21) & 0xF));
                    while(1)
                    {
                        // Temporary
                    }
                break;
            }

        break;

        case 2: // 010    LOAD STORE IMMEDIATE
                 // Needs Immed, Immed Pre Indexed, Immed Post Indexed
            switch((opcode >> 24) & 0x1)
            {
                case 0: // Immed Post Indexed
                    switch((opcode >> 21) & 0x1)
                    {
                        case 0: // Immed Post Indexed
                            cpu.LDST_IMD_POST(opcode);
                        break;

                        case 1: // IDK what this decodes to, need to check later
                            printf("UNKNOWN IMMED COMBO!\n");
                            while(1)
                            {

                            }
                        break;
                    }
                break;

                case 1:
                    switch((opcode >> 21) & 0x1)
                    {
                        case 0: // Immed
                            cpu.LDST_IMD(opcode);
                        break;

                        case 1: // Immed Pre Indexed
                            cpu.LDST_IMD_PRE(opcode);
                        break;
                    }
                break;
            }
        break;

        case 5: // 101
            switch((opcode & 0xF0000000) >> 28)
            {
                case 0xF:
                    printf("Unimplemented Branch and Branch with Link and Change to Thumb!\n");
                    while(1)
                    {
                        // Temporary
                    }
                break;

                default: // Branch and Branch with Link
                    cpu.BL(opcode);
                break;
            }
        break;

        default:
            printf("Unimplemented opcode 0x%X\n",opcode);
            while(1)
            {
                // This is temporary
            }
        break;
    }
}

// BEGINNING OF ARM OPCODES

void gbaCPU::BL(uint32_t opcode) // Branch or Branch with Link
{
    int branchOffset = opcode & 0x00FFFFFF;
    branchOffset = ((branchOffset << 8) >> 6); // Sign Extend
    if((opcode & 0x01000000) != 0)
    {
        cpu.R[14] = cpu.R[15] + 4;
    }
    cpu.R[15] = ((cpu.R[15] + branchOffset) + 8);
}

void gbaCPU::DPI_MOV(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    shiftAmount *= 2;
    cpu.R[Rd] = rotr32(value,shiftAmount);

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
        cpu.cpsr.Z = (cpu.R[Rd] == 0);
    }
    if(shiftAmount != 0)
    {
        cpu.cpsr.C = ((cpu.R[Rd] & 0x80000000) != 0);
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::LDST_IMD(uint32_t opcode)
{
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint16_t offset = (opcode & 0x0FFF);
    bool U, B, L;
    U = ((opcode >> 23) & 0x1);
    B = ((opcode >> 22) & 0x1);
    L = ((opcode >> 20) & 0x1);
    uint32_t address;
    switch(U)
    {
        case 0:
            address = cpu.R[Rn] - offset;
        break;

        case 1:
            address = cpu.R[Rn] + offset;
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            io.writeMem(address,2,cpu.R[Rd]);
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            io.writeMem(address,0,cpu.R[Rd]);
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}
void gbaCPU::LDST_IMD_PRE(uint32_t opcode)
{
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint16_t offset = (opcode & 0x0FFF);
    bool U, B, L;
    U = ((opcode >> 23) & 0x1);
    B = ((opcode >> 22) & 0x1);
    L = ((opcode >> 20) & 0x1);
    uint32_t address;
    switch(U)
    {
        case 0:
            address = cpu.R[Rn] - offset;
            cpu.R[Rn] = address;
        break;

        case 1:
            address = cpu.R[Rn] + offset;
            cpu.R[Rn] = address;
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            io.writeMem(address,2,cpu.R[Rd]);
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            io.writeMem(address,0,cpu.R[Rd]);
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}
void gbaCPU::LDST_IMD_POST(uint32_t opcode)
{
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint16_t offset = (opcode & 0x0FFF);
    bool U, B, L;
    U = ((opcode >> 23) & 0x1);
    B = ((opcode >> 22) & 0x1);
    L = ((opcode >> 20) & 0x1);
    uint32_t address = cpu.R[Rn];
    switch(U)
    {
        case 0:
            //address = cpu.R[Rn] - offset;
            cpu.R[Rn] = address - offset;
        break;

        case 1:
            //address = cpu.R[Rn] + offset;
            cpu.R[Rn] = address + offset;
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            io.writeMem(address,2,cpu.R[Rd]);
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            io.writeMem(address,0,cpu.R[Rd]);
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::MISC_MOV_SR_R(uint32_t opcode)
{
    uint8_t Rd = ((opcode >> 12) & 0xF);
    if(((opcode >> 22) & 0x1) != 0)
    {
        cpu.R[Rd] = readCPSR(cpu.spsr);
    }
    if(((opcode >> 22) & 0x1) == 0)
    {
        cpu.R[Rd] = readCPSR(cpu.cpsr);
    }
    cpu.R[15] += 4;
}

void gbaCPU::MISC_MOV_R_SR(uint32_t opcode)
{
    uint8_t Rm = (opcode & 0xF);
    bool f,s,x,c;
    f = ((opcode >> 19) & 0x1);
    s = ((opcode >> 18) & 0x1);
    x = ((opcode >> 17) & 0x1);
    c = ((opcode >> 16) & 0x1);

    if(((opcode >> 22) & 0x1) != 0)
    {
        uint32_t curCPSR = readCPSR(cpu.spsr);
        if(f == true)
        {
            curCPSR = curCPSR & 0x00FFFFFF;
            curCPSR = curCPSR | (cpu.R[Rm] & 0xFF000000);
        }
        if(s == true)
        {
            curCPSR = curCPSR & 0xFF00FFFF;
            curCPSR = curCPSR | (cpu.R[Rm] & 0x00FF0000);
        }
        if(x == true)
        {
            curCPSR = curCPSR & 0xFFFF00FF;
            curCPSR = curCPSR | (cpu.R[Rm] & 0x0000FF00);
        }
        if(c == true)
        {
            curCPSR = curCPSR & 0xFFFFFF00;
            curCPSR = curCPSR | (cpu.R[Rm] & 0x000000FF);
        }
        cpu.spsr = writeCPSR(cpu.spsr, curCPSR);
    }
    if(((opcode >> 22) & 0x1) == 0)
    {
        //cpu.R[Rm] = readCPSR(cpu.cpsr);
        uint32_t curCPSR = readCPSR(cpu.cpsr);
        if(f == true)
        {
            curCPSR = curCPSR & 0x00FFFFFF;
            curCPSR = curCPSR | (cpu.R[Rm] & 0xFF000000);
        }
        if(s == true)
        {
            curCPSR = curCPSR & 0xFF00FFFF;
            curCPSR = curCPSR | (cpu.R[Rm] & 0x00FF0000);
        }
        if(x == true)
        {
            curCPSR = curCPSR & 0xFFFF00FF;
            curCPSR = curCPSR | (cpu.R[Rm] & 0x0000FF00);
        }
        if(c == true)
        {
            curCPSR = curCPSR & 0xFFFFFF00;
            curCPSR = curCPSR | (cpu.R[Rm] & 0x000000FF);
        }
        cpu.cpsr = writeCPSR(cpu.cpsr, curCPSR);
    }
    cpu.R[15] += 4;
}
