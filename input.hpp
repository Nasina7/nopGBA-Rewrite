#include <SDL2/SDL.h>


class gbaInput {
    public:
        void handleInput();
    private:
        SDL_Event SDL_EVENT_HANDLING;
        int handleControls();
        int handleControlsr();
};

extern gbaInput input;
