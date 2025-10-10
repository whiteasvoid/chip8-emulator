#include <cstdint>
#include <fstream>

// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
class Chip8 {
public:

    Chip8();
    bool LoadROM(const std::string filename); // Takes a filename, reads the file in binary mode, and copies its contents into memory from 0x200 onward. It returns bool (true on success, false if file not found or too big).

    uint8_t memory[4096] = {};  // 4KB of RAM (0x000 to 0xFFF)
    uint8_t registers[16] = {}; // V0 to VF registers (V0 through VF - registers[0] => V0 & registers[15] => VF)

    uint16_t index = 0;             // I register (16-bit, for addressing) needed for pointing to memory addresses in operations like loading/storing multiple registers and drawing sprites
    uint16_t pc = 0x200;            // Program Counter (starts at ROM load address) tracks the address of the next instruction to execute
    uint16_t stack[16] = {};        // Stack for calls/returns (16 levels) preventing the emulator from losing track during branches
    uint8_t sp = 0;                 // Stack Pointer (points to top of stack) necessary to manage push/pop operations: increment on call (push), decrement on return (pop)

    uint8_t delayTimer = 0;         // Delay timer (decrements at 60Hz)
    uint8_t soundTimer = 0;         // Sound timer (beeps when >0, also at 60Hz)
};