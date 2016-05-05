#ifndef BSP_UI_BATTERY_H
#define BSP_UI_BATTERY_H
#include "ui_view.h"
#include "app_type.h"








struct ui_viewbattery_param_t
{
	struct ui_view_layout_t layout;
	uint32_t				flags;
	uint8_t					level;
	uint8_t					flashtick;
};



extern const struct ui_view_op_t ui_viewbattery_op;

#endif


