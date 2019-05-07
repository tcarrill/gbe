#include "test.h"

int test_ld_a(uint8_t r) {
	uint8_t opcodes[] =   { 0x7F, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D };
	uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L }
	int length = sizeof(opcodes) / sizeof(opcodes[0]);
	for (int i = 0; i < length; i++) {
		vm->memory[0] = opcodes[i];
		vm->r[registers[i]] = 14;
		emulate();
		
		assert(14, vm->r[A]);
	}
}