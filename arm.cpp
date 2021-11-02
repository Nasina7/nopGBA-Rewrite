#include "cpu.hpp"
#include "io.hpp"
#include "ui.hpp"
#include <stdio.h>

#define condEqual 0x0
#define condNotEqual 0x1
#define condCarrySet 0x2
#define condCarryClear 0x3
#define condNegativeSet 0x4
#define condNegativeClear 0x5
#define condOverflowSet 0x6
#define condOverflowClear 0x7
#define condCSetZClear 0x8
#define condCClearOrZSet 0x9
#define condSignedGreaterOrEqual 0xA
#define condSignedLessThan 0xB
#define condGreaterThan 0xC
#define condLessOrEqual 0xD
#define condAlways 0xE

void gbaCPU::decodeAndRunARM()
{
    //uint32_t opcode = io.get32bitOpcode(cpu.R[15] - 8);
    uint32_t opcode = io.readMem(cpu.R[15], 2);
    cpu.lastOpcodeRan = opcode;
    //printf("Opcode: 0x%X\n",opcode);

    uint8_t condition = (opcode >> 28);

    switch(condition)
    {
        case condEqual:
            if(cpu.cpsr.Z == 0)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.Z == 1)
            {
                cpu.runARM(opcode);
            }
        break;

        case condNotEqual:
            if(cpu.cpsr.Z == 1)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.Z == 0)
            {
                cpu.runARM(opcode);
            }
        break;

        case condCarrySet:
            if(cpu.cpsr.C == 0)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.C == 1)
            {
                cpu.runARM(opcode);
            }
        break;

        case condCarryClear:
            if(cpu.cpsr.C == 1)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.C == 0)
            {
                cpu.runARM(opcode);
            }
        break;

        case condNegativeSet:
            if(cpu.cpsr.N == 0)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.N == 1)
            {
                cpu.runARM(opcode);
            }
        break;

        case condNegativeClear:
            if(cpu.cpsr.N == 1)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.N == 0)
            {
                cpu.runARM(opcode);
            }
        break;

        case condOverflowSet:
            if(cpu.cpsr.V == 0)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.V == 1)
            {
                cpu.runARM(opcode);
            }
        break;

        case condOverflowClear:
            if(cpu.cpsr.V == 1)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.V == 0)
            {
                cpu.runARM(opcode);
            }
        break;

        case condCSetZClear:
            if(cpu.cpsr.C == 0 || cpu.cpsr.Z == 1)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.C == 1 && cpu.cpsr.Z == 0)
            {
                cpu.runARM(opcode);
            }
        break;

        case condCClearOrZSet:
            if(cpu.cpsr.C == 1 && cpu.cpsr.Z == 0)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.C == 0 || cpu.cpsr.Z == 1)
            {
                cpu.runARM(opcode);
            }
        break;

        case condGreaterThan:
            if(cpu.cpsr.Z != 0)
            {
                cpu.R[15] += 4;
                break;
            }
            if(cpu.cpsr.N != cpu.cpsr.V)
            {
                cpu.R[15] += 4;
                break;
            }
            if(cpu.cpsr.Z == 0 && cpu.cpsr.N == cpu.cpsr.V)
            {
                cpu.runARM(opcode);
            }
        break;

        case condLessOrEqual:
            if(cpu.cpsr.Z == 0 && cpu.cpsr.N == cpu.cpsr.V)
            {
                cpu.R[15] += 4;
                break;
            }
            if(cpu.cpsr.Z != 0 || cpu.cpsr.N != cpu.cpsr.V)
            {
                cpu.runARM(opcode);
            }
        break;

        case condSignedGreaterOrEqual:
            if(cpu.cpsr.V != cpu.cpsr.N)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.V == cpu.cpsr.N)
            {
                cpu.runARM(opcode);
            }
        break;

        case condSignedLessThan:
            if(cpu.cpsr.V == cpu.cpsr.N)
            {
                cpu.R[15] += 4;
            }
            if(cpu.cpsr.V != cpu.cpsr.N)
            {
                cpu.runARM(opcode);
            }
        break;

        case condAlways:
            cpu.runARM(opcode);
        break;

        default:
            printf("Unknown Condition 0x%X!\n",condition);
            printf("PC: 0x%X\n",cpu.R[15]);
            ui.pauseEmulation = true;
        break;
    }
}

void gbaCPU::runARM(uint32_t opcode)
{

    switch((opcode & 0x0E000000) >> 25) // Get XXXXYYYXXXXXXXXX
    {
        case 0: // 000
//E12FFF1E


            if((((opcode & 0x00000010) >> 4)) != 0 &&
              (((opcode & 0x00000080) >> 7)) != 0
              )
            {// Multiplies Extra Load Stores
                //handleMultiplyandExtra();
                switch((opcode >> 4) & 0xF)
                {
                    case 0x9: //Multiplies
                        switch((opcode >> 21) & 0x7)
                        {
                            case 0: // Multiply
                                switch((opcode >> 24) & 0x1)
                                {
                                    case 0:
                                        cpu.MUL(opcode);
                                    break;

                                    case 1:
                                        // Apparently this decodes to SWP?
                                        cpu.SWP(opcode);
                                    break;
                                }

                            break;

                            case 1: // Multiply Accumulate
                                cpu.MLA(opcode);
                            break;

                            case 2:
                                cpu.SWPB(opcode);
                            break;

                            case 4: // Unsigned Multiply Long
                                cpu.UMULL(opcode);
                            break;

                            case 5: // Unsigned Multiply Accumulate Long
                                cpu.UMLAL(opcode);
                            break;

                            case 6: // Signed Multiply Long
                                cpu.SMULL(opcode);
                            break;

                            case 7: // Signed Multiply Accumulate Long
                                cpu.SMLAL(opcode);
                            break;

                            default:
                                printf("Undefined Multiply! 0x%X\n", cpu.R[15]);
                                ui.pauseEmulation = true;
                            break;
                        }
                    break;

                    case 0xB: // Load store halfword
                        switch((opcode >> 22) & 0x1)
                        {
                            case 0:
                                LDST_HW_REG(opcode);
                            break;

                            case 1:
                                LDST_HW_IMD(opcode);
                            break;
                        }
                    break;

                    case 0xD ... 0xF: // Other LD/ST instructions
                        switch((opcode >> 22) & 0x1)
                        {
                            case 0:
                                LDST_HWB_REG(opcode);
                            break;

                            case 1:
                                LDST_HWB_IMD(opcode);
                            break;
                        }

                    break;
                }
                break; // Break out of 000 Case
            }
            if(((opcode & 0x00000010) >> 4) != 0)
            {
                //printf("0x%X\n",opcode);
                if
                    (((opcode >> 24) & 0x1) != 0 &&
                     ((opcode >> 23) & 0x1) == 0 &&
                     ((opcode >> 20) & 0x1) == 0)
                {
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
                                    cpu.BX(opcode);
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
                    break;
                }
                else
                {
                    switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
                    {
                        case 0x0:
                            cpu.DPRS_AND(opcode);
                        break;

                        case 0xC:
                            cpu.DPRS_ORR(opcode);
                        break;

                        case 0xD:
                            cpu.DPRS_MOV(opcode);
                        break;

                        default:
                            printf("DPRS 0x%X NOT IMPLEMENTED!\n",((opcode >> 21) & 0xF));
                            ui.pauseEmulation = true;
                        break;
                    }
                    break;
                }
            }
            else if(((opcode & 0x00000010) >> 4) == 0)
            {
                if
                    (((opcode >> 24) & 0x1) != 0 &&
                     ((opcode >> 23) & 0x1) == 0 &&
                     ((opcode >> 20) & 0x1) == 0)
                {
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
                                    cpu.BX(opcode);
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
                    break;
                }
                else
                {
                    switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
                    {
                        case 0x0:
                            cpu.DPIS_AND(opcode);
                        break;

                        case 0x1:
                            cpu.DPIS_EOR(opcode);
                        break;

                        case 0x2:
                            cpu.DPIS_SUB(opcode);
                        break;

                        case 0x3:
                            cpu.DPIS_RSB(opcode);
                        break;

                        case 0x4:
                            cpu.DPIS_ADD(opcode);
                        break;

                        case 0x5:
                            cpu.DPIS_ADC(opcode);
                        break;

                        case 0x6:
                            cpu.DPIS_SBC(opcode);
                        break;

                        case 0x7:
                            cpu.DPIS_RSC(opcode);
                        break;

                        case 0x8:
                            cpu.DPIS_TST(opcode);
                        break;

                        case 0x9:
                            cpu.DPIS_TEQ(opcode);
                        break;

                        case 0xA:
                            cpu.DPIS_CMP(opcode);
                        break;

                        case 0xB:
                            cpu.DPIS_CMN(opcode);
                        break;

                        case 0xC:
                            cpu.DPIS_ORR(opcode);
                        break;

                        case 0xD:
                            cpu.DPIS_MOV(opcode);
                        break;

                        case 0xE:
                            cpu.DPIS_BIC(opcode);
                        break;

                        case 0xF:
                            cpu.DPIS_MVN(opcode);
                        break;

                        default:
                            printf("IMPOSSIBLE CODE PATH!\n");
                            ui.pauseEmulation = true;
                        break;
                    }
                    break;
                }
            }

            printf("This should hopefully not happen.  0x%X\n", opcode);
            while(1)
            {

            }



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
                                cpu.BX(opcode);
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

            // DATA PROCESSING IMMEDIATE SHIFT
            if(((opcode >> 4) & 0x1) == 0)
            {
                switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
                {
                    case 0x0:
                        cpu.DPIS_AND(opcode);
                    break;

                    case 0x1:
                        cpu.DPIS_EOR(opcode);
                    break;

                    case 0x2:
                        cpu.DPIS_SUB(opcode);
                    break;

                    case 0x3:
                        cpu.DPIS_RSB(opcode);
                    break;

                    case 0x4:
                        cpu.DPIS_ADD(opcode);
                    break;

                    case 0x5:
                        cpu.DPIS_ADC(opcode);
                    break;

                    case 0x6:
                        cpu.DPIS_SBC(opcode);
                    break;

                    case 0x7:
                        cpu.DPIS_RSC(opcode);
                    break;

                    case 0x8:
                        cpu.DPIS_TST(opcode);
                    break;

                    case 0x9:
                        cpu.DPIS_TEQ(opcode);
                    break;

                    case 0xA:
                        cpu.DPIS_CMP(opcode);
                    break;

                    case 0xB:
                        cpu.DPIS_CMN(opcode);
                    break;

                    case 0xC:
                        cpu.DPIS_ORR(opcode);
                    break;

                    case 0xD:
                        cpu.DPIS_MOV(opcode);
                    break;

                    case 0xE:
                        cpu.DPIS_BIC(opcode);
                    break;

                    case 0xF:
                        cpu.DPIS_MVN(opcode);
                    break;

                    default:
                        printf("IMPOSSIBLE CODE PATH!\n");
                        ui.pauseEmulation = true;
                    break;
                }
                break;
            }
            // DATA PROCESSING REGISTER SHIFT
            if(((opcode >> 4) & 0x1) == 1)
            {
                switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
                {
                    case 0x0:
                        cpu.DPRS_AND(opcode);
                    break;

                    case 0xC:
                        cpu.DPRS_ORR(opcode);
                    break;

                    case 0xD:
                        cpu.DPRS_MOV(opcode);
                    break;

                    default:
                        printf("DPRS 0x%X NOT IMPLEMENTED!\n",((opcode >> 21) & 0xF));
                        ui.pauseEmulation = true;
                    break;
                }
                break;
            }


            printf("0001 ERROR!\n");
            ui.pauseEmulation = true;
        break;

        case 1: // 001
            // Check for SR Transfer
            if(((opcode >> 23) & 0x3) == 0x2 && ((opcode >> 20) & 0x1) == 0x0) // SR Transfer
            {
                cpu.MISC_MOV_I_SR(opcode);
            }
            switch(((opcode >> 21) & 0xF)) // DATA PROCESSING IMMEDIATE
            {

                case 0x0:
                    cpu.DPI_AND(opcode);
                break;

                case 0x1:
                    cpu.DPI_EOR(opcode);
                break;

                case 0x2:
                    cpu.DPI_SUB(opcode);
                break;

                case 0x3:
                    cpu.DPI_RSB(opcode);
                break;

                case 0x4:
                    cpu.DPI_ADD(opcode);
                break;

                case 0x5:
                    cpu.DPI_ADC(opcode);
                break;

                case 0x6:
                    cpu.DPI_SBC(opcode);
                break;

                case 0x7:
                    cpu.DPI_RSC(opcode);
                break;

                case 0x8:
                    cpu.DPI_TST(opcode);
                break;

                case 0x9:
                    cpu.DPI_TEQ(opcode);
                break;

                case 0xA:
                    cpu.DPI_CMP(opcode);
                break;

                case 0xB:
                    cpu.DPI_CMN(opcode);
                break;

                case 0xC:
                    cpu.DPI_ORR(opcode);
                break;

                case 0xD:
                    cpu.DPI_MOV(opcode);
                break;

                case 0xE:
                    cpu.DPI_BIC(opcode);
                break;

                case 0xF:
                    cpu.DPI_MVN(opcode);
                break;

                default:
                    printf("BAD_ERROR: Data Processing Immediate 0x%X not implemented!\n",((opcode >> 21) & 0xF));
                    ui.pauseEmulation = true;
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
                            ui.pauseEmulation = true;
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

        case 3: // Load Store Register Offset
            switch((opcode >> 24) & 0x1)
            {
                case 0: // Immed Post Indexed
                    switch((opcode >> 21) & 0x1)
                    {
                        case 0: // Immed Post Indexed
                            cpu.LDST_REG_POST(opcode);
                        break;

                        case 1: // IDK what this decodes to, need to check later
                            printf("UNKNOWN IMMED COMBO!\n");
                            ui.pauseEmulation = true;
                        break;
                    }
                break;

                case 1:
                    switch((opcode >> 21) & 0x1)
                    {
                        case 0: // Immed
                            cpu.LDST_REG(opcode);
                        break;

                        case 1: // Immed Pre Indexed
                            //cpu.LDST_IMD_PRE(opcode);
                           cpu.LDST_REG_PRE(opcode);
                        break;
                    }
                break;
            }
        break;

        case 4: // Load Store Multiple
            /*
                If LOADING PC, S indicates CPSR = SPSR
                For all STM and (LDM that doesn't load PC), S indicates that we are using the user mode banked registers, and not
                    the registers for the current mode [HAVEN'T IMPLEMENTED THIS YET, THROWS ERROR IF USED]

                P,U,and W indicate different addressing modes

                L indicates a load or store


                Bits that can be handled within a function,
                    W
                    U
                    P
                    If STM, S

                This is gonna be a mess, will need to cleanup later
            */

            switch((opcode >> 20) & 0x1)
            {
                case 0: // STM
                    cpu.STMIA(opcode);
                break;

                case 1: // LDM
                    cpu.LDMIA(opcode);
                break;
            }
            //ui.pauseEmulation = true;


        break;

        case 5: // 101
            switch((opcode & 0xF0000000) >> 28)
            {
                case 0xF:
                    printf("Unimplemented Branch and Branch with Link and Change to Thumb!\n");
                    ui.pauseEmulation = true;
                break;

                default: // Branch and Branch with Link
                    cpu.BL(opcode);
                break;
            }
        break;

        default:
            printf("Unimplemented opcode 0x%X\n",opcode);
            ui.pauseEmulation = true;
        break;
    }
}

// BEGINNING OF ARM OPCODES

void gbaCPU::BL(uint32_t opcode) // Branch or Branch with Link
{
    //ui.pauseEmulation = true;
    cpu.R[15] += 4;
    int branchOffset = opcode & 0x00FFFFFF;
    branchOffset = ((branchOffset << 8) >> 6); // Sign Extend
    if((opcode & 0x01000000) != 0)
    {
        cpu.R[14] = (cpu.R[15]);
    }
    cpu.R[15] = ((cpu.R[15] + branchOffset) + 4);
}

void gbaCPU::BX(uint32_t opcode) // Branch or Branch with Link
{
    uint8_t Rm = ((opcode) & 0xF);
    cpu.cpsr.T = ((cpu.R[Rm] & 0x1) != 0);
    cpu.R[15] = cpu.R[Rm] & 0xFFFFFFFE;
}

void gbaCPU::DPI_ADD(uint32_t opcode)
{
    cpu.R[15] += 8;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = cpu.R[Rn] + aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)aluVal));
            cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rn] ^ ( (uint64_t)aluVal ) ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[15] -= 4;
    cpu.R[Rd] = result;
}
void gbaCPU::DPI_CMN(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = cpu.R[Rn] + aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)aluVal));
            cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rn] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[15] += 4;
}

void gbaCPU::DPI_ADC(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = cpu.R[Rn] + aluVal + cpu.cpsr.C;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)aluVal + (uint64_t)cpu.cpsr.C));
            cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ ( (uint64_t)cpu.R[Rn] + (uint64_t)cpu.cpsr.C ) ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::DPI_SUB(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = cpu.R[Rn] - aluVal;
    //printf("SUSSY: 0x%X\n",aluVal);

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)aluVal));
            cpu.cpsr.V = (( (uint64_t)cpu.R[Rn] ^ (uint64_t)aluVal ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }

}

void gbaCPU::DPI_CMP(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = cpu.R[Rn] - aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)aluVal));
            cpu.cpsr.V = (( (uint64_t)cpu.R[Rn] ^ (uint64_t)aluVal ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[15] += 4;
}

void gbaCPU::DPI_SBC(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = cpu.R[Rn] - aluVal - (!cpu.cpsr.C);

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)aluVal - (uint64_t)(!cpu.cpsr.C)));
            cpu.cpsr.V = ( ( (uint64_t)cpu.R[Rn] ^ ( (uint64_t)aluVal - (uint64_t)(!cpu.cpsr.C) ) ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::DPI_RSB(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = aluVal - cpu.R[Rn];

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)aluVal - (uint64_t)cpu.R[Rn]));
            cpu.cpsr.V = (( (uint64_t)aluVal ^ (uint64_t)cpu.R[Rn] ) &
                     ( ( (uint64_t)aluVal ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::DPI_RSC(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    uint32_t result = aluVal - cpu.R[Rn] - (!cpu.cpsr.C);

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)aluVal - (uint64_t)cpu.R[Rn] - (uint64_t)(!cpu.cpsr.C)));
            cpu.cpsr.V = ( ( (uint64_t)aluVal ^ ( (uint64_t)cpu.R[Rn] - (uint64_t)(!cpu.cpsr.C) ) ) &
                     ( ( (uint64_t)aluVal ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4; // Could be an issue?
    }
}

void gbaCPU::DPI_EOR(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }
    uint32_t result = cpu.R[Rn] ^ aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::DPI_ORR(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }
    uint32_t result = cpu.R[Rn] | aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4;
    }
}

void gbaCPU::DPI_TEQ(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }
    uint32_t result = cpu.R[Rn] ^ aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 4; // Could be an issue?
    }
}

void gbaCPU::DPI_AND(uint32_t opcode)
{
    cpu.R[15] += 4;
    cpu.R[15] += 8;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal;
    if(shiftAmount != 0)
    {
        aluVal = rotr32(value,shiftAmount);
    }
    else
    {
        aluVal = value;
    }
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount)) & 0x80000000) != 0);
    }
    uint32_t result = cpu.R[Rn] & aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[15] -= 8;
    cpu.R[Rd] = result;
}

void gbaCPU::DPI_BIC(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }
    uint32_t result = cpu.R[Rn] & (~aluVal);

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4; // Could be an issue?
    }
}

void gbaCPU::DPI_TST(uint32_t opcode)
{
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    shiftAmount *= 2;
    uint32_t aluVal = rotr32(value,shiftAmount);
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }
    uint32_t result = cpu.R[Rn] & aluVal;

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
        }
    }
    if(Rd == 0xF)
    {
        if(cpu.cpsr.T == 0)
        {
            cpu.R[15] += 8;
        }
        if(cpu.cpsr.T == 1)
        {
            cpu.R[15] += 4;
        }
    }
    if(Rd != 0xF)
    {
        cpu.R[15] += 4; // Could be an issue?
    }
}

void gbaCPU::DPI_MOV(uint32_t opcode)
{
    cpu.R[15] += 4;
    //ui.pauseEmulation = true;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    shiftAmount *= 2;
    cpu.R[Rd] = rotr32(value,shiftAmount);
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
        cpu.cpsr.Z = (cpu.R[Rd] == 0);
        cpu.cpsr.C = carryOut;
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
    }

}

void gbaCPU::DPI_MVN(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    shiftAmount *= 2;
    cpu.R[Rd] = (~(rotr32(value,shiftAmount)));
    bool carryOut = cpu.cpsr.C;
    if(shiftAmount != 0)
    {
        carryOut = ((rotr32(value,(shiftAmount - 1)) & 0x80000000) != 0);
    }

    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        cpu.cpsr.N = ((cpu.R[Rd] & 0x80000000) != 0);
        cpu.cpsr.Z = (cpu.R[Rd] == 0);
        cpu.cpsr.C = carryOut;
        cpu.cpsr.V = 1;
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
    }
}

void gbaCPU::LDST_IMD(uint32_t opcode)
{
    cpu.R[15] += 4;
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
            if(Rn == 0xF)
            {
                address += 4;
            }
        break;

        case 1:
            address = cpu.R[Rn] + offset;
            if(Rn == 0xF)
            {
                address += 4;
            }
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,2,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,2,cpu.R[Rd] + 4);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            if(Rd != 15)
            {
                io.writeMem(address,0,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,0,cpu.R[Rd] + 4);
            }
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
}
void gbaCPU::LDST_IMD_PRE(uint32_t opcode)
{
    cpu.R[15] += 4;
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
            if(Rn == 0xF)
            {
                address += 4;
            }
            cpu.R[Rn] = address;
        break;

        case 1:
            address = cpu.R[Rn] + offset;
            if(Rn == 0xF)
            {
                address += 4;
            }
            cpu.R[Rn] = address;
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,2,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,2,cpu.R[Rd] + 4);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            if(Rd != 15)
            {
                io.writeMem(address,0,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,0,cpu.R[Rd] + 4);
            }
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
}
void gbaCPU::LDST_IMD_POST(uint32_t opcode)
{
    cpu.R[15] += 4;
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
            if(Rn == 0xF)
            {
                cpu.R[Rn] += 4;
            }
        break;

        case 1:
            //address = cpu.R[Rn] + offset;
            cpu.R[Rn] = address + offset;
            if(Rn == 0xF)
            {
                cpu.R[Rn] += 4;
            }
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,2,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,2,cpu.R[Rd] + 8);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            if(Rd != 15)
            {
                io.writeMem(address,0,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,0,cpu.R[Rd] + 8);
            }
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
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
        cpu.spsr = writeCPSR(cpu.spsr, curCPSR, false);
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
        cpu.cpsr = writeCPSR(cpu.cpsr, curCPSR, true);
    }
    cpu.R[15] += 4;
}

void gbaCPU::MISC_MOV_I_SR(uint32_t opcode)
{
    //ui.pauseEmulation = ui.allowPauseEmulation;
    uint32_t immedVal = (opcode & 0xFF);
    uint8_t rotAmount = ((opcode >> 8) & 0xF);
    immedVal = rotr32(immedVal, rotAmount * 2);
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
            curCPSR = curCPSR | (immedVal & 0xFF000000);
        }
        if(s == true)
        {
            curCPSR = curCPSR & 0xFF00FFFF;
            curCPSR = curCPSR | (immedVal & 0x00FF0000);
        }
        if(x == true)
        {
            curCPSR = curCPSR & 0xFFFF00FF;
            curCPSR = curCPSR | (immedVal & 0x0000FF00);
        }
        if(c == true)
        {
            curCPSR = curCPSR & 0xFFFFFF00;
            curCPSR = curCPSR | (immedVal & 0x000000FF);
        }
        cpu.spsr = writeCPSR(cpu.spsr, curCPSR, false);
    }
    if(((opcode >> 22) & 0x1) == 0)
    {
        //cpu.R[Rm] = readCPSR(cpu.cpsr);
        uint32_t curCPSR = readCPSR(cpu.cpsr);
        if(f == true)
        {
            curCPSR = curCPSR & 0x00FFFFFF;
            curCPSR = curCPSR | (immedVal & 0xFF000000);
        }
        if(s == true)
        {
            curCPSR = curCPSR & 0xFF00FFFF;
            curCPSR = curCPSR | (immedVal & 0x00FF0000);
        }
        if(x == true)
        {
            curCPSR = curCPSR & 0xFFFF00FF;
            curCPSR = curCPSR | (immedVal & 0x0000FF00);
        }
        if(c == true)
        {
            curCPSR = curCPSR & 0xFFFFFF00;
            curCPSR = curCPSR | (immedVal & 0x000000FF);
        }
        cpu.cpsr = writeCPSR(cpu.cpsr, curCPSR, true);
    }
    cpu.R[15] += 4;
}

void gbaCPU::LDST_HW_REG(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint16_t offset = cpu.R[(opcode & 0xF)];



    bool U, P, W, L;
    // L = Load or Store
    // U = Plus or Minus for offset
    // I = Register or Immediate offset
    // P & W = Addressing Mode
    U = ((opcode >> 23) & 0x1);
    P = ((opcode >> 24) & 0x1);
    W = ((opcode >> 21) & 0x1);
    L = ((opcode >> 20) & 0x1);

    /*
    PW
    00 = Post Indexed
    11 = Pre Indexed
    10 = Normal

    */
    uint8_t PW = (P << 1) | W;
    uint32_t address;
    if(PW == 0)
    {
        address = cpu.R[Rn];
    }
    if(PW != 0)
    {
        switch(U)
        {
            case 0:
                address = cpu.R[Rn] - offset;
            break;

            case 1:
                address = cpu.R[Rn] + offset;
            break;
        }
    }
    switch(L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,1,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,1,cpu.R[Rd] + 8);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,1); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
    if(PW == 3)
    {
        cpu.R[Rn] = address;
    }
    if(PW == 0)
    {
        switch(U)
        {
            case 0:
                cpu.R[Rn] = cpu.R[Rn] - offset;
            break;

            case 1:
                cpu.R[Rn] = cpu.R[Rn] + offset;
            break;
        }
        // Kinda big, need to optimize here
    }
}

void gbaCPU::LDST_HW_IMD(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint32_t offset = (((opcode >> 8) & 0xF) << 4) | (opcode & 0xF);



    bool U, P, W, L;
    // L = Load or Store
    // U = Plus or Minus for offset
    // P & W = Addressing Mode
    U = ((opcode >> 23) & 0x1);
    P = ((opcode >> 24) & 0x1);
    W = ((opcode >> 21) & 0x1);
    L = ((opcode >> 20) & 0x1);

    /*
    PW
    00 = Post Indexed
    11 = Pre Indexed
    10 = Normal

    */
    uint8_t PW = ((P & 0x1) << 1) | (W & 0x1);
    uint32_t address;
    if(PW == 0)
    {
        address = cpu.R[Rn];
    }
    if(PW != 0)
    {
        switch(U)
        {
            case false:
                address = cpu.R[Rn] - offset;
            break;

            case true:
                address = cpu.R[Rn] + offset;
            break;
        }
    }
    if(PW == 3)
    {
        cpu.R[Rn] = address;
    }
    switch(L)
    {
        case false: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,1,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,1,cpu.R[Rd] + 8);
            }
        break;

        case true: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,1); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
    if(PW == 0)
    {
        switch(U)
        {
            case false:
                cpu.R[Rn] = cpu.R[Rn] - offset;
            break;

            case true:
                cpu.R[Rn] = cpu.R[Rn] + offset;
            break;
        }
        // Kinda big, need to optimize here
    }
}

void gbaCPU::LDST_HWB_IMD(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint32_t offset = (((opcode >> 8) & 0xF) << 4) | (opcode & 0xF);



    bool U, P, W, L;
    bool sizeB = ((opcode >> 5) & 0x1);
    // L = Load or Store
    // U = Plus or Minus for offset
    // I = Immediate or Register
    // P & W = Addressing Mode
    U = ((opcode >> 23) & 0x1);
    P = ((opcode >> 24) & 0x1);
    W = ((opcode >> 21) & 0x1);
    L = ((opcode >> 20) & 0x1);

    /*
    PW
    00 = Post Indexed
    11 = Pre Indexed
    10 = Normal

    */
    uint8_t PW = ((P & 0x1) << 1) | (W & 0x1);
    uint32_t address;
    if(PW == 0)
    {
        address = cpu.R[Rn];
    }
    if(PW != 0)
    {
        switch(U)
        {
            case false:
                address = cpu.R[Rn] - offset;
            break;

            case true:
                address = cpu.R[Rn] + offset;
            break;
        }
    }
    if(PW == 3)
    {
        cpu.R[Rn] = address;
        if(ui.allowPauseEmulation == true)
        {
            ui.pauseEmulation = true;
        }
    }
    switch(L)
    {
        case false: // Store Word (32 bit)
            switch(sizeB)
            {
                case false: // Byte
                    if(Rd != 15)
                    {
                        io.writeMem(address,0,cpu.R[Rd]);
                    }
                    if(Rd == 15)
                    {
                        io.writeMem(address,0,cpu.R[Rd] + 8);
                    }
                break;

                case true:
                    if(Rd != 15)
                    {
                        io.writeMem(address,1,cpu.R[Rd]);
                    }
                    if(Rd == 15)
                    {
                        io.writeMem(address,1,cpu.R[Rd] + 8);
                    }
                break;
            }

        break;

        case true: // Load Word (32 bit)
            switch(sizeB)
            {
                case false:
                    {
                        int32_t signExtend = io.readMem(address,0); // TODO, ADD ADDRESS ROTATION
                        //printf("SIGN: 0x%X\n", signExtend);
                        //int64_t signExtend = cpu.R[Rd]
                        signExtend = signExtend << 24;
                        //printf("SIGN3: 0x%X\n", signExtend);
                        signExtend = signExtend >> 24;
                        //printf("SIGN2: 0x%X\n", signExtend);
                        cpu.R[Rd] = signExtend;
                    }
                break;

                case true:
                    {
                        int32_t signExtend = io.readMem(address,1); // TODO, ADD ADDRESS ROTATION
                        //int64_t signExtend = cpu.R[Rd]
                        signExtend = signExtend << 16;
                        signExtend = signExtend >> 16;
                        cpu.R[Rd] = signExtend;
                    }
                break;
            }

        break;
    }
    if(PW == 0)
    {
        switch(U)
        {
            case false:
                cpu.R[Rn] = cpu.R[Rn] - offset;
            break;

            case true:
                cpu.R[Rn] = cpu.R[Rn] + offset;
            break;
        }
        // Kinda big, need to optimize here
    }
}


void gbaCPU::LDST_HWB_REG(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint32_t offset = cpu.R[Rm];



    bool U, P, W, L;
    bool sizeB = ((opcode >> 5) & 0x1);
    // L = Load or Store
    // U = Plus or Minus for offset
    // I = Immediate or Register
    // P & W = Addressing Mode
    U = ((opcode >> 23) & 0x1);
    P = ((opcode >> 24) & 0x1);
    W = ((opcode >> 21) & 0x1);
    L = ((opcode >> 20) & 0x1);

    /*
    PW
    00 = Post Indexed
    11 = Pre Indexed
    10 = Normal

    */
    uint8_t PW = ((P & 0x1) << 1) | (W & 0x1);
    uint32_t address;
    if(PW == 0)
    {
        address = cpu.R[Rn];
    }
    if(PW != 0)
    {
        switch(U)
        {
            case false:
                address = cpu.R[Rn] - offset;
            break;

            case true:
                address = cpu.R[Rn] + offset;
            break;
        }
    }
    if(PW == 3)
    {
        cpu.R[Rn] = address;
        if(ui.allowPauseEmulation == true)
        {
            ui.pauseEmulation = true;
        }
    }
    switch(L)
    {
        case false: // Store Word (32 bit)
            switch(sizeB)
            {
                case false: // Byte
                    if(Rd != 15)
                    {
                        io.writeMem(address,0,cpu.R[Rd]);
                    }
                    if(Rd == 15)
                    {
                        io.writeMem(address,0,cpu.R[Rd] + 8);
                    }
                break;

                case true:
                    if(Rd != 15)
                    {
                        io.writeMem(address,1,cpu.R[Rd]);
                    }
                    if(Rd == 15)
                    {
                        io.writeMem(address,1,cpu.R[Rd] + 8);
                    }
                break;
            }

        break;

        case true: // Load Word (32 bit)
            switch(sizeB)
            {
                case false:
                    {
                        //printf("SIGN\n");
                        int32_t signExtend = io.readMem(address,0); // TODO, ADD ADDRESS ROTATION
                        //printf("0x%X\n",signExtend);
                        //int64_t signExtend = cpu.R[Rd]
                        signExtend = signExtend << 24;
                        signExtend = signExtend >> 24;
                        cpu.R[Rd] = signExtend;
                    }
                break;

                case true:
                    {
                        int32_t signExtend = io.readMem(address,1); // TODO, ADD ADDRESS ROTATION
                        //int64_t signExtend = cpu.R[Rd]
                        signExtend = signExtend << 16;
                        signExtend = signExtend >> 16;
                        cpu.R[Rd] = signExtend;
                    }
                break;
            }

        break;
    }
    if(PW == 0)
    {
        switch(U)
        {
            case false:
                cpu.R[Rn] = cpu.R[Rn] - offset;
            break;

            case true:
                cpu.R[Rn] = cpu.R[Rn] + offset;
            break;
        }
        // Kinda big, need to optimize here
    }
}

void gbaCPU::LDMIA(uint32_t opcode)
{
    ui.pauseEmulation = ui.allowPauseEmulation;
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint16_t regList = (opcode & 0xFFFF);
    bool P,U,W,S;
    P = ((opcode >> 24) & 0x1);
    U = ((opcode >> 23) & 0x1);
    W = ((opcode >> 21) & 0x1);
    S = ((opcode >> 22) & 0x1);

    uint8_t setBits = 0;
    bool setBitArray[16];
    for(int i = 0; i < 16; i++)
    {
        setBitArray[i] = false;
        if(((regList >> i) & 0x1) != 0)
        {
            setBits++;
            setBitArray[i] = true;
        }
    }

    uint32_t startAddress = cpu.R[Rn];
    switch((P << 1) | U)
    {
        case 0: // Decrement After
            startAddress -= (setBits * 4);
            startAddress += 4;
        break;

        case 1: // Increment After

        break;

        case 2: // Decrement Before
            startAddress -= (setBits * 4);
        break;

        case 3: // Increment Before
            startAddress += 4;
        break;
    }

    uint8_t cpsrModeCurrent = cpu.cpsr.mode;
    if(S == 1 && setBitArray[15] == false)
    {
        modeSwitch(0x1F);
    }

    uint32_t endAddress = cpu.R[Rn] + (setBits * 4);
    if(P == 0)
    {
        endAddress -= 4;
    }


    for(int i = 0; i < 16; i++)
    {
        if(setBitArray[i] == true)
        {
            cpu.R[i] = io.readMem(startAddress, 2); // Is this correct?
            startAddress += 4;
            if(i == 15)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        }
    }
    if(S == 1 && setBitArray[15] == false)
    {
        modeSwitch(cpsrModeCurrent);
    }
    if(W == 1 && setBitArray[Rn] != true)
    {
        switch(U)
        {
            case 0:
                cpu.R[Rn] = cpu.R[Rn] - (setBits * 4);
            break;

            case 1:
                cpu.R[Rn] = cpu.R[Rn] + (setBits * 4);
            break;
        }
    }
    if(S == 1 && setBitArray[15] == true)
    {
        cpu.cpsr = cpu.spsr;
    }
}

void gbaCPU::STMIA(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint16_t regList = (opcode & 0xFFFF);
    bool P,U,W,S;
    P = ((opcode >> 24) & 0x1);
    U = ((opcode >> 23) & 0x1);
    W = ((opcode >> 21) & 0x1);
    S = ((opcode >> 22) & 0x1);



    uint8_t setBits = 0;
    bool setBitArray[16];
    for(int i = 0; i < 16; i++)
    {
        setBitArray[i] = false;
        if(((regList >> i) & 0x1) != 0)
        {
            setBits++;
            setBitArray[i] = true;
        }
    }

    uint32_t startAddress = cpu.R[Rn];
    switch((P << 1) | U)
    {
        case 0: // Decrement After
            startAddress -= (setBits * 4);
            startAddress += 4;
        break;

        case 1: // Increment After

        break;

        case 2: // Decrement Before
            startAddress -= (setBits * 4);
        break;

        case 3: // Increment Before
            startAddress += 4;
        break;
    }

    uint8_t cpsrModeCurrent = cpu.cpsr.mode;
    if(S == 1)
    {
        modeSwitch(0x1F);
    }

    uint32_t endAddress = cpu.R[Rn] + (setBits * 4);
    if(P == 0)
    {
        endAddress -= 4;
    }


    for(int i = 0; i < 16; i++)
    {
        if(setBitArray[i] == true)
        {
            if(i != 15)
            {
                io.writeMem(startAddress, 2, cpu.R[i]);
            }
            if(i == 15)
            {
                io.writeMem(startAddress, 2, cpu.R[i]);
            }
            startAddress += 4;
        }
    }

    //uint8_t cpsrModeCurrent = cpu.cpsr.mode;
    if(S == 1)
    {
        modeSwitch(cpsrModeCurrent);
    }
    if(W == 1)
    {
        switch(U)
        {
            case 0:
                cpu.R[Rn] = cpu.R[Rn] - (setBits * 4);
            break;

            case 1:
                cpu.R[Rn] = cpu.R[Rn] + (setBits * 4);
            break;
        }
    }
}

void gbaCPU::DPIS_AND(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] & aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPRS_ORR(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = cpu.R[((opcode >> 8) & 0xF)];
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = (cpu.R[Rm] << shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            }
            else if(shiftAmount == 32)
            {
                aluVal = 0;
                carryOut = (cpu.R[Rm] & 0x1);
            }
            else
            {
                aluVal = 0;
                carryOut = 0;
            }
        break;

        case 1: // LSR
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = (cpu.R[Rm] >> shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
            else if(shiftAmount == 32)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
            else
            {
                aluVal = 0;
                carryOut = 0;
            }
        break;

        case 2: // ASR
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
            else
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if((shiftAmount & 0xF) == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
            else
            {
                aluVal = rotr32(cpu.R[Rm],(shiftAmount & 0xF) );
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] | aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPRS_AND(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = cpu.R[((opcode >> 8) & 0xF)];
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = (cpu.R[Rm] << shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            }
            else if(shiftAmount == 32)
            {
                aluVal = 0;
                carryOut = (cpu.R[Rm] & 0x1);
            }
            else
            {
                aluVal = 0;
                carryOut = 0;
            }
        break;

        case 1: // LSR
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = (cpu.R[Rm] >> shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
            else if(shiftAmount == 32)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
            else
            {
                aluVal = 0;
                carryOut = 0;
            }
        break;

        case 2: // ASR
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
            else
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if((shiftAmount & 0xF) == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
            else
            {
                aluVal = rotr32(cpu.R[Rm],(shiftAmount & 0xF) );
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] & aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPRS_MOV(uint32_t opcode)
{
    cpu.R[15] += 4;
    //ui.pauseEmulation = true;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = cpu.R[((opcode >> 8) & 0xF)];
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    if(Rm == 0xF)
    {
        cpu.R[Rm] += 8;
    }

    switch(addrMode)
    {
        case 0: // LSL
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = (cpu.R[Rm] << shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            }
            else if(shiftAmount == 32)
            {
                aluVal = 0;
                carryOut = (cpu.R[Rm] & 0x1);
            }
            else
            {
                aluVal = 0;
                carryOut = 0;
            }
        break;

        case 1: // LSR
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = (cpu.R[Rm] >> shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
            else if(shiftAmount == 32)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
            else
            {
                aluVal = 0;
                carryOut = 0;
            }
        break;

        case 2: // ASR
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if(shiftAmount < 32)
            {
                aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
            else
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
            else if((shiftAmount & 0xF) == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
            else
            {
                aluVal = rotr32(cpu.R[Rm],(shiftAmount & 0xF) );
                carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            }
        break;
    }


    if(Rm == 0xF)
    {
        cpu.R[Rm] -= 8;
    }
    uint32_t result = aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

/*
void gbaCPU::DPIS_MOV(uint32_t opcode)
{
    if(ui.allowPauseEmulation == true)
    {
        //ui.pauseEmulation = true;
    }
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            //cpu.cpsr.Z = 1;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
    if(Rd != 0xF)
    {
        cpu.R[15] += 4; // Could be an issue?
    }
    if(Rd == 0xF)
    {
        if(cpu.cpsr.T == 0)
        {
            cpu.R[15] += 8;
        }
        if(cpu.cpsr.T == 1)
        {
            cpu.R[15] += 4;
            //printf("PC: 0x%X\n",cpu.R[15]);
            //ui.pauseEmulation = true;
            //ui.runNoStep = false;
            //ui.useRunTo = false;
        }
    }
}
*/

void gbaCPU::DPIS_MOV(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;

    if(Rm == 0xF)
    {
        cpu.R[Rm] += 4;
    }
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = (aluVal);
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
        }
    }
    if(Rm == 0xF)
    {
        cpu.R[Rm] -= 4;
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_MVN(uint32_t opcode)
{
    if(ui.allowPauseEmulation)
    {
        ui.pauseEmulation = true;
    }
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    if(Rm == 0xF)
    {
        cpu.R[Rm] += 4;
    }
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = (~aluVal);
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            cpu.cpsr.V = 1;
        }
    }
    if(Rm == 0xF)
    {
        cpu.R[Rm] -= 4;
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_TST(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] & aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
}

void gbaCPU::DPIS_BIC(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] & (~aluVal);
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_ORR(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] | aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_EOR(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] ^ aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_TEQ(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] ^ aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = carryOut;
            // MISSING V IMPLEMENTATION, SEE PG. 107
        }
    }
}

void gbaCPU::DPIS_ADD(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] + aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)aluVal));
            cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_CMN(uint32_t opcode)
{
    cpu.R[15] += 4;
    //ui.pauseEmulation = true;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] + aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)aluVal));
            cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ (uint64_t)cpu.R[Rm] ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
}

void gbaCPU::DPIS_SUB(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] - aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)aluVal));
            cpu.cpsr.V = (( (uint64_t)cpu.R[Rn] ^ (uint64_t)aluVal ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_RSB(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = aluVal - cpu.R[Rn];
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)aluVal - (uint64_t)cpu.R[Rn]));
            cpu.cpsr.V = (( (uint64_t)aluVal ^ (uint64_t)cpu.R[Rn] ) &
                     ( ( (uint64_t)aluVal ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_RSC(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = aluVal - cpu.R[Rn] - (!cpu.cpsr.C);
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)aluVal - (uint64_t)cpu.R[Rn] - (uint64_t)(!cpu.cpsr.C)));
            cpu.cpsr.V = ( ( (uint64_t)aluVal ^ ( (uint64_t)cpu.R[Rn] - (uint64_t)(!cpu.cpsr.C) ) ) &
                     ( ( (uint64_t)aluVal ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_CMP(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] - aluVal;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)aluVal));
            cpu.cpsr.V = (( (uint64_t)cpu.R[Rn] ^ (uint64_t)aluVal ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
}

void gbaCPU::DPIS_SBC(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] - aluVal - (!cpu.cpsr.C);
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF >= ((uint64_t)cpu.R[Rn] - (uint64_t)aluVal - (uint64_t)(!cpu.cpsr.C)));
            cpu.cpsr.V = ( ( (uint64_t)cpu.R[Rn] ^ ( (uint64_t)aluVal - (uint64_t)(!cpu.cpsr.C) ) ) &
                     ( ( (uint64_t)cpu.R[Rn] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::DPIS_ADC(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint32_t value = (opcode & 0xFF);
    uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t addrMode = ((opcode >> 5) & 0x3);
    uint32_t aluVal = 0;
    bool carryOut = 0;
    switch(addrMode)
    {
        case 0: // LSL
            aluVal = (cpu.R[Rm] << shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] << ( shiftAmount - 1 ) ) & 0x80000000 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = cpu.R[Rm];
                carryOut = cpu.cpsr.C;
            }
        break;

        case 1: // LSR
            aluVal = (cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                aluVal = 0;
                carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
            }
        break;

        case 2: // ASR
            aluVal = ((int32_t)cpu.R[Rm] >> shiftAmount);
            carryOut = ( ( ( cpu.R[Rm] >> ( shiftAmount - 1 ) ) & 0x1 ) != 0);
            if(shiftAmount == 0)
            {
                switch((cpu.R[Rm] & 0x80000000))
                {
                    case 0:
                        aluVal = 0;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;

                    case 0x80000000:
                        aluVal = 0xFFFFFFFF;
                        carryOut = ((cpu.R[Rm] & 0x80000000) != 0);
                    break;
                }
            }
        break;

        case 3: // RR or RRX
            if(shiftAmount == 0) // RRX
            {
                aluVal = ((cpu.cpsr.C << 31) | (cpu.R[Rm] >> 1));
                carryOut = ((cpu.R[Rm] & 0x1) != 0);
            }
            if(shiftAmount != 0) // RX
            {
                aluVal = rotr32(cpu.R[Rm],shiftAmount);
                carryOut = ((aluVal & 0x80000000) != 0);
            }
        break;
    }


    uint32_t result = cpu.R[Rn] + aluVal + cpu.cpsr.C;
    if(((opcode >> 20) & 0x1) != 0) // If bit 20 is 1
    {
        if(Rd == 0xF)
        {
            //cpu.cpsr = cpu.spsr;
            cpu.cpsr = writeCPSR(cpu.cpsr, readCPSR(cpu.spsr),true);
        }
        if(Rd != 0xF)
        {
            cpu.cpsr.N = ((result & 0x80000000) != 0);
            cpu.cpsr.Z = (result == 0);
            cpu.cpsr.C = (0xFFFFFFFF < ((uint64_t)cpu.R[Rn] + (uint64_t)aluVal + (uint64_t)cpu.cpsr.C));
            cpu.cpsr.V = ( ~ ( (uint64_t)cpu.R[Rd] ^ ( (uint64_t)cpu.R[Rm] + (uint64_t)cpu.cpsr.C ) ) &
                     ( ( (uint64_t)cpu.R[Rd] ^ result ) ) &
                     0x80000000 ) != 0;
        }
    }
    cpu.R[Rd] = result;
}

void gbaCPU::MUL(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rs = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 16) & 0xF);
    uint32_t result = 0;

    result = (cpu.R[Rm] * cpu.R[Rs]);

    bool S = ((opcode >> 20) & 0x1);
    if(S == 1)
    {
        cpu.cpsr.N = ((result & 0x80000000) != 0);
        cpu.cpsr.Z = (result == 0);
    }
    cpu.R[Rd] = result;
}

void gbaCPU::SWP(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint32_t memData = io.readMem(cpu.R[Rn],2);
    io.writeMem(cpu.R[Rn], 2, cpu.R[Rm]);
    cpu.R[Rd] = memData;
}

void gbaCPU::SWPB(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint32_t memData = io.readMem(cpu.R[Rn],0);
    io.writeMem(cpu.R[Rn], 0, cpu.R[Rm]);
    cpu.R[Rd] = memData;
}

void gbaCPU::UMULL(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rs = ((opcode >> 8) & 0xF);
    uint8_t RdHi = ((opcode >> 16) & 0xF);
    uint8_t RdLo = ((opcode >> 12) & 0xF);
    uint64_t result = 0;

    result = ((uint64_t)cpu.R[Rm] * (uint64_t)cpu.R[Rs]);

    uint64_t andValue = (0x1 << 63);

    bool S = ((opcode >> 20) & 0x1);
    if(S == 1)
    {
        cpu.cpsr.N = ((result & andValue) != 0); // This looks weird lol
        cpu.cpsr.Z = (result == 0);
    }
    cpu.R[RdHi] = result >> 32;
    cpu.R[RdLo] = result;
}

void gbaCPU::UMLAL(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rs = ((opcode >> 8) & 0xF);
    uint8_t RdHi = ((opcode >> 16) & 0xF);
    uint8_t RdLo = ((opcode >> 12) & 0xF);
    uint64_t result = 0;

    result = ((uint64_t)cpu.R[Rm] * (uint64_t)cpu.R[Rs]);

    uint64_t addVal = ((cpu.R[RdHi] << 32) | cpu.R[RdLo]);

    //result += addVal;

    uint64_t andValue = (0x1 << 63);

    cpu.R[RdHi] = (result >> 32) + cpu.R[RdHi] + (0xFFFFFFFF > ((uint64_t)cpu.R[RdLo] + (uint64_t)result));
    cpu.R[RdLo] = result + cpu.R[RdLo];

    bool S = ((opcode >> 20) & 0x1);
    if(S == 1)
    {
        cpu.cpsr.N = ((cpu.R[RdHi] & 0x80000000) != 0); // This looks weird lol
        cpu.cpsr.Z = (cpu.R[RdHi] == 0 && cpu.R[RdLo] == 0);
    }
}

void gbaCPU::SMLAL(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rs = ((opcode >> 8) & 0xF);
    uint8_t RdHi = ((opcode >> 16) & 0xF);
    uint8_t RdLo = ((opcode >> 12) & 0xF);
    int64_t result = 0;
    int64_t signExtend, signExtend2;
    signExtend = cpu.R[Rm];
    signExtend2 = cpu.R[Rs];
    signExtend = signExtend << 32;
    signExtend = signExtend >> 32;
    signExtend2 = signExtend2 << 32;
    signExtend2 = signExtend2 >> 32;

    result = (signExtend2 * signExtend);

    cpu.R[RdHi] = (result >> 32) + cpu.R[RdHi] + (0xFFFFFFFF > ((uint64_t)cpu.R[RdLo] + (uint64_t)result));
    cpu.R[RdLo] = result + cpu.R[RdLo];

    uint64_t andValue = (0x1 << 63);

    bool S = ((opcode >> 20) & 0x1);
    if(S == 1)
    {
        cpu.cpsr.N = ((cpu.R[RdHi] & 0x80000000) != 0); // This looks weird lol
        cpu.cpsr.Z = (cpu.R[RdHi] == 0 && cpu.R[RdLo] == 0);
    }
    //cpu.R[RdHi] = result >> 32;
    //cpu.R[RdLo] = result;
}

void gbaCPU::SMULL(uint32_t opcode)
{
    cpu.R[15] += 4;
    if(ui.allowPauseEmulation == true)
    {
        ui.pauseEmulation = true;
    }
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rs = ((opcode >> 8) & 0xF);
    uint8_t RdHi = ((opcode >> 16) & 0xF);
    uint8_t RdLo = ((opcode >> 12) & 0xF);
    int64_t result = 0;
    int64_t signExtend, signExtend2;
    signExtend = cpu.R[Rm];
    signExtend2 = cpu.R[Rs];
    signExtend = signExtend << 32;
    signExtend = signExtend >> 32;
    signExtend2 = signExtend2 << 32;
    signExtend2 = signExtend2 >> 32;

    result = (signExtend2 * signExtend);

    uint64_t andValue = (0x1 << 63);

    bool S = ((opcode >> 20) & 0x1);
    if(S == 1)
    {
        cpu.cpsr.N = ((result & 0x8000000000000000) != 0); // This looks weird lol
        cpu.cpsr.Z = (result == 0);
    }
    cpu.R[RdHi] = result >> 32;
    cpu.R[RdLo] = result;
}

void gbaCPU::MLA(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rm = ((opcode) & 0xF);
    uint8_t Rs = ((opcode >> 8) & 0xF);
    uint8_t Rd = ((opcode >> 16) & 0xF);
    uint8_t Rn = ((opcode >> 12) & 0xF);
    uint32_t result = 0;

    result = ((cpu.R[Rm] * cpu.R[Rs]) + cpu.R[Rn]);

    bool S = ((opcode >> 20) & 0x1);
    if(S == 1)
    {
        cpu.cpsr.N = ((result & 0x80000000) != 0);
        cpu.cpsr.Z = (result == 0);
    }
    cpu.R[Rd] = result;
}

void gbaCPU::LDST_REG(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint32_t offset = cpu.R[Rm];
    if(Rm == 0xF)
    {
        offset += 4;
    }
    uint32_t scaledReg = ((opcode >> 4) & 0xFF);
    bool U, B, L;
    U = ((opcode >> 23) & 0x1);
    B = ((opcode >> 22) & 0x1);
    L = ((opcode >> 20) & 0x1);
    uint32_t address;
    if(scaledReg != 0)
    {
        uint8_t shiftType = ((opcode >> 5) & 0x3);
        uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
        switch(shiftType)
        {
            case 0: // LSL
                offset = offset << shiftAmount;
            break;

            case 1: // LSR
                offset = offset >> shiftAmount;
                if(shiftAmount == 0)
                {
                    offset = 0;
                }
            break;

            case 2: // ASR
                if(shiftAmount == 0)
                {
                    if((offset & 0x80000000) != 0)
                    {
                        offset = 0xFFFFFFFF;
                    }
                    if((offset & 0x80000000) == 0)
                    {
                        offset = 0;
                    }
                }
                if(shiftAmount != 0)
                {
                    offset = (int32_t)offset >> shiftAmount;
                }
            break;

            case 3:
                if(shiftAmount == 0) // RRX
                {
                    offset = (cpu.cpsr.C << 31) | (offset >> 1);
                }
                if(shiftAmount != 0) // ROR
                {
                    offset = rotr32(offset,shiftAmount);
                }
            break;
        }
    }
    switch(U)
    {
        case 0:
            address = cpu.R[Rn] - offset;
            if(Rn == 0xF)
            {
                address += 4;
            }
        break;

        case 1:
            address = cpu.R[Rn] + offset;
            if(Rn == 0xF)
            {
                address += 4;
            }
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,2,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,2,cpu.R[Rd] + 4);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            if(Rd != 15)
            {
                io.writeMem(address,0,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,0,cpu.R[Rd] + 4);
            }
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
}

void gbaCPU::LDST_REG_PRE(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint32_t offset = cpu.R[Rm];
    if(Rm == 0xF)
    {
        offset += 4;
    }
    uint32_t scaledReg = ((opcode >> 4) & 0xFF);
    bool U, B, L;
    U = ((opcode >> 23) & 0x1);
    B = ((opcode >> 22) & 0x1);
    L = ((opcode >> 20) & 0x1);
    uint32_t address;
    if(scaledReg != 0)
    {
        uint8_t shiftType = ((opcode >> 5) & 0x3);
        uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
        switch(shiftType)
        {
            case 0: // LSL
                offset = offset << shiftAmount;
            break;

            case 1: // LSR
                offset = offset >> shiftAmount;
                if(shiftAmount == 0)
                {
                    offset = 0;
                }
            break;

            case 2: // ASR
                if(shiftAmount == 0)
                {
                    if((offset & 0x80000000) != 0)
                    {
                        offset = 0xFFFFFFFF;
                    }
                    if((offset & 0x80000000) == 0)
                    {
                        offset = 0;
                    }
                }
                if(shiftAmount != 0)
                {
                    offset = (int32_t)offset >> shiftAmount;
                }
            break;

            case 3:
                if(shiftAmount == 0) // RRX
                {
                    offset = (cpu.cpsr.C << 31) | (offset >> 1);
                }
                if(shiftAmount != 0) // ROR
                {
                    offset = rotr32(offset,shiftAmount);
                }
            break;
        }
    }
    switch(U)
    {
        case 0:
            address = cpu.R[Rn] - offset;
            if(Rn == 0xF)
            {
                address += 4;
            }
            cpu.R[Rn] = address;
        break;

        case 1:
            address = cpu.R[Rn] + offset;
            if(Rn == 0xF)
            {
                address += 4;
            }
            cpu.R[Rn] = address;
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,2,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,2,cpu.R[Rd] + 4);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            if(Rd != 15)
            {
                io.writeMem(address,0,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,0,cpu.R[Rd] + 4);
            }
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
}

void gbaCPU::LDST_REG_POST(uint32_t opcode)
{
    cpu.R[15] += 4;
    uint8_t Rn = ((opcode >> 16) & 0xF);
    uint8_t Rd = ((opcode >> 12) & 0xF);
    uint8_t Rm = ((opcode) & 0xF);
    uint32_t offset = cpu.R[Rm];
    if(Rm == 0xF)
    {
        offset += 4;
    }
    uint32_t scaledReg = ((opcode >> 4) & 0xFF);
    bool U, B, L;
    U = ((opcode >> 23) & 0x1);
    B = ((opcode >> 22) & 0x1);
    L = ((opcode >> 20) & 0x1);
    uint32_t address;
    if(scaledReg != 0)
    {
        uint8_t shiftType = ((opcode >> 5) & 0x3);
        uint8_t shiftAmount = ((opcode >> 7) & 0x1F);
        switch(shiftType)
        {
            case 0: // LSL
                offset = offset << shiftAmount;
            break;

            case 1: // LSR
                offset = offset >> shiftAmount;
                if(shiftAmount == 0)
                {
                    offset = 0;
                }
            break;

            case 2: // ASR
                if(shiftAmount == 0)
                {
                    if((offset & 0x80000000) != 0)
                    {
                        offset = 0xFFFFFFFF;
                    }
                    if((offset & 0x80000000) == 0)
                    {
                        offset = 0;
                    }
                }
                if(shiftAmount != 0)
                {
                    offset = (int32_t)offset >> shiftAmount;
                }
            break;

            case 3:
                if(shiftAmount == 0) // RRX
                {
                    offset = (cpu.cpsr.C << 31) | (offset >> 1);
                }
                if(shiftAmount != 0) // ROR
                {
                    offset = rotr32(offset,shiftAmount);
                }
            break;
        }
    }
    switch(U)
    {
        case 0:
            address = cpu.R[Rn];
            if(Rn == 0xF)
            {
                cpu.R[Rn] += 4;
            }
            cpu.R[Rn] -= offset;
        break;

        case 1:
            address = cpu.R[Rn];
            if(Rn == 0xF)
            {
                cpu.R[Rn] += 4;
            }
            cpu.R[Rn] += offset;
        break;
    }
    switch(B << 1 | L)
    {
        case 0: // Store Word (32 bit)
            if(Rd != 15)
            {
                io.writeMem(address,2,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,2,cpu.R[Rd] + 4);
            }
        break;

        case 1: // Load Word (32 bit)
            cpu.R[Rd] = io.readMem(address,2); // TODO, ADD ADDRESS ROTATION
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;

        case 2: // Store Byte
            if(Rd != 15)
            {
                io.writeMem(address,0,cpu.R[Rd]);
            }
            if(Rd == 15)
            {
                io.writeMem(address,0,cpu.R[Rd] + 4);
            }
        break;

        case 3: // Load Byte
            cpu.R[Rd] = io.readMem(address,0);
            if(Rd == 0xF)
            {
                cpu.R[15] = cpu.R[15] & 0xFFFFFFFC;
            }
        break;
    }
}
