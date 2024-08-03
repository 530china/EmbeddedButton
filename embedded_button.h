#ifndef __EMBEDDED_BUTTON_H__
#define __EMBEDDED_BUTTON_H__

#include "stdint.h"
#include "string.h"

typedef uint32_t key_value_type_t;
typedef uint32_t state_bits_type_t;
typedef uint8_t kv_match_operator_type_t;

//According to your need to modify the constants.
#define TICKS_INTERVAL    5 //ms
#define DEBOUNCE_TICKS    3 //MAX 16
#define SHORT_TICKS       (350 /TICKS_INTERVAL)
#define LONG_TICKS        (1000 /TICKS_INTERVAL)

#define NONE_PRESS_KV       0
#define SINGLE_CLICK_KV     0b010
#define DOUBLE_CLICK_KV     0b01010

#define SINGLE_CLICK_THEN_LONG_PRESS_KV     0b01011
#define DOUBLE_CLICK_THEN_LONG_PRESS_KV     0b0101011

#define LONG_PRESEE_START       0b011
#define LONG_PRESEE_HOLD        0b0111
#define LONG_PRESEE_HOLD_END    0b01110

#define KV_MATCH_OPERATOR_NULL             (0)
#define KV_MATCH_OPERATOR_BITWISE_AND      (1 << 0)
#define KV_MATCH_OPERATOR_BITWISE_OR       (1 << 1)
#define KV_MATCH_OPERATOR_BITWISE_NOT      (1 << 2)
#define KV_MATCH_OPERATOR_BITWISE_XOR      (1 << 2)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

typedef struct {
    key_value_type_t operand;
    kv_match_operator_type_t operator;
    key_value_type_t tar_result;
    void (*kv_func_cb)(void*);
} key_value_match_map_t;

typedef struct button_obj_t {
    uint8_t  debounce_cnt : 4;
    uint8_t  active_level : 1;
    uint8_t  read_level : 1;
    uint8_t  read_level_update : 1;
    uint8_t  event_analyze_en : 1;
    uint8_t  id;
    uint16_t ticks;
    state_bits_type_t state_bits;
    key_value_type_t key_value;
    uint8_t  (*_read_button_func_ptr)(uint8_t button_id_);
    const key_value_match_map_t *kv_match_map_ptr;
    size_t map_size;
    struct button_obj_t* next;
}button_obj_t;

void button_init(struct button_obj_t* button            ,       \
                 uint8_t(*read_button_func_ptr)(uint8_t),       \
                 uint8_t active_level                   ,       \
                 uint8_t button_id                      ,       \
                 const key_value_match_map_t *kv_match_map_ptr, \
                 size_t map_size                                \
                 );
int  button_start(struct button_obj_t* handle);
void button_stop(struct button_obj_t* handle);
void button_ticks(void);
key_value_type_t get_button_key_value(struct button_obj_t* button);
uint8_t check_is_repeat_click_mode(struct button_obj_t* button);

#endif
