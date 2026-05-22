#ifndef H_TIMER_H
#define H_TIMER_H

#include <stdint.h>


typedef void (*timer_callback_func_ptr)(void);

void timer_init(timer_callback_func_ptr callback);
void timer_start_1_ms(void);
uint32_t timer_getTicks(void);
void timer_stop_1_ms(void);



#endif