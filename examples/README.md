# 高级用法示例

- 大于等于n连击按n连击处理,见[example_callback.c](./example_callback.c)中的``quintuple_click_handle``函数和其匹配规则；
- 连击事件如果只想通过一个回调处理,不想每一个连接都注册一个回调,见[example_callback.c](./example_callback.c)中``repeat_click_handle``函数和其匹配规则;
- 键值以长按处理就会触发的事件（包括短按长按、单纯长按）,见[example_callback.c](./example_callback.c)中``filter_ending_with_long_press_handle``函数和其匹配规则；
