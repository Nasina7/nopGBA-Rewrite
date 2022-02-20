#include "input.hpp"
#include "io.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "ui.hpp"

gbaInput input;

int gbaInput::handleControlsr()
{
    switch( SDL_EVENT_HANDLING.key.keysym.sym )
        {

        case SDLK_z:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 1);
        break;

        case SDLK_x:
            io.KEYINPUT = io.KEYINPUT | (0x1);
        break;

        case SDLK_SPACE:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 2);
        break;

        case SDLK_RETURN:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 3);
        break;

        case SDLK_UP:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 6);
        break;

        case SDLK_DOWN:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 7);
        break;

        case SDLK_LEFT:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 5);
        break;

        case SDLK_RIGHT:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 4);
        break;

        case SDLK_a:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 9);
        break;

        case SDLK_s:
            io.KEYINPUT = io.KEYINPUT | (0x1 << 8);
        break;

        }
    return 0;
}
int gbaInput::handleControls()
{
    switch( SDL_EVENT_HANDLING.key.keysym.sym )
        {

        case SDLK_z:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 1));
        break;

        case SDLK_x:
            io.KEYINPUT = io.KEYINPUT & (~(0x1));
        break;

        case SDLK_SPACE:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 2));
        break;

        case SDLK_RETURN:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 3));
        break;

        case SDLK_UP:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 6));
        break;

        case SDLK_DOWN:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 7));
        break;

        case SDLK_LEFT:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 5));
        break;

        case SDLK_RIGHT:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 4));
        break;

        case SDLK_a:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 9));
        break;

        case SDLK_s:
            io.KEYINPUT = io.KEYINPUT & (~(0x1 << 8));
        break;

        }
    return 0;
}

void gbaInput::handleInput()
{
    while( SDL_PollEvent( &SDL_EVENT_HANDLING)) // While Event to handle Random Stuff
    {
        ImGui_ImplSDL2_ProcessEvent(&SDL_EVENT_HANDLING);
        if (SDL_EVENT_HANDLING.type == SDL_KEYDOWN) // If a key is being pressed, handle controls.
        {   // Handle Controls
            handleControls();
        }
        if (SDL_EVENT_HANDLING.type == SDL_KEYUP)
        {
            handleControlsr();
        }
        if(SDL_EVENT_HANDLING.type == SDL_QUIT)
        {
            ui.endEmulator = true;
        }
    }
}
