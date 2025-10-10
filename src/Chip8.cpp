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