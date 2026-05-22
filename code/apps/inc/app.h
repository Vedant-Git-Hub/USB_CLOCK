#ifndef H_APP_H
#define H_APP_H

#include <stdint.h>

#include "buttons.h"


typedef enum{
DEFAULT_CLK = 0,
DEFAULT_DOW,
DEFAULT_DATE,
MENU,
MAX_CLK_STATES
}E_CLK_STATES;

typedef enum{
SET = 0,
BRT,
EXT,
MAX_MENU_STATES,
IDLE
}E_MENU_STATES;




int8_t app_init(void);
void app_run(void);


#endif