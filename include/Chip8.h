#include <cstdint>
#include <fstream>
#include <random>
#include <chrono>

// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
class Chip8 {
public:

    Chip8();
    bool LoadROM(const std::string filename); // Takes a filename, reads the file in binary mode, and copies its contents into memory from 0x200 onward. It returns bool (true on success, false if file not found or too big).
    void Cycle();

    uint8_t memory[4096] = {};  // 4KB of RAM (0x000 to 0xFFF)
    uint8_t registers[16] = {}; // V0 to VF registers (V0 through VF - registers[0] => V0 & registers[15] => VF)

    uint16_t index = 0;             // I register (16-bit, for addressing) needed for pointing to memory addresses in operations like loading/storing multiple registers and drawing sprites
    uint16_t pc = 0x200;            // Program Counter (starts at ROM load address) tracks the address of the next instruction to execute
    uint16_t stack[16] = {};        // Stack for calls/returns (16 levels) preventing the emulator from losing track during branches
    uint8_t sp = 0;                 // Stack Pointer (points to top of stack) necessary to manage push/pop operations: increment on call (push), decrement on return (pop)

    uint8_t delayTimer = 0;         // Delay timer (decrements at 60Hz)
    uint8_t soundTimer = 0;         // Sound timer (beeps when >0, also at 60Hz)

    uint8_t keypad[16] = {};        // 0-F keys (0=released, 1=pressed)
                                    /*
                                    * Each index represents one key on the Chip-8's hexadecimal keypad (keys 0 to F, where F is 15 in decimal).
                                    * When a key is pressed (example, via keyboard input in SDL), set keypad[key] = 1; when released, = 0. Opcodes like FX0A (wait for key press) or EX9E/EXA1 (skip if key pressed/released) will check this array to handle user input in games (example, moving a paddle in Pong).
                                    * Chip-8 originally used a 4x4 hex keypad (like old calculators: rows 1-2-3-C, 4-5-6-D, etc.)
                                    * In our case we are gonna use 16 keys for hex input (0-9, A-F)
                                    */
    uint32_t gfx[64 * 32] = {};     // Display buffer (64 wide x 32 tall)
                                    /* 
                                    * Serve as a framebuffer for the display.
                                    * Chip-8 screen is 64 pixels wide by 32 tall (total 2048 pixels).
                                    * Each element represents one pixel: 0x00000000 means "off" (black), 0xFFFFFFFF means "on" (white).
                                    * The draw opcode (DXYN) will read sprite data from memory, XOR it onto this buffer at coordinates (from registers VX/VY), and set VF=1 if any pixels flip (collision).
                                    */
    bool drawFlag = false;          // Set to true when draw opcode runs
                                    /*
                                    * When a draw opcode (like DXYN or 00E0 clear screen) executes in the emulation cycle, we set drawFlag = true.
                                    * In the main loop, if true, we update the SDL window with the gfx buffer, then reset to false. This avoids redrawing every cycle.
                                    */

    uint16_t opcode = 0;            // Current opcode (2 bytes)
                                    /*
                                    * In each emulation cycle, we fetch the next instruction: opcode = (memory[pc] << 8) | memory[pc+1]; (combines two bytes into 16 bits).
                                    * Then, we decode it (example, via switch on opcode & 0xF000) to execute actions like jumps or adds. PC increments by 2 afterward.
                                    */

    std::default_random_engine randGen;                        // RNG Engine
	unsigned rngSeed = 0;							           // RNG Seed
	std::uniform_int_distribution<unsigned short> randByte;    // Random byte (0-255) generator

private:
    // Opcode handlers (placeholders)
    void OP_0nnn();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xnn();
    void OP_4xnn();
    void OP_5xy0();
    void OP_6xnn();
    void OP_7xnn();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxnn();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
    void OP_NULL();  // For invalid opcodes
};