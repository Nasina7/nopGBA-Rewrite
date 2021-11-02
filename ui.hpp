#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "imgui_memory_editor.h"


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
        bool startEmulation;

        bool endEmulator;


    private:
        void testWindow();

        void errorWindow();
        const char* errorWindowMessage;
        uint32_t errorArgument;
        bool displayError;

        void registerWindow();
        void intRegisterWindow();

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
        void setRunEmulation(bool value);

        void ramOptionsWindow();
        bool showRamSection[10];

        void runToWindow();

        const char* glsl_version = "#version 130";
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        SDL_GLContext gl_context;
        SDL_Window* window;
        SDL_Renderer* render;
};

extern gbaUI ui;
