#include "embedded_button.h"

enum button_id_e {
    btn1_id,
    btn2_id,
};

struct button_obj_t button1;
struct button_obj_t button2;

uint8_t read_button_pin(uint8_t button_id)
{
    // you can share the GPIO read function with multiple Buttons
    switch(button_id)
    {
        case btn1_id:
            return get_button1_value(); //Require self implementation
            break;
        case btn2_id:
            return get_button2_value(); //Require self implementation
            break;

        default:
            return 0;
            break;
    }

    return 0;
}


const key_value_map_t button1_map[] =
{
    {
        .key_value = SINGLE_CLICK_KV,
        .kv_func_cb = btn1_single_click_handle
    },
    {
        .key_value = DOUBLE_CLICK_KV,
        .kv_func_cb = btn1_double_click_handle
    },
    {
        .key_value = LONG_PRESEE_START,
        .kv_func_cb = btn1_long_press_handle
    },
    {
        .key_value = SINGLE_CLICK_THEN_LONG_PRESS_KV,
        .kv_func_cb = btn1_single_click_then_long_press_handle
    },
    {
        .key_value = DOUBLE_CLICK_THEN_LONG_PRESS_KV,
        .kv_func_cb = btn1_double_click_then_long_press_handle
    }
};

const key_value_map_t button2_map[] =
{
    {
        .key_value = SINGLE_CLICK_KV,
        .kv_func_cb = btn2_single_click_handle
    },
    {
        .key_value = DOUBLE_CLICK_KV,
        .kv_func_cb = btn2_double_click_handle
    },
    {
        .key_value = LONG_PRESEE_START,
        .kv_func_cb = btn2_long_press_handle
    },
    {
        .key_value = SINGLE_CLICK_THEN_LONG_PRESS_KV,
        .kv_func_cb = btn2_single_click_then_long_press_handle
    },
    {
        .key_value = DOUBLE_CLICK_THEN_LONG_PRESS_KV,
        .kv_func_cb = btn2_double_click_then_long_press_handle
    }
};

int main()
{
    button_init(&button1, read_button_pin, 0, btn1_id, button1_map, ARRAY_SIZE(button1_map));
    button_init(&button2, read_button_pin, 0, btn2_id, button2_map, ARRAY_SIZE(button2_map));

    button_start(&button1);
    button_start(&button2);

    //make the timer invoking the button_ticks() interval 5ms.
    //This function is implemented by yourself.
    __timer_start(button_ticks, 0, 5);

    while(1)
    {}
}