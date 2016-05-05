#ifndef BSP_UI_ALTITUDE_H
#define BSP_UI_ALTITUDE_H
#include "ui_view.h"
#include "app_type.h"






struct ui_viewvalue_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
	uint16_t	value;
	uint8_t		valuediv;
	uint8_t 	fontsize;
	uint8_t		valuesize;
};



extern const struct ui_view_op_t ui_viewvalue_op;

#endif


