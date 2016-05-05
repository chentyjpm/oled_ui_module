

#include "string.h"
#include "ui_client.h"

#define UI_VIEWCLIENT_SIZEX		24
#define UI_VIEWCLIEMT_STRPOS	10

vsf_err_t 	ui_viewclient_ondraw(void *param, struct ui_view_buf_t *dpbuf)
{

	return VSFERR_NONE;
	
}

const struct ui_view_op_t ui_viewclient_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewclient_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};


