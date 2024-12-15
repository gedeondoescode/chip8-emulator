#include "Chip8.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>

auto rand_seed = std::chrono::system_clock::now().time_since_epoch().count();

Chip8::Chip8() : rand_eng(rand_seed) {
  opcode = 0;
  sp = 0;
  I = 0;

  pc = 0x200;

  rand_byte = std::uniform_int_distribution<uint8_t>(0, 255U);

  // Clear sprites/display
  for (int i = 0; i < sizeof(display); i++) {
    display[i] = 0;
  };

  // Clear stack
  for (int i = 0; i < sizeof(stack); i++) {
    stack[i] = 0;
  };

  // Clear registers
  for (int i = 0; i < 16; i++) {
    V[i] = 0;
  };

  // Clear memory
  for (int i = 0; i < sizeof(memory); i++) {
    memory[i] = 0;
  };

  // Load font into memory
  for (int i = 0; i < 80; i++) {
    memory[i] = font[i];
  }
}

void Chip8::load_rom(const char *filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    auto file_size = file.tellg();
    char *buffer = new char[file_size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, file_size);
    file.close();

    for (long i = 0; i < file_size; i++) {
      memory[0x200 + i] = buffer[i];
    }
    delete[] buffer;
  };
}

void Chip8::cycle() {
  // Fetch the current instruction
  opcode = (memory[pc] << 8) | memory[pc + 1];

  // 4-bit identifiers
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;

  // Bit format
  uint16_t address = opcode & 0x0FFF;
  uint8_t byte = opcode & 0x00FF;

  // Immediately increment the PC
  pc += 2;

  // Decode the opcode
  switch (opcode & 0xF000) {
    case 0x0000:

      switch (opcode & 0x000F) {
        case 0x0000:  // CLS: Clears the screen
          for (int i = 0; i < sizeof(display); i++) {
            display[i] = 0;
          };
          drawFlag = true;
          break;

        case 0x000E:  // RET: return from subroutine
          pc = stack[--sp];
          break;

        default:  // Handle unknown instructions
          printf("Opcode instruction not recognized: 0x%X\n", opcode);
          break;
      }
      break;

    case 0x1000:  //  JP: Jump to address location FFF
      pc = address;
      break;

    case 0x2000:  // CALL: call subroutine at address FFF
      stack[sp++] = pc;
      pc = address;
      break;

    case 0x3000:  // SE: skip if V[X] == byte
      if (V[X] == byte) {
        pc += 2;
      }
      break;

    case 0x4000:  // SNE: skip if V[X] != byte
      if (V[X] != byte) {
        pc += 2;
      }
      break;

    case 0x5000:  // SE: skip if VX == VY
      if (V[X] == V[Y]) {
        pc += 2;
      }
      break;

    case 0x6000:  // LD: Sets VX = kk
      V[X] = byte;
      break;

    case 0x7000:  // ADD: Set Vx = VY + kk
      V[X] += byte;
      break;

    case 0x8000:  // Arithmetic Logic
      switch (opcode & 0x000F) {
        case 0x0:  // LD: Set VX = VY (0x8XY0)
          V[X] = V[Y];
          break;

        case 0x1:  // OR: Set VX = VX OR VY (0x8XY1)
          V[X] |= V[Y];
          break;

        case 0x2:  // AND: Set VX = VX AND VY (0x8XY2)
          V[X] = V[X] & V[Y];
          break;

        case 0x3:  // XOR: Set VX = VX XOR VY (0x8XY3)
          V[X] = V[X] ^ V[Y];
          break;

        case 0x4:  // ADD: Set VX = VX + VY and set VF = carry (0x8XY4)
          V[X] += V[Y];

          if (V[X] >= 256u) {
            V[0xF] = 1;  // carry flag
          } else {
            V[0xF] = 0;
          }
          V[X] = (V[Y] + V[X]) & 0xFF;
          break;

        case 0x5:
          if (V[X] > V[Y]) {
            V[0xF] = 1;  // set to NOT borrow
          } else {
            V[0xF] = 0;
          }

          V[X] -= V[Y];
          break;

        case 0x6:
          msb = V[X] & 1;
          V[X] = V[X] >> 1;
          V[0xF] = msb;

          break;

        case 0x7:
          V[X] = V[Y] - V[X];

          if (V[Y] >= V[X]) {
            V[0xF] = 1;  // set to NOT borrow
          } else {
            V[0xF] = 0;
          }
          break;

        case 0xE:
          msb = V[X] >> 7;

          V[X] = V[X] << 1;
          V[0xF] = msb;

          break;
      }
      break;

    case 0x9000:  // SNE: skip if V[X] != v[Y]
      if (V[X] != V[Y]) {
        pc += 2;
      }
      break;

    case 0xA000:  // LD: Set I = nnn
      I = address;
      break;

    case 0xB000:
      pc = address + V[0x0];
      break;

    case 0xC000:
      V[X] = rand_byte(rand_eng) & byte;
      break;

    case 0xD000:
      draw_sprite(X, Y, opcode & 0x000F);
      drawFlag = true;
      break;

    case 0xE000:
      switch (opcode & 0x00FF) {
        case 0x9E:
          // TODO
          printf("TODO! Unimplemented: 0x%X\n", opcode);
          break;
        case 0x1A:
          // TODO
          printf("TODO! Unimplemented: 0x%X\n", opcode);
          break;
      }
      break;

    case 0xF000:
      switch (opcode & 0x00FF) {
        case 0x1E:
          I += V[X];
          break;

        case 0x33:
          // Ones
          memory[I + 2] = V[X] % 10;
          V[X] /= 10;

          // Tens
          memory[I + 1] = V[X] % 10;
          V[X] /= 10;

          // Hundreds
          memory[I] = V[X] % 10;

          break;

        // LD: Store registers V0 through Vx in memory starting at location I
        case 0x55:
          for (int i = 0; i <= X; i++) {
            memory[I + i] = V[i];
          }
          break;

        case 0x65:
          for (int i = 0; i <= X; i++) {
            V[i] = memory[I + i];
          }
          break;
      }
      break;

    default:
      printf("Opcode instruction not recognized: 0x%X\n", opcode);
      break;
  }
};

void Chip8::draw_sprite(uint8_t X, uint8_t Y, uint8_t height) {
  uint8_t x = V[X] % SCREEN_WIDTH;
  uint8_t y = V[Y] % SCREEN_HEIGHT;
  uint8_t pixel;

  V[0xF] = 0;  // Reset V[F] register

  for (int y_line = 0; y_line < height; y_line++) {
    pixel = memory[I + y_line];  // Draw pixel

    for (int x_col = 0; x_col < 8; x_col++) {
      if ((pixel & (0x80 >> x_col)) != 0) {
        int wrapped_x = (x + x_col) % SCREEN_WIDTH;    // Wrap horizontally
        int wrapped_y = (y + y_line) % SCREEN_HEIGHT;  // Wrap vertically

        int index = wrapped_x + (wrapped_y * 64);

        if (display[index] == 1) {
          V[0xF] = 1;  // Enable Collision
        }
        display[index] ^= 1;
      }
    };
  }
};