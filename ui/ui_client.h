#ifndef BSP_UI_CLIENT_H
#define BSP_UI_CLIENT_H
#include "ui_view.h"
#include "app_type.h"


struct ui_viewclient_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
	struct spidersens_client_t *client;
};



extern const struct ui_view_op_t ui_viewclient_op;

#endif


