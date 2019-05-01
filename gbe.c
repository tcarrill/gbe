#include <string.h>
#include "gbe.h"

#define MASK8(x) (x & 0xF)

Vm *vm;

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

void inc16(uint8_t r1, uint8_t r2) {
		uint16_t v = (vm->r[r1] << 8) | vm->r[r2];
		v++;
		vm->r[r1] = v >> 8;
		vm->r[r2] = v & 0xFF;
}

void dec16(uint8_t r1, uint8_t r2) {
		uint16_t v = (vm->r[r1] << 8) | vm->r[r2];
		v--;
		vm->r[r1] = v >> 8;
		vm->r[r2] = v & 0xFF;
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
	uint8_t opcode = fetch();
  	switch (opcode) {
	  case 0x00: break;
	  case 0x01: 
				vm->r[C] = fetch();
				vm->r[B] = fetch();
				break;
	  case 0x02: 
				{
					uint16_t address = (vm->r[B] << 8) | vm->r[C];
					vm->memory[address] = vm->r[A];
				}
				break;
	  case 0x03: inc16(B, C); break;
	  case 0x04: inc(B); break;
	  case 0x05: dec(B); break;
	  case 0x06: vm->r[B] = fetch(); break;
	  case 0x07: todo_implement(); break;
	  case 0x08: todo_implement(); break;
	  case 0x09: todo_implement(); break;
	  case 0x0A: 
				{
					uint16_t address = (vm->r[B] << 8) | vm->r[C];
					vm->r[A] = vm->memory[address]
				}
				break;
	  case 0x0B: dec16(B, C); break;
	  case 0x0C: inc(C); break;
	  case 0x0D: dec(C); break;
	  case 0x0E: vm->r[C] = fetch(); break;
	  case 0x0F: todo_implement(); break;
	  case 0x10: todo_implement(); break;
	  case 0x11: 
				vm->r[E] = fetch();
				vm->r[D] = fetch();
				break;
	  case 0x12: 
				{
					uint16_t address = (vm->r[D] << 8) | vm->r[E];
					vm->memory[address] = vm->r[A];
				}
				break;
	  case 0x13: inc16(D, E); break;
	  case 0x14: inc(D); break;
	  case 0x15: dec(D); break;
	  case 0x16: vm->r[D] = fetch(); break;
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
	  case 0x1B: dec16(D, E); break;
	  case 0x1C: inc(E); break;
	  case 0x1D: dec(E); break;
	  case 0x1E: vm->r[E] = fetch(); break;
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
	  case 0x23: inc16(H, L); break;
	  case 0x24: inc(H); break;
	  case 0x25: dec(H); break;
	  case 0x26: vm->r[H] = fetch(); break;
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
	  case 0x2B: dec16(H, L); break;
	  case 0x2C: inc(L); break;
	  case 0x2D: dec(L); break;
	  case 0x2E: vm->r[L] = fetch(); break;
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
	  case 0x33: vm->sp++; break;
	  case 0x34: todo_implement(); break;
	  case 0x35: todo_implement(); break;
	  case 0x36: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = fetch();
				}
				break;
	  case 0x37: todo_implement(); break;
	  case 0x38: todo_implement(); break;
	  case 0x39: todo_implement(); break;
	  case 0x3A: todo_implement(); break;
	  case 0x3B: vm->sp--; break;
	  case 0x3C: inc(A); break;
	  case 0x3D: dec(A); break;
	  case 0x3E: vm->r[A] = fetch(); break;
	  case 0x3F: todo_implement(); break;
	  case 0x40: vm->r[B] = vm->r[B]; break;
	  case 0x41: vm->r[B] = vm->r[C]; break;
	  case 0x42: vm->r[B] = vm->r[D]; break;
	  case 0x43: vm->r[B] = vm->r[E]; break;
	  case 0x44: vm->r[B] = vm->r[H]; break;
	  case 0x45: vm->r[B] = vm->r[L]; break;
	  case 0x46: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[B] = vm->memory[address];
  				}
				break;
	  case 0x47: todo_implement(); break;
	  case 0x48: vm->r[C] = vm->r[B]; break;
	  case 0x49: vm->r[C] = vm->r[C]; break;
	  case 0x4A: vm->r[C] = vm->r[D]; break;
	  case 0x4B: vm->r[C] = vm->r[E]; break;
	  case 0x4C: vm->r[C] = vm->r[H]; break;
	  case 0x4D: vm->r[C] = vm->r[L]; break;
	  case 0x4E: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[C] = vm->memory[address];
  				}
				break;
	  case 0x4F: vm->r[C] = vm->r[A]; break;
	  case 0x50: vm->r[D] = vm->r[B]; break;
	  case 0x51: vm->r[D] = vm->r[C]; break;
	  case 0x52: vm->r[D] = vm->r[D]; break;
	  case 0x53: vm->r[D] = vm->r[E]; break;
	  case 0x54: vm->r[D] = vm->r[H]; break;
	  case 0x55: vm->r[D] = vm->r[L]; break;
	  case 0x56: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[D] = vm->memory[address];
  				}
				break;
	  case 0x57: vm->r[D] = vm->r[A]; break;
	  case 0x58: vm->r[E] = vm->r[B]; break;
	  case 0x59: vm->r[E] = vm->r[C]; break;
	  case 0x5A: vm->r[E] = vm->r[D]; break;
	  case 0x5B: vm->r[E] = vm->r[E]; break;
	  case 0x5C: vm->r[E] = vm->r[H]; break;
	  case 0x5D: vm->r[E] = vm->r[L]; break;
	  case 0x5E: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[E] = vm->memory[address];
  				}
				break;
	  case 0x5F: todo_implement(); break;
	  case 0x60: vm->r[H] = vm->r[B]; break;
	  case 0x61: vm->r[H] = vm->r[C]; break;
	  case 0x62: vm->r[H] = vm->r[D]; break;
	  case 0x63: vm->r[H] = vm->r[E]; break;
	  case 0x64: vm->r[H] = vm->r[H]; break;
	  case 0x65: vm->r[H] = vm->r[L]; break;
	  case 0x66: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[H] = vm->memory[address];
  				}
				break;
	  case 0x67: vm->r[H] = vm->r[A]; break;
	  case 0x68: vm->r[L] = vm->r[B]; break;
	  case 0x69: vm->r[L] = vm->r[C]; break;
	  case 0x6A: vm->r[L] = vm->r[D]; break;
	  case 0x6B: vm->r[L] = vm->r[E]; break;
	  case 0x6C: vm->r[L] = vm->r[H]; break;
	  case 0x6D: vm->r[L] = vm->r[L]; break;
	  case 0x6E: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[L] = vm->memory[address];
  				}
				break;
	  case 0x6F: vm->r[L] = vm->r[A]; break;
	  case 0x70: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = vm->r[B];
  				}
				break;
	  case 0x71: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = vm->r[C];
				}
				break;
	  case 0x72: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = vm->r[D];
				}
				break;
	  case 0x73: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = vm->r[E];
				}
				break;
	  case 0x74: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = vm->r[H];
				}
				break;
	  case 0x75: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->memory[address] = vm->r[L];
				}
				break;
	  case 0x76: todo_implement(); break;
	  case 0x77: 
				{
					uint16_t address = (vm->r[H] << 8) | vm->r[L];
	 				vm->memory[address] = vm->r[A];
 				}
				break;
	  case 0x78: vm->r[A] = vm->r[B]; break;
	  case 0x79: vm->r[A] = vm->r[C]; break;
	  case 0x7A: vm->r[A] = vm->r[D]; break;
	  case 0x7B: vm->r[A] = vm->r[E]; break;
	  case 0x7C: vm->r[A] = vm->r[H]; break;
	  case 0x7D: vm->r[A] = vm->r[L]; break;
	  case 0x7E: 
	  			{
	  				uint16_t address = (vm->r[H] << 8) | vm->r[L];
					vm->r[A] = vm->memory[address];
	  		  	}
	  			break;
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
	  case 0xCB: cb(); break;
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
	  case 0xFA: 
				{
					uint16_t address = (fetch() << 8) | fetch();
					vm->r[A] = vm->memory[address]
				}
				break;
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