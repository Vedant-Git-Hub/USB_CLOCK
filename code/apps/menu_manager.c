#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <util/delay.h>

#include "max7219.h"
#include "ds3231.h"
#include "buttons.h"
#include "timer.h"
#include "app.h"
#include "menu_manager.h"



static void menu_clk(char *rtc_buff);
static void menu_day(void);
static void menu_date(char *rtc_buff);
static void menu_set(void);



extern MAX7219_CONFIG *config;
extern RTC_HANDLER rtc_handler;
extern volatile E_CLK_STATES clk_state;





void app_menuHandler()
{
    bool in_menu = true;
    int8_t menu_state = CLK;
    int8_t sub_menu_state = IDLE;
    T_BTN_QUEUE_INFO btn_info;
    char rtc_buff[10];
    char *menu_options[MAX_MENU_STATES] = {
        "CLK",
        "DAY",
        "DAT",
        "SET",
        "EXT"
    };


    while(in_menu)
    {

        if(button_QueueRead(&btn_info))
        {
            if(btn_info.btn_type == UP_BTN)
            {
                menu_state--;
                menu_state = (menu_state < 0) ? 0 : menu_state;
            }
            if(btn_info.btn_type == SEL_BTN)
            {
                sub_menu_state = menu_state;
                menu_state = IDLE;
            }
            if(btn_info.btn_type == DOWN_BTN)
            {
                menu_state++;
                menu_state = (menu_state >= MAX_MENU_STATES) ? (MAX_MENU_STATES - 1) : menu_state;
            }
        }   
        
        if(menu_state != IDLE)
        {
            max7219_staticText(config, menu_options[menu_state], 5);
        }

        switch(sub_menu_state)
        {
            case IDLE:
            break;

            case CLK:
            menu_clk(rtc_buff);
            menu_state = sub_menu_state;
            sub_menu_state = IDLE;
            break;

            case DAY:
            menu_day();
            menu_state = sub_menu_state;
            sub_menu_state = IDLE;
            break;

            case DATE:
            menu_date(rtc_buff);
            menu_state = sub_menu_state;
            sub_menu_state = IDLE;
            break;

            case SET:
            menu_set();
            menu_state = sub_menu_state;
            sub_menu_state = IDLE;
            break;

            case EXT:
            sub_menu_state = IDLE;
            menu_state = IDLE;
            in_menu = false;
            clk_state = DEFAULT;
            break;
        }

    }
}

static void menu_clk(char *rtc_buff)
{
    sprintf(rtc_buff, "%02d:%02d", rtc_handler.rtc_rd.hr, rtc_handler.rtc_rd.min);
    max7219_scrollText(config, rtc_buff, 2, RTL, 1);
}

static void menu_day()
{
    max7219_staticText(config, ds3231_getDayOfWeek(rtc_handler.rtc_rd.day_of_week), 5);
    _delay_ms(1000);
}

static void menu_date(char *rtc_buff)
{
    sprintf(rtc_buff, "%02d/%02d/%02d", rtc_handler.rtc_rd.date, rtc_handler.rtc_rd.month, rtc_handler.rtc_rd.year);
    max7219_scrollText(config, rtc_buff, 2, RTL, 1);
}

static void menu_set()
{
    RTC_TIME rtc_wr;

    rtc_wr = rtc_handler.rtc_rd;

    max7219_staticText(config, "H:", 1);
    _delay_ms(1000);
}