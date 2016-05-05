#ifndef BSP_UI_SIGHAL_H
#define BSP_UI_SIGHAL_H
#include "ui_view.h"
#include "app_type.h"








struct ui_viewsignal_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
	uint16_t	value;
};



extern const struct ui_view_op_t ui_viewsignal_op;

#endif


