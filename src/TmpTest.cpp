

extern "C"
{
#include "sdl2/SDL.h"
};

#include <iostream>


namespace tmpTest {


  int run() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
      printf("Could not initialize SDL - %s\n", SDL_GetError());
      return -1;
    } else {
      printf("SDL_Init success.");
      return 0;
    }
  }
}