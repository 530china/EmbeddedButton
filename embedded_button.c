
#include "embedded_button.h"

static struct button_obj_t* button_list_head = NULL;

/**
  * @brief  Initializes the button struct.
  * @param  button: the button strcut.
  * @param  read_button_func_ptr: read button level function ptr.
  * @param  active_level: GPIO level when button is pressed .
  * @param  button_id: the button id.
  * @param  map_ptr: key-value map
  * @param  map_size: map size
  * @retval None
  */
void button_init(struct button_obj_t* button            , \
                 uint8_t(*read_button_func_ptr)(uint8_t), \
                 uint8_t active_level                   , \
                 uint8_t button_id                      , \
                 const key_value_map_t *map_ptr         , \
                 size_t map_size                          \
                 )
{
    memset(button, 0, sizeof(struct button_obj_t));
    button->key_value = (key_value_type_t)NONE_PRESS_KV;
    button->_read_button_func_ptr = read_button_func_ptr;
    button->read_level = button->_read_button_func_ptr(button_id);
    button->active_level = active_level;
    button->id = button_id;
    button->map_ptr = map_ptr;
    button->map_size = map_size;
}

/**
  * @brief  Inquire the button event happen.
  * @param  button: the button button strcut.
  * @retval button key value.
  */
key_value_type_t get_button_event(struct button_obj_t* button)
{
    return (key_value_type_t)(button->key_value);
}

/**
  * @brief  Add a bit to the end of the number
  * @param  state_bits: src number point.
  * @param  bit: tartget bit
  * @retval none.
  */
static void __append_bit(state_bits_type_t* state_bits, uint8_t bit)
{
    *state_bits = (*state_bits << 1) | bit;
}

/**
  * @brief  check number if match the target bits
  * @param  state_bits: src number point.
  * @param  target: tartget bits
  * @param  target_bits_number: target bits number
  * @retval 1 is match, 0 is not match.
  */
static uint8_t __check_if_the_bits_match(key_value_type_t *state_bits, key_value_type_t target, uint8_t target_bits_number)
{
    key_value_type_t mask = (1 << target_bits_number) - 1;

    return (((*state_bits) & mask) == target? 1 : 0);
}

/**
  * @brief  Button driver core function, driven by 2 principles
  *         1. As long as the key value is not zero, the time tick++
  *         2. As long as the button status changes,
                    add a bit£¨1: press, 0: release£©and
                    reset tick time (ensure that the tick is the time it was pressed or raised)
  * @param  handle: the button handle strcut.
  * @retval None
  */
void button_handler(struct button_obj_t* button)
{
    uint8_t read_gpio_level = button->_read_button_func_ptr(button->id);

    if((button->state_bits) > 0) button->ticks++;

    /*------------button debounce button---------------*/
    if(read_gpio_level != button->read_level) { //not equal to prev one
        //continue read DEBOUNCE_TICKS times same new level change
        if(++(button->debounce_cnt) >= DEBOUNCE_TICKS) {
            button->read_level = read_gpio_level;
            button->read_level_update = 1;
            button->debounce_cnt = 0;
        }
    } else { //leved not change ,counter reset.
        button->debounce_cnt = 0;
    }

    if(button->read_level_update) {
        if(button->read_level == button->active_level) {
            __append_bit(&button->state_bits, 1);
        }
        else {
            __append_bit(&button->state_bits, 0);
        }

        button->read_level_update = 0;
        button->ticks = 0;
    }

    if(button->ticks > SHORT_TICKS) {
        if(button->read_level != button->active_level) {
            button->event_analyze_en = 1;
        }
    }

    if(button->ticks > LONG_TICKS) {
        if((button->read_level == button->active_level) \
        && __check_if_the_bits_match(&button->state_bits, 0b01, 2)) // long press start
        {
            __append_bit(&button->state_bits, 1);
            button->event_analyze_en = 1;
        }
        else if((button->read_level == button->active_level) \
             && __check_if_the_bits_match(&button->state_bits, 0b011, 3)) // long press hold
        {
            __append_bit(&button->state_bits, 1);
            button->event_analyze_en = 1;
        }
    }

    if((button->state_bits) && (button->event_analyze_en)) {
        // button event processing
        button->key_value = button->state_bits;

        for(size_t i = 0; i < button->map_size; i++) {
            if((button->map_ptr[i].key_value == button->key_value)
            && (button->map_ptr[i].kv_func_cb))
            {
                button->map_ptr[i].kv_func_cb(button);
            }
        }

        // button is released state
        if(button->read_level != button->active_level) {
            button->event_analyze_en = 0;
            button->state_bits = 0;
            button->ticks = 0;
        }

    }
}

/**
  * @brief  Start the button work, add the button struct into work list.
  * @param  button: target button strcut.
  * @retval 0: succeed. -1: already exist.
  */
int button_start(struct button_obj_t* button)
{
    struct button_obj_t* target = button_list_head;
    while(target) {
        if(target == button) return -1; //already exist.
        target = target->next;
    }
    button->next = button_list_head;
    button_list_head = button;

    return 0;
}

/**
  * @brief  Stop the button work, remove the button struct off work list.
  * @param  button: target button strcut.
  * @retval None
  */
void button_stop(struct button_obj_t* button)
{
    struct button_obj_t** curr;
    for(curr = &button_list_head; *curr; ) {
        struct button_obj_t* entry = *curr;
        if (entry == button) {
            *curr = entry->next;
            return;
        } else
            curr = &entry->next;
    }
}

/**
  * @brief  Background ticks, timer repeat invoking interval 5ms.
  * @param  None.
  * @retval None
  */
void button_ticks()
{
    struct button_obj_t* target;
    for(target = button_list_head; target; target = target->next) {
        button_handler(target);
    }
}
