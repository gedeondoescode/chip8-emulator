#pragma once

#include <cstdint>
#include <random>

static const unsigned int SCREEN_WIDTH = 64;
static const unsigned int SCREEN_HEIGHT = 32;
static const unsigned int WIN_WIDTH = SCREEN_WIDTH * 15;
static const unsigned int WIN_HEIGHT = SCREEN_HEIGHT * 15;

class Chip8 {
 public:
  Chip8();
  uint8_t font[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
      0xF0, 0x80, 0xF0, 0x80,
      0x80  // F
  };
  uint8_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
  bool drawFlag;
  uint8_t msb;
  uint16_t byte_result;
  uint16_t flag_result;

  void load_rom(const char *filename);
  void cycle();
  void draw_sprite(uint8_t X, uint8_t Y, uint8_t height);

 private:
  uint8_t memory[4096];  // RAM: 0x000 -> 0x1FF
  uint16_t stack[16];    // Limited to 16 to prevent overflow
  uint16_t pc;           // Program Counter
  uint8_t sp;            // Stack Pointer
  uint16_t I;            // Index register
  uint8_t V[16];         // variable registers: V0 -> VF
  uint16_t opcode;       // Instruction

  uint8_t delay_timer;
  uint8_t sound_timer;

  std::default_random_engine rand_eng;
  std::uniform_int_distribution<uint8_t> rand_byte;
};