/*
 * ds3231.c
 *
 *  Created on: Dec 28, 2024
 *      Author: Vedant Bhale
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "i2c.h"
#include "ds3231.h"


#define DS3231_ADDR				0xD0

#define SEC_REG_ADDR			0x00
#define MIN_REG_ADDR			0x01
#define HR_REG_ADDR				0x02
#define DAY_REG_ADDR			0x03
#define DAT_REG_ADDR			0x04
#define MON_REG_ADDR			0x05
#define YR_REG_ADDR				0x06
#define CTRL_REG_ADDR			0x0E
#define CTRL_STAT_REG_ADDR		0x0F
#define TEMP_MSB_ADDR			0x11
#define TEMP_LSB_ADDR			0x12


static uint8_t ds3231_bcdToHex(uint8_t );
static uint8_t ds3231_hexToBcd(uint8_t );


const char *day_of_week_table[] = {
		"SUN",
		"MON",
		"TUE",
		"WED",
		"THU",
		"FRI",
		"SAT"
};


bool ds3231_init()
{
	uint8_t set_reg_add;

	set_reg_add =  SEC_REG_ADDR;

	tw_init(TW_FREQ_400K, true);

	tw_master_transmit(DS3231_ADDR, &set_reg_add, sizeof(set_reg_add), false);

	return 1;
}

bool ds3231_readTimeStamp(RTC_TIME *rtc)
{
	uint8_t data[7];
	uint8_t set_reg_add;

	set_reg_add =  SEC_REG_ADDR;

	tw_master_transmit(DS3231_ADDR, &set_reg_add, sizeof(set_reg_add), true);

	tw_master_receive(DS3231_ADDR, data, sizeof(data));

	rtc->sec = ds3231_bcdToHex(data[0]);
	rtc->min = ds3231_bcdToHex(data[1]);
	rtc->hr = ds3231_bcdToHex(data[2]);
	rtc->day_of_week = ds3231_bcdToHex(data[3]);
	rtc->date = ds3231_bcdToHex(data[4]);
	rtc->month = ds3231_bcdToHex(data[5]);
	rtc->year = ds3231_bcdToHex(data[6]);

	return 1;
}

bool ds3231_writeTimeStamp(RTC_TIME *rtc)
{
	uint8_t data[8];

	data[0] = SEC_REG_ADDR;
	data[1] = ds3231_hexToBcd(rtc->sec);
	data[2] = ds3231_hexToBcd(rtc->min);
	data[3] = ds3231_hexToBcd(rtc->hr);
	data[4] = ds3231_hexToBcd(rtc->day_of_week);
	data[5] = ds3231_hexToBcd(rtc->date);
	data[6] = ds3231_hexToBcd(rtc->month);
	data[7] = ds3231_hexToBcd(rtc->year);

	tw_master_transmit(DS3231_ADDR, data, sizeof(data), false);

	return 1;
}

void ds3231_getTemperature(char *buff)
{
	uint8_t data[2];
	uint8_t set_reg_add;

	set_reg_add =  TEMP_MSB_ADDR;

	tw_master_transmit(DS3231_ADDR, &set_reg_add, sizeof(set_reg_add), true);

	tw_master_receive(DS3231_ADDR, data, sizeof(data));

	if(buff != NULL)
	{
		sprintf(buff, "%dC", (data[0] & 0x7F));
	}
	
}

const char *ds3231_getDayOfWeek(uint8_t dow)
{
	return day_of_week_table[dow];
}

static uint8_t ds3231_bcdToHex(uint8_t bcd)
{
	return ((bcd >> 4) *  10 + (bcd & 0x0F));
}

static uint8_t ds3231_hexToBcd(uint8_t hex)
{
	return ((hex / 10) << 4) + (hex % 10);
}
