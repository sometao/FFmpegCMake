/*
 * learning ffmpeg from
 * https://github.com/leixiaohua1020/simplest_ffmpeg_player/blob/master/simplest_video_play_sdl2/simplest_video_play_sdl2.cpp
 * simplest_video_play_sdl2.cpp
 */

extern "C" {
#include "sdl2/SDL.h"
};

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

// Refresh Event
#define REFRESH_EVENT (SDL_USEREVENT + 1)

#define BREAK_EVENT (SDL_USEREVENT + 2)

namespace lxh {
namespace player_sdl2 {


const int bpp = 12;

char* inputYUVFile = "D:/data/tmp/ffmepg_test/output002.yuv";


int screen_w = 800;
int screen_h = 600;
const int pixel_w = 1920; 
const int pixel_h = 1080;

unsigned char buffer[pixel_w * pixel_h * bpp / 8];

int thread_exit = 0;

int refresh_video(void* opaque) {
  thread_exit = 0;
  while (!thread_exit) {
    SDL_Event event;
    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    SDL_Delay(40);
  }
  thread_exit = 0;
  // Break
  SDL_Event event;
  event.type = BREAK_EVENT;
  SDL_PushEvent(&event);

  return 0;
}

int launch() {
  cout << "Hi, player sdl2." << endl;


  if (SDL_Init(SDL_INIT_VIDEO)) {
    printf("Could not initialize SDL - %s\n", SDL_GetError());
    return -1;
  }

  SDL_Window* screen;
  // SDL 2.0 Support for multiple windows
  screen =
      SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!screen) {
    printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
    return -1;
  }
  SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

  Uint32 pixformat = 0;

  // IYUV: Y + U + V  (3 planes)
  // YV12: Y + V + U  (3 planes)
  pixformat = SDL_PIXELFORMAT_IYUV;

  SDL_Texture* sdlTexture =
      SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

  FILE* fp = NULL;
  fp = fopen(inputYUVFile, "rb+");

  if (fp == NULL) {
    printf("cannot open this file\n");
    return -1;
  }


  SDL_Thread* refresh_thread = SDL_CreateThread(refresh_video, NULL, NULL);
  SDL_Event event;
  SDL_Rect sdlRect;
  while (1) {
    // Wait
    SDL_WaitEvent(&event);
    if (event.type == REFRESH_EVENT) {
      if (fread(buffer, 1, pixel_w * pixel_h * bpp / 8, fp) != pixel_w * pixel_h * bpp / 8) {
        // Loop
        fseek(fp, 0, SEEK_SET);
        fread(buffer, 1, pixel_w * pixel_h * bpp / 8, fp);
      }

      SDL_UpdateTexture(sdlTexture, NULL, buffer, pixel_w);

      // FIX: If window is resize
      sdlRect.x = 0;
      sdlRect.y = 0;
      sdlRect.w = screen_w;
      sdlRect.h = screen_h;

      SDL_RenderClear(sdlRenderer);
      SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
      SDL_RenderPresent(sdlRenderer);

    } else if (event.type == SDL_WINDOWEVENT) {
      // If Resize
      SDL_GetWindowSize(screen, &screen_w, &screen_h);
    } else if (event.type == SDL_QUIT) {
      thread_exit = 1;
    } else if (event.type == BREAK_EVENT) {
      break;
    }
  }
  SDL_Quit();
  return 0;
}

}  // namespace player_sdl2

}  // namespace lxh
