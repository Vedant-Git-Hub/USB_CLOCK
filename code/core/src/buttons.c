#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "buttons.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define UP_BUTTON           PD2
#define SEL_BUTTON          PD3
#define DOWN_BUTTON         PD4



static bool button_queueIsEmpty(void);
static bool button_queueIsFull(void);



static uint8_t prev_btn_state = 0;
static callback_func_ptr button_callback_func = NULL;
static T_BTN_QUEUE_HANDLE btn_queue_hnd;


ISR(PCINT2_vect) 
{
    E_BUTTON_TYPE button_type;
    E_BUTTON_STATE button_state;
    uint8_t curr_btn_state = PIND;
    uint8_t changed_btn = curr_btn_state ^ prev_btn_state;

    if(NULL == button_callback_func)
    {
        prev_btn_state = curr_btn_state;
        return;
    }

    if(changed_btn & (1 << UP_BUTTON))
    {
        button_type = UP_BTN;
        button_state = (curr_btn_state & (1 << UP_BUTTON)) ? PRESSED : RELEASED;
        button_callback_func(button_type, button_state);
    }

    if(changed_btn & (1 << SEL_BUTTON))
    {
        button_type = SEL_BTN;
        button_state = (curr_btn_state & (1 << SEL_BUTTON)) ? PRESSED : RELEASED;
        button_callback_func(button_type, button_state);
    }

    if(changed_btn & (1 << DOWN_BUTTON))
    {
        button_type = DOWN_BTN;
        button_state = (curr_btn_state & (1 << DOWN_BUTTON)) ? PRESSED : RELEASED;
        button_callback_func(button_type, button_state);
    }

    prev_btn_state = curr_btn_state;    
}


void buttons_init(callback_func_ptr callback)
{
    DDRD &= ~(1 << UP_BUTTON);
    DDRD &= ~(1 << SEL_BUTTON);
    DDRD &= ~(1 << DOWN_BUTTON);

    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20); //Enable interrupts on pins PD2 to PD4
    PCICR |= (1 << PCIE2); //Enable interrupts from PCINT23 to PCINT16

    button_callback_func = callback;
}

bool button_is_pressed(E_BUTTON_TYPE btn)
{
    bool state = false;

    if(btn == UP_BTN)
    {
        state = PIND & (1 << UP_BUTTON);
    }

    if(btn == SEL_BTN)
    {
        state = PIND & (1 << SEL_BUTTON);
    }

    if(btn == DOWN_BTN)
    {
        state = PIND & (1 << DOWN_BUTTON);
    }

    return state;
}

void button_queueInit()
{
    for(uint8_t idx = 0; idx < BTN_QUEUE_SIZE; idx++)
    {
        btn_queue_hnd.btn_queue[idx].valid_btn_press = false;
    }

    btn_queue_hnd.wr_idx = 0;
    btn_queue_hnd.rd_idx = 0;
    btn_queue_hnd.queue_is_empty = true;
    btn_queue_hnd.queue_is_full = false;
}

bool button_QueueAdd(E_BUTTON_TYPE btn_type, E_BUTTON_STATE btn_state)
{
    if(!button_queueIsFull())
    {
        btn_queue_hnd.btn_queue[btn_queue_hnd.wr_idx].btn_type = btn_type;
        btn_queue_hnd.btn_queue[btn_queue_hnd.wr_idx].btn_state = btn_state;
        btn_queue_hnd.btn_queue[btn_queue_hnd.wr_idx].valid_btn_press = true;
        btn_queue_hnd.wr_idx++;

        return 1;
    }

    return 0;
}

bool button_QueueRead(T_BTN_QUEUE_INFO *ret)
{
    if(!button_queueIsEmpty())
    {
        T_BTN_QUEUE_INFO temp;
        uint8_t max = btn_queue_hnd.wr_idx;

        if(ret == NULL)
        {
            return 0;
        }

        btn_queue_hnd.rd_idx = 0;
        temp = btn_queue_hnd.btn_queue[btn_queue_hnd.rd_idx];

        for(uint8_t arr_idx = 0; arr_idx < max; arr_idx++)
        {
            btn_queue_hnd.btn_queue[arr_idx] = btn_queue_hnd.btn_queue[arr_idx + 1];
        }

        btn_queue_hnd.btn_queue[btn_queue_hnd.wr_idx].valid_btn_press = false;
        btn_queue_hnd.wr_idx--;

        ret->btn_state = temp.btn_state;
        ret->btn_type = temp.btn_type;
        ret->valid_btn_press = temp.valid_btn_press;

        return 1;
    }

    return 0;
}

static bool button_queueIsEmpty()
{
    if(btn_queue_hnd.wr_idx == 0 && btn_queue_hnd.rd_idx == 0)
    {
        btn_queue_hnd.queue_is_empty = true;
    }
    else
    {
        btn_queue_hnd.queue_is_empty = false;
    }

    return btn_queue_hnd.queue_is_empty;
}

static bool button_queueIsFull()
{
    if(btn_queue_hnd.wr_idx == BTN_QUEUE_SIZE && btn_queue_hnd.rd_idx == 0)
    {
        btn_queue_hnd.queue_is_full = true;
    }
    else
    {
        btn_queue_hnd.queue_is_full = false;
    }

    return btn_queue_hnd.queue_is_full;
}