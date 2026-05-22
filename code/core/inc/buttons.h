#ifndef H_BUTTONS_H
#define H_BUTTONS_H

#include <stdint.h>


#define BTN_QUEUE_SIZE      10


typedef enum{
    UP_BTN = 0,
    SEL_BTN,
    DOWN_BTN
}E_BUTTON_TYPE;

typedef enum{
    PRESSED = 0,
    RELEASED
}E_BUTTON_STATE;

typedef struct{
    bool btn_state;
    uint32_t btn_debounce_cnt;
}T_BTN_INFO;

typedef struct{
    E_BUTTON_STATE btn_state;
    E_BUTTON_TYPE btn_type;
    bool valid_btn_press;
}T_BTN_QUEUE_INFO;

typedef struct{
    uint8_t wr_idx;
    uint8_t rd_idx;
    bool queue_is_empty;
    bool queue_is_full;
    T_BTN_QUEUE_INFO btn_queue[BTN_QUEUE_SIZE]; 
}T_BTN_QUEUE_HANDLE;



typedef void (*callback_func_ptr)(E_BUTTON_TYPE button_type, E_BUTTON_STATE button_state);

void buttons_init(callback_func_ptr callback);
bool button_is_pressed(E_BUTTON_TYPE btn);

void button_queueInit(void);
bool button_QueueAdd(E_BUTTON_TYPE btn_type, E_BUTTON_STATE btn_state);
bool button_QueueRead(T_BTN_QUEUE_INFO *ret);
bool button_queueIsEmpty(void);
bool button_queueIsFull(void);


#endif