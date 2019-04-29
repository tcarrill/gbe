#include <string.h>
#include "gbe.h"

#define MASK8(x) (x & 0xF)

const char *byte_to_binary(int x) {
    static char b[9];
    b[0] = '\0';

    for (int z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

void print_registers() {
	printf("\nRegisters\n");
	printf("A    BC    DE    HL    PC    SP\n");
	printf("%02x  %02x%02x  %02x%02x  %02x%02x  %04x  %04x\n", vm->r[A], vm->r[B], vm->r[C], vm->r[D], vm->r[E], vm->r[H], vm->r[L], vm->pc, vm->sp);
	printf("   ZNHC\n");
	printf("F: %sb\n", byte_to_binary(vm->r[F]));
}

void todo_implement() {
  vm->pc--;
  printf("0x%02x is an unimplemented instruction!\n", vm->memory[vm->pc]);
  print_registers();
  free(vm);
  exit(1);
}

uint8_t fetch() {
	uint8_t opcode = vm->memory[vm->pc];
	vm->pc++;
	return opcode;
}

void rl_carry(uint8_t r) {
	
}

void cb() {
	uint8_t opcode = fetch();
	switch (opcode) {
		case 0x7C: 
					(vm->r[H] & (1 << 7)) == 0 ? FLAG_SET(FZ) : FLAG_CLEAR(FZ);
					FLAG_CLEAR(FN);
					FLAG_SET(FH); 
					break;
		case 0x11: 
					{
					int8_t carry = (vm->r[C] & 0x80) >> 7;
					(vm->r[C] & 0x80) ? FLAG_SET(FC) : FLAG_CLEAR(FC);
					
					vm->r[C] <<= 1;
					vm->r[C] += carry;
					
					(vm->r[C] == 0) ? FLAG_SET(FZ) : FLAG_CLEAR(FZ);
					break;
					}
	}
}

void inc(uint8_t r) {
	((vm->r[r] & 0x0F) == 0x0F) ? FLAG_SET(FH) : FLAG_CLEAR(FH);
	vm->r[r]++;
	(vm->r[r] == 0) ? FLAG_SET(FZ) : FLAG_CLEAR(FZ);
	FLAG_CLEAR(FN);
}

void dec(uint8_t r) {
	(vm->r[r] & 0x0F) ? FLAG_CLEAR(FH) : FLAG_SET(FH);	
	vm->r[r]--;
	(vm->r[r] == 0) ? FLAG_SET(FZ) : FLAG_CLEAR(FZ);
	FLAG_SET(FN);
}

int emulate() {
	printf("#######################################\n");
	printf("0x%04x: ", vm->pc);
	uint8_t opcode = fetch();
	printf("0x%02x\n", opcode);
	print_registers();
	printf("#######################################\n");
  	switch (opcode) {
	  case 0x00: break;
	  case 0x01: todo_implement(); break;
	  case 0x02: todo_implement(); break;
	  case 0x03: todo_implement(); break;
	  case 0x04: inc(B); break;
	  case 0x05: dec(B); break;
	  case 0x06:
	  			vm->r[B] = fetch();
	  			break;
	  case 0x07: todo_implement(); break;
	  case 0x08: todo_implement(); break;
	  case 0x09: todo_implement(); break;
	  case 0x0A: todo_implement(); break;
	  case 0x0B: todo_implement(); break;
	  case 0x0C: inc(C); break;
	  case 0x0D: 
	  			dec(C); 
				break;
	  case 0x0E:
	            vm->r[C] = fetch();
	  			break;
	  case 0x0F: todo_implement(); break;
	  case 0x10: todo_implement(); break;
	  case 0x11: 
	            vm->r[E] = fetch();
				vm->r[D] = fetch();
				break;
	  case 0x12: todo_implement(); break;
	  case 0x13: 
				{
					uint16_t de = (vm->r[D] << 8) | vm->r[E];
	 				de++;
	 				vm->r[D] = de >> 8;
	 				vm->r[E] = de & 0xFF;
	 				break;
 				}
	  case 0x14: todo_implement(); break;
	  case 0x15: todo_implement(); break;
	  case 0x16: 
	  			vm->r[D] = fetch();
				break;
	  case 0x17: 
	  			{
  				uint8_t carry = FLAG_SET(FC) ? 1 : 0;
	
  				(vm->r[A] & 0x80) ? FLAG_SET(FC) : FLAG_CLEAR(FC);
	
  				vm->r[A] <<= 1;
  				vm->r[A] += carry;
	
  				FLAG_CLEAR(FZ | FN | FH);
				}
				break;
	  case 0x18:
	  			{
					int8_t offset = (int8_t)fetch(); //convert to signed 2s complement
					vm->pc += offset;
				}
	  			break;
	  case 0x19: todo_implement(); break;
	  case 0x1A: 
	  			{
		  			uint16_t address = (vm->r[D] << 8) | vm->r[E];
		  			vm->r[A] = vm->memory[address];
					break;
	  	  		}
	  case 0x1B: todo_implement(); break;
	  case 0x1C: todo_implement(); break;
	  case 0x1D: todo_implement(); break;
	  case 0x1E: 
				vm->r[E] = fetch();
				break;
	  case 0x1F: todo_implement(); break;
	  case 0x20: 
	  			{
	  	 			if (FLAG_CHECK(FZ) == 0) {
						int8_t offset = (int8_t)fetch(); //convert to signed 2s complement
						vm->pc += offset;
	  	 			} else {
	  	 				vm->pc++;
	  	 			}
	  				break;
				}
	  case 0x21: 
	  			vm->r[L] = fetch();
	  		  	vm->r[H] = fetch();
				break;
	  case 0x22: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
	 				vm->memory[address] = vm->r[A];
	 			   	address++;
	 			  	vm->r[H] = address >> 8;
	 			 	vm->r[L] = address & 0xFF;
 				}
				break;
	  case 0x23:
	  			{
					uint16_t hl = (vm->r[H] << 8) | vm->r[L];
   					hl++;
  					vm->r[H] = hl >> 8;
 					vm->r[L] = hl & 0xFF;
				}
	  			break;
	  case 0x24: todo_implement(); break;
	  case 0x25: todo_implement(); break;
	  case 0x26: 
	  			vm->r[H] = fetch();
				break;
	  case 0x27: todo_implement(); break;
	  case 0x28:
				{
					if (FLAG_CHECK(FZ) == 1) {
						int8_t offset = (int8_t)fetch(); //convert to signed 2s complement
						vm->pc += offset;
					} else {
						vm->pc++;
					}
				}
	  			break;
	  case 0x29: todo_implement(); break;
	  case 0x2A: todo_implement(); break;
	  case 0x2B: todo_implement(); break;
	  case 0x2C: todo_implement(); break;
	  case 0x2D: todo_implement(); break;
	  case 0x2E: 
	  			vm->r[L] = fetch();
				break;
	  case 0x2F: todo_implement(); break;
	  case 0x30: todo_implement(); break;
	  case 0x31: 
	  			{
	  				uint8_t lb = fetch();
	  		  		uint8_t hb = fetch();  
	  				vm->sp = (hb << 8) | lb;
					break;
				}
	  case 0x32: 
	  			{
	  				uint16_t address = (vm->r[H] << 8) | vm->r[L];
				 	vm->memory[address] = vm->r[A];
				 	address--;
				 	vm->r[H] = address >> 8;
				 	vm->r[L] = address & 0xFF;
	  			 	break;
			 	}
	  case 0x33: todo_implement(); break;
	  case 0x34: todo_implement(); break;
	  case 0x35: todo_implement(); break;
	  case 0x36: todo_implement(); break;
	  case 0x37: todo_implement(); break;
	  case 0x38: todo_implement(); break;
	  case 0x39: todo_implement(); break;
	  case 0x3A: todo_implement(); break;
	  case 0x3B: todo_implement(); break;
	  case 0x3C: todo_implement(); break;
	  case 0x3D:
				dec(A);
				break;
	  case 0x3E: 
	  			vm->r[A] = fetch();
				break;
	  case 0x3F: todo_implement(); break;
	  case 0x40: vm->r[B] = vm->r[B]; break;
	  case 0x41: vm->r[B] = vm->r[C]; break;
	  case 0x42: vm->r[B] = vm->r[D]; break;
	  case 0x43: vm->r[B] = vm->r[E]; break;
	  case 0x44: vm->r[B] = vm->r[H]; break;
	  case 0x45: vm->r[B] = vm->r[L]; break;
	  case 0x46: todo_implement(); break;
	  case 0x47: todo_implement(); break;
	  case 0x48: vm->r[C] = vm->r[B]; break;
	  case 0x49: vm->r[C] = vm->r[C]; break;
	  case 0x4A: vm->r[C] = vm->r[D]; break;
	  case 0x4B: vm->r[C] = vm->r[E]; break;
	  case 0x4C: vm->r[C] = vm->r[H]; break;
	  case 0x4D: vm->r[C] = vm->r[L]; break;
	  case 0x4E: todo_implement(); break;
	  case 0x4F: vm->r[C] = vm->r[A]; break;
	  case 0x50: vm->r[D] = vm->r[B]; break;
	  case 0x51: vm->r[D] = vm->r[C]; break;
	  case 0x52: vm->r[D] = vm->r[D]; break;
	  case 0x53: vm->r[D] = vm->r[E]; break;
	  case 0x54: vm->r[D] = vm->r[H]; break;
	  case 0x55: vm->r[D] = vm->r[L]; break;
	  case 0x56: todo_implement(); break;
	  case 0x57: vm->r[D] = vm->r[A]; break;
	  case 0x58: vm->r[E] = vm->r[B]; break;
	  case 0x59: vm->r[E] = vm->r[C]; break;
	  case 0x5A: vm->r[E] = vm->r[D]; break;
	  case 0x5B: vm->r[E] = vm->r[E]; break;
	  case 0x5C: vm->r[E] = vm->r[H]; break;
	  case 0x5D: vm->r[E] = vm->r[L]; break;
	  case 0x5E: todo_implement(); break;
	  case 0x5F: todo_implement(); break;
	  case 0x60: vm->r[H] = vm->r[B]; break;
	  case 0x61: vm->r[H] = vm->r[C]; break;
	  case 0x62: vm->r[H] = vm->r[D]; break;
	  case 0x63: vm->r[H] = vm->r[E]; break;
	  case 0x64: vm->r[H] = vm->r[H]; break;
	  case 0x65: vm->r[H] = vm->r[L]; break;
	  case 0x66: todo_implement(); break;
	  case 0x67: vm->r[H] = vm->r[A]; break;
	  case 0x68: vm->r[L] = vm->r[B]; break;
	  case 0x69: vm->r[L] = vm->r[C]; break;
	  case 0x6A: vm->r[L] = vm->r[D]; break;
	  case 0x6B: vm->r[L] = vm->r[E]; break;
	  case 0x6C: vm->r[L] = vm->r[H]; break;
	  case 0x6D: vm->r[L] = vm->r[L]; break;
	  case 0x6E: todo_implement(); break;
	  case 0x6F: vm->r[L] = vm->r[A]; break;
	  case 0x70: todo_implement(); break;
	  case 0x71: todo_implement(); break;
	  case 0x72: todo_implement(); break;
	  case 0x73: todo_implement(); break;
	  case 0x74: todo_implement(); break;
	  case 0x75: todo_implement(); break;
	  case 0x76: todo_implement(); break;
	  case 0x77: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
	 				vm->memory[address] = vm->r[A];
	 				break;
 				}
	  case 0x78: vm->r[A] = vm->r[B]; break;
	  case 0x79: vm->r[A] = vm->r[C]; break;
	  case 0x7A: vm->r[A] = vm->r[D]; break;
	  case 0x7B: vm->r[A] = vm->r[E]; break;
	  case 0x7C: vm->r[A] = vm->r[H]; break;
	  case 0x7D: vm->r[A] = vm->r[L]; break;
	  case 0x7E: todo_implement(); break;
	  case 0x7F: vm->r[A] = vm->r[A]; break;
	  case 0x80: todo_implement(); break;
	  case 0x81: todo_implement(); break;
	  case 0x82: todo_implement(); break;
	  case 0x83: todo_implement(); break;
	  case 0x84: todo_implement(); break;
	  case 0x85: todo_implement(); break;
	  case 0x86: todo_implement(); break;
	  case 0x87: todo_implement(); break;
	  case 0x88: todo_implement(); break;
	  case 0x89: todo_implement(); break;
	  case 0x8A: todo_implement(); break;
	  case 0x8B: todo_implement(); break;
	  case 0x8C: todo_implement(); break;
	  case 0x8D: todo_implement(); break;
	  case 0x8E: todo_implement(); break;
	  case 0x8F: todo_implement(); break;
	  case 0x90: todo_implement(); break;
	  case 0x91: todo_implement(); break;
	  case 0x92: todo_implement(); break;
	  case 0x93: todo_implement(); break;
	  case 0x94: todo_implement(); break;
	  case 0x95: todo_implement(); break;
	  case 0x96: todo_implement(); break;
	  case 0x97: todo_implement(); break;
	  case 0x98: todo_implement(); break;
	  case 0x99: todo_implement(); break;
	  case 0x9A: todo_implement(); break;
	  case 0x9B: todo_implement(); break;
	  case 0x9C: todo_implement(); break;
	  case 0x9D: todo_implement(); break;
	  case 0x9E: todo_implement(); break;
	  case 0x9F: todo_implement(); break;
	  case 0xA0: todo_implement(); break;
	  case 0xA1: todo_implement(); break;
	  case 0xA2: todo_implement(); break;
	  case 0xA3: todo_implement(); break;
	  case 0xA4: todo_implement(); break;
	  case 0xA5: todo_implement(); break;
	  case 0xA6: todo_implement(); break;
	  case 0xA7: todo_implement(); break;
	  case 0xA8: todo_implement(); break;
	  case 0xA9: todo_implement(); break;
	  case 0xAA: todo_implement(); break;
	  case 0xAB: todo_implement(); break;
	  case 0xAC: todo_implement(); break;
	  case 0xAD: todo_implement(); break;
	  case 0xAE: todo_implement(); break;
	  case 0xAF: 
	  			vm->r[A] ^= vm->r[A];
				vm->r[A] == 0 ? FLAG_SET(FZ) : FLAG_CLEAR(FZ);
				FLAG_CLEAR(FN | FH | FC);
				break;
	  case 0xB0: todo_implement(); break;
	  case 0xB1: todo_implement(); break;
	  case 0xB2: todo_implement(); break;
	  case 0xB3: todo_implement(); break;
	  case 0xB4: todo_implement(); break;
	  case 0xB5: todo_implement(); break;
	  case 0xB6: todo_implement(); break;
	  case 0xB7: todo_implement(); break;
	  case 0xB8: todo_implement(); break;
	  case 0xB9: todo_implement(); break;
	  case 0xBA: todo_implement(); break;
	  case 0xBB: todo_implement(); break;
	  case 0xBC: todo_implement(); break;
	  case 0xBD: todo_implement(); break;
	  case 0xBE: todo_implement(); break;
	  case 0xBF: todo_implement(); break;
	  case 0xC0: todo_implement(); break;
	  case 0xC1:
	  			vm->r[C] = vm->memory[vm->sp];
	  		  	vm->sp++;
	  		  	vm->r[B] = vm->memory[vm->sp];
	  		  	vm->sp++;
	  			break;
	  case 0xC2: todo_implement(); break;
	  case 0xC3: 
	  			{
	  				uint8_t lb = fetch();
	  		  		uint8_t hb = fetch();  
	  				vm->pc = (hb << 8) | lb;
					break;
				}
	  case 0xC4: todo_implement(); break;
	  case 0xC5:
	  			vm->sp--;
	  		  	vm->memory[vm->sp] = vm->r[B];				
	  		  	vm->sp--;
	  		  	vm->memory[vm->sp] = vm->r[C];
	  			break;
	  case 0xC6: todo_implement(); break;
	  case 0xC7: todo_implement(); break;
	  case 0xC8: todo_implement(); break;
	  case 0xC9:
	  			vm->pc = vm->memory[vm->sp] | (vm->memory[vm->sp + 1] << 8);
				vm->sp += 2;
	  			break;
	  case 0xCA: todo_implement(); break;
	  case 0xCB: 
	  			cb();
				break;
	  case 0xCC: todo_implement(); break;
	  case 0xCD: 
	  			{
	  				uint16_t ret = vm->pc + 2;
	  				vm->memory[vm->sp - 1] = (ret >> 8) & 0xff;
	  				vm->memory[vm->sp - 2] = ret & 0xff;
	  				vm->sp = vm->sp - 2;
	  				uint8_t lb = fetch();
	  		  		uint8_t hb = fetch(); 
	  				vm->pc = (hb << 8) | lb;		
	  			}
	   			break;
	  case 0xCE: todo_implement(); break;
	  case 0xCF: todo_implement(); break;
	  case 0xD0: todo_implement(); break;
	  case 0xD1: todo_implement(); break;
	  case 0xD2: todo_implement(); break;
	  case 0xD3: todo_implement(); break;
	  case 0xD4: todo_implement(); break;
	  case 0xD5: todo_implement(); break;
	  case 0xD6: todo_implement(); break;
	  case 0xD7: todo_implement(); break;
	  case 0xD8: todo_implement(); break;
	  case 0xD9: todo_implement(); break;
	  case 0xDA: todo_implement(); break;
	  case 0xDB: todo_implement(); break;
	  case 0xDC: todo_implement(); break;
	  case 0xDD: todo_implement(); break;
	  case 0xDE: todo_implement(); break;
	  case 0xDF: todo_implement(); break;
	  case 0xE0: 
				{
					int8_t offset = (int8_t)fetch();
					uint16_t address = 0xFF00 + offset;
					vm->memory[address] = vm->r[A];
					printf("[0x%04x]: 0x%02x\n", address, vm->memory[address]);
    			}
				break;
	  case 0xE1: todo_implement(); break;
	  case 0xE2:
	  			{
					uint16_t address = 0xFF00 + vm->r[C];
		  			vm->memory[address] = vm->r[A];	
					printf("[0x%04x]: 0x%02x\n", address, vm->memory[address]);
	  		    }
				break;
	  case 0xE3: todo_implement(); break;
	  case 0xE4: todo_implement(); break;
	  case 0xE5: todo_implement(); break;
	  case 0xE6: todo_implement(); break;
	  case 0xE7: todo_implement(); break;
	  case 0xE8: todo_implement(); break;
	  case 0xE9: todo_implement(); break;
	  case 0xEA:
	  			{
					uint16_t address = (fetch() << 8) | fetch();
	  		  		vm->memory[address] = vm->r[A];
					printf("[0x%04x]: 0x%02x\n", address, vm->memory[address]);
				}
				break;
	  case 0xEB: todo_implement(); break;
	  case 0xEC: todo_implement(); break;
	  case 0xED: todo_implement(); break;
	  case 0xEE: todo_implement(); break;
	  case 0xEF: todo_implement(); break;
	  case 0xF0: todo_implement(); break;
				{
					uint8_t offset = fetch();
					uint16_t address = 0xFF00 + offset;
					vm->r[A] = vm->memory[address];
					printf("a: 0x%02x\n", vm->r[A]);					
				}
	  			break;
	  case 0xF1: todo_implement(); break;
	  case 0xF2: todo_implement(); break;
	  case 0xF3: todo_implement(); break;
	  case 0xF4: todo_implement(); break;
	  case 0xF5: todo_implement(); break;
	  case 0xF6: todo_implement(); break;
	  case 0xF7: todo_implement(); break;
	  case 0xF8: todo_implement(); break;
	  case 0xF9: todo_implement(); break;
	  case 0xFA: todo_implement(); break;
	  case 0xFB: todo_implement(); break;
	  case 0xFC: todo_implement(); break;
	  case 0xFD: todo_implement(); break;
	  case 0xFE:  
	  			{
		  		  	uint8_t value = fetch();
		  		  	uint8_t res = vm->r[A] - value;
		  		  	res == 0 ? FLAG_SET(FZ) : FLAG_CLEAR(FZ);
		  		  	vm->r[A] < res ? FLAG_SET(FC) : FLAG_CLEAR(FC);
		  		  	(MASK8(vm->r[A]) - MASK8(value)) < 0 ? FLAG_SET(FH) : FLAG_CLEAR(FH);
		  		  	FLAG_SET(FN);
	  		  	}
	  		  	break;
	  case 0xFF: todo_implement(); break;   
  	}

  	return 1;
}

void init_vm() {	
    vm = (Vm *)malloc(sizeof(Vm));
    if (vm == NULL) {
		printf("Could not initialize VM\n");
		exit(1);
    }
	for (int i = 0; i < 8; i++) {
		vm->r[i] = 0x00;
	}
    vm->pc = 0x0000;
    vm->sp = 0xFFFE;
  	vm->memory = calloc(1, 0xFFFF);
}

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
