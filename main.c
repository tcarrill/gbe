#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gbe.h"

long readBinary(uint8_t **buffer, char *const filename) {
  FILE *fileptr = fopen(filename, "rb");

  fseek(fileptr, 0, SEEK_END);
  long filelen = ftell(fileptr);
  rewind(fileptr);
  *buffer = (uint8_t*)malloc((filelen + 1) * sizeof(char));
  fread(*buffer, filelen, 1, fileptr);
  fclose(fileptr);

  return filelen;
}

void read_cart_info() {
  printf("Cartridge Header Information\n");
  printf("Title: ");
  for (int i = 0x0134; i < 0x0143; i++) {
    printf("%c", vm->memory[i]);
  }
  
  printf("\n");
  switch(vm->memory[0x0143]) {
  case 0x00: printf("Non Color GB\n"); break;
  case 0x80: printf("Color GB\n"); break;
  }

  printf("Cartridge type: ");
  switch(vm->memory[0x0147]) {
  case 0x00: printf("ROM ONLY\n"); break;
  case 0x01: printf("ROM+MBC1\n"); break;
  case 0x02: printf("ROM+MBC1+RAM\n"); break;
  case 0x03: printf("ROM+MBC1+RAM+BATT\n"); break;
  case 0x05: printf("ROM+MBC2\n"); break;
  case 0x06: printf("ROM+MBC2+BATTERY\n"); break;
  case 0x08: printf("ROM+RAM\n"); break;
  case 0x09: printf("ROM+RAM+BATTERY\n"); break;
  case 0x0B: printf("ROM+MMM01\n"); break;
  case 0x0C: printf("ROM+MMM01+SRAM\n"); break;
  case 0x0D: printf("ROM+MMM01+SRAM+BATT\n"); break;
  case 0x0F: printf("ROM+MBC3+TIMER+BATT\n"); break;
  case 0x10: printf("ROM+MBC3+TIMER+RAM+BATT\n"); break;
  case 0x11: printf("ROM+MBC3\n"); break;
  case 0x12: printf("ROM+MBC3+RAM\n"); break;
  case 0x13: printf("ROM+MBC3+RAM+BATT\n"); break;
  case 0x19: printf("ROM+MBC5\n"); break;
  case 0x1A: printf("ROM+MBC5+RAM\n"); break;
  case 0x1B: printf("ROM+MBC5+RAM+BATT\n"); break;
  case 0x1C: printf("ROM+MBC5+RUMBLE\n"); break;
  case 0x1D: printf("ROM+MBC5+RUMBLE+SRAM\n"); break;
  case 0x1E: printf("ROM+MBC5+RUMBLE+SRAM+BATT\n"); break;
  case 0x1F: printf("Pocket Camera\n"); break;
  case 0xFD: printf("Bandai TAMA5\n"); break;
  case 0xFE: printf("Hudson HuC-3\n"); break;
  case 0xFF: printf("Hudson HuC-1\n"); break;
  }
  printf("ROM size: ");
  switch(vm->memory[0x0148]) {
  case 0x00: printf("256Kbit = 32KByte = 2 banks\n"); break;
  case 0x01: printf("512Kbit = 64KByte = 4 banks\n"); break;
  case 0x02: printf("1Mbit = 128KByte = 8 banks\n"); break;
  case 0x03: printf("2Mbit = 256KByte = 16 banks\n"); break;
  case 0x04: printf("4Mbit = 512KByte = 32 banks\n"); break;
  case 0x05: printf("8Mbit = 1MByte = 64 banks\n"); break;
  case 0x06: printf("16Mbit = 2MByte = 128 banks\n"); break;
  case 0x52: printf("9Mbit = 1.1MByte = 72 banks\n"); break;
  case 0x53: printf("10Mbit = 1.2MByte = 80 banks\n"); break;
  case 0x54: printf("12Mbit = 1.5MByte = 96 banks\n"); break;
  }
  
  printf("RAM size: ");
  switch(vm->memory[0x0149]) {
  case 0x00: printf("None\n"); break;
  case 0x01: printf("16kBit = 2kB = 1 bank\n"); break;
  case 0x02: printf("64kBit = 8kB = 1 bank\n"); break;
  case 0x03: printf("256kBit = 32kB = 4 banks\n"); break;
  case 0x04: printf("1MBit =128kB =16 banks\n"); break;
  }
  
  printf("Language: ");
  switch(vm->memory[0x014A]) {
  case 0x00: printf("Japanese\n"); break;
  case 0x01: printf("Non-Japanese\n"); break;
  }
  
  printf("\n");  
}

int main(int argc, char *argv[]) {
  init_vm();
  memcpy(vm->memory, BOOT_ROM, 256);

  uint8_t *cart = NULL;
  long length = readBinary(&cart, argv[1]);
  for (int i = 0x0100; i <= 0x7FFF; i++) {
    vm->memory[i] = cart[i];
  }
  free(cart);

  read_cart_info();

  int running = 1;
  while (running) {
    running = emulate();
  }

  free(vm);
}
