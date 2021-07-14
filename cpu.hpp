#include <stdint.h>
#define rotl32(x,a) ((x << a) | (x >> (32 - a)))
#define rotl16(x,a) ((x << a) | (x >> (16 - a)))
#define rotl8(x,a) ((x << a) | (x >> (8 - a)))
#define rotr32(x,a) ((x >> a) | (x << (32 - a)))
#define rotr16(x,a) ((x >> a) | (x << (16 - a)))
#define rotr8(x,a) ((x >> a) | (x << (8 - a)))


struct cpsrS {
    bool N;
    bool Z;
    bool C;
    bool V;
    bool Q;
    bool res1[2]; // Bits 26 & 25
    uint16_t res2; // Bits 24 to 8
    bool I;
    bool F;
    bool T;
    uint8_t mode;   // Only bits 0-4 should be writable!  Use & 0x1F whenever accessing
};

class gbaCPU {
    public:

        uint32_t R[16]; // R Registers
        uint64_t cycles;
        uint64_t opcodesRan;
        uint8_t scanline;

        cpsrS cpsr;
        cpsrS prevCpsr; // All registers this point and below need to be rewritten when working on mode switching

        cpsrS spsr;
        cpsrS spsr_svc;
        cpsrS spsr_irq;

        uint32_t R1314_svc[2];
        uint32_t R1314_irq[2];


        void doOpcode();
        uint32_t readCPSR(cpsrS value);
        cpsrS writeCPSR(cpsrS value, uint32_t value2);
    private:
        void decodeAndRunARM();
        void runARM(uint32_t opcode);
        void decodeAndRunTHUMB();
        void runTHUMB(uint16_t opcode);




        // Full opcode list
        // ARM
        void BL(uint32_t opcode);
        void BX(uint32_t opcode);
        void DPI_MOV(uint32_t opcode);
        void DPI_ADD(uint32_t opcode);

        void DPI_ADC(uint32_t opcode);
        void DPI_AND(uint32_t opcode);
        void DPI_BIC(uint32_t opcode);
        void DPI_CMN(uint32_t opcode);
        void DPI_CMP(uint32_t opcode);
        void DPI_EOR(uint32_t opcode);
        void DPI_MVN(uint32_t opcode);
        void DPI_ORR(uint32_t opcode);
        void DPI_RSB(uint32_t opcode);
        void DPI_RSC(uint32_t opcode);
        void DPI_SBC(uint32_t opcode);
        void DPI_SUB(uint32_t opcode);
        void DPI_TEQ(uint32_t opcode);
        void DPI_TST(uint32_t opcode);


        void DPR_ADD(uint32_t opcode);
        void LDST_IMD(uint32_t opcode);
        void LDST_IMD_PRE(uint32_t opcode);
        void LDST_IMD_POST(uint32_t opcode);

        void MISC_MOV_SR_R(uint32_t opcode);
        void MISC_MOV_R_SR(uint32_t opcode);


        // Thumb
        void T_LD_LP(uint16_t opcode);
        void T_LSL_IMD(uint16_t opcode);
        void T_LSR_IMD(uint16_t opcode);
        void T_ASR_IMD(uint16_t opcode);

        void T_CMP_IMD(uint16_t opcode);
        void T_ADD_IMD(uint16_t opcode);
        void T_SUB_IMD(uint16_t opcode);
        void T_MOV_IMD(uint16_t opcode);

        void T_ADD_REG(uint16_t opcode);
        void T_SUB_REG(uint16_t opcode);

        void T_AND_ALU(uint16_t opcode);
        void T_EOR_ALU(uint16_t opcode);
        void T_LSL_ALU(uint16_t opcode);
        void T_LSR_ALU(uint16_t opcode);
        void T_ASR_ALU(uint16_t opcode);
        void T_ADC_ALU(uint16_t opcode);
        void T_SBC_ALU(uint16_t opcode);
        void T_ROR_ALU(uint16_t opcode);
        void T_TST_ALU(uint16_t opcode);
        void T_NEG_ALU(uint16_t opcode);
        void T_CMP_ALU(uint16_t opcode);
        void T_CMN_ALU(uint16_t opcode);
        void T_ORR_ALU(uint16_t opcode);
        void T_MUL_ALU(uint16_t opcode);
        void T_BIC_ALU(uint16_t opcode);
        void T_MVN_ALU(uint16_t opcode);


        void T_BL(uint16_t opcode);

        void T_B_EQ(uint16_t opcode);
        void T_B_NE(uint16_t opcode);
        void T_B_CS(uint16_t opcode);
        void T_B_CC(uint16_t opcode);
        void T_B_MI(uint16_t opcode);
        void T_B_PL(uint16_t opcode);
        void T_B_VS(uint16_t opcode);
        void T_B_VC(uint16_t opcode);
        void T_B_HI(uint16_t opcode);
        void T_B_LS(uint16_t opcode);
        void T_B_GE(uint16_t opcode);
        void T_B_LT(uint16_t opcode);
        void T_B_GT(uint16_t opcode);
        void T_B_LE(uint16_t opcode);
};

class gbCPU {
    public:


};











extern gbaCPU cpu;
