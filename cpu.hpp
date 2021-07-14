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
        void decodeAndRunTHUMB(uint16_t opcode);




        // Full opcode list
        // ARM
        void BL(uint32_t opcode);
        void DPI_MOV(uint32_t opcode);

        void DPR_ADD(uint32_t opcode);
        void LDST_IMD(uint32_t opcode);
        void LDST_IMD_PRE(uint32_t opcode);
        void LDST_IMD_POST(uint32_t opcode);

        void MISC_MOV_SR_R(uint32_t opcode);
        void MISC_MOV_R_SR(uint32_t opcode);


        // Thumb

};











extern gbaCPU cpu;
