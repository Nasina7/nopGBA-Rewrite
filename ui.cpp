#include "ui.hpp"
#include "io.hpp"
#include "file.hpp"
#include "display.hpp"
#include "cpu.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "string"
#include <time.h>
#include <string>
#include <sstream>


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

    for(int i = 0; i < 0x10000; i++)
    {
        ram.save[i] = 0xFF;
    }
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
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
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
    SDL_GL_SetSwapInterval(1);

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

void gbaUI::registerWindow()
{
    ImGui::Begin("Registers");
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
    ImGui::InputScalar("Last Opcode:", ImGuiDataType_U32, &cpu.lastOpcodeRan, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::End();
}

void gbaUI::intRegisterWindow()
{
    ImGui::Begin("Interrupt Registers");
    ImGui::InputScalar("IE:", ImGuiDataType_U16, &io.IE, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalar("IF:", ImGuiDataType_U16, &io.IF, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalar("Opcodes Ran:", ImGuiDataType_U64, &cpu.opcodesRan, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::Checkbox("IME",&io.IME);

    ImGui::End();
}

void gbaUI::setRunEmulation(bool value)
{
    ui.pauseEmulation = !value;
    ui.runNoStep = false;
    ui.useRunTo = false;
}

void gbaUI::debugWindow()
{
    ImGui::Begin("Debug");
    if(ImGui::Button("Load ROM"))
    {
        igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey","Choose File", ".gba,.bin",".");
    }
    if(igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
    {
        if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
        {
            std::string fileNamePath = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
            std::cout<<"fileNamePath: "<<fileNamePath<<std::endl;
            const char* romName = fileNamePath.c_str();
            std::cout<<"ResultROM: "<<romName<<std::endl;
            if(!fileLoad.loadROM(romName))
            {
                std::cout<<"ROM COULD NOT BE LOADED!"<<std::endl;
            }
            if(!fileLoad.loadBIOS())
            {
                std::cout<<"BIOS COULD NOT BE LOADED!"<<std::endl;
            }

        }
        igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
        ui.startEmulation = true;
        ui.runNoStep = true;
        ui.useRunTo = false;
        ui.pauseEmulation = false;

        emuInit();
        /*
        for(int i = 0; i < 13; i++)
        {
            cpu.R[i] = 0;
        }
        cpu.cpsr.T = 0;

        cpu.R[13] = 0x03007F00;
        cpu.R[14] = 0x08000000;
        cpu.R[15] = 0x08000008;
        cpu.cpsr.mode = 0x1F;
        */
    }



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
    if(ImGui::Button("Run"))
    {
        runNoStep = true;
        ui.pauseEmulation = false;
    }
    ImGui::Checkbox("Enable Open Bus",&ram.openBusEnable);
    ImGui::Checkbox("Allow PAUSE EMULATION",&ui.allowPauseEmulation);
    ImGui::End();
}

void gbaUI::ramOptionsWindow()
{
    ImGui::Begin("RAM Viewer Options");

    ImGui::Checkbox("BIOS",&showRamSection[0]);
    ImGui::Checkbox("ON BOARD RAM",&showRamSection[1]);
    ImGui::Checkbox("ON CHIP RAM",&showRamSection[2]);
    ImGui::Checkbox("PAL RAM",&showRamSection[3]);
    ImGui::Checkbox("VIDEO RAM",&showRamSection[4]);
    ImGui::Checkbox("OBJECT RAM",&showRamSection[5]);
    ImGui::Checkbox("ROM",&showRamSection[6]);
    ImGui::Checkbox("SAVE RAM",&showRamSection[7]);

    ImGui::End();


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

    palView = 11;
    renderPalViewer();
    glBindTexture(GL_TEXTURE_2D, palView);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0x10,0x10 * 2,0,GL_RGBA,GL_UNSIGNED_BYTE,*palTexture);
    glGenerateMipmap(GL_TEXTURE_2D);


    ImGui::Image((void*)(intptr_t)palView,ImVec2(0x10* 10,0x20 * 10));
    ImGui::End();
}
void gbaUI::bgTileViewer()
{
    ImGui::Begin("BG Tile Viewer");

    bgTileView = 12;
    renderBgTileViewer();
    glBindTexture(GL_TEXTURE_2D, bgTileView);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0x10 * 8,0x40 * 8,0,GL_RGBA,GL_UNSIGNED_BYTE,*bgTileTexture);
    glGenerateMipmap(GL_TEXTURE_2D);


    ImGui::Image((void*)(intptr_t)bgTileView,ImVec2(0x10 * 8,0x40 * 8));
    ImGui::End();
}

void gbaUI::displayViewer()
{
    ImGui::Begin("Display");
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
    //printf("Exit 6 Loop\n");
    ImGui::End();
    //printf("Successfully Wrote Screen\n");
}

time_t seconds;
uint64_t framesPerSecond;

uint64_t secondsPassed = 0;

void gbaUI::handleUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();


    testWindow();
    registerWindow();
    intRegisterWindow();
    palViewer();
    bgTileViewer();
    displayViewer();
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
        ss << "nopGBA | FPS: ";
        ss << framesPerSecond;
        std::string test = ss.str();
        SDL_SetWindowTitle(window, test.c_str());
        framesPerSecond = 0;
        secondsPassed++;
        if(secondsPassed == 15)
        {
            endEmulator = true;
        }
    }
    seconds = time(NULL);

    ImGui::Render();
    glViewport(0, 0, 800, 600);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}
