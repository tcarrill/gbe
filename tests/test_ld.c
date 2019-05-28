#include "test.h"

void setup_registers(uint8_t value) {
	vm->r[A] = value;
	vm->r[B] = value;
	vm->r[C] = value;
	vm->r[D] = value;
	vm->r[E] = value;
	vm->r[H] = value;
	vm->r[L] = value;
}

int test_ld_a() {
	printf("\nTesting LD A instructions\n");
	uint8_t opcodes[] =   { 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D };
	int length = sizeof(opcodes) / sizeof(opcodes[0]);
	
    int failed = 0;
    int passed = 0;
	
	uint8_t value = 15;
	setup_registers(value);
	
	for (int i = 0; i < length; i++) {
		printf("0x%02x", opcodes[i]);
		vm->r[A] = 0;
		vm->pc = 0;
		vm->memory[0] = opcodes[i];
		emulate();
		
		assert(value, vm->r[A]) ? passed++ : failed++;
	}
	printf("Passed: %d\tFailed: %d\n", passed, failed);
}