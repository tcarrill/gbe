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

void print_registers(Vm *vm) {
	printf("\nRegisters\n");
	printf("A    BC    DE    HL    PC    SP\n");
	printf("%02x  %02x%02x  %02x%02x  %02x%02x  %04x  %04x\n", vm->cpu->a, vm->cpu->b, vm->cpu->c, vm->cpu->d, vm->cpu->e, vm->cpu->h, vm->cpu->l, vm->cpu->pc, vm->cpu->sp);
	printf("   ZNHC\n");
	printf("F: %sb\n", byte_to_binary(vm->cpu->f));
}

void todo_implement(Vm *vm) {
  vm->cpu->pc -= 1;
  printf("0x%02x is an unimplemented instruction!\n", vm->memory[vm->cpu->pc]);
  print_registers(vm);
  free(vm->cpu);
  free(vm);
  exit(1);
}

uint8_t fetch(Vm *vm) {
	uint8_t opcode = vm->memory[vm->cpu->pc];
	vm->cpu->pc++;
	return opcode;
}

void rl_carry(uint8_t f, uint8_t r) {
  	uint8_t out = BIT_CHECK(r, 7);
	uint8_t in = BIT_CHECK(f, C);
	out == 1 ? BIT_SET(f, C) : BIT_CLEAR(f, C);
	r <<= 1;
	if (in == 1) {
		BIT_SET(r, 0);
	} 
	r == 0 ? BIT_SET(f, Z) : BIT_CLEAR(f, Z);
	BIT_CLEAR(f, N);
	BIT_CLEAR(f, H);
}

void cb(Vm *vm) {
	Cpu *cpu = vm->cpu;
	uint8_t opcode = fetch(vm);
	switch (opcode) {
		case 0x7C: 
					BIT_CHECK(cpu->h, 7) == 0 ? BIT_SET(cpu->f, Z) : BIT_CLEAR(cpu->f, Z);
					BIT_CLEAR(cpu->f, N);
					BIT_SET(cpu->f, H); 
					break;
		case 0x11: 
					rl_carry(cpu->f, cpu->c);
					break;
	}
}

void dec(uint8_t f, uint8_t r) {
	r--;
	r == 0 ? BIT_SET(f, Z) : BIT_CLEAR(f, Z);
	MASK8(r) == 0xF ? BIT_SET(f, H) : BIT_CLEAR(f, H);
	BIT_SET(f, N);
}

int emulate(Vm *vm) {
	Cpu *cpu = vm->cpu;
	uint8_t opcode = fetch(vm);
  	switch (opcode) {
	  case 0x00: break;
	  case 0x01: todo_implement(vm); break;
	  case 0x02: todo_implement(vm); break;
	  case 0x03: todo_implement(vm); break;
	  case 0x04: todo_implement(vm); break;
	  case 0x05:
	  			cpu->b--;
  	  			cpu->b == 0 ? BIT_SET(cpu->f, Z) : BIT_CLEAR(cpu->f, Z);
  				MASK8(cpu->b) == 0xF ? BIT_SET(cpu->f, H) : BIT_CLEAR(cpu->f, H);
  				BIT_SET(cpu->f, N);
	  	 		break;
	  case 0x06:
	  			cpu->b = fetch(vm);
	  			break;
	  case 0x07: todo_implement(vm); break;
	  case 0x08: todo_implement(vm); break;
	  case 0x09: todo_implement(vm); break;
	  case 0x0A: todo_implement(vm); break;
	  case 0x0B: todo_implement(vm); break;
	  case 0x0C: 
	  			cpu->c++;
				cpu->c == 0 ? BIT_SET(cpu->f, Z) : BIT_CLEAR(cpu->f, Z);
				MASK8(cpu->c) == 0 ? BIT_SET(cpu->f, H) : BIT_CLEAR(cpu->f, H);
				BIT_CLEAR(cpu->f, N);			
				break;
	  case 0x0D: todo_implement(vm); break;
	  case 0x0E:
	            cpu->c = fetch(vm);
	  			break;
	  case 0x0F: todo_implement(vm); break;
	  case 0x10: todo_implement(vm); break;
	  case 0x11: 
	            cpu->e = fetch(vm);
				cpu->d = fetch(vm);
				break;
	  case 0x12: todo_implement(vm); break;
	  case 0x13: 
				{
					uint16_t de = (cpu->d << 8) | cpu->e;
	 				de++;
	 				cpu->d = de >> 8;
	 				cpu->e = de & 0xFF;
	 				break;
 				}
	  case 0x14: todo_implement(vm); break;
	  case 0x15: todo_implement(vm); break;
	  case 0x16: todo_implement(vm); break;
	  case 0x17:
	  			{
	  		  	rl_carry(cpu->f, cpu->a);
	  	 		break;
				}
	  case 0x18: todo_implement(vm); break;
	  case 0x19: todo_implement(vm); break;
	  case 0x1A: 
	  			{
		  			uint16_t address = (cpu->d << 8) | cpu->e;
		  			cpu->a = vm->memory[address];
					break;
	  	  		}
	  case 0x1B: todo_implement(vm); break;
	  case 0x1C: todo_implement(vm); break;
	  case 0x1D: todo_implement(vm); break;
	  case 0x1E: todo_implement(vm); break;
	  case 0x1F: todo_implement(vm); break;
	  case 0x20: 
	  			{
	  	 			if (BIT_CHECK(cpu->f, Z) == 0) {
						int8_t offset = (int8_t)fetch(vm); //convert to signed 2s complement
						cpu->pc += offset;
	  	 			} else {
	  	 				cpu->pc++;
	  	 			}
	  				break;
				}
	  case 0x21: 
	  			cpu->l = fetch(vm);
	  		  	cpu->h = fetch(vm);
				break;
	  case 0x22: 
				{
					uint16_t address = (cpu->h << 8) | cpu->l;
	 				vm->memory[address] = cpu->a;
	 			   	address++;
	 			  	cpu->h = address >> 8;
	 			 	cpu->l = address & 0xFF;
 				}
				break;
	  case 0x23:
	  			{
					uint16_t hl = (cpu->h << 8) | cpu->l;
   					hl++;
  					cpu->h = hl >> 8;
 					cpu->l = hl & 0xFF;
				}
	  			break;
	  case 0x24: todo_implement(vm); break;
	  case 0x25: todo_implement(vm); break;
	  case 0x26: todo_implement(vm); break;
	  case 0x27: todo_implement(vm); break;
	  case 0x28: todo_implement(vm); break;
	  case 0x29: todo_implement(vm); break;
	  case 0x2A: todo_implement(vm); break;
	  case 0x2B: todo_implement(vm); break;
	  case 0x2C: todo_implement(vm); break;
	  case 0x2D: todo_implement(vm); break;
	  case 0x2E: todo_implement(vm); break;
	  case 0x2F: todo_implement(vm); break;
	  case 0x30: todo_implement(vm); break;
	  case 0x31: 
	  			{
	  				uint8_t lb = fetch(vm);
	  		  		uint8_t hb = fetch(vm);  
	  				cpu->sp = (hb << 8) | lb;
					break;
				}
	  case 0x32: 
	  			{
	  				uint16_t address = (cpu->h << 8) | cpu->l;
				 	vm->memory[address] = cpu->a;
				 	address--;
				 	cpu->h = address >> 8;
				 	cpu->l = address & 0xFF;
	  			 	break;
			 	}
	  case 0x33: todo_implement(vm); break;
	  case 0x34: todo_implement(vm); break;
	  case 0x35: todo_implement(vm); break;
	  case 0x36: todo_implement(vm); break;
	  case 0x37: todo_implement(vm); break;
	  case 0x38: todo_implement(vm); break;
	  case 0x39: todo_implement(vm); break;
	  case 0x3A: todo_implement(vm); break;
	  case 0x3B: todo_implement(vm); break;
	  case 0x3C: todo_implement(vm); break;
	  case 0x3D:
				cpu->a--;
				cpu->a == 0 ? BIT_SET(cpu->f, Z) : BIT_CLEAR(cpu->f, Z);
				MASK8(cpu->a) == 0xF ? BIT_SET(cpu->f, H) : BIT_CLEAR(cpu->f, H);
				BIT_SET(cpu->f, N);
				break;
	  case 0x3E: 
	  			cpu->a = fetch(vm);
				break;
	  case 0x3F: todo_implement(vm); break;
	  case 0x40: cpu->b = cpu->b; break;
	  case 0x41: cpu->b = cpu->c; break;
	  case 0x42: cpu->b = cpu->d; break;
	  case 0x43: cpu->b = cpu->e; break;
	  case 0x44: cpu->b = cpu->h; break;
	  case 0x45: cpu->b = cpu->l; break;
	  case 0x46: todo_implement(vm); break;
	  case 0x47: todo_implement(vm); break;
	  case 0x48: cpu->c = cpu->b; break;
	  case 0x49: cpu->c = cpu->c; break;
	  case 0x4A: cpu->c = cpu->d; break;
	  case 0x4B: cpu->c = cpu->e; break;
	  case 0x4C: cpu->c = cpu->h; break;
	  case 0x4D: cpu->c = cpu->l; break;
	  case 0x4E: todo_implement(vm); break;
	  case 0x4F: cpu->c = cpu->a; break;
	  case 0x50: cpu->d = cpu->b; break;
	  case 0x51: cpu->d = cpu->c; break;
	  case 0x52: cpu->d = cpu->d; break;
	  case 0x53: cpu->d = cpu->e; break;
	  case 0x54: cpu->d = cpu->h; break;
	  case 0x55: cpu->d = cpu->l; break;
	  case 0x56: todo_implement(vm); break;
	  case 0x57: todo_implement(vm); break;
	  case 0x58: cpu->e = cpu->b; break;
	  case 0x59: cpu->e = cpu->c; break;
	  case 0x5A: cpu->e = cpu->d; break;
	  case 0x5B: cpu->e = cpu->e; break;
	  case 0x5C: cpu->e = cpu->h; break;
	  case 0x5D: cpu->e = cpu->l; break;
	  case 0x5E: todo_implement(vm); break;
	  case 0x5F: todo_implement(vm); break;
	  case 0x60: cpu->h = cpu->b; break;
	  case 0x61: cpu->h = cpu->c; break;
	  case 0x62: cpu->h = cpu->d; break;
	  case 0x63: cpu->h = cpu->e; break;
	  case 0x64: cpu->h = cpu->h; break;
	  case 0x65: cpu->h = cpu->l; break;
	  case 0x66: todo_implement(vm); break;
	  case 0x67: todo_implement(vm); break;
	  case 0x68: cpu->l = cpu->b; break;
	  case 0x69: cpu->l = cpu->c; break;
	  case 0x6A: cpu->l = cpu->d; break;
	  case 0x6B: cpu->l = cpu->e; break;
	  case 0x6C: cpu->l = cpu->h; break;
	  case 0x6D: cpu->l = cpu->l; break;
	  case 0x6E: todo_implement(vm); break;
	  case 0x6F: todo_implement(vm); break;
	  case 0x70: todo_implement(vm); break;
	  case 0x71: todo_implement(vm); break;
	  case 0x72: todo_implement(vm); break;
	  case 0x73: todo_implement(vm); break;
	  case 0x74: todo_implement(vm); break;
	  case 0x75: todo_implement(vm); break;
	  case 0x76: todo_implement(vm); break;
	  case 0x77: 
				{
					uint16_t address = (cpu->h << 8) | cpu->l;
	 				vm->memory[address] = cpu->a;
	 				break;
 				}
	  case 0x78: cpu->a = cpu->b; break;
	  case 0x79: cpu->a = cpu->c; break;
	  case 0x7A: cpu->a = cpu->d; break;
	  case 0x7B: cpu->a = cpu->e; break;
	  case 0x7C: cpu->a = cpu->h; break;
	  case 0x7D: cpu->a = cpu->l; break;
	  case 0x7E: todo_implement(vm); break;
	  case 0x7F: cpu->a = cpu->a; break;
	  case 0x80: todo_implement(vm); break;
	  case 0x81: todo_implement(vm); break;
	  case 0x82: todo_implement(vm); break;
	  case 0x83: todo_implement(vm); break;
	  case 0x84: todo_implement(vm); break;
	  case 0x85: todo_implement(vm); break;
	  case 0x86: todo_implement(vm); break;
	  case 0x87: todo_implement(vm); break;
	  case 0x88: todo_implement(vm); break;
	  case 0x89: todo_implement(vm); break;
	  case 0x8A: todo_implement(vm); break;
	  case 0x8B: todo_implement(vm); break;
	  case 0x8C: todo_implement(vm); break;
	  case 0x8D: todo_implement(vm); break;
	  case 0x8E: todo_implement(vm); break;
	  case 0x8F: todo_implement(vm); break;
	  case 0x90: todo_implement(vm); break;
	  case 0x91: todo_implement(vm); break;
	  case 0x92: todo_implement(vm); break;
	  case 0x93: todo_implement(vm); break;
	  case 0x94: todo_implement(vm); break;
	  case 0x95: todo_implement(vm); break;
	  case 0x96: todo_implement(vm); break;
	  case 0x97: todo_implement(vm); break;
	  case 0x98: todo_implement(vm); break;
	  case 0x99: todo_implement(vm); break;
	  case 0x9A: todo_implement(vm); break;
	  case 0x9B: todo_implement(vm); break;
	  case 0x9C: todo_implement(vm); break;
	  case 0x9D: todo_implement(vm); break;
	  case 0x9E: todo_implement(vm); break;
	  case 0x9F: todo_implement(vm); break;
	  case 0xA0: todo_implement(vm); break;
	  case 0xA1: todo_implement(vm); break;
	  case 0xA2: todo_implement(vm); break;
	  case 0xA3: todo_implement(vm); break;
	  case 0xA4: todo_implement(vm); break;
	  case 0xA5: todo_implement(vm); break;
	  case 0xA6: todo_implement(vm); break;
	  case 0xA7: todo_implement(vm); break;
	  case 0xA8: todo_implement(vm); break;
	  case 0xA9: todo_implement(vm); break;
	  case 0xAA: todo_implement(vm); break;
	  case 0xAB: todo_implement(vm); break;
	  case 0xAC: todo_implement(vm); break;
	  case 0xAD: todo_implement(vm); break;
	  case 0xAE: todo_implement(vm); break;
	  case 0xAF: 
	  			cpu->a ^= cpu->a;
				cpu->a == 0 ? BIT_SET(cpu->f, Z) : BIT_CLEAR(cpu->f, Z);
				BIT_CLEAR(cpu->f, N);
				BIT_CLEAR(cpu->f, H);
				BIT_CLEAR(cpu->f, C);
				break;
	  case 0xB0: todo_implement(vm); break;
	  case 0xB1: todo_implement(vm); break;
	  case 0xB2: todo_implement(vm); break;
	  case 0xB3: todo_implement(vm); break;
	  case 0xB4: todo_implement(vm); break;
	  case 0xB5: todo_implement(vm); break;
	  case 0xB6: todo_implement(vm); break;
	  case 0xB7: todo_implement(vm); break;
	  case 0xB8: todo_implement(vm); break;
	  case 0xB9: todo_implement(vm); break;
	  case 0xBA: todo_implement(vm); break;
	  case 0xBB: todo_implement(vm); break;
	  case 0xBC: todo_implement(vm); break;
	  case 0xBD: todo_implement(vm); break;
	  case 0xBE: todo_implement(vm); break;
	  case 0xBF: todo_implement(vm); break;
	  case 0xC0: todo_implement(vm); break;
	  case 0xC1:
	  			cpu->c = vm->memory[cpu->sp];
	  		  	cpu->sp++;
	  		  	cpu->b = vm->memory[cpu->sp];
	  		  	cpu->sp++;
	  			break;
	  case 0xC2: todo_implement(vm); break;
	  case 0xC3: 
	  			{
	  				uint8_t lb = fetch(vm);
	  		  		uint8_t hb = fetch(vm);  
	  				cpu->pc = (hb << 8) | lb;
					break;
				}
	  case 0xC4: todo_implement(vm); break;
	  case 0xC5:
	  			cpu->sp--;
	  		  	vm->memory[cpu->sp] = cpu->b;				
	  		  	cpu->sp--;
	  		  	vm->memory[cpu->sp] = cpu->c;
	  			break;
	  case 0xC6: todo_implement(vm); break;
	  case 0xC7: todo_implement(vm); break;
	  case 0xC8: todo_implement(vm); break;
	  case 0xC9:
	  			cpu->pc = vm->memory[cpu->sp] | (vm->memory[cpu->sp + 1] << 8);
				cpu->sp += 2;
	  			break;
	  case 0xCA: todo_implement(vm); break;
	  case 0xCB: 
	  			cb(vm);
				break;
	  case 0xCC: todo_implement(vm); break;
	  case 0xCD: 
	  			{
	  				uint16_t ret = cpu->pc + 2;
	  				vm->memory[cpu->sp - 1] = (ret >> 8) & 0xff;
	  				vm->memory[cpu->sp - 2] = ret & 0xff;
	  				cpu->sp = cpu->sp - 2;
	  				uint8_t lb = fetch(vm);
	  		  		uint8_t hb = fetch(vm); 
	  				cpu->pc = (hb << 8) | lb;		
	  			}
	   			break;
	  case 0xCE: todo_implement(vm); break;
	  case 0xCF: todo_implement(vm); break;
	  case 0xD0: todo_implement(vm); break;
	  case 0xD1: todo_implement(vm); break;
	  case 0xD2: todo_implement(vm); break;
	  case 0xD3: todo_implement(vm); break;
	  case 0xD4: todo_implement(vm); break;
	  case 0xD5: todo_implement(vm); break;
	  case 0xD6: todo_implement(vm); break;
	  case 0xD7: todo_implement(vm); break;
	  case 0xD8: todo_implement(vm); break;
	  case 0xD9: todo_implement(vm); break;
	  case 0xDA: todo_implement(vm); break;
	  case 0xDB: todo_implement(vm); break;
	  case 0xDC: todo_implement(vm); break;
	  case 0xDD: todo_implement(vm); break;
	  case 0xDE: todo_implement(vm); break;
	  case 0xDF: todo_implement(vm); break;
	  case 0xE0: 
				{
					int8_t offset = (int8_t)fetch(vm);
					uint16_t address = 0xFF00 + offset;
					vm->memory[address] = cpu->a;
					break;	
    			}
	  case 0xE1: todo_implement(vm); break;
	  case 0xE2:
	  			{
					uint16_t address = 0xFF00 + cpu->c;
		  			vm->memory[address] = cpu->a;
					break;	
	  		    }
	  case 0xE3: todo_implement(vm); break;
	  case 0xE4: todo_implement(vm); break;
	  case 0xE5: todo_implement(vm); break;
	  case 0xE6: todo_implement(vm); break;
	  case 0xE7: todo_implement(vm); break;
	  case 0xE8: todo_implement(vm); break;
	  case 0xE9: todo_implement(vm); break;
	  case 0xEA:
	  			{
					uint8_t address = (fetch(vm) << 8) | fetch(vm);
	  		  		vm->memory[address] = cpu->a;
	  				break;
				}
	  case 0xEB: todo_implement(vm); break;
	  case 0xEC: todo_implement(vm); break;
	  case 0xED: todo_implement(vm); break;
	  case 0xEE: todo_implement(vm); break;
	  case 0xEF: todo_implement(vm); break;
	  case 0xF0: todo_implement(vm); break;
	  case 0xF1: todo_implement(vm); break;
	  case 0xF2: todo_implement(vm); break;
	  case 0xF3: todo_implement(vm); break;
	  case 0xF4: todo_implement(vm); break;
	  case 0xF5: todo_implement(vm); break;
	  case 0xF6: todo_implement(vm); break;
	  case 0xF7: todo_implement(vm); break;
	  case 0xF8: todo_implement(vm); break;
	  case 0xF9: todo_implement(vm); break;
	  case 0xFA: todo_implement(vm); break;
	  case 0xFB: todo_implement(vm); break;
	  case 0xFC: todo_implement(vm); break;
	  case 0xFD: todo_implement(vm); break;
	  case 0xFE:  
	  {
		  uint8_t value = fetch(vm);
		  uint8_t res = cpu->a - value;
		  res == 0 ? BIT_SET(cpu->f, Z) : BIT_CLEAR(cpu->f, Z);
		  cpu->a < res ? BIT_SET(cpu->f, C) : BIT_CLEAR(cpu->f, C);
		  (MASK8(cpu->a) - MASK8(value)) < 0 ? BIT_SET(cpu->f, H) : BIT_CLEAR(cpu->f, H);
		  BIT_SET(cpu->f, N);
	  }
	  break;
	  case 0xFF: todo_implement(vm); break;   
  	}

  	return 1;
}

Vm* init_vm() {
	Cpu *cpu = (Cpu *)malloc(sizeof(Cpu));
	if (cpu == NULL) {
		return NULL;
	}
    cpu->a = 0x01;
    cpu->f = 0xB0;
    cpu->b = 0x00;
    cpu->c = 0x13;
    cpu->d = 0x00;
    cpu->e = 0xD8;
    cpu->h = 0x01;
    cpu->l = 0x4D;
    cpu->pc = 0x0000;
    cpu->sp = 0xFFFE;
	
    Vm *vm = (Vm *)malloc(sizeof(Vm));
    if (vm == NULL) {
      return NULL;
    }
	vm->cpu = cpu;
  vm->memory = calloc(1, 0xFFFF);

  return vm;
}

long readBinary(unsigned char **buffer, char *const filename) {
	FILE *fileptr = fopen(filename, "rb");

	fseek(fileptr, 0, SEEK_END);
	long filelen = ftell(fileptr);
	rewind(fileptr);
	*buffer = (unsigned char*)malloc((filelen + 1) * sizeof(char)); 
	fread(*buffer, filelen, 1, fileptr); 
	fclose(fileptr); 
	
	return filelen; 
}

int main(int argc, char *argv[]) {  
    Vm* vm = init_vm();
    memcpy(vm->memory, BOOT_ROM, 256);
	
	unsigned char *cart = NULL;
  	long length = readBinary(&cart, argv[1]);
	for (int i = 0x0100; i <= 0x7FFF; i++) {
		vm->memory[i] = cart[i];
	}	
  	free(cart);

  	int running = 1;
  	while (running) {
    	running = emulate(vm);  
 	}

  	free(vm->cpu);
  	free(vm);
}
