

#include "string.h"
#include "ui_value.h"


const struct ui_view_op_t ui_viewvalue_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewdefault_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};


