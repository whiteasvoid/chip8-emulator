#include <iostream>
#include "../include/Chip8.h"

int main() {
    Chip8 emulator;

	// Test: Set random values in memory and registers and print them
    emulator.memory[0] = 0xAB;
    emulator.registers[0] = 0xCD;

    std::cout << "Memory[0]: 0x" << std::hex << (int)(emulator.memory[0]) << std::endl;
    std::cout << "Register V0: 0x" << std::hex << (int)(emulator.registers[0]) << std::endl;

    emulator.pc = 0x300;
    emulator.index = 0x123;
    emulator.stack[0] = 0x456;
    emulator.sp = 1;
    emulator.delayTimer = 60;

    std::cout << "PC: 0x" << std::hex << emulator.pc << std::endl;
    std::cout << "Index (I): 0x" << std::hex << emulator.index << std::endl;
    std::cout << "Stack[0]: 0x" << std::hex << emulator.stack[0] << std::endl;
    std::cout << "SP: " << static_cast<int>(emulator.sp) << std::endl;
    std::cout << "Delay Timer: " << static_cast<int>(emulator.delayTimer) << std::endl;

    /*
    The first 10 fonts are digits 0-9 (indices 0 to 9 in fontSet).
    'A' is the 11th font (index 10, since counting starts at 0).
    Start offset for 'A': 10 sprites × 5 bytes = 50 bytes into the font data.
    Fonts load starting at memory[0x050], so 'A' begins at 0x050 + 50 = 0x082.
    The expression grabs the first byte of 'A's sprite (row 1 of its pixel data).
    */
    // Check font loading (e.g., first byte of '0' and 'A')
    std::cout << "Font '0' first byte (should be 0xf0): 0x" << std::hex << static_cast<int>(emulator.memory[0x050]) << std::endl;
    std::cout << "Font 'A' first byte (should be 0xf0): 0x" << std::hex << static_cast<int>(emulator.memory[0x050 + 5 * 10]) << std::endl;  // 'A' starts after 10 digits

    // Load a ROM
    if (emulator.LoadROM("IBMTest.ch8")) {  // Replace with your ROM filename
        std::cout << "ROM loaded successfully!" << std::endl;
        // Print first two bytes of ROM (opcode at 0x200)
        std::cout << "First opcode: 0x" << std::hex << static_cast<int>(emulator.memory[0x200])
            << static_cast<int>(emulator.memory[0x201]) << std::endl;
    }
    else {
        std::cout << "Failed to load ROM." << std::endl;
    }

    return 0;
}