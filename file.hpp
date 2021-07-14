#include <iostream>

class fileLoadClass {
    public:
        bool loadBIOS();
        bool loadROM(const char* filename);
    private:
        FILE* rom;
        FILE* bios;

};

extern fileLoadClass fileLoad;


/*
    Function that loads a rom file (takes char* argument)
    Function that loads the BIOS into RAM
*/
