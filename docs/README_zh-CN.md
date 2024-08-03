<h1 align="center">EmbeddedButton</h1>

<p align="center">
<a href="https://github.com/530china/EmbeddedButton/blob/master/LICENSE" target="blank">
<img src="https://img.shields.io/github/license/rahuldkjain/github-profile-readme-generator?style=flat-square" alt="github-profile-readme-generator license" />
</a>
<a href="https://github.com/530china/EmbeddedButton/stargazers" target="blank">
<img src="https://img.shields.io/github/stars/rahuldkjain/github-profile-readme-generator?style=flat-square" alt="github-profile-readme-generator stars"/>
</a>
<a href="https://github.com/530china/EmbeddedButton/issues" target="blank">
<img src="https://img.shields.io/github/issues/rahuldkjain/github-profile-readme-generator?style=flat-square" alt="github-profile-readme-generator issues"/>
</a>
<a href="https://github.com/530china/EmbeddedButton/pulls" target="blank">
<img src="https://img.shields.io/github/issues-pr/rahuldkjain/github-profile-readme-generator?style=flat-square" alt="github-profile-readme-generator pull-requests"/>
</a>
</p>

<h2>? 简介</h2>
EmbeddedButton是一个轻量级简单易用的嵌入式按键驱动模块，可无限拓展按键;

- 支持多连击、长按、短按长按等多种按键事件；
- 模块通过几个简单原则完成了整个代码逻辑的支撑;
- 核心处理采取数据驱动方式，支持位运算键值匹配，仅内置基本键值定义，其他键值含义由用户通过**配置键值匹配规则**自行定义，而不用侵入式修改代码，灵活性极强；

## ? 特性

> 1.依靠简单几个原则，支持起整个按键判断逻辑
- 只要键值非零，时间tick++
- 只要按键状态发生变化，改变一次键值（**__append_bit()**），tick时间清零（确保tick为按下或抬起的时间）
- 以tick时间的长短及按键抬起作为一次状态结束的判断依据，可以很好的实现短按长按等操作；

> 2.使用C语言实现，巧妙利用位运算来实现每个按键键值的二进制记录表示，1代表按下，0代表松开

键值 | 说明
--- | ---
0b0 | 未按下
0b010 | 单击
0b01010 | 双击
0b01010...n | n连击
0b011 | 长按开始
0b0111| 长按保持
0b01110|长按结束
0b01011|短按然后长按
0b0101011 | 双击然后长按
0b01010..n11 | n连击然后长按

> 3.核心处理采取数据驱动方式，支持位运算键值匹配：
- 关键数据结构，键值匹配规则配置表：
```c
typedef struct {
    key_value_type_t operand;           // 操作数
    kv_match_operator_type_t operator;  // 操作符
    key_value_type_t tar_result;        // 目标结果
    void (*kv_func_cb)(void*);          // 符合匹配后调用的回调函数
} key_value_match_map_t;

```
- 关键算法：
```c
key_value_type_t operand_origin = button->kv_match_map_ptr[i].operand;
key_value_type_t operand_result = button->kv_match_map_ptr[i].operand;
kv_match_operator_type_t operator =button->kv_match_map_ptr[i].operator;
key_value_type_t tar_result = button->kv_match_map_ptr[i].tar_result;

if(operator == KV_MATCH_OPERATOR_NULL)
    operand_result = button->key_value;
else if(operator & KV_MATCH_OPERATOR_BITWISE_AND)
    operand_result = (operand_origin & button->key_value);
else if(operator & KV_MATCH_OPERATOR_BITWISE_OR)
    operand_result = (operand_origin | button->key_value);
else if(operator & KV_MATCH_OPERATOR_BITWISE_NOT)
    operand_result = ~(button->key_value);
else if(operator & KV_MATCH_OPERATOR_BITWISE_XOR)
    operand_result = (operand_origin ^ button->key_value);

if(operand_result == tar_result)
{
    button->kv_match_map_ptr[i].kv_func_cb(button);
}
```

- 支持的操作符：
```c
#define KV_MATCH_OPERATOR_NULL             (0)      // 无操作符，仅通过(key_value == tar_result)?判断, 默认是这个
#define KV_MATCH_OPERATOR_BITWISE_AND      (1 << 0) // 按位与操作符，(operand & key_value == tar_result)?
#define KV_MATCH_OPERATOR_BITWISE_OR       (1 << 1) // 按位或操作符，(operand | key_value == tar_result)?
#define KV_MATCH_OPERATOR_BITWISE_NOT      (1 << 2) // 按位取反操作符，(~ key_value == tar_result)?
#define KV_MATCH_OPERATOR_BITWISE_XOR      (1 << 2) // 按位异或操作符，(operand ^ key_value == tar_result)?
```

> 4.基于面向对象方式设计思路，每个按键对象单独用一份数据结构管理

## ? 如何食用

### 1）使用
<details>
<summary>点击展开/折叠C代码<img src="https://media.giphy.com/media/WUlplcMpOCEmTGBtBW/giphy.gif" width="30"></summary>

- 以使用callback方式为例：
```c
// 1.包含头文件
#include "embedded_button.h"

// 2.定义按键实体
struct button_obj_t button1;

// 3.GPIO电平读取接口设置
uint8_t read_button_pin(uint8_t button_id)
{
    // you can share the GPIO read function with multiple Buttons
    switch(button_id)
    {
        case 0:
            return get_button1_value(); // 用户自行实现
            break;

        default:
            return 0;
            break;
    }

    return 0;
}

// 4. 配置键值匹配规则(设置回调事件)
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
        printf("/****quintuple click****/\r\n");
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
...

int main()
{
/************************************************
****5.初始化按键对象，参数含义分别为
****
****- 按键实体
****- 绑定按键的GPIO电平读取接口**read_button1_pin()**
****- 设置有效触发电平
****- 按键ID
****- 键值匹配规则配置表
****- 键值匹配规则配置表大小
*************************************************/
    button_init(&button1, read_button_pin, 0, 0, button1_map, ARRAY_SIZE(button1_map));
    // 6.启动按键
    button_start(&button1);

    // 7. 设置一个5ms间隔的定时器循环调用按键后台处理函数 button_ticks()
    __timer_start(button_ticks, 0, 5);

    while(1)
    {}
}
```
![Alt text](image.png)
<br></details>

### 2）调试

<details>
<summary>点击展开/折叠<img src="https://media.giphy.com/media/WUlplcMpOCEmTGBtBW/giphy.gif" width="30"></summary>

- 定义EB_DEBUG_PRINTF宏后将会开启键值打印，例如下面，需要将printf换成你的打印函数：
```c
#define EB_DEBUG_PRINTF printf
```
![alt text](key_value_log.png)
<br></details>

## ? 其他
- 本项目基于本人实际开发中遇到的一些按键驱动使用体验问题，在他人项目（见参考链接）的思想基础上，开发的此按键驱动模块，之前提到了本模块的优势，下面说下有待改进的地方：对于多按键时组合按键的表示方式，目前还没有想到比较优雅的实现方式，后续有头绪后会进一步改进，补齐这一环。最后，感谢帮助思考我的小伙伴[shawnfeng0](https://github.com/shawnfeng0)以及正在使用此模块的小伙伴，欢迎一起开发改进！
- 更多高级用法见 [examples](../examples/README.md)

## ? 参考链接
- [MultiButton](https://github.com/0x1abin/MultiButton)
- [FlexibleButton](https://github.com/murphyzhao/FlexibleButton/tree/master)
- [安富莱按键FIFO思想](https://www.armbbs.cn/forum.php?mod=viewthread&tid=111527&highlight=%B0%B4%BC%FC)
