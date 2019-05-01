#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gbe.h"

int assert(int expected, int actual) {
  int success = 0;

  if (expected != actual) {
    printf("\t-FAIL: Got %d, expected %d\n", actual, expected);
    success = 0;
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

int test_inc() {
  printf("\nTesting 8-bit increment instructions\n");
  uint8_t opcodes[]   = { 0x3c, 0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c };
  uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
  int length = sizeof(opcodes) / sizeof(opcodes[0]);

  for (int i = 0; i < length; i++) {
    printf("0x%02x", opcodes[i]);
    reset_vm();

    vm->memory[0] = opcodes[i];
    vm->r[registers[i]] = 1;
    emulate();

    assert(2, vm->r[registers[i]]);
  }

  return 0;
}

int test_dec() {
  printf("\nTesting 8-bit decrement instructions\n");
  uint8_t opcodes[]   = { 0x3d, 0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d };
  uint8_t registers[] = { A,    B,    C,    D,    E,    H,    L };
  int length = sizeof(opcodes) / sizeof(opcodes[0]);

  for (int i = 0; i < length; i++) {
    printf("0x%02x", opcodes[i]);
    reset_vm();

    vm->memory[0] = opcodes[i];
    vm->r[registers[i]] = 1;
    emulate();

    assert(0, vm->r[registers[i]]);
  }

  return 0;
}

int test_flags() {
  printf("\nTesting flags\n");

  return 0;
}

int main(int argc, char *argv[]) {
  int success = 0;
  init_vm();
  test_inc();
  test_dec();
  return 0;
}
