#控件Viewimg Viewstr
##Viewimg控件
Viewimg控件是一个显示图片的控件
主要功能是显示一个图片

	struct ui_viewimg_param_t
	{
		struct ui_view_layout_t layout;
		uint32_t				flags;
		char 					*img;
		uint32_t 				imgsize;
	};

为了简化设计
图片目前只能使用数组方式存储
图片控件比较简单

前面的布局参数请参考 文档 基础View

转换图片为数组
可以使用Image2Lcd来转换
填写指针和长度即可 

例如

	const struct ui_viewimg_param_t logo = 
	{
		{0, 0, 64, 6}, UI_VIEW_FLAG_ENABLE,
		(char *)gImage_logo, sizeof(gImage_logo)
	};

定义完成后 只需要加载时调用内置的img方法 就可以正常显示了

	ui_op.addview(&ui, (void *)&logo, &ui_viewimg_op);

##Viewstr控件
Viewstr控件是一个显示文本的控件
主要功能是显示一个文本

	#define UI_VIEW_STRUBIG			2
	#define UI_VIEW_STRBIG			1
	#define UI_VIEW_STRSMALL		0

	struct ui_viewstr_param_t
	{
		struct ui_view_layout_t layout;
		uint32_t	flags;
		char *str;
		uint8_t font;
	};

英文文本支持3种不同大小的字体 如果你都不喜欢 你可以替换自己想要的字体
中文文本因为毕竟大 所以只支持使用 2行 即 16X8的尺寸做为字库 目前使用是Lcmzimo 这个软件

中文英文允许混合输入 但是因为中文只允许使用 UI_VIEW_STRBIG 所以都是占用2行的

使用也很简单

	const struct ui_viewstr_param_t bootstring = 
	{
		{0, 6, 128, 2}, UI_VIEW_FLAG_ENABLE,
		"正在开机012" ,UI_VIEW_STRBIG
	};

定义完成后 只需要加载时调用内置的img方法 就可以正常显示了

	ui_op.addview(&ui, (void *)&logo, &ui_viewstr_op);

注意:
因为中文字库非常大
目前需要显示的中文需要预先添加到字库里面才可以正常显示
字库文件`gb2312.h`

img和str都支持在运行中修改内容

注意:
目前代码中并没有临界区保护
需要避免操作中出现临界
如在中断中修改图片的长度和文本的长度(strlen)。
尽可能在同一个优先级进行UI的操作 不要超过ui_op.ondisp的优先级






