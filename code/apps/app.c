#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "max7219.h"
#include "ds3231.h"
#include "buttons.h"
#include "timer.h"
#include "app.h"
#include "menu_manager.h"




#define MATRIX_HEIGHT		8
#define MATRIX_WIDTH		32

#define BTN_DEBOUNCE_MS		30

#define RTC_READ_FREQ_MS    100



static void app_defaultClkHandler(void);
static void app_defaultDowHandler(void);
static void app_defaultDateHandler(void);
static void button_callbackHandler(E_BUTTON_TYPE button_type, E_BUTTON_STATE button_state);
static void timer_callbackHandler();




MAX7219_CONFIG *config = NULL;
RTC_HANDLER rtc_handler;
volatile E_CLK_STATES clk_state;

static T_BTN_INFO up_btn;
static T_BTN_INFO sel_btn;
static T_BTN_INFO down_btn;





int8_t app_init()
{
 	config = max7219_init(MATRIX_HEIGHT, MATRIX_WIDTH);
  	if(!config)
  	{
		  return 0;
  	}

  	bool ret_stat = ds3231_init();
  	if(!ret_stat)
 	{
		  return 0;
	}
    rtc_handler.rtc_rd_counter = 0;
    ds3231_readTimeStamp(&rtc_handler.rtc_rd); 

	max7219_intensity(config, MIN_INTENSITY);

    button_queueInit();
	buttons_init(button_callbackHandler);

	timer_init(timer_callbackHandler);
	timer_start_1_ms();

	sei(); //GLOBAL interrupt enable

    clk_state = DEFAULT_CLK;

	return 1;
}

void app_run()
{
	while(1)
	{
        T_BTN_QUEUE_INFO btn_info;

        if(button_QueueRead(&btn_info))
        {
            if(btn_info.btn_type == SEL_BTN)
            {
                clk_state = (clk_state == DEFAULT_CLK) ? MENU : DEFAULT_CLK;  
            }

			if((btn_info.btn_type == UP_BTN) || (btn_info.btn_type == DOWN_BTN))
            {
				clk_state++;
				clk_state = (clk_state == MENU) ? DEFAULT_CLK : clk_state;
            }
        }

        switch(clk_state)
        {
            case DEFAULT_CLK:
            app_defaultClkHandler();
            break;

			case DEFAULT_DOW:
            app_defaultDowHandler();
            break;

			case DEFAULT_DATE:
            app_defaultDateHandler();
            break;

            case MENU:
            app_menuHandler();
            break;
        }
	}    
}

static void app_defaultClkHandler()
{
    char rtc_buff[10];

    sprintf(rtc_buff, "%02d:%02d", rtc_handler.rtc_rd.hr, rtc_handler.rtc_rd.min);
	max7219_scrollText(config, rtc_buff, 2, RTL, 1);
}

static void app_defaultDowHandler()
{
	static uint32_t prev_time = 0;
	max7219_staticText(config, ds3231_getDayOfWeek(rtc_handler.rtc_rd.day_of_week), 5);

	if(prev_time == 0)
	{
		prev_time = timer_getTicks();
	}

	if((timer_getTicks() - prev_time) >= 1500)
	{
		clk_state = DEFAULT_CLK;
		prev_time = 0;
	}
}

static void app_defaultDateHandler()
{
    char rtc_buff[10];

    sprintf(rtc_buff, "%02d/%02d/%02d", rtc_handler.rtc_rd.date, rtc_handler.rtc_rd.month, rtc_handler.rtc_rd.year);
    max7219_scrollText(config, rtc_buff, 2, RTL, 1);
	clk_state = DEFAULT_CLK;
}

static void button_callbackHandler(E_BUTTON_TYPE button_type, E_BUTTON_STATE button_state)
{
	//TO DO: Handle different buttons and their states
	if(button_type == UP_BTN && (up_btn.btn_state == false))
	{
		if(button_state == PRESSED)
		{
			up_btn.btn_state = true;
			up_btn.btn_debounce_cnt = timer_getTicks();
		}
	}

	if(button_type == SEL_BTN && (sel_btn.btn_state == false))
	{
		if(button_state == PRESSED)
		{
			sel_btn.btn_state = true;
			sel_btn.btn_debounce_cnt = timer_getTicks();
		}
	}

	if(button_type == DOWN_BTN && (down_btn.btn_state == false))
	{
		if(button_state == PRESSED)
		{
			down_btn.btn_state = true;
			down_btn.btn_debounce_cnt = timer_getTicks();
		}
	}
}

static void timer_callbackHandler()
{
	if(up_btn.btn_state == true)
	{
		if( timer_getTicks() - up_btn.btn_debounce_cnt >= BTN_DEBOUNCE_MS)
		{
			if(button_is_pressed(UP_BTN))
			{
				button_QueueAdd(UP_BTN, PRESSED);
			}

			up_btn.btn_state = false;
		}
	}

	if(sel_btn.btn_state == true)
	{
		if( timer_getTicks() - sel_btn.btn_debounce_cnt >= BTN_DEBOUNCE_MS)
		{
			if(button_is_pressed(SEL_BTN))
			{
				button_QueueAdd(SEL_BTN, PRESSED);
			}

			sel_btn.btn_state = false;
		}
	}

	if(down_btn.btn_state == true)
	{
		if( timer_getTicks() - down_btn.btn_debounce_cnt >= BTN_DEBOUNCE_MS)
		{
			if(button_is_pressed(DOWN_BTN))
			{
				button_QueueAdd(DOWN_BTN, PRESSED);
			}

			down_btn.btn_state = false;
		}
	}

    rtc_handler.rtc_rd_counter++;

    if(rtc_handler.rtc_rd_counter >= RTC_READ_FREQ_MS)
    {
        ds3231_readTimeStamp(&rtc_handler.rtc_rd);  
        rtc_handler.rtc_rd_counter = 0;
    }
}