#include <stdint.h>
#include <time.h>
#include <string>

class gbaSave {
    public:
        uint8_t currentSaveType;
        void saveData();
        void loadData();
        std::string saveName;

        time_t lastSaveTime;
        time_t currentTime;

        bool saveWasLoaded = false;

        uint8_t accessSRAM(uint32_t location, bool write, uint8_t value);



        uint8_t flashState = 0xFF;
        uint16_t chipID;
        uint8_t otherFlashPage[0x10000];
        uint8_t issueCommand = 0;
        uint8_t currentFlashPage = 0;

    private:
        void saveSRAM();
        //void saveEEPROM();
        void saveFLASH();

        void loadSRAM();
        //void loadEEPROM();
        void loadFLASH();


        void writeFLASH(uint32_t location, uint8_t value);
        uint8_t readFLASH(uint32_t location);

};

extern gbaSave save;
