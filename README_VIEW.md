基础控件View的结构

View控件是整个ui体系的基础
参考android的实现方式简化而来

	struct ui_view_layout_t
	{
		uint8_t xpos;
		uint8_t ypos;
		uint8_t xsize;
		uint8_t ysize;
	};

	enum ui_view_flags
	{
		UI_VIEW_FLAG_ENABLE = 1,
		UI_VIEW_FLAG_FOCUSEABLE = 2,
		UI_VIEW_FLAG_ISFOCUSED = 4,
		UI_VIEW_FLAG_CLICKABLE = 8
	};

	enum ui_view_evts
	{
		UI_VIEW_EVT_CLICK = 1,
		UI_VIEW_EVT_DOWN = 2,
	};

	struct ui_view_param_t
	{
		struct ui_view_layout_t layout;
		uint32_t	flags;
	
	};

	struct ui_view_op_t
	{
		vsf_err_t 					(*init)(void *param);
		vsf_err_t 					(*fini)(void *param);
		struct ui_view_layout_t* 	(*onlayout)(void *param);
		vsf_err_t 					(*ondraw)(void *param, struct ui_view_buf_t *viewbuf);
		vsf_err_t					(*onfocus)(void *param, uint8_t isfocused);
		vsf_err_t 					(*onevt)(void *param, uint8_t evt);
	};

上面是一个view的组成

layout 结构表示view的位置 因为对于单色OLED 和LCD一般尺寸都很小这里为了节约内存使用了uint8位

x 坐标
y 表示行号
Xsize X长度
Ysize Y的占用行数

对于一个View其占用空间不能比内容大否则会导致出错

对于一个View 除了位置之外还有一个flag的选项
用于表示这个View的状态
ENABLE的View才会被显示 否则会被忽略
FOCUSEABLE的View可以被选中 否则会被跳过
ISFOCUSED 表示正在被选中
CLICKABLE 表示View可以被按下

View组成就是这个2个选项

然后做为一个View 需要实现这几个方法

init 初始化 在addview时调用

fini 析构	在delview时调用

onlayout	每一个view会有一个layout过程用于实现一些动画或者 后期可能会做得自动布局

ondraw		这个函数需要实现view的绘制 使用传入的viewbuf 填充view的内容

onfocus		这个函数在View被选中/取消时调用 可以对view本身的显示做相应处理

onevt		这个函数目前是处理按下事件的 当然可以扩展处理更多个事件的情况

