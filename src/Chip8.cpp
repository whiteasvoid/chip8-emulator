#include "../include/Chip8.h"
#include <iostream>

/*
Each font sprite is 4 pixels wide and 5 pixels tall.
Sprites are stored as bytes where each byte represents one row of pixels, and each bit in the byte represents a pixel (1 = on / lit, 0 = off / dark). Since the width is 4 pixels, only the most significant 4 bits of each byte are used (the lower 4 bits are ignored or zero).
So, per character : 5 rows = 5 bytes.
Total characters : 16 (0 - F).
16 characters × 5 bytes each = 80 bytes.That's why the array is sized [80].

In Chip-8, each font sprite is designed to be 4 pixels wide
Font set (5 bytes per character for 16 characters: 0-F)
64x32 display
*/
const uint8_t fontSet[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0: 0xF0, 0x90, 0x90, 0x90, 0xF0 => Binary: 1111, 1001, 1001, 1001, 1111
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    // Load fonts into memory starting at 0x050
    // The first 0x000-0x04F (0-79) were often reserved for other system use (like variables or the interpreter itself in original implementations)
    // So fonts go from 0x050 to 0x09F (80-159, exactly 80 bytes)
    for (int i = 0; i < 80; ++i) {
        memory[0x050 + i] = fontSet[i];
    }

    // Seed RNG with current time
    rngSeed = std::chrono::steady_clock::now().time_since_epoch().count(); // Queries the steady clock for the current time, computes the duration since its epoch, and extracts the tick count as an integer.
    randGen.seed(rngSeed);
    randByte = std::uniform_int_distribution<unsigned short>(0, 255);
}

bool Chip8::LoadROM(const std::string filename) {
    // Open the ROM file in binary mode and instantly seek to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << filename << std::endl;
        return false;
    }
    // Get the file size (current position due to std::ios::ate)
    std::streamsize size = file.tellg();
    // Reset the file pointer to the beginning for reading
    file.seekg(0, std::ios::beg);
    // The CHIP-8 memory starts loading ROMs at 0x200 (512 bytes). And its available memory max ~3583 bytes (from 0x200 to 0xFFF.
    // The total available space is memory size minus this starting offset.
    if (size > sizeof(memory) - 0x200) {
        std::cerr << "ROM too large: " << size << " bytes" << std::endl;
        return false;
    }
    // Read the ROM content directly into the CHIP-8 memory buffer,
    // starting at the required program load address (0x200).
    file.read((char*)(memory + 0x200), size);
    if (!file) {
        // This check handles potential read errors (e.g., partial read)
        std::cerr << "Failed to read ROM" << std::endl;
        return false;
    }
    return true;
}

void Chip8::Cycle() {
    // Combine two bytes into opcode
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Increment PC early (some opcodes may change it)
    pc += 2;

    // Decode and execute based on first nibble (opcode >> 12)
    switch (opcode >> 12) {
    case 0x0: OP_0nnn(); break;
    case 0x1: OP_1nnn(); break;
    case 0x2: OP_2nnn(); break;
    case 0x3: OP_3xnn(); break;
    case 0x4: OP_4xnn(); break;
    case 0x5: OP_5xy0(); break;
    case 0x6: OP_6xnn(); break;
    case 0x7: OP_7xnn(); break;
    case 0x8: {
        // Sub-switch for last nibble
        switch (opcode & 0x000F) {
        case 0x0: OP_8xy0(); break;
        case 0x1: OP_8xy1(); break;
        case 0x2: OP_8xy2(); break;
        case 0x3: OP_8xy3(); break;
        case 0x4: OP_8xy4(); break;
        case 0x5: OP_8xy5(); break;
        case 0x6: OP_8xy6(); break;
        case 0x7: OP_8xy7(); break;
        case 0xE: OP_8xyE(); break;
        default: OP_NULL(); break;
        }
        break;
    }
    case 0x9: OP_9xy0(); break;
    case 0xA: OP_Annn(); break;
    case 0xB: OP_Bnnn(); break;
    case 0xC: OP_Cxnn(); break;
    case 0xD: OP_Dxyn(); break;
    case 0xE: {
        switch (opcode & 0x00FF) {
        case 0x009E: OP_Ex9E(); break;
        case 0x00A1: OP_ExA1(); break;
        default: OP_NULL(); break;
        }
        break;
    }
    case 0xF: {
        switch (opcode & 0x00FF) {
        case 0x0007: OP_Fx07(); break;
        case 0x000A: OP_Fx0A(); break;
        case 0x0015: OP_Fx15(); break;
        case 0x0018: OP_Fx18(); break;
        case 0x001E: OP_Fx1E(); break;
        case 0x0029: OP_Fx29(); break;
        case 0x0033: OP_Fx33(); break;
        case 0x0055: OP_Fx55(); break;
        case 0x0065: OP_Fx65(); break;
        default: OP_NULL(); break;
        }
        break;
    }
    default: OP_NULL(); break;
    }
}

// Stub implementations for all opcode handlers
void Chip8::OP_0nnn() { /* Sys call, clear, or return - to implement */ }
void Chip8::OP_1nnn() { /* Jump to nnn */ }
void Chip8::OP_2nnn() { /* Call subroutine at nnn */ }
void Chip8::OP_3xnn() { /* Skip if VX == nn */ }
void Chip8::OP_4xnn() { /* Skip if VX != nn */ }
void Chip8::OP_5xy0() { /* Skip if VX == VY */ }
void Chip8::OP_6xnn() { /* Set VX to nn */ }
void Chip8::OP_7xnn() { /* Add nn to VX */ }
void Chip8::OP_8xy0() { /* Set VX = VY */ }
void Chip8::OP_8xy1() { /* Set VX = VX | VY */ }
void Chip8::OP_8xy2() { /* Set VX = VX & VY */ }
void Chip8::OP_8xy3() { /* Set VX = VX ^ VY */ }
void Chip8::OP_8xy4() { /* Add VY to VX, VF carry */ }
void Chip8::OP_8xy5() { /* Subtract VY from VX, VF borrow */ }
void Chip8::OP_8xy6() { /* Shift VX right, VF LSB */ }
void Chip8::OP_8xy7() { /* Set VX = VY - VX, VF borrow */ }
void Chip8::OP_8xyE() { /* Shift VX left, VF MSB */ }
void Chip8::OP_9xy0() { /* Skip if VX != VY */ }
void Chip8::OP_Annn() { /* Set I to nnn */ }
void Chip8::OP_Bnnn() { /* Jump to nnn + V0 */ }
void Chip8::OP_Cxnn() { /* Set VX to random & nn */ }
void Chip8::OP_Dxyn() { /* Draw sprite at (VX,VY) height n */ }
void Chip8::OP_Ex9E() { /* Skip if key VX pressed */ }
void Chip8::OP_ExA1() { /* Skip if key VX not pressed */ }
void Chip8::OP_Fx07() { /* Set VX = delay timer */ }
void Chip8::OP_Fx0A() { /* Wait for key press, store in VX */ }
void Chip8::OP_Fx15() { /* Set delay timer = VX */ }
void Chip8::OP_Fx18() { /* Set sound timer = VX */ }
void Chip8::OP_Fx1E() { /* Add VX to I */ }
void Chip8::OP_Fx29() { /* Set I to font sprite for VX */ }
void Chip8::OP_Fx33() { /* Store BCD of VX at I,I+1,I+2 */ }
void Chip8::OP_Fx55() { /* Store V0-VX at I to I+X */ }
void Chip8::OP_Fx65() { /* Load V0-VX from I to I+X */ }
void Chip8::OP_NULL() {
    std::cout << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
}