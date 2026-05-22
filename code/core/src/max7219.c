/*
 * max7219.c
 *
 *  Created on: Dec 26, 2024
 *      Author: Vedant Bhale
 */


#include "max7219.h"
#include "Fonts.h"
#include "spi.h"
#include <util/delay.h>
#include <avr/pgmspace.h>


#define CS_LOW()			spi_setCS();
#define CS_HIGH()			spi_resetCS();

#define NO_OPERATION			0x00
#define COLUMN_0			0x01
#define COLUMN_1			0x02
#define COLUMN_2			0x03
#define COLUMN_3			0x04
#define COLUMN_4			0x05
#define COLUMN_5			0x06
#define COLUMN_6			0x07
#define COLUMN_7			0x08
#define DECODE_MODE			0x09
#define INTENSITY			0x0A
#define SCAN_LIMIT			0x0B
#define SHUTDOWN			0x0C
#define DISP_TEST			0x0F



static void max7219_writeReg(MAX7219_CONFIG *, uint8_t, uint8_t, uint8_t);
static bool max7219_copyAlphabet(MAX7219_CONFIG *, const char *, uint16_t, SHIFT_DIR);
static void max7219_shiftDisplay(MAX7219_CONFIG *, SHIFT_DIR);
static void delay(uint32_t);


MAX7219_CONFIG* max7219_init(uint8_t height, uint8_t width)
{
	MAX7219_CONFIG *config;

	spi_init();

	config = (MAX7219_CONFIG *) malloc(sizeof(MAX7219_CONFIG));

	if(config)
	{
		config->pixel_x = width;
		config->pixel_y = height;
		config->total_matrix_x = width / 8;
		config->total_matrix_y = height / 8;
		config->total_matrix = config->total_matrix_x * config->total_matrix_y;

		config->display_buffer = (uint8_t *) malloc(config->total_matrix * 8);

		if(config->display_buffer)
		{
			memset(config->display_buffer, 0, config->total_matrix * 8);
			max7219_shutdown(config, SHUTDOWN_OFF);
			max7219_decodeMode(config, DECODE_OFF);
			max7219_scanLimit(config, SCAN_FULL);
			max7219_displayTest(config, DISP_TEST_ON);
			delay(100);
			max7219_displayTest(config, DISP_TEST_OFF);
			max7219_intensity(config, MIN_INTENSITY);
			max7219_clearDisplay(config);
			max7219_updateDisplay(config);
			return config;
		}
	}

	return NULL;
}

void max7219_updateDisplay(MAX7219_CONFIG *config)
{
	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
		for(uint8_t col_num = 0; col_num < 8; col_num++)
		{
			max7219_writeReg(config, disp_num, col_num + 1, config->display_buffer[col_num + (8 * disp_num)]);
		}
	}
}

void max7219_clearDisplay(MAX7219_CONFIG *config)
{
	memset(config->display_buffer, 0, config->total_matrix * 8);
}

void max7219_setDisplay(MAX7219_CONFIG *config)
{
	memset(config->display_buffer, 0xFF, config->total_matrix * 8);
}

void max7219_drawPixel(MAX7219_CONFIG *config, uint8_t x, uint8_t y, PIXEL_STATE state)
{
	uint8_t disp_num = x / 8;

	if(state == ON)
	{
		config->display_buffer[(disp_num * 8) + y] |= (0x01 << (7 - (x & 0x07)));
	}

	else if(state == OFF)
	{
		config->display_buffer[(disp_num * 8) + y] &= ~(0x01 << (7 - (x & 0x07)));
	}

	else
	{
		config->display_buffer[(disp_num * 8) + y] ^= (0x01 << (7 - (x & 0x07)));
	}
}


void max7219_scrollText(MAX7219_CONFIG *config, const char *str, uint8_t scroll_speed, SHIFT_DIR dir, uint8_t freq)
{
	uint16_t str_len = strlen(str);
	bool round_complete = false;

	max7219_clearDisplay(config);

	do
	{
		round_complete = max7219_copyAlphabet(config, str, str_len, dir);

		if(round_complete)
		{
			round_complete = false;
			freq--;
		}

		max7219_shiftDisplay(config, dir);
		max7219_updateDisplay(config);
		delay(50 * scroll_speed);

	}while(freq);

	max7219_clearDisplay(config);
	max7219_updateDisplay(config);
}

void max7219_staticText(MAX7219_CONFIG *config, const char *str, uint8_t col_num)
{
	uint16_t str_len = strlen(str);
	uint16_t str_ind = 0;
	char *alph;
	char alph_data[8];

	max7219_clearDisplay(config);

	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
	        alph = (char *) pgm_read_word(&Alphabets[(uint8_t)str[str_ind]]);
	        memcpy_P(alph_data, alph, 8);


		for(uint8_t row_num = 0; row_num < 8; row_num++)
		{
			config->display_buffer[disp_num * 8 + row_num] |= alph_data[row_num] << 0x01;
		}

		str_ind++;
		if(str_ind >= str_len)
		{
			break;
		}
	}

	for(uint8_t shift_val = 0; shift_val < col_num; shift_val++)
	{
		max7219_shiftDisplay(config, LTR);
	}

	max7219_updateDisplay(config);
}

void max7219_shutdown(MAX7219_CONFIG *config, uint8_t state)
{
	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
		max7219_writeReg(config, disp_num, SHUTDOWN, state);
	}
}

void max7219_intensity(MAX7219_CONFIG *config, uint8_t intensity)
{
	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
		max7219_writeReg(config, disp_num, INTENSITY, intensity);
	}
}

void max7219_decodeMode(MAX7219_CONFIG *config, uint8_t mode)
{
	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
		max7219_writeReg(config, disp_num, DECODE_MODE, mode);
	}
}

void max7219_scanLimit(MAX7219_CONFIG *config, uint8_t limit)
{
	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
		max7219_writeReg(config, disp_num, SCAN_LIMIT, limit);
	}
}

void max7219_displayTest(MAX7219_CONFIG *config, uint8_t state)
{
	for(uint8_t disp_num = 0; disp_num < config->total_matrix; disp_num++)
	{
		max7219_writeReg(config, disp_num, DISP_TEST, state);
	}
}

static void max7219_writeReg(MAX7219_CONFIG *config, uint8_t display_num, uint8_t address, uint8_t reg_data)
{
	uint8_t buffer_size = config->total_matrix * 2;
	uint8_t data[buffer_size];

	for(uint8_t curr_disp = 0, data_ind = 0; curr_disp < config->total_matrix; curr_disp++)
	{
		if(curr_disp == display_num)
		{
			data[data_ind++] = address;
			data[data_ind++] = reg_data;
		}
		else
		{
			data[data_ind++] = NO_OPERATION;
			data[data_ind++] = 0x00;
		}
	}

	CS_LOW();
	spi_transmitBytes(data, buffer_size);
	CS_HIGH();
}

static void max7219_shiftDisplay(MAX7219_CONFIG *config, SHIFT_DIR dir)
{
	int8_t disp_num = 0;
	uint8_t row_num = 0;

	if(dir == LTR)
	{
		for(disp_num = config->total_matrix - 1; disp_num >= 0; disp_num--)
		{
			for(row_num = 0; row_num < 8; row_num++)
			{
				config->display_buffer[disp_num * 8 + row_num] >>= 1;

				if(disp_num != 0)
				{
					config->display_buffer[disp_num * 8 + row_num] |= ((config->display_buffer[(disp_num - 1) * 8 + row_num] & 0x01) << 7);
				}
			}
		}
	}
	else
	{
		for(disp_num = 0; disp_num < config->total_matrix; disp_num++)
		{
			for(row_num = 0; row_num < 8; row_num++)
			{
				config->display_buffer[disp_num * 8 + row_num] <<= 1;

				if(disp_num != (config->total_matrix - 1))
				{
					config->display_buffer[disp_num * 8 + row_num] |= (config->display_buffer[((disp_num + 1) * 8) + row_num] >> 0x07) & 0x01;
				}
			}
		}
	}
}

static bool max7219_copyAlphabet(MAX7219_CONFIG *config, const char *str, uint16_t str_len, SHIFT_DIR dir)
{
	static uint8_t curr_bit_pos = 0;
	static uint16_t curr_ind = 0;
	uint8_t disp_num;
	char *alph;
	char alph_data[8];
	static uint8_t space_count = 0;


	alph = (char *) pgm_read_word(&Alphabets[(uint8_t)str[curr_ind]]);
	memcpy_P(alph_data, alph, 8);

	if(curr_ind <= str_len - 1)
	{

		for(uint8_t row_num = 0; row_num < 8; row_num++)
		{
			if(dir == RTL)
			{
				disp_num = config->total_matrix - 1;
				config->display_buffer[(disp_num * 8) + row_num] |= (alph_data[row_num] >> (6 - curr_bit_pos)) & 0x01;
			}
			else
			{
				disp_num = 0;
				config->display_buffer[(disp_num * 8) + row_num] |= ((alph_data[row_num] >> (curr_bit_pos)) & 0x01) << 0x07;
			}
		}

		curr_bit_pos++;
		if(curr_bit_pos > 6)
		{
			curr_bit_pos = 0;
			curr_ind++;
		}

	}
	else
	{
		space_count++;
		if(space_count > config->total_matrix * 8)
		{
			space_count = 0;
			curr_ind = 0;
			return true;
		}
	}

	return false;
}

static void delay(uint32_t n)
{
	while(n--)
	{
		_delay_ms(1);
	}
}

