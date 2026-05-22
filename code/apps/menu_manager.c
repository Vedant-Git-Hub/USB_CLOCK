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


typedef enum{
    SET_HR = 0,
    SET_MIN,
    SET_SEC,
    SET_DOW,
    SET_DD,
    SET_MM,
    SET_YY 
}T_SET_STATES;




static void menu_set(void);
static void menu_setBrightness(void);


extern MAX7219_CONFIG *config;
extern RTC_HANDLER rtc_handler;
extern volatile E_CLK_STATES clk_state;





void app_menuHandler()
{
    bool in_menu = true;
    int8_t menu_state = SET;
    int8_t sub_menu_state = IDLE;
    T_BTN_QUEUE_INFO btn_info;
    char rtc_buff[10];
    char *menu_options[MAX_MENU_STATES] = {
        "SET",
        "BRT",
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

            case SET:
            menu_set();
            menu_state = sub_menu_state;
            sub_menu_state = IDLE;
            break;

            case BRT:
            menu_setBrightness();
            menu_state = sub_menu_state;
            sub_menu_state = IDLE;
            break;

            case EXT:
            sub_menu_state = IDLE;
            menu_state = IDLE;
            in_menu = false;
            clk_state = DEFAULT_CLK;
            break;
        }

    }
}

static void menu_set()
{
    RTC_TIME rtc_wr;
    T_BTN_QUEUE_INFO btn_info;
    bool in_set_menu = true;
    T_SET_STATES set_states = SET_HR;
    int8_t pressed_btn = -1;
    char rtc_buff[10];

    rtc_wr = rtc_handler.rtc_rd;

    while(in_set_menu)
    {   
        if(button_QueueRead(&btn_info))
        {
            if(btn_info.btn_type == UP_BTN)
            {
                pressed_btn = UP_BTN;
            }
            if(btn_info.btn_type == SEL_BTN)
            {
                pressed_btn = SEL_BTN;
            }
            if(btn_info.btn_type == DOWN_BTN)
            {
                pressed_btn = DOWN_BTN;
            }
        } 
        
        switch(set_states)
        {
            case SET_HR:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.hr = (rtc_wr.hr + 1) % 24;
                }
                if(pressed_btn == SEL_BTN)
                {
                    set_states = SET_MIN;
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.hr = (rtc_wr.hr == 0) ? 23 : rtc_wr.hr - 1;
                }
                sprintf(rtc_buff, "H:%02d", rtc_wr.hr);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;

            case SET_MIN:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.min = (rtc_wr.min + 1) % 60;
                }
                if(pressed_btn == SEL_BTN)
                {
                    set_states = SET_SEC;
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.min = (rtc_wr.min == 0) ? 59 : rtc_wr.min - 1;
                }
                sprintf(rtc_buff, "M:%02d", rtc_wr.min);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;
            
            case SET_SEC:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.sec = (rtc_wr.sec + 1) % 60;
                }
                if(pressed_btn == SEL_BTN)
                {
                    set_states = SET_DOW;
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.sec = (rtc_wr.sec == 0) ? 59 : rtc_wr.sec - 1;
                }
                sprintf(rtc_buff, "S:%02d", rtc_wr.sec);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;

            case SET_DOW:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.day_of_week = (rtc_wr.day_of_week + 1) % 7;
                }
                if(pressed_btn == SEL_BTN)
                {
                    set_states = SET_DD;
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.day_of_week = (rtc_wr.day_of_week == 0) ? 6 : rtc_wr.day_of_week - 1;
                }
                sprintf(rtc_buff, "W:%02d", rtc_wr.day_of_week);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;

            case SET_DD:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.date++;
                    if(rtc_wr.date > 31)
                    {
                        rtc_wr.date = 1;
                    }
                }
                if(pressed_btn == SEL_BTN)
                {
                    set_states = SET_MM;
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.date = (rtc_wr.date == 1) ? 31 : rtc_wr.date - 1;
                }
                sprintf(rtc_buff, "DD%02d", rtc_wr.date);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;

            case SET_MM:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.month++;
                    if(rtc_wr.month > 12)
                    {
                        rtc_wr.month = 1;
                    }
                }
                if(pressed_btn == SEL_BTN)
                {
                    set_states = SET_YY;
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.month = (rtc_wr.month == 1) ? 12 : rtc_wr.month - 1;
                }
                sprintf(rtc_buff, "MM%02d", rtc_wr.month);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;

            case SET_YY:
                if(pressed_btn == UP_BTN)
                {
                    rtc_wr.year = (rtc_wr.year + 1) % 100;
                }
                if(pressed_btn == SEL_BTN)
                {
                    in_set_menu = false;
                    ds3231_writeTimeStamp(&rtc_wr);
                }
                if(pressed_btn == DOWN_BTN)
                {
                    rtc_wr.year = (rtc_wr.year == 0) ? 99 : rtc_wr.year - 1;
                }
                sprintf(rtc_buff, "YY%02d", rtc_wr.year);
                max7219_staticText(config, rtc_buff, 1);
                pressed_btn = -1;
            break;            
        }

    }
    
}

static void menu_setBrightness()
{
    char rtc_buff[10];
    bool in_brt_menu = true;
    static uint8_t brightness = 0;
    T_BTN_QUEUE_INFO btn_info;

    while(in_brt_menu)
    {
        if(button_QueueRead(&btn_info))
        {
            if(btn_info.btn_type == UP_BTN)
            {
                brightness = (brightness + 1) % (MAX_INTENSITY + 1);
                max7219_intensity(config, brightness);
            }
            if(btn_info.btn_type == SEL_BTN)
            {
                in_brt_menu = false;
                max7219_intensity(config, brightness);
            }
            if(btn_info.btn_type == DOWN_BTN)
            {
                brightness = (brightness == 0) ? MAX_INTENSITY : (brightness - 1);
                max7219_intensity(config, brightness);
            }
        }
        
        sprintf(rtc_buff, "BT%02d", brightness);
        max7219_staticText(config, rtc_buff, 1);
    }
}
