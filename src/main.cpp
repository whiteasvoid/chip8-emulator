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

    // New: Test keypad
    emulator.keypad[1] = 1;  // Press key '1'
    std::cout << "Keypad[1] pressed: " << static_cast<int>(emulator.keypad[1]) << std::endl;

    // Test display
    emulator.gfx[0] = 0xFFFFFFFF;  // Set top-left pixel 'on'
    emulator.gfx[64 * 31 + 63] = 0xFFFFFFFF;  // Bottom-right
    std::cout << "GFX[0]: 0x" << std::hex << emulator.gfx[0] << std::endl;
    std::cout << "GFX[last]: 0x" << std::hex << emulator.gfx[64 * 31 + 63] << std::endl;

    emulator.drawFlag = true;
    std::cout << "Draw flag: " << emulator.drawFlag << std::endl;

    // Test RNG
    /*for (int i = 0; i < 5; ++i) {
        uint8_t randVal = static_cast<uint8_t>(emulator.randByte(emulator.randGen));
        std::cout << "Random byte " << i << ": 0x" << std::hex << static_cast<int>(randVal) << std::endl;
    }*/
    for (int i = 0; i < 5; ++i) {
        uint8_t randVal = static_cast<uint8_t>(emulator.randByte(emulator.randGen));
        std::cout << "Random byte " << i << ": " << std::dec << static_cast<int>(randVal)
            << " (decimal) / 0x" << std::hex << static_cast<int>(randVal) << " (hex)" << std::endl;
    }

    std::cout << "RNG Seed: " << emulator.rngSeed << std::endl;

    // Test Cycle
    if (emulator.LoadROM("IBMTest.ch8")) {
        for (int i = 0; i < 5; ++i) {  // Run 5 cycles
            emulator.Cycle();
            std::cout << "Cycle " << i << " - Opcode: 0x" << std::hex << emulator.opcode
                << " PC: 0x" << emulator.pc << std::endl;
        }
    }

    return 0;
}