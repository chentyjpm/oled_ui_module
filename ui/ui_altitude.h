#ifndef BSP_UI_ALTITUDE_H
#define BSP_UI_ALTITUDE_H
#include "ui_view.h"
#include "app_type.h"








struct ui_viewaltitude_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
	int32_t 	value;
	uint8_t		isvalid;
};



extern const struct ui_view_op_t ui_viewaltitude_op;

#endif


