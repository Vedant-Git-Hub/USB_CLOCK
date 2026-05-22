/*
 * max7219.h
 *
 *  Created on: Dec 26, 2024
 *      Author: Vedant Bhale
 */

#ifndef INC_MAX7219_H_
#define INC_MAX7219_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SHUTDOWN_ON			0x00
#define SHUTDOWN_OFF		0x01
#define DECODE_OFF			0x00
#define MIN_INTENSITY		0x00
#define MED_INTENSITY		0x07
#define MAX_INTENSITY		0x0F
#define SCAN_FULL			0x07
#define DISP_TEST_ON		0x01
#define DISP_TEST_OFF		0x00

typedef struct{
	uint8_t pixel_x;
	uint8_t pixel_y;
	uint8_t total_matrix;
	uint8_t total_matrix_x;
	uint8_t total_matrix_y;
	uint8_t *display_buffer;
}MAX7219_CONFIG;

typedef enum{
	OFF = 0,
	ON,
	TOGGLE
}PIXEL_STATE;

typedef enum{
	LTR = 0,
	RTL
}SHIFT_DIR;


MAX7219_CONFIG* max7219_init(uint8_t, uint8_t);
void max7219_updateDisplay(MAX7219_CONFIG *);
void max7219_clearDisplay(MAX7219_CONFIG *);
void max7219_setDisplay(MAX7219_CONFIG *);
void max7219_drawPixel(MAX7219_CONFIG *, uint8_t , uint8_t , PIXEL_STATE );
void max7219_scrollText(MAX7219_CONFIG *, const char *, uint8_t , SHIFT_DIR , uint8_t );
void max7219_staticText(MAX7219_CONFIG *, const char *, uint8_t );
void max7219_shutdown(MAX7219_CONFIG *, uint8_t );
void max7219_intensity(MAX7219_CONFIG *, uint8_t );
void max7219_decodeMode(MAX7219_CONFIG *, uint8_t );
void max7219_scanLimit(MAX7219_CONFIG *, uint8_t );
void max7219_displayTest(MAX7219_CONFIG *, uint8_t );


#endif /* INC_MAX7219_H_ */
