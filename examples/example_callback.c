#include "embedded_button.h"

enum button_id_e {
    btn1_id = 0,
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

void single_click_handle(void* btn)
{
    //do something...
    printf("/****single click****/\r\n");
}

void double_click_handle(void* btn)
{
    //do something...
    printf("/****double click****/\r\n");
}

void long_press_handle(void* btn)
{
    //do something...
    printf("/****long press****/\r\n");
}

void single_click_then_long_press_handle(void* btn)
{
    //do something...
    printf("/****single click and long press****/\r\n");
}

void quintuple_click_handle(void* btn)
{
    //do something...
    if(check_is_repeat_click_mode(btn))
    {// To implement the logic where any key press that occurs five or more times in succession is treated as a five-hit press.
        printf("/****quintuple click****/\r\n");
    }

}

void repeat_click_handle(void* btn)
{
    //do something...
    if(check_is_repeat_click_mode(btn))
    {
        printf("/****repeat click****/\r\n");

        if(get_button_key_value(&button2) == 0b1010)
        {
            printf("/****button2 double click****/\r\n");
        }

        if(get_button_key_value(&button2) == 0b101010)
        {
            printf("/****button2 triple click****/\r\n");
        }

        if(get_button_key_value(&button2) == 0b10101010)
        {
            printf("/****button2 quadruple click****/\r\n");
        }
    }
}

void filter_ending_with_long_press_handle(void* btn)
{
    printf("/****an event ending with a long press on a key****/\r\n");
}

const key_value_match_map_t button1_map[] =
{
    {
        .tar_result = SINGLE_CLICK_KV,
        .kv_func_cb = single_click_handle
    },
    {
        .tar_result = DOUBLE_CLICK_KV,
        .kv_func_cb = double_click_handle
    },
    {
        .tar_result = LONG_PRESEE_START,
        .kv_func_cb = long_press_handle
    },
    {
        .tar_result = SINGLE_CLICK_THEN_LONG_PRESS_KV,
        .kv_func_cb = single_click_then_long_press_handle
    },
    {
        .operand = 0b1010101010,
        .operator = KV_MATCH_OPERATOR_BITWISE_AND,
        .tar_result = 0b1010101010,
        .kv_func_cb = quintuple_click_handle
    }
};

const key_value_match_map_t button2_map[] =
{
    {
        .operand = 0b1010,
        .operator = KV_MATCH_OPERATOR_BITWISE_AND,
        .tar_result = 0b1010,
        .kv_func_cb = repeat_click_handle
    },
    {
        .operand = 0b1111,
        .operator = KV_MATCH_OPERATOR_BITWISE_AND,
        .tar_result = 0b1110,
        .kv_func_cb = filter_ending_with_long_press_handle
    }
};


int main()
{
    button_init(&button1, read_button_pin, 0, btn1_id, button1_map, ARRAY_SIZE(button1_map));
    button_start(&button1);

    button_init(&button2, read_button_pin, 0, btn2_id, button2_map, ARRAY_SIZE(button2_map));
    button_start(&button2);

    //make the timer invoking the button_ticks() interval 5ms.
    //This function is implemented by yourself.
    __timer_start(button_ticks, 0, 5);

    while(1)
    {}
}