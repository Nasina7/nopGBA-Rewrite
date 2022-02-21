#include <stdint.h>
#define rotl32(x,a) ((x << a) | (x >> (32 - a)))
#define rotl16(x,a) ((x << a) | (x >> (16 - a)))
#define rotl8(x,a) ((x << a) | (x >> (8 - a)))
#define rotr32(x,a) ((x >> a) | (x << (32 - a)))
#define rotr16(x,a) ((x >> a) | (x << (16 - a)))
#define rotr8(x,a) ((x >> a) | (x << (8 - a)))

#define opPerFrame 70224
// Normally 280896, but having trouble optimizing emu rn.
#define modeArm 0
#define modeThumb 1

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
//typedef void (*thumbLookup)();
class gbaCPU {
    public:
        typedef void (thumbLookup)(short unsigned int);
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


        static void doOpcode();
        void handleTimers();
        static void doInterrupts();
        static void handleDMA();
        static uint32_t readCPSR(cpsrS value);
        cpsrS writeCPSR(cpsrS value, uint32_t value2, bool doModeSwitch);
        uint8_t modeSwitch(uint8_t switchMode);
        uint32_t lastOpcodeRan;
        static void generateThumbLookup();


        int64_t timerScheduler[4];
        int timerFreqLookup[4] =
        {
            1,
            64,
            256,
            1024
        };




        // Thumb

        void T_INVALID(uint16_t opcode);

        void T_SWI(uint16_t opcode);

        void T_SDP_ADD(uint16_t opcode);
        void T_SDP_CMP(uint16_t opcode);
        void T_SDP_MOV(uint16_t opcode);

        void T_STR_IMD(uint16_t opcode);
        void T_STR_SP(uint16_t opcode);
        void T_STRH_IMD(uint16_t opcode);
        void T_STRB_IMD(uint16_t opcode);
        void T_LDR_IMD(uint16_t opcode);
        void T_LDR_SP(uint16_t opcode);
        void T_LDRH_IMD(uint16_t opcode);
        void T_LDRB_IMD(uint16_t opcode);

        void T_LDMIA(uint16_t opcode);
        void T_STMIA(uint16_t opcode);
        void T_PUSH(uint16_t opcode);
        void T_POP(uint16_t opcode);

        void T_ADD_SIMD(uint16_t opcode);
        void T_SUB_SIMD(uint16_t opcode);

        void T_ADJ_STACK(uint16_t opcode);

        void T_LDST_REG(uint16_t opcode);
        void T_LDSTSBH_REG(uint16_t opcode);

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

        void T_ADD_SP(uint16_t opcode);
        void T_ADD_PC(uint16_t opcode);

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


        void T_B(uint16_t opcode);
        void T_BL(uint16_t opcode);
        void T_BX(uint16_t opcode);

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

        void doAudioDMATransfer(uint32_t startAddrDMA, uint32_t endAddrDMA, uint32_t wordCountDMA, uint32_t dmaControl);
        void runARM(uint32_t opcode);
    private:
        void decodeAndRunARM();
        void decodeAndRunTHUMB();
        void runTHUMB(uint16_t opcode);





        // Full opcode list
        // ARM

        void SWI(uint32_t opcode);
        void STMIA(uint32_t opcode);
        void LDMIA(uint32_t opcode);

        void MUL(uint32_t opcode);
        void UMULL(uint32_t opcode);
        void UMLAL(uint32_t opcode);
        void SMULL(uint32_t opcode);
        void SMLAL(uint32_t opcode);
        void MLA(uint32_t opcode);

        void SWP(uint32_t opcode);
        void SWPB(uint32_t opcode);


        void DPIS_MOV(uint32_t opcode);
        void DPIS_ADD(uint32_t opcode);
        void DPIS_ADC(uint32_t opcode);
        void DPIS_AND(uint32_t opcode);
        void DPIS_BIC(uint32_t opcode);
        void DPIS_CMN(uint32_t opcode);
        void DPIS_CMP(uint32_t opcode);
        void DPIS_EOR(uint32_t opcode);
        void DPIS_MVN(uint32_t opcode);
        void DPIS_ORR(uint32_t opcode);
        void DPIS_RSB(uint32_t opcode);
        void DPIS_RSC(uint32_t opcode);
        void DPIS_SBC(uint32_t opcode);
        void DPIS_SUB(uint32_t opcode);
        void DPIS_TEQ(uint32_t opcode);
        void DPIS_TST(uint32_t opcode);

        void DPRS_AND(uint32_t opcode);
        void DPRS_SUB(uint32_t opcode);
        void DPRS_ADD(uint32_t opcode);
        void DPRS_ADC(uint32_t opcode);
        void DPRS_ORR(uint32_t opcode);
        void DPRS_MOV(uint32_t opcode);

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

        void LDST_REG(uint32_t opcode);
        void LDST_REG_PRE(uint32_t opcode);
        void LDST_REG_POST(uint32_t opcode);

        void LDST_HW_IMD(uint32_t opcode);
        void LDST_HW_REG(uint32_t opcode);

        void LDST_HWB_IMD(uint32_t opcode);
        void LDST_HWB_REG(uint32_t opcode);

        void MISC_MOV_SR_R(uint32_t opcode);
        void MISC_MOV_R_SR(uint32_t opcode);
        void MISC_MOV_I_SR(uint32_t opcode);

};

class gbCPU {
    public:


};











extern gbaCPU cpu;
