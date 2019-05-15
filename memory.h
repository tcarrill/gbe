#ifndef GBE_H
#define GBE_H

#include "gbe.h"

uint8_t read_byte(uint16_t address);
uint16_t read_word(uint16_t address);

void write_byte(unint16_t address, uint8_t value);
void write_word(uint16_t address, uint16_t value);
	
#endif