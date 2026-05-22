#ifndef H_SPI_H
#define H_SPI_H

#include <stdint.h>

#define MOSI_DIR        PB3
#define MISO_DIR        PB4
#define SCK_DIR         PB5
#define CS              PB2
 
 
void spi_init(void);
void spi_writeByte(uint8_t );
void spi_setCS(void);
void spi_resetCS(void);
void spi_transmitBytes(uint8_t *, uint8_t );

#endif
