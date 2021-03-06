#include "test.h"

int assert(int expected, int actual) {
  int success = 0;

  if (expected != actual) {
    printf("\t-FAIL: actual %d, expected %d\n", actual, expected);
    success = -1;
  } else {
    printf("\t+PASS\n");
    success = 1;
  }
 
  return success;
}

void reset_vm() {
  for (int i = 0; i < 8; i++) {
    vm->r[i] = 0x00;
  }
  vm->pc = 0x0000;
  vm->sp = 0xFFFE;

  memset(vm->memory, 0, 0xFFFF);
}

void ld16(uint8_t r1, uint8_t r2) {
  
}

int test_in16c() {
  printf("\nTesting 16-bit increment instructions\n");
  uint8_t opcodes[]   = { 0x03, 0x13, 0x23, 0x33 };
  uint8_t registers[] = { B,C,  D,E,  H,L };
  int length = sizeof(opcodes) / sizeof(opcodes[0]);
  
  int failed = 0;
  int passed = 0;
  
  for (int i = 0; i < length; i++) {
    printf("0x%02x", opcodes[i]);
    reset_vm();

    vm->memory[0] = opcodes[i];
    vm->r[registers[i + 1]] = 1;
    emulate();

    assert(2, vm->r[registers[i]]) ? passed++ : failed++;
  }
  printf("Passed: %d\tFailed: %d\n", passed, failed);

  return 0;
}

int test_inc() {
  printf("\nTesting 8-bit increment instructions\n");
  uint8_t opcodes[]   = { 0x3c, 0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c };
  uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
  int length = sizeof(opcodes) / sizeof(opcodes[0]);

  int failed = 0;
  int passed = 0;

  for (int i = 0; i < length; i++) {
    printf("0x%02x", opcodes[i]);
    reset_vm();

    vm->memory[0] = opcodes[i];
    vm->r[registers[i]] = 1;
    emulate();

    assert(2, vm->r[registers[i]]) ? passed++ : failed++;
  }
  printf("Passed: %d\tFailed: %d\n", passed, failed);
  
  return 0;
}

int test_dec() {
  printf("\nTesting 8-bit decrement instructions\n");
  uint8_t opcodes[]   = { 0x3d, 0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d };
  uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
  int length = sizeof(opcodes) / sizeof(opcodes[0]);

  int failed = 0;
  int passed = 0;

  for (int i = 0; i < length; i++) {
    printf("0x%02x", opcodes[i]);
    reset_vm();

    vm->memory[0] = opcodes[i];
    vm->r[registers[i]] = 1;
    emulate();

    assert(0, vm->r[registers[i]]) ? passed++ : failed++;
  }
  printf("Passed: %d\tFailed: %d\n", passed, failed);

  return 0;
}

int test_flags() {
  printf("\nTesting flags\n");

  return 0;
}

int test_add() {
	printf("\nTesting 8-bit add instructions\n");
	uint8_t opcodes[]   = { 0x87, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85 };
	uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
	int length = sizeof(opcodes) / sizeof(opcodes[0]);
	
    int failed = 0;
    int passed = 0;
	
	for (int i = 0; i < length; i++) {
		printf("0x%02x", opcodes[i]);
		reset_vm();
		
		vm->memory[0] = opcodes[i];
		vm->r[A] = 10;
		vm->r[registers[i]] = 20;
		uint8_t expected = vm->r[A] + vm->r[registers[i]];
		emulate();
		
		assert(expected, vm->r[A]) ? passed++ : failed++;
	}
	printf("Passed: %d\tFailed: %d\n", passed, failed);
	
	return 0;
}

int test_add_overflow() {
	printf("\nTesting 8-bit add overflow\n");
	uint8_t opcodes[]   = { 0x87, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85 };
	uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
	int length = sizeof(opcodes) / sizeof(opcodes[0]);
	
    int failed = 0;
    int passed = 0;
	
	for (int i = 0; i < length; i++) {
		printf("0x%02x", opcodes[i]);
		reset_vm();
		
		vm->memory[0] = opcodes[i];
		vm->r[A] = 255;
		if (registers[i] != A) {
			vm->r[registers[i]] = 10;
		}
		
		uint8_t expected = vm->r[A] + vm->r[registers[i]];
		emulate();
		
		int result = 0;
		result += assert(1, FLAG_CHECK(FC));
		result += assert(1, FLAG_CHECK(FH));
		result += assert(expected, vm->r[A]);
		
		result > 0 ? passed++ : failed++;
	}
	printf("Passed: %d\tFailed: %d\n", passed, failed);
	
	return 0;
}

int test_sub() {
	printf("\nTesting 8-bit sub instructions\n");
	uint8_t opcodes[]   = { 0x87, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85 };
	uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
	int length = sizeof(opcodes) / sizeof(opcodes[0]);
	
    int failed = 0;
    int passed = 0;
	
	for (int i = 0; i < length; i++) {
		printf("0x%02x", opcodes[i]);
		reset_vm();
		
		vm->memory[0] = opcodes[i];
		vm->r[A] = 10;
		vm->r[registers[i]] = 20;
		uint8_t expected = vm->r[A] + vm->r[registers[i]];
		emulate();
		
		assert(expected, vm->r[A]) ? passed++ : failed++;
	}
	printf("Passed: %d\tFailed: %d\n", passed, failed);
	
	return 0;
}

void load_memory(uint8_t *instructions) {
  int length = sizeof(instructions) / sizeof(instructions[0]);
  for (int i = 0; i < length; i++) {
    vm->memory[i] = instructions[i];
  }
}

int main(int argc, char *argv[]) {
  int success = 0;
  init_vm();
  test_inc();
  test_dec();
  test_add();
  test_ld_a();
  test_add_overflow();
  /*
  printf("\n");
  reset_vm();
  
  uint8_t instr[] = { 0x26, 0x19, 0x2e, 0x81, 0x36, 37, 0x76 };
  
  load_memory(instr);
  int running = 1;
  while(running) {
    running = emulate();
  }
  printf("%d\n", vm->memory[0x1981]);
  assert(37, vm->memory[0x1981]);
  
  reset_vm();
  vm->r[A] = 4;
  vm->memory[0] = 0x2f;
  vm->memory[1] = 0x76;
  running = 1;
  while(running) {
	  running = emulate();
  }
  print_registers();
*/  
  return 0;
}
