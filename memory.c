#include "memory.h"

uint8_t read_byte(uint16_t address) {
	// todo: map address to proper memory locations
	return vm->memory[address];
}