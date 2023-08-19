#include "embedded_button.h"

struct button_obj_t button1;

uint8_t read_button_pin(uint8_t button_id)
{
    // you can share the GPIO read function with multiple Buttons
    switch(button_id)
    {
        case 0:
            return get_button1_value(); //Require self implementation
            break;

        default:
            return 0;
            break;
    }

    return 0;
}


int main()
{
    static uint8_t btn1_event_val;

    button_init(&button1, read_button_pin, 0, 0, NULL, 0);
    button_start(&button1);

    //make the timer invoking the button_ticks() interval 5ms.
    //This function is implemented by yourself.
    __timer_start(button_ticks, 0, 5);

    while(1)
    {
        if(btn1_event_val != get_button_event(&button1)) {
            btn1_event_val = get_button_event(&button1);

            if(btn1_event_val == SINGLE_CLICK_KV) {
                //do something
            } else if(btn1_event_val == DOUBLE_CLICK_KV) {
                //do something
            } else if(btn1_event_val == SINGLE_CLICK_THEN_LONG_PRESS_KV) {
                //do something
            }
        }
    }
}
