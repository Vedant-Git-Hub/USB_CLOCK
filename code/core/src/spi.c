#include <stdint.h> 
#include <avr/io.h>
#include "spi.h"



void spi_init()
{
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);// | (1 << CPOL) | (1 << CPHA);
	DDRB |= (1 << PB2);
        DDRB |= (1 << MOSI_DIR);
	DDRB |= (1 << SCK_DIR); 
	DDRB |= (1 << CS);
        DDRB &= ~(1 << MISO_DIR);
}

void spi_writeByte(uint8_t data)
{
        SPDR = data;

        while(!(SPSR & (1 << SPIF)));
}

void spi_setCS()
{
       PORTB |= (1 << CS);
}

void spi_resetCS()
{
       PORTB &= ~(1 << CS);
}

void spi_transmitBytes(uint8_t *data, uint8_t buffer_size)
{
	for(uint8_t byte_num = 0; byte_num < buffer_size; byte_num++)
	{
		spi_writeByte(data[byte_num]);
	}
}
