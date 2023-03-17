#ifndef CHIP8_ALIAS_H_
#define CHIP8_ALIAS_H_


/// Most Chip-8 programs start at location 0x200 (512), but some begin at 0x600 (1536). Programs beginning at 0x600 are
/// intended for the ETI 660 computer.
#define PROGRAM_START_LOCATION (0x200u)

#endif