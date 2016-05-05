#ifndef BSP_UI_BTN_H
#define BSP_UI_BTN_H
#include "ui_view.h"
#include "app_type.h"




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



extern const struct ui_view_op_t ui_viewbtn_op;

#endif


