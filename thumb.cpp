#include "cpu.hpp"
#include "io.hpp"
#include <stdio.h>

void gbaCPU::decodeAndRunTHUMB()
{
    uint16_t opcode = io.readMem(cpu.R[15]- 4, 1);
    printf("Opcode: 0x%X\n",opcode);
    cpu.runTHUMB(opcode);
}

void gbaCPU::runTHUMB(uint16_t opcode)
{
    switch((opcode >> 13) & 0x7)
    {
        case 0:
            switch((opcode >> 10) & 0x7)
            {
                case 0 ... 5: // 000-??X Shift by Immediate
                    switch((opcode >> 11) & 0x3)
                    {
                        case 0: // LSL
                            cpu.T_LSL_IMD(opcode);
                        break;

                        case 1: // LSR
                            cpu.T_LSR_IMD(opcode);
                        break;

                        case 2: // ASR
                            cpu.T_ASR_IMD(opcode);
                        break;

                        default:
                            printf("BAD_ERROR: Impossible Code Path!\n");
                            while(1)
                            {

                            }
                        break;
                    }
                break;

                case 6: // 000-110 Add Subtract Register
                    switch((opcode >> 9) & 0x1)
                    {
                        case 0: // Add
                            cpu.T_ADD_REG(opcode);
                        break;

                        case 1: // Sub
                            cpu.T_SUB_REG(opcode);
                        break;
                    }
                break;

                case 7: // 000-111 Add Subtract Immediate
                    printf("THUMB: Add Subtract Immediate not implemented!\n");
                    while(1)
                    {

                    }
                break;
            }
        break;

        case 1:
            switch((opcode >> 11) & 0x3)
            {
                case 0: // MOV
                    cpu.T_MOV_IMD(opcode);
                break;

                case 1: // CMP
                    cpu.T_CMP_IMD(opcode);
                break;

                case 2: // ADD
                    cpu.T_ADD_IMD(opcode);
                break;

                case 3: // SUB
                    cpu.T_SUB_IMD(opcode);
                break;
            }
        break;

        case 2: // 010
            switch((opcode >> 10) & 0x7)
            {
                case 0: // 010-000  Data Processing Register
                    switch((opcode >> 6) & 0xF)
                    {
                        case 0:
                            cpu.T_AND_ALU(opcode);
                        break;

                        case 1:
                            cpu.T_EOR_ALU(opcode);
                        break;

                        case 2:
                            cpu.T_LSL_ALU(opcode);
                        break;

                        case 3:
                            cpu.T_LSR_ALU(opcode);
                        break;

                        case 4:
                            cpu.T_ASR_ALU(opcode);
                        break;

                        case 5:
                            cpu.T_ADC_ALU(opcode);
                        break;

                        case 6:
                            cpu.T_SBC_ALU(opcode);
                        break;

                        case 7:
                            cpu.T_ROR_ALU(opcode);
                        break;

                        case 8:
                            cpu.T_TST_ALU(opcode);
                        break;

                        case 9:
                            cpu.T_NEG_ALU(opcode);
                        break;

                        case 0xA:
                            cpu.T_CMP_ALU(opcode);
                        break;

                        case 0xB:
                            cpu.T_CMN_ALU(opcode);
                        break;

                        case 0xC:
                            cpu.T_ORR_ALU(opcode);
                        break;

                        case 0xD:
                            cpu.T_MUL_ALU(opcode);
                        break;

                        case 0xE:
                            cpu.T_BIC_ALU(opcode);
                        break;

                        case 0xF:
                            cpu.T_MVN_ALU(opcode);
                        break;
                    }
                break;

                case 1: // 010-001  Special Data Processing
                    printf("THUMB: Special Data Processing not implemented!\n");
                    while(1)
                    {

                    }
                break;

                case 2 ... 3: // 010-01X  Load from Literal Pool
                    T_LD_LP(opcode);
                break;

                case 4 ... 7: // 010-1XX  Load Store Register Offset
                    printf("THUMB: Load Store from Register Offset not implemented!\n");
                    while(1)
                    {

                    }
                break;
            }
        break;

        case 6: // 110
            switch((opcode >> 8) & 0x1F)
            {
                case 0x0 ... 0xF: // 110-0XXXX Load Store Multiple
                    printf("THUMB: Load Store Multiple not implemented!\n");
                    while(1)
                    {

                    }
                break;

                case 0x10 ... 0x1D: // Conditional Branch
                    switch((opcode >> 8) & 0xF)
                    {
                        case 0:
                            cpu.T_B_EQ(opcode);
                        break;

                        case 1:
                            cpu.T_B_NE(opcode);
                        break;

                        case 2:
                            cpu.T_B_CS(opcode);
                        break;

                        case 3:
                            cpu.T_B_CC(opcode);
                        break;

                        case 4:
                            cpu.T_B_MI(opcode);
                        break;

                        case 5:
                            cpu.T_B_PL(opcode);
                        break;

                        case 6:
                            cpu.T_B_VS(opcode);
                        break;

                        case 7:
                            cpu.T_B_VC(opcode);
                        break;

                        case 8:
                            cpu.T_B_HI(opcode);
                        break;

                        case 9:
                            cpu.T_B_LS(opcode);
                        break;

                        case 0xA:
                            cpu.T_B_GE(opcode);
                        break;

                        case 0xB:
                            cpu.T_B_LT(opcode);
                        break;

                        case 0xC:
                            cpu.T_B_GT(opcode);
                        break;

                        case 0xD:
                            cpu.T_B_LE(opcode);
                        break;

                        default:
                            printf("BAD_ERROR: Impossible Code Path!\n");
                            while(1)
                            {

                            }
                        break;
                    }
                break;

                case 0x1E: // Undefined Instruction
                    printf("THUMB ERROR: Invalid opcode 0x%X!\n",opcode);
                    while(1)
                    {
                        // This is temporary
                    }
                break;

                case 0x1F: // Software Interrupt
                    printf("THUMB: Software Interrupt not implemented!\n");
                    while(1)
                    {

                    }
                break;
            }
        break;

        case 7:
            switch((opcode >> 11) & 3)
            {
                case 0: // Unconditional Branch
                    printf("THUMB: Unconditional Branch not Implemented! 0x%X\n",opcode);
                    while(1)
                    {
                        // This is temporary
                    }
                break;

                case 1: // BLX Suffix or Undefined Instruction
                    printf("THUMB: THIS SHOULD NOT HAPPEN! 0x%X\n",opcode);
                    while(1)
                    {
                        // This is temporary
                    }
                break;

                case 2: // BL Prefix
                    cpu.T_BL(opcode);
                break;

                case 3: // BL Suffix
                    printf("THUMB: THIS SHOULD NOT HAPPEN! 0x%X\n",opcode);
                    while(1)
                    {
                        // This is temporary
                    }
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

// BEGINNING OF THUMB OPCODES
void gbaCPU::T_LD_LP(uint16_t opcode)
{
    uint32_t location = cpu.R[15] & 0xFFFFFFFC;
    location += ((opcode & 0x00FF) * 4);
    uint8_t Rd = ((opcode >> 8) & 0x7);
    cpu.R[Rd] = io.readMem(location, 2);
    cpu.R[15] += 2;
}

void gbaCPU::T_LSL_IMD(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode) & 0x7);
    uint8_t shiftVal = ((opcode >> 6) & 0x1F);

    if(shiftVal == 0)
    {
        cpu.R[Rd] = cpu.R[Rm];
    }
    if(shiftVal != 0)
    {
        cpu.cpsr.C = ( ( ( cpu.R[Rm] >> (32 - shiftVal) ) & 0x1 ) != 0); // != 0 might be unneeded
        cpu.R[Rd] = (cpu.R[Rm] << shiftVal);
    }
    cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
    cpu.cpsr.Z = (cpu.R[Rd] == 0);
    cpu.R[15] += 2;
}

void gbaCPU::T_LSR_IMD(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode) & 0x7);
    uint8_t shiftVal = ((opcode >> 6) & 0x1F);

    if(shiftVal == 0)
    {
        cpu.R[Rd] = cpu.R[Rm];
        cpu.cpsr.C = ((cpu.R[Rd] & 0x80000000) != 0);
    }
    if(shiftVal != 0)
    {
        cpu.cpsr.C = ( ( ( cpu.R[Rd] >> (shiftVal - 1) ) & 0x1 ) != 0); // != 0 might be unneeded
        cpu.R[Rd] = (cpu.R[Rm] >> shiftVal);
    }
    cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
    cpu.cpsr.Z = (cpu.R[Rd] == 0);
    cpu.R[15] += 2;
}

void gbaCPU::T_ASR_IMD(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode) & 0x7);
    uint8_t shiftVal = ((opcode >> 6) & 0x1F);

    if(shiftVal == 0)
    {
        cpu.cpsr.C = ((cpu.R[Rd] & 0x80000000) != 0);
        if(cpu.cpsr.C == 0)
        {
            cpu.R[Rd] = 0;
        }
        if(cpu.cpsr.C == 1)
        {
            cpu.R[Rd] = 0xFFFFFFFF;
        }
    }
    if(shiftVal != 0)
    {
        cpu.cpsr.C = ( ( ( cpu.R[Rd] >> (shiftVal - 1) ) & 0x1 ) != 0); // != 0 might be unneeded
        cpu.R[Rd] = ((int32_t)cpu.R[Rm] >> shiftVal);
    }
    cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
    cpu.cpsr.Z = (cpu.R[Rd] == 0);
    cpu.R[15] += 2;
}

void gbaCPU::T_B_EQ(uint16_t opcode)
{
    if(cpu.cpsr.Z == 1)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.Z == 0)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_NE(uint16_t opcode)
{
    if(cpu.cpsr.Z == 0)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.Z == 1)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_CS(uint16_t opcode)
{
    if(cpu.cpsr.C == 1)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.C == 0)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_CC(uint16_t opcode)
{
    if(cpu.cpsr.C == 0)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.C == 1)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_MI(uint16_t opcode)
{
    if(cpu.cpsr.N == 1)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.N == 0)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_PL(uint16_t opcode)
{
    if(cpu.cpsr.N == 0)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.N == 1)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_VS(uint16_t opcode)
{
    if(cpu.cpsr.V == 1)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.V == 0)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_VC(uint16_t opcode)
{
    if(cpu.cpsr.V == 0)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 4;
    }
    if(cpu.cpsr.V == 1)
    {
        cpu.R[15] += 2;
    }
}
void gbaCPU::T_B_HI(uint16_t opcode)
{
    if(cpu.cpsr.C == 1 && cpu.cpsr.Z == 0)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 2;
    }
    cpu.R[15] += 2;
}
void gbaCPU::T_B_LS(uint16_t opcode)
{
    if(cpu.cpsr.C == 0 || cpu.cpsr.Z == 1)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 2;
    }
    cpu.R[15] += 2;
}
void gbaCPU::T_B_GE(uint16_t opcode)
{
    if(cpu.cpsr.N == cpu.cpsr.V)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 2;
    }
    cpu.R[15] += 2;
}
void gbaCPU::T_B_LT(uint16_t opcode)
{
    if(cpu.cpsr.N != cpu.cpsr.V)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 2;
    }
    cpu.R[15] += 2;
}
void gbaCPU::T_B_GT(uint16_t opcode)
{
    if(cpu.cpsr.N == cpu.cpsr.V && cpu.cpsr.Z == 0)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 2;
    }
    cpu.R[15] += 2;
}
void gbaCPU::T_B_LE(uint16_t opcode)
{
    if(cpu.cpsr.N != cpu.cpsr.V || cpu.cpsr.Z == 1)
    {
        int8_t jumpVal = (opcode & 0xFF);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += (jumpVal);
        cpu.R[15] += 2;
    }
    cpu.R[15] += 2;
}

void gbaCPU::T_CMP_IMD(uint16_t opcode)
{
    uint8_t Rn = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = cpu.R[Rn] - imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= (cpu.R[Rn] - imdVal));
    // MISSING V IMPLEMENTATION, SEE PG. 339
    cpu.R[15] += 2;
}
void gbaCPU::T_SUB_IMD(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = cpu.R[Rd] - imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= (cpu.R[Rd] - imdVal));
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
    cpu.R[15] += 2;
}
void gbaCPU::T_ADD_IMD(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = cpu.R[Rd] + imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF < (cpu.R[Rd] + imdVal));
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
    cpu.R[15] += 2;
}
void gbaCPU::T_ADD_REG(uint16_t opcode)
{
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode >> 6) & 0x7);

    uint32_t result = cpu.R[Rn] + cpu.R[Rm];


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF < (cpu.R[Rn] + cpu.R[Rm]));
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
    cpu.R[15] += 2;
}
void gbaCPU::T_SUB_REG(uint16_t opcode)
{
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode >> 6) & 0x7);

    uint32_t result = cpu.R[Rn] - cpu.R[Rm];


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= (cpu.R[Rn] - cpu.R[Rm]));
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
    cpu.R[15] += 2;
}
void gbaCPU::T_MOV_IMD(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
    cpu.R[15] += 2;
}

void gbaCPU::T_BL(uint16_t opcode)
{
    cpu.R[14] = cpu.R[15] - 2;
    uint32_t fullOpcode = (io.readMem(cpu.R[15] - 4,1) << 16) | io.readMem(cpu.R[15] - 2,1);
    int32_t offset = opcode & 0x7FF;
    offset = offset << 12;
    offset = offset << 9; // Sign Extend
    offset = offset >> 9;
    cpu.R[15] += offset;
    uint32_t offset2 = fullOpcode & 0x7FF;
    cpu.R[15] += offset2;
    cpu.R[15] += offset2;
    cpu.R[15] += 4;
}

void gbaCPU::T_AND_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = cpu.R[Rd] & cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_BIC_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = cpu.R[Rd] & (!cpu.R[Rm]);

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_MVN_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = (!cpu.R[Rm]);

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_ORR_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = cpu.R[Rd] | cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_MUL_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = cpu.R[Rd] * cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_TST_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = cpu.R[Rd] & cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    //cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_EOR_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = cpu.R[Rd] ^ cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}


void gbaCPU::T_LSL_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    if((cpu.R[Rm] & 0xFF) == 0)
    {
        result = cpu.R[Rd];
    }
    if((cpu.R[Rm] & 0xFF) < 32 && (cpu.R[Rm] & 0xFF) != 0)
    {
        cpu.cpsr.C =  ( ( ( cpu.R[Rd] >> ( 32 - ( cpu.R[Rm] & 0xFF ) ) ) & 0x1 ) != 0); // != 0 Might be unneeded
        result = cpu.R[Rd] << (cpu.R[Rm] & 0xFF);
    }
    if((cpu.R[Rm] & 0xFF) == 32)
    {
        cpu.cpsr.C = (cpu.R[Rd] & 0x1);
        result = 0;
    }
    if((cpu.R[Rm] & 0xFF) > 32)
    {
        cpu.cpsr.C = 0;
        result = 0;
    }

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_LSR_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    if((cpu.R[Rm] & 0xFF) == 0)
    {
        result = cpu.R[Rd];
    }
    if((cpu.R[Rm] & 0xFF) < 32 && (cpu.R[Rm] & 0xFF) != 0)
    {
        cpu.cpsr.C =  ( ( ( cpu.R[Rd] >> ( cpu.R[Rm] - 1) ) & 0x1 ) != 0); // != 0 Might be unneeded
        result = cpu.R[Rd] >> (cpu.R[Rm] & 0xFF);
    }
    if((cpu.R[Rm] & 0xFF) == 32)
    {
        cpu.cpsr.C = ((cpu.R[Rd] & 0x80000000) != 0);
        result = 0;
    }
    if((cpu.R[Rm] & 0xFF) > 32)
    {
        cpu.cpsr.C = 0;
        result = 0;
    }

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_ASR_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    if((cpu.R[Rm] & 0xFF) == 0)
    {
        result = cpu.R[Rd];
    }
    if((cpu.R[Rm] & 0xFF) < 32 && (cpu.R[Rm] & 0xFF) != 0)
    {
        cpu.cpsr.C =  ( ( ( cpu.R[Rd] >> ( cpu.R[Rm] - 1) ) & 0x1 ) != 0); // != 0 Might be unneeded
        result = (int32_t)cpu.R[Rd] >> (cpu.R[Rm] & 0xFF);
    }
    if((cpu.R[Rm] & 0xFF) >= 32)
    {
        cpu.cpsr.C = ((cpu.R[Rd] & 0x80000000) != 0);
        if((cpu.R[Rd] & 0x80000000) != 0)
        {
            result = 0xFFFFFFFF;
        }
        if((cpu.R[Rd] & 0x80000000) == 0)
        {
            result = 0;
        }
    }

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}


void gbaCPU::T_ADC_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    result = cpu.R[Rd] + cpu.R[Rm] + cpu.cpsr.C;

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF < (cpu.R[Rd] + cpu.R[Rm] + cpu.cpsr.C));
    // MISSING V FLAG IMPLEMENTATION SEE PG. 308

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_SBC_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    result = cpu.R[Rd] - cpu.R[Rm] - (!cpu.cpsr.C);

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= (cpu.R[Rd] - cpu.R[Rm] - (!cpu.cpsr.C)));
    // MISSING V FLAG IMPLEMENTATION SEE PG. 308

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_CMP_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    result = cpu.R[Rd] - cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= (cpu.R[Rd] - cpu.R[Rm]));
    // MISSING V FLAG IMPLEMENTATION SEE PG. 308

    //cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_CMN_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    result = cpu.R[Rd] + cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF < (cpu.R[Rd] + cpu.R[Rm]));
    // MISSING V FLAG IMPLEMENTATION SEE PG. 308

    //cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_NEG_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    result = 0 - cpu.R[Rm];

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= (0 - cpu.R[Rm]));
    // MISSING V FLAG IMPLEMENTATION SEE PG. 377

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_ROR_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result;

    if((cpu.R[Rm] & 0xFF) == 0)
    {
        result = cpu.R[Rd];
    }
    if((cpu.R[Rm] & 0xF) == 0 && (cpu.R[Rm] & 0xFF) != 0)
    {
        cpu.cpsr.C = ( ( cpu.R[Rd] & 0x80000000 ) != 0);
        result = cpu.R[Rd];
    }
    if((cpu.R[Rm] & 0xF) > 0)
    {
        cpu.cpsr.C = (((cpu.R[Rd] >> ((cpu.R[Rm] & 0xF) - 1) ) & 0x1) != 0);
        result = rotr32(cpu.R[Rd],(cpu.R[Rm] & 0xF));
    }

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}
