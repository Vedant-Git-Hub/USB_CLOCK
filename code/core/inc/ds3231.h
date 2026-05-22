/*
 * ds3231.h
 *
 *  Created on: Dec 28, 2024
 *      Author: Vedant Bhale
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

typedef struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hr;
	uint8_t day_of_week;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t format;
	uint8_t am_pm;
}RTC_TIME;

typedef struct{
	uint8_t rtc_rd_counter;
	RTC_TIME rtc_rd;
}RTC_HANDLER;


bool ds3231_init(void);
bool ds3231_readTimeStamp(RTC_TIME *);
bool ds3231_writeTimeStamp(RTC_TIME *);
const char *ds3231_getDayOfWeek(uint8_t );

#endif /* INC_DS3231_H_ */
