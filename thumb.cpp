#include "cpu.hpp"
#include "io.hpp"
#include "ui.hpp"
#include <stdio.h>

void gbaCPU::decodeAndRunTHUMB()
{
    //uint16_t opcode = io.readMem(cpu.R[15], 1);
    //cpu.lastOpcodeRan = io.readMem(cpu.R[15], 1);
    //printf("Opcode: 0x%X\n",opcode);
    cpu.runTHUMB(io.readMem(cpu.R[15], 1));
}
typedef void (gbaCPU::*thumbLookup2)(short unsigned int);

thumbLookup2 thumbLookupTable[0x10000];

thumbLookup2 getThumbOpcode(uint16_t opcode)
{
    switch((opcode) & 0xE000)
    {
        case 0x0:
            switch((opcode >> 10) & 0x7)
            {
                case 0 ... 5: // 000-??X Shift by Immediate
                    switch((opcode >> 11) & 0x3)
                    {
                        case 0: // LSL
                            return &gbaCPU::T_LSL_IMD;
                        break;

                        case 1: // LSR
                            return &gbaCPU::T_LSR_IMD;
                        break;

                        case 2: // ASR
                            return &gbaCPU::T_ASR_IMD;
                        break;

                        default:
                            return &gbaCPU::T_INVALID;
                        break;
                    }
                break;

                case 6: // 000-110 Add Subtract Register
                    switch((opcode >> 9) & 0x1)
                    {
                        case 0: // Add
                            return &gbaCPU::T_ADD_REG;
                        break;

                        case 1: // Sub
                            return &gbaCPU::T_SUB_REG;
                        break;
                    }
                break;

                case 7: // 000-111 Add Subtract Immediate
                    switch((opcode >> 9) & 0x1)
                    {
                        case 0: // Add
                            return &gbaCPU::T_ADD_SIMD;
                        break;

                        case 1: // Sub
                            return &gbaCPU::T_SUB_SIMD;
                        break;
                    }
                break;
            }
        break;

        case 0x2000:
            switch((opcode >> 11) & 0x3)
            {
                case 0: // MOV
                    return &gbaCPU::T_MOV_IMD;
                break;

                case 1: // CMP
                    return &gbaCPU::T_CMP_IMD;
                break;

                case 2: // ADD
                    return &gbaCPU::T_ADD_IMD;
                break;

                case 3: // SUB
                    return &gbaCPU::T_SUB_IMD;
                break;
            }
        break;

        case 0x4000: // 010
            switch((opcode >> 10) & 0x7)
            {
                case 0: // 010-000  Data Processing Register
                    switch((opcode >> 6) & 0xF)
                    {
                        case 0:
                            return &gbaCPU::T_AND_ALU;
                        break;

                        case 1:
                            return &gbaCPU::T_EOR_ALU;
                        break;

                        case 2:
                            return &gbaCPU::T_LSL_ALU;
                        break;

                        case 3:
                            return &gbaCPU::T_LSR_ALU;
                        break;

                        case 4:
                            return &gbaCPU::T_ASR_ALU;
                        break;

                        case 5:
                            return &gbaCPU::T_ADC_ALU;
                        break;

                        case 6:
                            return &gbaCPU::T_SBC_ALU;
                        break;

                        case 7:
                            return &gbaCPU::T_ROR_ALU;
                        break;

                        case 8:
                            return &gbaCPU::T_TST_ALU;
                        break;

                        case 9:
                            return &gbaCPU::T_NEG_ALU;
                        break;

                        case 0xA:
                            return &gbaCPU::T_CMP_ALU;
                        break;

                        case 0xB:
                            return &gbaCPU::T_CMN_ALU;
                        break;

                        case 0xC:
                            return &gbaCPU::T_ORR_ALU;
                        break;

                        case 0xD:
                            return &gbaCPU::T_MUL_ALU;
                        break;

                        case 0xE:
                            return &gbaCPU::T_BIC_ALU;
                        break;

                        case 0xF:
                            return &gbaCPU::T_MVN_ALU;
                        break;
                    }
                break;

                case 1: // 010-001  Special Data Processing or Branch Exchange
                    switch((opcode >> 8) & 0x3)
                    {
                        case 0:
                            return &gbaCPU::T_SDP_ADD;
                        break;

                        case 1:
                            return &gbaCPU::T_SDP_CMP;
                        break;

                        case 2:
                            return &gbaCPU::T_SDP_MOV;
                        break;

                        case 3:
                            return &gbaCPU::T_BX;
                        break;

                        default:
                            return &gbaCPU::T_INVALID;
                        break;
                    }
                break;

                case 2 ... 3: // 010-01X  Load from Literal Pool
                    return &gbaCPU::T_LD_LP;
                break;

                case 4 ... 7: // 010-1XX  Load Store Register Offset
                    switch((opcode >> 9) & 0x1)
                    {
                        case 0:
                            return &gbaCPU::T_LDST_REG;
                        break;

                        case 1:
                            return &gbaCPU::T_LDSTSBH_REG;
                        break;
                    }

                break;
            }
        break;

        case 0x6000: // Lost store word/byte immediate offset
            switch((opcode >> 11) & 0x3)
            {
                case 0: // STR
                    return &gbaCPU::T_STR_IMD;
                break;

                case 1: // LDR
                    return &gbaCPU::T_LDR_IMD;
                break;

                case 2: // STRB
                    return &gbaCPU::T_STRB_IMD;
                break;

                case 3: // LDRB
                    return &gbaCPU::T_LDRB_IMD;
                break;
            }
        break;

        case 0x8000:
            switch((opcode >> 11) & 0x3)
            {
                case 0:
                    return &gbaCPU::T_STRH_IMD;
                break;

                case 1:
                    return &gbaCPU::T_LDRH_IMD;
                break;

                case 2:
                    return &gbaCPU::T_STR_SP;
                break;

                case 3:
                    return &gbaCPU::T_LDR_SP;
                break;
            }
        break;

        case 0xA000:
            switch((opcode >> 12) & 0x1)
            {
                case 0: // Add to SP or PC
                    switch((opcode >> 11) & 0x1)
                    {
                        case 0: // Add to PC and store in Rd
                            return &gbaCPU::T_ADD_PC;
                        break;

                        case 1: // Add to SP and store in Rd
                            return &gbaCPU::T_ADD_SP;
                        break;
                    }
                break;

                case 1: // Misc Instructions
                    switch((opcode >> 9) & 0x3)
                    {
                        case 0:
                            return &gbaCPU::T_ADJ_STACK;
                        break;

                        case 2: // Push/Pop Register List
                            switch((opcode >> 11) & 0x1)
                            {
                                case 0: // PUSH
                                    return &gbaCPU::T_PUSH;
                                break;

                                case 1: // POP
                                    return &gbaCPU::T_POP;
                                break;
                            }
                        break;

                        case 3:
                            return &gbaCPU::T_INVALID;
                        break;

                        default:
                            return &gbaCPU::T_INVALID;
                        break;
                    }
                break;
            }
        break;

        case 0xC000: // 110
            switch((opcode >> 8) & 0x1F)
            {
                case 0x0 ... 0xF: // 110-0XXXX Load Store Multiple
                    switch((opcode >> 11) & 0x1)
                    {
                        case 0: // STMIA
                            return &gbaCPU::T_STMIA;
                        break;

                        case 1: // LDMIA
                            return &gbaCPU::T_LDMIA;
                        break;
                    }
                break;

                case 0x10 ... 0x1D: // Conditional Branch
                    switch((opcode >> 8) & 0xF)
                    {
                        case 0:
                            return &gbaCPU::T_B_EQ;
                        break;

                        case 1:
                            return &gbaCPU::T_B_NE;
                        break;

                        case 2:
                            return &gbaCPU::T_B_CS;
                        break;

                        case 3:
                            return &gbaCPU::T_B_CC;
                        break;

                        case 4:
                            return &gbaCPU::T_B_MI;
                        break;

                        case 5:
                            return &gbaCPU::T_B_PL;
                        break;

                        case 6:
                            return &gbaCPU::T_B_VS;
                        break;

                        case 7:
                            return &gbaCPU::T_B_VC;
                        break;

                        case 8:
                            return &gbaCPU::T_B_HI;
                        break;

                        case 9:
                            return &gbaCPU::T_B_LS;
                        break;

                        case 0xA:
                            return &gbaCPU::T_B_GE;
                        break;

                        case 0xB:
                            return &gbaCPU::T_B_LT;
                        break;

                        case 0xC:
                            return &gbaCPU::T_B_GT;
                        break;

                        case 0xD:
                            return &gbaCPU::T_B_LE;
                        break;

                        default:
                            return &gbaCPU::T_INVALID;
                        break;
                    }
                break;

                case 0x1E: // Undefined Instruction
                    return &gbaCPU::T_INVALID;
                break;

                case 0x1F: // Software Interrupt
                    return &gbaCPU::T_SWI;
                break;
            }
        break;

        case 0xE000:
            switch((opcode >> 11) & 3)
            {
                case 0: // Unconditional Branch
                    return &gbaCPU::T_B;
                break;

                case 1: // BLX Suffix or Undefined Instruction
                    return &gbaCPU::T_INVALID;
                break;

                case 2: // BL Prefix
                    return &gbaCPU::T_BL;
                break;

                case 3: // BL Suffix
                    return &gbaCPU::T_INVALID;
                break;
            }
        break;

        default:
            return &gbaCPU::T_INVALID;
        break;
    }
}

void gbaCPU::generateThumbLookup()
{
    for(uint32_t i = 0; i < 0x10000; i++)
    {
        thumbLookupTable[i] = getThumbOpcode(i);
    }
}

void gbaCPU::runTHUMB(uint16_t opcode)
{
    (cpu.*thumbLookupTable[opcode])(opcode);
    //(cpu->(*thumbLookupTable[opcode]))(opcode);
    return;
    switch((opcode) & 0xE000)
    {
        case 0x0:
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
                            ui.pauseEmulation = true;
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
                    switch((opcode >> 9) & 0x1)
                    {
                        case 0: // Add
                            cpu.T_ADD_SIMD(opcode);
                        break;

                        case 1: // Sub
                            cpu.T_SUB_SIMD(opcode);
                        break;
                    }
                break;
            }
        break;

        case 0x2000:
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

        case 0x4000: // 010
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

                case 1: // 010-001  Special Data Processing or Branch Exchange
                    switch((opcode >> 8) & 0x3)
                    {
                        case 0:
                            T_SDP_ADD(opcode);
                        break;

                        case 1:
                            T_SDP_CMP(opcode);
                        break;

                        case 2:
                            T_SDP_MOV(opcode);
                        break;

                        case 3:
                            T_BX(opcode);
                        break;

                        default:
                            printf("BAD_ERROR: IMPOSSIBLE CODE PATH!\n");
                            ui.pauseEmulation = true;
                        break;
                    }
                break;

                case 2 ... 3: // 010-01X  Load from Literal Pool
                    T_LD_LP(opcode);
                break;

                case 4 ... 7: // 010-1XX  Load Store Register Offset
                    switch((opcode >> 9) & 0x1)
                    {
                        case 0:
                            T_LDST_REG(opcode);
                        break;

                        case 1:
                            T_LDSTSBH_REG(opcode);
                        break;
                    }

                break;
            }
        break;

        case 0x6000: // Lost store word/byte immediate offset
            switch((opcode >> 11) & 0x3)
            {
                case 0: // STR
                    T_STR_IMD(opcode);
                break;

                case 1: // LDR
                    T_LDR_IMD(opcode);
                break;

                case 2: // STRB
                    T_STRB_IMD(opcode);
                break;

                case 3: // LDRB
                    T_LDRB_IMD(opcode);
                break;
            }
        break;

        case 0x8000:
            switch((opcode >> 11) & 0x3)
            {
                case 0:
                    cpu.T_STRH_IMD(opcode);
                break;

                case 1:
                    cpu.T_LDRH_IMD(opcode);
                break;

                case 2:
                    cpu.T_STR_SP(opcode);
                break;

                case 3:
                    cpu.T_LDR_SP(opcode);
                break;
            }
        break;

        case 0xA000:
            switch((opcode >> 12) & 0x1)
            {
                case 0: // Add to SP or PC
                    switch((opcode >> 11) & 0x1)
                    {
                        case 0: // Add to PC and store in Rd
                            cpu.T_ADD_PC(opcode);
                        break;

                        case 1: // Add to SP and store in Rd
                            cpu.T_ADD_SP(opcode);
                        break;
                    }
                break;

                case 1: // Misc Instructions
                    switch((opcode >> 9) & 0x3)
                    {
                        case 0:
                            T_ADJ_STACK(opcode);
                        break;

                        case 2: // Push/Pop Register List
                            switch((opcode >> 11) & 0x1)
                            {
                                case 0: // PUSH
                                    T_PUSH(opcode);
                                break;

                                case 1: // POP
                                    T_POP(opcode);
                                break;
                            }
                        break;

                        case 3:
                            printf("THUMB: Unimplemented 101 Software Interrupt!\n");
                            // Does this even exist on the GBA?  Need to check later.
                            ui.pauseEmulation = true;
                        break;

                        default:
                            printf("BAD_ERROR: Invalid Instruction!\n");
                            ui.pauseEmulation = true;
                        break;
                    }
                break;
            }
        break;

        case 0xC000: // 110
            switch((opcode >> 8) & 0x1F)
            {
                case 0x0 ... 0xF: // 110-0XXXX Load Store Multiple
                    switch((opcode >> 11) & 0x1)
                    {
                        case 0: // STMIA
                            T_STMIA(opcode);
                        break;

                        case 1: // LDMIA
                            T_LDMIA(opcode);
                        break;
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
                            ui.pauseEmulation = true;
                        break;
                    }
                break;

                case 0x1E: // Undefined Instruction
                    printf("THUMB ERROR: Invalid opcode 0x%X!\n",opcode);
                    ui.pauseEmulation = true;
                break;

                case 0x1F: // Software Interrupt
                    cpu.T_SWI(opcode);
                break;
            }
        break;

        case 0xE000:
            switch((opcode >> 11) & 3)
            {
                case 0: // Unconditional Branch
                    cpu.T_B(opcode);
                break;

                case 1: // BLX Suffix or Undefined Instruction
                    printf("THUMB: THIS SHOULD NOT HAPPEN! 0x%X\n",opcode);
                    ui.pauseEmulation = true;
                break;

                case 2: // BL Prefix
                    cpu.T_BL(opcode);
                break;

                case 3: // BL Suffix
                    printf("THUMB: THIS SHOULD NOT HAPPEN! 0x%X\n",opcode);
                    ui.pauseEmulation = true;
                break;
            }
        break;

        default:
            printf("Unimplemented opcode 0x%X\n",opcode);
            ui.pauseEmulation = true;
        break;
    }
}

// BEGINNING OF THUMB OPCODES
void gbaCPU::T_STMIA(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rn = ((opcode >> 8) & 0x7);
    uint8_t regList = (opcode & 0xFF);
    uint32_t startAddress = cpu.R[Rn];
    uint8_t setBits = 0;
    bool setBitArray[8];
    for(int i = 0; i < 8; i++)
    {
        setBitArray[i] = false;
        if(((regList >> i) & 0x1) != 0)
        {
            setBits++;
            setBitArray[i] = true;
        }
    }
    for(int i = 0; i < 8; i++)
    {
        if(setBitArray[i] == true)
        {
            io.writeMem(startAddress, 2, cpu.R[i]);
            startAddress += 4;
        }
    }
    cpu.R[Rn] = cpu.R[Rn] + (setBits * 4);
}

void gbaCPU::T_LDMIA(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rn = ((opcode >> 8) & 0x7);
    uint8_t regList = (opcode & 0xFF);
    uint32_t startAddress = cpu.R[Rn];
    uint8_t setBits = 0;
    bool setBitArray[8];
    for(int i = 0; i < 8; i++)
    {
        setBitArray[i] = false;
        if(((regList >> i) & 0x1) != 0)
        {
            setBits++;
            setBitArray[i] = true;
        }
    }
    for(int i = 0; i < 8; i++)
    {
        if(setBitArray[i] == true)
        {
            cpu.R[i] = io.readMem(startAddress, 2); // Is this correct?
            startAddress += 4;
        }
    }
    cpu.R[Rn] = cpu.R[Rn] + (setBits * 4);
}

void gbaCPU::T_LD_LP(uint16_t opcode)
{
    cpu.R[15] += 2;
    cpu.R[15] += 2;
    uint32_t location = cpu.R[15] & 0xFFFFFFFC;
    location += ((opcode & 0x00FF) * 4);
    uint8_t Rd = ((opcode >> 8) & 0x7);
    cpu.R[15] -= 2;
    cpu.R[Rd] = io.readMem(location, 2);
}

void gbaCPU::T_LSL_IMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
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
}

void gbaCPU::T_LSR_IMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint8_t shiftVal = ((opcode >> 6) & 0x1F);

    if(shiftVal == 0)
    {
        cpu.cpsr.C = ((cpu.R[Rm] & 0x80000000) != 0);
        cpu.R[Rd] = 0;
    }
    if(shiftVal != 0)
    {
        cpu.cpsr.C = ( ( ( cpu.R[Rm] >> (shiftVal - 1) ) & 0x1 ) != 0); // != 0 might be unneeded
        cpu.R[Rd] = (cpu.R[Rm] >> shiftVal);
    }
    cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
    cpu.cpsr.Z = (cpu.R[Rd] == 0);
}

void gbaCPU::T_ASR_IMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
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
void gbaCPU::T_B(uint16_t opcode)
{
    int32_t jumpVal = (opcode & 0x7FF);
    jumpVal = jumpVal << 1;
    jumpVal = jumpVal << 20;
    jumpVal = jumpVal >> 20;
    cpu.R[15] += jumpVal;
    cpu.R[15] += 4;

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
    cpu.R[15] += 2;
    uint8_t Rn = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = cpu.R[Rn] - imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = ((uint64_t)0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)imdVal));
    cpu.cpsr.V = (( (uint64_t)cpu.R[Rn] ^ (uint64_t)imdVal ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
}
void gbaCPU::T_SUB_IMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = cpu.R[Rd] - imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = ((uint64_t)0xFFFFFFFF >= ((uint64_t)cpu.R[Rd] - (uint64_t)imdVal));
    cpu.cpsr.V = (( (uint64_t)cpu.R[Rd] ^ (uint64_t)imdVal ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
}
void gbaCPU::T_ADD_IMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = cpu.R[Rd] + imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = ((uint64_t)0xFFFFFFFF < ((uint64_t)cpu.R[Rd] + (uint64_t)imdVal));

    cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)imdVal ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;

    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
}

void gbaCPU::T_ADD_PC(uint16_t opcode)
{
    //ui.pauseEmulation = true;
    cpu.R[15] += 2;
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    cpu.R[Rd] = (cpu.R[15] + (imdVal << 2)) - 2;
    cpu.R[15] -= 2;
}
void gbaCPU::T_ADD_SP(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    cpu.R[Rd] = cpu.R[13] + (imdVal << 2);
}
void gbaCPU::T_ADD_SIMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x7);


    uint32_t result = cpu.R[Rn] + imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)imdVal));
    cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)imdVal ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 309
}
void gbaCPU::T_SUB_SIMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x7);


    uint32_t result = cpu.R[Rn] - imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)imdVal));
    cpu.cpsr.V = (( (uint64_t)cpu.R[Rd] ^ (uint64_t)imdVal ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 309
}
void gbaCPU::T_ADD_REG(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode >> 6) & 0x7);

    uint32_t result = cpu.R[Rn] + cpu.R[Rm];


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)cpu.R[Rm]));
    cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;

    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
}
void gbaCPU::T_SUB_REG(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rm = ((opcode >> 6) & 0x7);

    uint32_t result = cpu.R[Rn] - cpu.R[Rm];


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)cpu.R[Rm]));
    cpu.cpsr.V = (( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
}
void gbaCPU::T_MOV_IMD(uint16_t opcode)
{
    cpu.R[15] += 2;
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = (opcode & 0xFF);

    uint32_t result = imdVal;


    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.R[Rd] = result;
    // MISSING V IMPLEMENTATION, SEE PG. 339
}

void gbaCPU::T_BL(uint16_t opcode)
{
    cpu.R[14] = (cpu.R[15] + 4) | cpu.cpsr.T;
    uint32_t fullOpcode = (io.readMem(cpu.R[15],1) << 16) | io.readMem(cpu.R[15] + 2,1);
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

void gbaCPU::T_BX(uint16_t opcode)
{
    // Does this cause error?
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint8_t highReg = ((opcode >> 6) & 0x1);
    Rm = Rm + (highReg * 8);
    //printf("Loaded Location: 0x%X\n", cpu.R[Rm]);
    cpu.cpsr.T = (cpu.R[Rm] & 0x1);
    //printf("Chosen Thumb Mode: 0x%X\n", cpu.cpsr.T);
    cpu.R[15] = (cpu.R[Rm] & 0xFFFFFFFE);
    //printf("Jumped To 0x%X\n", cpu.R[15]);
    if(Rm == 15)
    {
        if(cpu.cpsr.T == 1)
        {
            cpu.R[15] += 2;
        }
        else
        {
            cpu.R[15] += 4;
        }
    }
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
    uint32_t result = cpu.R[Rd] & (~cpu.R[Rm]);

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_MVN_ALU(uint16_t opcode)
{
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint32_t result = (~cpu.R[Rm]);

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
    uint32_t result = 0;

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
    uint32_t result = 0;

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
    cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rd] + (uint64_t)cpu.R[Rm] + (uint64_t)cpu.cpsr.C));
    cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ ( (uint64_t)cpu.R[Rm] + (uint64_t)cpu.cpsr.C ) ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;

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
    cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rd] - (uint64_t)cpu.R[Rm] - (uint64_t)(!cpu.cpsr.C)));
    cpu.cpsr.V = ( ( (uint64_t)cpu.R[Rd] ^ ( (uint64_t)cpu.R[Rm] + (uint64_t)(!cpu.cpsr.C) ) ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;

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
    cpu.cpsr.C = ((uint64_t)0xFFFFFFFF >= ((uint64_t)cpu.R[Rd] - (uint64_t)cpu.R[Rm]));
    cpu.cpsr.V = (( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;

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
    cpu.cpsr.C = ((uint64_t)0xFFFFFFFF < ((uint64_t)cpu.R[Rd] + (uint64_t)cpu.R[Rm]));
    cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;

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
    cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)0 - (uint64_t)cpu.R[Rm]));
    // MISSING V FLAG IMPLEMENTATION SEE PG. 377

    cpu.R[Rd] = result;
    cpu.R[15] += 2;
}

void gbaCPU::T_ROR_ALU(uint16_t opcode)
{
    //ui.pauseEmulation = true;
    uint8_t Rd = (opcode & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    //printf("RD: 0x%X\n", Rd);
    //printf("RM: 0x%X\n", Rm);
    //printf("RDval: 0x%X\n", cpu.R[Rd]);
    //printf("RMval: 0x%X\n", cpu.R[Rm]);
    uint32_t result;

    if((cpu.R[Rm] & 0xFF) == 0)
    {
        result = cpu.R[Rd];
        //printf("PATH 1 CHOSEN\n");
    }
    //if((cpu.R[Rm] & 0xF) == 0 && (cpu.R[Rm] & 0xFF) != 0)
    //{
    //    cpu.cpsr.C = ( ( cpu.R[Rd] & 0x80000000 ) != 0);
    //    result = cpu.R[Rd];
    //}
    else
    {
        //printf("PATH 2 CHOSEN\n");
        cpu.cpsr.C = (((cpu.R[Rd] >> ((cpu.R[Rm] & 0x1F) - 1) ) & 0x1) != 0);
        result = rotr32(cpu.R[Rd],(cpu.R[Rm] & 0x1F));
    }

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    //cpu.cpsr.N = 1;

    cpu.R[Rd] = result;
    //printf("RESULT: 0x%X\n", result);
    cpu.R[15] += 2;
}


void gbaCPU::T_LDR_IMD(uint16_t opcode)
{
    //ui.pauseEmulation =true;
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x1F);

    uint32_t address = cpu.R[Rn] + (imdVal * 4);
    //address = address & 0xFFFFFFFC;
    cpu.R[Rd] = io.readMem(address,2);
    cpu.R[15] += 2;
}
void gbaCPU::T_LDST_REG(uint16_t opcode)
{
    uint8_t Ro = ((opcode >> 6) & 0x7);
    uint8_t Rb = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    bool L,B;
    L = ((opcode >> 11) & 0x1);
    B = ((opcode >> 10) & 0x1);

    uint32_t address = cpu.R[Rb] + cpu.R[Ro];
    switch((L << 1) | B)
    {
        case 0: // Store Word
            io.writeMem(address, 2, cpu.R[Rd]);
        break;

        case 1: // Store Byte
            io.writeMem(address, 0, cpu.R[Rd]);
        break;

        case 2: // Load Word
            cpu.R[Rd] = io.readMem(address,2);
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
        break;
    }
    cpu.R[15] += 2;
}

void gbaCPU::T_LDSTSBH_REG(uint16_t opcode)
{
    uint8_t Ro = ((opcode >> 6) & 0x7);
    uint8_t Rb = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    bool H,S;
    H = ((opcode >> 11) & 0x1);
    S = ((opcode >> 10) & 0x1);

    uint32_t address = cpu.R[Rb] + cpu.R[Ro];
    switch((S << 1) | H)
    {
        case 0:
            io.writeMem(address, 1, cpu.R[Rd]);
        break;

        case 1:
            cpu.R[Rd] = io.readMem(address,1);
            //io.writeMem(address, 0, cpu.R[Rd]);
        break;

        case 2:
            {
                int32_t signExtend = io.readMem(address,0);
                signExtend = signExtend << 24;
                signExtend = signExtend >> 24;
                cpu.R[Rd] = signExtend;
            }
        break;

        case 3:
            {
                int32_t signExtend = io.readMem(address,1);
                signExtend = signExtend << 16;
                signExtend = signExtend >> 16;
                cpu.R[Rd] = signExtend;
            }
        break;
    }
    cpu.R[15] += 2;
}

void gbaCPU::T_LDR_SP(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = ((opcode) & 0xFF);

    uint32_t address = cpu.R[13] + (imdVal * 4);
    cpu.R[Rd] = io.readMem(address,2);
    cpu.R[15] += 2;
}

void gbaCPU::T_LDRH_IMD(uint16_t opcode)
{
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x1F);
//printf("0x%X\n", cpu.R[Rn] + (imdVal << 1));
    //uint32_t address = cpu.R[Rn] + (imdVal * 2);
    cpu.R[Rd] = io.readMem(cpu.R[Rn] + (imdVal << 1),1);
    cpu.R[15] += 2;
}
void gbaCPU::T_LDRB_IMD(uint16_t opcode)
{
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x1F);
    //printf("imdVAL: 0x%X\n", imdVal);

    uint32_t address = cpu.R[Rn] + (imdVal);
    cpu.R[Rd] = io.readMem(address,0);
    cpu.R[15] += 2;
}
void gbaCPU::T_STR_IMD(uint16_t opcode)
{
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x1F);

    uint32_t address = cpu.R[Rn] + (imdVal * 4);
    io.writeMem(address,2,cpu.R[Rd]);
    cpu.R[15] += 2;
}

void gbaCPU::T_STR_SP(uint16_t opcode)
{
    uint8_t Rd = ((opcode >> 8) & 0x7);
    uint8_t imdVal = ((opcode) & 0xFF);

    uint32_t address = cpu.R[13] + (imdVal * 4);
    io.writeMem(address,2,cpu.R[Rd]);
    if(Rd == 15)
    {
        io.writeMem(address,2,cpu.R[Rd] - 8);
    }
    cpu.R[15] += 2;
}

void gbaCPU::T_STRH_IMD(uint16_t opcode)
{
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x1F);

    uint32_t address = cpu.R[Rn] + (imdVal * 2);
    io.writeMem(address,1,cpu.R[Rd]);
    cpu.R[15] += 2;
}
void gbaCPU::T_STRB_IMD(uint16_t opcode)
{
    uint8_t Rn = ((opcode >> 3) & 0x7);
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t imdVal = ((opcode >> 6) & 0x1F);

    uint32_t address = cpu.R[Rn] + (imdVal);
    io.writeMem(address,0,cpu.R[Rd]);
    cpu.R[15] += 2;
}

void gbaCPU::T_PUSH(uint16_t opcode)
{
    uint16_t regList = (opcode & 0x1FF);

    uint8_t setBits = 0;
    bool setBitArray[9];
    for(int i = 0; i < 9; i++)
    {
        setBitArray[i] = false;
        if(((regList >> i) & 0x1) != 0)
        {
            setBits++;
            setBitArray[i] = true;
        }
    }
    uint32_t startAddress = (cpu.R[13] - 4 *(setBits));
    //uint32_t endAddress = cpu.R[13] - 4;


    for(int i = 0; i < 9; i++)
    {
        if(setBitArray[i] == true && i != 8)
        {
            io.writeMem(startAddress, 2, cpu.R[i]);
            startAddress += 4;
        }
        if(setBitArray[i] == true && i == 8)
        {
            io.writeMem(startAddress, 2, cpu.R[14]);
            startAddress += 4;
        }
    }
    cpu.R[13] = cpu.R[13] - 4*(setBits);
    cpu.R[15] += 2;
}

void gbaCPU::T_POP(uint16_t opcode)
{
    uint16_t regList = (opcode & 0x1FF);

    uint8_t setBits = 0;
    bool setBitArray[9];
    for(int i = 0; i < 9; i++)
    {
        setBitArray[i] = false;
        if(((regList >> i) & 0x1) != 0)
        {
            setBits++;
            setBitArray[i] = true;
        }
    }
    uint32_t startAddress = cpu.R[13];
    uint32_t endAddress = cpu.R[13] + 4 * (setBits);

    for(int i = 0; i < 9; i++)
    {
        if(setBitArray[i] == true && i != 8)
        {
            cpu.R[i] = io.readMem(startAddress, 2);
            startAddress += 4;
        }
        if(setBitArray[i] == true && i == 8)
        {
            cpu.R[15] = io.readMem(startAddress, 2);
            startAddress += 4;
        }
    }
    cpu.R[13] = endAddress;
    if(setBitArray[8] == false)
    {
        cpu.R[15] += 2;
    }
}

void gbaCPU::T_SDP_ADD(uint16_t opcode)
{
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint8_t H2 = ((opcode >> 6) & 0x1);
    uint8_t H1 = ((opcode >> 7) & 0x1);
    Rd = Rd + (H1 * 8);
    Rm = Rm + (H2 * 8);
    cpu.R[Rd] = cpu.R[Rd] + cpu.R[Rm];
    if(Rm == 0xF)
    {
        cpu.R[Rd] += 4;
    }
    if(Rd == 0xF)
    {
        cpu.R[Rd] += 4;
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 2;
    }
}

void gbaCPU::T_SDP_CMP(uint16_t opcode)
{
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint8_t H2 = ((opcode >> 6) & 0x1);
    uint8_t H1 = ((opcode >> 7) & 0x1);
    Rd = Rd + (H1 * 8);
    Rm = Rm + (H2 * 8);
    uint32_t result = cpu.R[Rd] - cpu.R[Rm];
    if(Rm == 0xF)
    {
        result -= 4;
    }
    if(Rd == 0xF)
    {
        result += 4;
    }

    cpu.cpsr.N = ((result & 0x80000000) != 0);
    cpu.cpsr.Z = (result == 0);
    cpu.cpsr.C = ((uint64_t)0xFFFFFFFF >= ((uint64_t)cpu.R[Rd] - (uint64_t)cpu.R[Rm]));
    cpu.cpsr.V = (( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
    cpu.R[15] += 2;
}

void gbaCPU::T_SDP_MOV(uint16_t opcode)
{
    uint8_t Rd = ((opcode) & 0x7);
    uint8_t Rm = ((opcode >> 3) & 0x7);
    uint8_t H2 = ((opcode >> 6) & 0x1);
    uint8_t H1 = ((opcode >> 7) & 0x1);
    Rd = Rd + (H1 * 8);
    Rm = Rm + (H2 * 8);
    cpu.R[Rd] = cpu.R[Rm];
    if(Rm == 0xF)
    {
        cpu.R[Rd] += 4;
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 2;
    }
}

void gbaCPU::T_ADJ_STACK(uint16_t opcode)
{
    bool S = ((opcode >> 7) & 0x1);
    uint16_t offset = (opcode & 0x7F);
    offset = offset << 2;
    switch(S)
    {
        case 0:
            cpu.R[13] += offset;
        break;

        case 1:
            cpu.R[13] -= offset;
        break;
    }
    cpu.R[15] += 2;
}

void gbaCPU::T_SWI(uint16_t opcode)
{
    cpu.R1314_svc[1] = (cpu.R[15] + 2);
    //printf("Return Address: 0x%X\n", (cpu.R[15] + 2));
    //printf("R14: 0x%X\n", cpu.R[14]);
    cpu.spsr_svc = cpu.cpsr;
    //printf("R14: 0x%X\n", cpu.R[14]);
    modeSwitch(0x13);
    //printf("R14: 0x%X\n", cpu.R[14]);
    cpu.cpsr.T = false;
    cpu.cpsr.I = true;
    cpu.R[15] = 0x8;
    //printf("R14: 0x%X\n", cpu.R[14]);
    //cpu.cpsr.mode = 0x13;

    //if((opcode & 0xFF) == 0xC)
    //{
    //    return;
    //}
    //printf("RUNNING SOFTWARE INTERRUPT 0x%X\n",(opcode & 0xFF));
    //ui.pauseEmulation = true;
}

void gbaCPU::T_INVALID(uint16_t opcode)
{
    printf("Unknown Thumb Opcode 0x%X\n", opcode);
    ui.pauseEmulation = true;
    //printf("RUNNING SOFTWARE INTERRUPT 0x%X\n",(opcode & 0xFF));
    //ui.pauseEmulation = true;
}
