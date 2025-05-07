#include <SDL3/SDL.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>

class Emulator {

public:

    /* Screen dimensions */
    const int wt = 320;
    const int ht = 288;
    const int gbResX = 160;
    const int gbResY = 144;
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    int ticks = 0;
   
    bool romLoaded = false;
    bool running = false;
    bool writetodisplay = false;
    //menu shit
    bool paused = false;
    int menuItemSelected = 0;
    int menuItemOpened = 0;
    int menuItemOptionSelected = 0;
  
    //to config file
    int selectedColorPallete = 0;

   
};
extern Emulator emu;