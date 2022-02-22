#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "imgui_memory_editor.h"
#include <atomic>

using namespace std;

class gbaUI {
    public:
        int initUI();
        void handleUI();
        void createErrorWindow(const char* message, uint32_t argument);
        bool pauseEmulation;
        bool allowPauseEmulation;
        MemoryEditor mem_edit;
        bool useRunTo;
        bool runNoStep;
        uint32_t runToLocation;
        bool experimentalSpeedup;
        bool disableTimers;
        bool useVSYNC = true;

        bool endEmulator;
        bool startEmulation;

        uint64_t framesPerSecondEMU;
        uint8_t biosLoaded;

        bool allowCpuRun = false;
        SDL_Window* window;

    private:
        void testWindow();

        void flashWindow();

        void errorWindow();
        const char* errorWindowMessage;
        uint32_t errorArgument;
        bool displayError;

        void registerWindow();
        void dmaWindow();
        void dispRegWindow();
        void intRegisterWindow();
        void speedHacks();

        void displayViewer();
        GLuint screenView;

        void palViewer();
        GLuint palView;
        void renderPalViewer();
        uint32_t palTexture[0x20][0x10];

        void bgTileViewer();
        GLuint bgTileView;
        void renderBgTileViewer();
        uint32_t bgTileTexture[8 * 0x40][8 * 0x10];

        void debugWindow();
        void mainWindow();
        void setRunEmulation(bool value);

        void ramOptionsWindow();
        bool showRamSection[10];

        void runToWindow();


        const char* glsl_version = "#version 130";
        const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        SDL_GLContext gl_context;
        SDL_Renderer* render;
};

extern gbaUI ui;
