#include <SDL2/SDL.h>

#include <iostream>

#include "Chip8.h"

Chip8 chip8;

// Declare pointer
SDL_Window *window;
SDL_Surface *dst;

void draw_pixels();

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: ./Chip8 <ROM> " << std::endl;
    return 1;
  }
  const char *romName = argv[1];
  bool running = true;

  // TODO: Bind Keypad input to QWERTY

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window failed to initalize: %s\n",
                 SDL_GetError());
  };

  // Create App Window
  window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT,
                            SDL_WINDOW_ALLOW_HIGHDPI);
  dst = SDL_GetWindowSurface(window);

  chip8.load_rom(romName);

  while (running) {
    chip8.cycle();

    if (chip8.drawFlag) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
          std::cout << "Exiting now..." << std::endl;
        }
      }

      draw_pixels();
      SDL_UpdateWindowSurface(window);
    }
  }

  // Destroy the window
  SDL_DestroyWindow(window);
  window = NULL;

  // Initiate SDL Cleanup
  SDL_Quit();
  return 0;
};

void draw_pixels() {
  SDL_PixelFormat *myPixel = dst->format;
  SDL_Rect rect;

  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      rect.x = x * 15;
      rect.y = y * 15;
      rect.w = 15;
      rect.h = 15;

      if (chip8.display[x + (y * 64)] == 1) {
        SDL_FillRect(dst, &rect, SDL_MapRGB(myPixel, 255, 204, 1));  // Jonquil

      } else {
        SDL_FillRect(dst, &rect,
                     SDL_MapRGB(myPixel, 153, 103, 1));  // Golden Brown
      }
    }
  }
}