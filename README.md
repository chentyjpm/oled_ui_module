oled_ui_module
===

oled_ui_module software framework
create by chenty


这是我写的一个ui支持库

协议主要用于 类似 5110 这种单色 8位的 lcd 或 OLED的显示驱动

ui库的设计比较简单 原理是把一个屏幕划分成一个一个的控件 然后挂载不同的控件实现显示以及不同的功能

去除了和上层以及底层的关联 模块化独立出来 以便移植和使用

目前实现的控件有3个

str	文本控件
img 	图片控件
btn	按钮控件

所有的显示控件派生于相同的基类 view
struct ui_view_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
};

控件实现以下方法
struct ui_view_op_t
{
	vsf_err_t 			(*init)(void *param);
	vsf_err_t 			(*fini)(void *param);
	struct ui_view_layout_t* 	(*onlayout)(void *param);
	vsf_err_t 			(*ondraw)(void *param, struct ui_view_buf_t *viewbuf);
	vsf_err_t			(*onfocus)(void *param, uint8_t isfocused);
	vsf_err_t 			(*onevt)(void *param, uint8_t evt);
};

ui库通过调用不同的方法来实现控件的显示 聚焦 点击


ui库的使用方法从下

首先需要提供的配置参数

显示区域长度
#define DISP_SIZE_H		128
显示区域行数 对于oled 或者 5510这种单色屏幕
#define DISP_SIZE_V		8
控件最大数量
#define UI_CELL_MAX		20

按键宏 
菜单键键值
#define KEY_VALUE_MENU			KEY_VALUE_KEY0
确定键键值
#define KEY_VALUE_ENTER			KEY_VALUE_KEY1




首先需要定义 ui使用的空间 一般单片机环境 就初始化静态结构体即可
如 struct ui_param_t ui;

这是ui模块提供调用的方法
struct ui_op_t
{
	vsf_err_t 	(*init)(struct ui_param_t *param);
	vsf_err_t 	(*fini)(struct ui_param_t *param);
	vsf_err_t 	(*ondisp)(struct ui_param_t *param, struct vsf_buffer_t *dpbuf);
	vsf_err_t 	(*onkey)(struct ui_param_t *param, uint8_t key);	
	vsf_err_t	(*addview)(struct ui_param_t *param, void *view, const struct ui_view_op_t *type);
	vsf_err_t	(*delview)(struct ui_param_t *param, void *view);
};


首先需要对ui模块初始化
ui_op.init(&ui);

然后ui 模块就可以使用了
这时没有任何控件 所以需要 注册控件给ui模块
控件可以为常量 也可以为变量 为了节省内存 不变的显示内容可以存储在flash里面

例如 文本控件
const struct ui_viewstr_param_t bootstring = 
{
	{0, 6, 128, 2}, UI_VIEW_FLAG_ENABLE,
	"正在开机" ,UI_VIEW_STRBIG
};

控件的注册需要提供2个参数 一个是控件本身的结构体 一个是控件的方法 这个地方应该有改进的控件 初期设计是这样做的
ui_op.addview(&ui, (void *)&bootstring, &ui_viewstr_op);

这时控件就注册好了
如何显示出来呢

需要如下步骤 
首先需要提供一个可以显示区域的内存
通过struct vsf_buffer_t 结构体传入
显示内存空间需要安装 0 1 2 3 行顺序排布 （目前）

每次调用 ui_op.ondisp(&ui, &dpbuf); 会刷新一次显示 在btn 控件中 这个刷新速度就是focus闪烁的速度

建议用法是 在定时器中循环调用 同时在更新内容时 直接调用


出现按键事件是 需要将按键evt 传递给ui模块 ui会自动做出切换focus 或者 调用控件 click事件等操作

方法是 ui_op.onkey(&ui, keyvalue);


当界面关闭 或者切换时 有2种方式 一种可以把控件DISABLE 一种是del控件

del控件是这样操作的
ui_op.delview(&ui, (void *)&bootstring);

当不需要ui释放内存时调用 
ui_op.fini(&ui);

动态分配的内存ui不会释放需要根据系统环境调用相应的函数释放内存

Todo:控件使用详解





