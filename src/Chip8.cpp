#include "Chip8.h"

#include <algorithm>
#include <fstream>
#include <iostream>

void Chip8::init() {
  opcode = 0;
  I = 0;

  pc = 0x200;

  for (int i = 0; i < sizeof(display); i++) {
    display[i] = 0;
  };

  for (int i = 0; i < sizeof(stack); i++) {
    stack[i] = 0;
  };

  for (int i = 0; i < sizeof(V); i++) {
    V[i] = 0;
  };

  for (int i = 0; i < sizeof(memory); i++) {
    memory[i] = 0;
  };

  for (int i = 0; i < 80; i++) {
    memory[i] = font[i];
  }
}

void Chip8::loadRom(const char *filename) {
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
  // printf("%s", opcode);
  // Fetch the current instruction
  // opcode = (memory[pc] << 8) | memory[pc + 1];
  if (pc >= 0 && pc < 4095) {
    opcode = memory[pc] << 8 | memory[pc + 1];
  } else {
    printf("Error: pc out of bounds (pc = %d)\n", pc);
  }

  // 4-bit identifiers
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;

  // Immediately increment the PC
  pc += 2;

  // Decode the opcode
  switch (opcode & 0xF000) {
    case 0x0000:

      switch (opcode & 0x000F) {
        case 0x0000:
          // CLS: Clears the screen
          for (int i = 0; i < sizeof(display); i++) {
            display[i] = 0;
          };
          drawFlag = true;
          break;

        case 0x000E:
          // RET: return from subroutine
          std::cout << "TODO: unimplemented!" << std::endl;
          break;

        // Handle unknown instructions temporarily
        default:
          printf("Opcode instruction not recognized: 0x%X\n", opcode);
          break;
      }
      break;

    case 0x1000:  //  JP: Jump to address location nnn
      pc = opcode & 0x0FFF;
      break;

    case 0x6000:  // LD: Sets VX = kk
      V[X] = opcode & 0x00FF;
      break;

    case 0x7000:  // ADD: Set Vx = Vx + kk
      V[X] += opcode & 0x00FF;
      break;

    case 0xA000:  // LD: Set I = nnn
      I = opcode & 0x0FFF;
      break;

    case 0xD000:
      draw_sprite(X, Y, opcode & 0x000F);
      drawFlag;
      break;

    default:
      printf("Opcode instruction not recognized: 0x%X\n", opcode);
      break;
  }
};

void Chip8::draw_sprite(uint8_t X, uint8_t Y, uint8_t height) {
  uint8_t x = V[X] % 64;
  uint8_t y = V[Y] % 32;
  uint8_t pixel;

  V[0xF] = 0;  // Reset V[F] register

  for (int y_line = 0; y_line < height; y_line++) {
    pixel = memory[I + y_line];  // Draw pixel

    for (int x_col = 0; x_col < 8; x_col++) {
      if ((pixel & (0x80 >> x_col)) != 0) {
        int wrapped_x = (x + x_col) % 64;   // Wrap horizontally
        int wrapped_y = (y + y_line) % 32;  // Wrap vertically

        int index = wrapped_x + (wrapped_y * 64);

        if (display[index] == 1) {
          V[0xF] = 1;
        }
        display[index] ^= 1;
      }
    };
  }
};