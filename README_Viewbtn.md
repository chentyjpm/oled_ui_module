#控件Viewbtn

	struct ui_viewbtn_param_t
	{
		struct ui_view_layout_t layout;
		uint32_t				flags;
		char 					*img;
		uint32_t				imgsize;
		char 					*str;
		uint8_t					strfont;
		void 					(*onclick)(void *param);
		void  					*onclickparam;
		uint32_t				flashtick;
	};


btn控件比img和str相对要复杂一些

btn控件即支持显示一个图片也支持显示一个文件

原则是 

有图片就显示图片

图片指针不为NULL 则显示图片

否则就显示文字 

如果文字都没有。。。。好吧那就是白的

下面是 按下的回调函数 和回调函数参数
这个函数在 控件被按下时会调用 同时会传递 参数给回调函数

最后一个flashtick 是用来闪烁的不需要初始化

示例：

	static void ui_fac_softupgrade(void *param)
	{
		sys_op.enterdfu();
	}

	struct ui_viewbtn_param_t softupgradebtn =
	{
		.layout = {0, 4, 64, 2},
		.flags = UI_VIEW_FLAG_ENABLE | UI_VIEW_FLAG_FOCUSEABLE | UI_VIEW_FLAG_CLICKABLE,
		.str = "固件升级",
		.strfont = UI_VIEW_STRBIG,
		.onclick = ui_fac_softupgrade,
	};

	ui_op.addview(&ui, (void *)&softupgradebtn, &ui_viewbtn_op);

注意：

能够被选中的控件不可以初始化为常量 必须是变量
