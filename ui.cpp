#include "ui.hpp"
#include "io.hpp"
#include "file.hpp"
#include "display.hpp"
#include "cpu.hpp"
#include "input.hpp"
#include "audio.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "string"
#include <time.h>
#include <string>
#include <sstream>
#include <atomic>


gbaUI ui;

void emuInit()
{
    cpu.opcodesRan = 0;
    for(int i = 0; i < 13; i++)
    {
        cpu.R[i] = 0;
    }

    cpu.R[13] = 0x03007F00;
    cpu.R[14] = 0x08000000;
    cpu.R1314_svc[0] = 0x03007FE0;
    cpu.R1314_svc[1] = 0x08000000;
    cpu.R1314_irq[0] = 0x03007F00;
    cpu.R1314_irq[1] = 0x08000000;
    cpu.R[15] = 0x08000000;
    cpu.cpsr.mode = 0x1F;
    cpu.spsr.mode = 0x10;
    cpu.cpsr.T = false;
    cpu.cpsr.N = false;
    cpu.cpsr.C = false;
    cpu.cpsr.F = false;
    cpu.cpsr.I = false;
    cpu.cpsr.Q = false;
    cpu.cpsr.V = false;
    cpu.cpsr.Z = false;


    for(int i = 0; i < 0x10000; i++)
    {
        ram.save[i] = 0xFF;
    }
    io.WAITCNT = 0x4317;
    io.IE = 0;
    io.IF = 0;
    io.IME = 0;
    io.LY = 0;
    io.VCOUNT = 0;
    for(int i = 0; i < 4; i++)
    {
        io.bgCNT[i] = 0;
        io.bgXScroll[i] = 0;
        io.bgYScroll[i] = 0;
        io.dmaControl[i] = 0;
        io.dmaEndAddress[i] = 0;
        io.dmaStartAddress[i] = 0;
        io.dmaWordCount[i] = 0;
        io.HALTCNT = 0;
        io.lastEndAddr[i] = 0;
        io.lastStartAddr[i] = 0;
        io.TMCNT_C[i] = 0;
        io.TMCNT_H[i] = 0;
        io.TMCNT_L[i] = 0;
    }
    ui.startEmulation = true;
    ui.pauseEmulation = false;
    //gbaREG.IME = false;
    //gbaREG.IE = 0;
    //gbaREG.IF = 0;
}

void gbaUI::createErrorWindow(const char* message, uint32_t argument)
{
    errorWindowMessage = message;
    errorArgument = argument;
    displayError = true;
}


int gbaUI::initUI()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    audio.want.freq = 32768;
    audio.want.format = AUDIO_U16;
    audio.want.channels = 1;
    audio.want.samples = 16 * 4; // 16 * 4
    audio.want.callback = audioCallback;
    audio.dev = SDL_OpenAudioDevice(NULL, 0, &audio.want, &audio.have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    SDL_OpenAudio(&audio.want, &audio.have);


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("nopGBA Rev.2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, window_flags);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    // 0 = NO VSYNC, 1 = VSYNC
    if(SDL_GL_SetSwapInterval(0) == -1)
    {
        printf("ERROR: %s\n",SDL_GetError());
    }

    glewInit();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(window,gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return 0;
}

void gbaUI::testWindow()
{
    ImGui::Begin("testWindow");
    ImGui::Text("Hello, world");
    ImGui::End();
}

void gbaUI::errorWindow()
{
    ImGui::Begin("Error");
    ImGui::Text(errorWindowMessage, errorArgument);
    ImGui::End();
}

void gbaUI::dispRegWindow()
{
    ImGui::Begin("BG Regs");
    if(ImGui::IsWindowCollapsed() == false)
    {
        ImGui::InputScalar("LCD Control: ", ImGuiDataType_U16, &io.lcdControl, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        for(int i = 0; i < 4; i++)
        {
            ImGui::Text("BG Regs");
            ImGui::InputScalar("Background Control: ", ImGuiDataType_U16, &io.bgCNT[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Background X: ", ImGuiDataType_U16, &io.bgXScroll[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Background Y: ", ImGuiDataType_U16, &io.bgYScroll[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        }
    }

    ImGui::End();


    ImGui::Begin("Timer Regs");
    if(ImGui::IsWindowCollapsed() == false)
    {
        for(int i = 0; i < 4; i++)
        {
            ImGui::Text("Timer");
            ImGui::InputScalar("Timer Control: ", ImGuiDataType_U16, &io.TMCNT_H[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Timer Reload: ", ImGuiDataType_U16, &io.TMCNT_L[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Timer Counter: ", ImGuiDataType_U16, &io.TMCNT_C[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Scheduler: ", ImGuiDataType_S32, &cpu.timerScheduler[i], NULL, NULL, "%i", ImGuiInputTextFlags_CharsDecimal);
        }
    }

    ImGui::End();


    ImGui::Begin("owie my ears");

    if(ImGui::IsWindowCollapsed() == false)
    {
        float sampleArray[32];
        for(int x = 0; x < 32; x++)
        {
            sampleArray[x] = audio.samples[x];
        }
        ImGui::PlotLines("Audio Data", sampleArray, 32);
    }

    ImGui::End();


}

void gbaUI::dmaWindow()
{
    ImGui::Begin("DMA Regs");
    if(ImGui::IsWindowCollapsed() == false)
    {
        for(int i = 0; i < 4; i++)
        {
            ImGui::Text("DMA");
            ImGui::InputScalar("Start Address: ", ImGuiDataType_U32, &io.dmaStartAddress[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("End Address: ", ImGuiDataType_U32, &io.dmaEndAddress[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Word Count: ", ImGuiDataType_U16, &io.dmaWordCount[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Control: ", ImGuiDataType_U16, &io.dmaControl[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::InputScalar("Last Start Addr: ", ImGuiDataType_U32, &io.lastStartAddr[i], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        }
        ImGui::InputScalar("DATA: ", ImGuiDataType_U8, &audio.samples[0], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("Sample Per Frame: ", ImGuiDataType_U32, &audio.averageFreqPerFrame, NULL, NULL, "%i", ImGuiInputTextFlags_CharsDecimal);
        audio.averageFreqPerFrame = 0;
        audio.frameSampleCount = 0;
    }

    ImGui::End();
}

void gbaUI::registerWindow()
{
    ImGui::Begin("Registers");
    if(ImGui::IsWindowCollapsed() == false)
    {
        ImGui::InputScalar("R0:", ImGuiDataType_U32, &cpu.R[0], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R1:", ImGuiDataType_U32, &cpu.R[1], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R2:", ImGuiDataType_U32, &cpu.R[2], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R3:", ImGuiDataType_U32, &cpu.R[3], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R4:", ImGuiDataType_U32, &cpu.R[4], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R5:", ImGuiDataType_U32, &cpu.R[5], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R6:", ImGuiDataType_U32, &cpu.R[6], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R7:", ImGuiDataType_U32, &cpu.R[7], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R8:", ImGuiDataType_U32, &cpu.R[8], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R9:", ImGuiDataType_U32, &cpu.R[9], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R10:", ImGuiDataType_U32, &cpu.R[10], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R11:", ImGuiDataType_U32, &cpu.R[11], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R12:", ImGuiDataType_U32, &cpu.R[12], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R13:", ImGuiDataType_U32, &cpu.R[13], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R14:", ImGuiDataType_U32, &cpu.R[14], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R15:", ImGuiDataType_U32, &cpu.R[15], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);

        // Display CPSR
        ImGui::Checkbox("N",&cpu.cpsr.N);
        ImGui::Checkbox("Z",&cpu.cpsr.Z);
        ImGui::Checkbox("C",&cpu.cpsr.C);
        ImGui::Checkbox("V",&cpu.cpsr.V);
        ImGui::Checkbox("IRQ Flag",&cpu.cpsr.I);
        ImGui::Checkbox("FIQ FLAG",&cpu.cpsr.F);
        ImGui::Checkbox("Thumb Mode",&cpu.cpsr.T);
        ImGui::InputScalar("Mode", ImGuiDataType_U8, &cpu.cpsr.mode, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);

        ImGui::InputScalar("R13_irq:", ImGuiDataType_U32, &cpu.R1314_irq[0], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R14_irq:", ImGuiDataType_U32, &cpu.R1314_irq[1], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R13_svc:", ImGuiDataType_U32, &cpu.R1314_svc[0], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("R14_svc:", ImGuiDataType_U32, &cpu.R1314_svc[1], NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("Last Opcode:", ImGuiDataType_U32, &cpu.lastOpcodeRan, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
    }


    ImGui::End();
}

void gbaUI::intRegisterWindow()
{
    ImGui::Begin("Interrupt Registers");
    if(ImGui::IsWindowCollapsed() == false)
    {
        ImGui::InputScalar("IE:", ImGuiDataType_U16, &io.IE, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("IF:", ImGuiDataType_U16, &io.IF, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputScalar("Opcodes Ran:", ImGuiDataType_U64, &cpu.opcodesRan, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::Checkbox("IME",&io.IME);
    }


    ImGui::End();
}

void gbaUI::setRunEmulation(bool value)
{
    ui.pauseEmulation = !value;
    ui.runNoStep = false;
    ui.useRunTo = false;
}

void gbaUI::speedHacks()
{
    ImGui::Begin("Speed Hacks / Settings");
    if(ImGui::IsWindowCollapsed() == false)
    {
        ImGui::Checkbox("0x358 Skip",&ui.experimentalSpeedup);
        ImGui::Checkbox("Disable Timers",&ui.disableTimers);
    }
    ImGui::End();
}

void gbaUI::debugWindow()
{
    ImGui::Begin("Debug");
    if(ImGui::IsWindowCollapsed() == false)
    {
        if(ImGui::Button("Step"))
        {
            setRunEmulation(true);
        }
        ImGui::InputScalar("Location to Run To: ", ImGuiDataType_U32, &ui.runToLocation, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        if(ImGui::Button("Run to Location"))
        {
            useRunTo = true;
            ui.pauseEmulation = false;
        }
        ImGui::Checkbox("Pause Emulation",&ui.pauseEmulation);
        ImGui::Checkbox("Enable Open Bus",&ram.openBusEnable);
        //ImGui::Checkbox("Allow PAUSE EMULATION",&ui.allowPauseEmulation);
    }

    ImGui::End();
}

void gbaUI::ramOptionsWindow()
{
    ImGui::Begin("RAM Viewer Options");

    bool wasCollapsed = ImGui::IsWindowCollapsed();
    if(wasCollapsed == false)
    {
        ImGui::Checkbox("BIOS",&showRamSection[0]);
        ImGui::Checkbox("ON BOARD RAM",&showRamSection[1]);
        ImGui::Checkbox("ON CHIP RAM",&showRamSection[2]);
        ImGui::Checkbox("PAL RAM",&showRamSection[3]);
        ImGui::Checkbox("VIDEO RAM",&showRamSection[4]);
        ImGui::Checkbox("OBJECT RAM",&showRamSection[5]);
        ImGui::Checkbox("ROM",&showRamSection[6]);
        ImGui::Checkbox("SAVE RAM",&showRamSection[7]);
    }

    ImGui::End();

    if(wasCollapsed == false)
    {
        if(showRamSection[0] == true) // BIOS
        {
            ImGui::Begin("BIOS");
            mem_edit.DrawContents(ram.bios,sizeof(ram.bios), 0);
            ImGui::End();
        }
        if(showRamSection[1] == true) // ON BOARD
        {
            ImGui::Begin("ON BOARD");
            mem_edit.DrawContents(ram.onBoard,sizeof(ram.onBoard), 0x02000000);
            ImGui::End();
        }
        if(showRamSection[2] == true) // ON CHIP
        {
            ImGui::Begin("ON CHIP");
            mem_edit.DrawContents(ram.onChip,sizeof(ram.onChip), 0x03000000);
            ImGui::End();
        }
        if(showRamSection[3] == true) // ON PALLETE
        {
            ImGui::Begin("PALLETE RAM");
            mem_edit.DrawContents(ram.bg_ObjPal,sizeof(ram.bg_ObjPal), 0x05000000);
            ImGui::End();
        }
        if(showRamSection[4] == true) // VIDEO
        {
            ImGui::Begin("VIDEO RAM");
            mem_edit.DrawContents(ram.video,sizeof(ram.video), 0x06000000);
            ImGui::End();
        }
        if(showRamSection[5] == true) // OBJECT
        {
            ImGui::Begin("OBJECT RAM");
            mem_edit.DrawContents(ram.object,sizeof(ram.object), 0x07000000);
            ImGui::End();
        }
        if(showRamSection[6] == true) // ROM
        {
            ImGui::Begin("ROM");
            mem_edit.DrawContents(ram.rom,sizeof(ram.rom), 0x08000000);
            ImGui::End();
        }
        if(showRamSection[7] == true) // SAVE
        {
            ImGui::Begin("SAVE RAM");
            mem_edit.DrawContents(ram.save,sizeof(ram.save), 0x0E000000);
            ImGui::End();
        }
    }

}

void gbaUI::renderPalViewer()
{
    uint32_t location;
    for(int y = 0; y < (0x10 * 2); y++)
    {
        for(int x = 0; x < 0x10; x++)
        {
            location = 0x05000000 + (x * 2) + (y * 0x20);
            uint16_t color = io.readMem(location, 1);
            palTexture[y][x] = (0xFF << 24) |
                               ((((color >> 10) & 0x1F) << 3) << 16) |
                               ((((color >> 5) & 0x1F) << 3) << 8) |
                               (((color & 0x1F) << 3));


            if(location > 0x050003FF)
            {
                printf("OVERFLOW IN PAL VIEWER!\n");
                while(1)
                {

                }
            }
        }
    }
    //printf("Location: 0x%X\n",location);
}

void gbaUI::renderBgTileViewer()
{
    uint32_t location;
    // Assumes 4BPP
    for(int yTile = 0; yTile < 0x40; yTile++)
    {
        for(int xTile = 0; xTile < 0x10; xTile++)
        {
            for(int y = 0; y < 8; y++)
            {
                for(int x = 0; x < 8; x++)
                {
                    uint32_t curTileLocation = 0x06000000 + (yTile * 0x200) + (xTile * 0x20) + (y * 4) + ((x / 2) * 1);

                    uint8_t tileData = io.readMem(curTileLocation, 0);
                    switch(x % 2)
                    {
                        case 0:
                            location = 0x05000000 + ((tileData & 0xF) * 2);
                        break;

                        case 1:
                            location = 0x05000000 + (((tileData >> 4) & 0xF) * 2);
                        break;
                    }


                    //location = 0x05000000 + ((tileData & 0xF) * 2) + ((tileData >> 4) * 0x20);
                    uint16_t color = io.readMem(location, 1);
                    bgTileTexture[y + (yTile * 8)][x + (xTile * 8)] = (0xFF << 24) |
                                       ((((color >> 10) & 0x1F) << 3) << 16) |
                                       ((((color >> 5) & 0x1F) << 3) << 8) |
                                       (((color & 0x1F) << 3));
                }
            }
        }
    }
    //printf("Location: 0x%X\n",location);
}

void gbaUI::palViewer()
{
    ImGui::Begin("Pallete");

    if(ImGui::IsWindowCollapsed() == false)
    {
        palView = 11;
        renderPalViewer();
        glBindTexture(GL_TEXTURE_2D, palView);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0x10,0x10 * 2,0,GL_RGBA,GL_UNSIGNED_BYTE,*palTexture);
        glGenerateMipmap(GL_TEXTURE_2D);


        ImGui::Image((void*)(intptr_t)palView,ImVec2(0x10* 10,0x20 * 10));
    }

    ImGui::End();
}
void gbaUI::bgTileViewer()
{
    ImGui::Begin("BG Tile Viewer");

    if(ImGui::IsWindowCollapsed() == false)
    {
        bgTileView = 12;
        renderBgTileViewer();
        glBindTexture(GL_TEXTURE_2D, bgTileView);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0x10 * 8,0x40 * 8,0,GL_RGBA,GL_UNSIGNED_BYTE,*bgTileTexture);
        glGenerateMipmap(GL_TEXTURE_2D);


        ImGui::Image((void*)(intptr_t)bgTileView,ImVec2(0x10 * 8,0x40 * 8));
    }
    ImGui::End();
}

void gbaUI::displayViewer()
{
    ImGui::Begin("Display");
    if(ImGui::IsWindowCollapsed() == false)
    {
        screenView = 10;
        screen.drawScreen();
        //printf("Exit S Loop\n");
        glBindTexture(GL_TEXTURE_2D, screenView);
        //printf("Exit 1 Loop\n");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //printf("Exit 2 Loop\n");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //printf("Exit 3 Loop\n");
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,*screen.screenArray);
        //printf("Exit 4 Loop\n");
        glGenerateMipmap(GL_TEXTURE_2D);
        //printf("Exit 5 Loop\n");


        ImGui::Image((void*)(intptr_t)screenView,ImVec2(240 * 2,160 * 2));
        ImGui::Checkbox("Force Draw BGs: ", &screen.forceDrawBGs);
        ImGui::Checkbox("Force Draw BG 0: ", &screen.forceDrawBg[0]);
        ImGui::Checkbox("Force Draw BG 1: ", &screen.forceDrawBg[1]);
        ImGui::Checkbox("Force Draw BG 2: ", &screen.forceDrawBg[2]);
        ImGui::Checkbox("Force Draw BG 3: ", &screen.forceDrawBg[3]);
    }
    //printf("Exit 6 Loop\n");
    ImGui::End();
    //printf("Successfully Wrote Screen\n");
}

time_t seconds;
uint64_t framesPerSecond;

uint64_t secondsPassed = 0;

bool initedUI = false;

void gbaUI::mainWindow()
{
    ImGuiIO io2 = ImGui::GetIO();
    ImVec2 winsize(io2.DisplaySize.x, io2.DisplaySize.y);
    ImGui::SetNextWindowSize(winsize);
    ImVec2 winpos(0, 0);
    ImGui::SetNextWindowPos(winpos);

    ImGui::Begin("Main Window", 0, ImGuiWindowFlags_NoBringToFrontOnFocus |
                                   ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_MenuBar);
    ImGui::GetStyle().WindowRounding = 0.0f;
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open ROM"))
            {
                igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey","Choose File", ".gba,.bin",".");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }



    if(igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
    {
        if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
        {
            std::string fileNamePath = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
            std::cout<<"fileNamePath: "<<fileNamePath<<std::endl;
            const char* romName = fileNamePath.c_str();
            std::cout<<"ResultROM: "<<romName<<std::endl;
            for(int i = 0; i < sizeof(ram.rom); i++)
            {
                ram.rom[i] = 0xFF;
            }
            if(!fileLoad.loadROM(romName))
            {
                std::cout<<"ROM COULD NOT BE LOADED!"<<std::endl;
            }
            if(!fileLoad.loadBIOS())
            {
                std::cout<<"BIOS COULD NOT BE LOADED!"<<std::endl;
                biosLoaded = 2;
            }
            else
            {
                biosLoaded = 1;
            }
            igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
            if(biosLoaded == 1)
            {
                emuInit();
            }
        }
    }
    screenView = 10;
    screen.drawScreen();
    //printf("Exit S Loop\n");
    glBindTexture(GL_TEXTURE_2D, screenView);
    //printf("Exit 1 Loop\n");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //printf("Exit 2 Loop\n");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //printf("Exit 3 Loop\n");
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,*screen.screenArray);
    //printf("Exit 4 Loop\n");
    glGenerateMipmap(GL_TEXTURE_2D);
    //printf("Exit 5 Loop\n");


    ImGui::Image((void*)(intptr_t)screenView,ImVec2(240 * 2,160 * 2));

    ImGui::End();

    if(biosLoaded == 2)
    {
        ImGui::Begin("Missing BIOS");
        ImGui::Text("The bios file appears to be missing.");
        ImGui::Text("A bios file must be present in the 'bios' folder next to the executable.");
        ImGui::Text("The bios file itself needs to be named 'bios.rom'");
        ImGui::Text("If you are unable to dump the official bios, then you may be able to");
        ImGui::Text("find unofficial ones online, but those haven't been tested as much.");
        ImGui::End();
    }
}

void gbaUI::handleUI()
{
    while(ui.endEmulator == false)
    {
        if(initedUI == false)
        {
            initUI();
            initedUI = true;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        mainWindow();
        //testWindow();
        speedHacks();
        dmaWindow();
        dispRegWindow();
        registerWindow();
        intRegisterWindow();
        palViewer();
        bgTileViewer();
        //displayViewer();
        ramOptionsWindow();

        if(displayError == true)
        {
            errorWindow();
        }
        debugWindow();


        framesPerSecond++;

        if(seconds != time(NULL))
        {
            std::stringstream ss;
            ss << "nopGBA | UI FPS: ";
            ss << framesPerSecond;
            ss << " | Emulated FPS: ";
            ss << ui.framesPerSecondEMU;
            std::string test = ss.str();
            SDL_SetWindowTitle(window, test.c_str());
            framesPerSecond = 0;
            ui.framesPerSecondEMU = 0;
            secondsPassed++;
            if(secondsPassed == 15)
            {
                //endEmulator = true;
            }
        }
        seconds = time(NULL);

        ImGui::Render();
        input.handleInput();
        glViewport(0, 0, 800, 600);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

}
