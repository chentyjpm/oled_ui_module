#ifndef BSP_UI_MENU_H
#define BSP_UI_MENU_H

#include "ui_view.h"

struct ui_menu_item_t
{
	struct ui_menu_item_t 	*next;
	void 					*param;
	vsf_err_t 				(*onevt)(void *param, uint8_t evt);
	uint8_t					type;
	uint8_t					active;
	uint8_t					sizex;
	uint8_t					sizey;
};

struct ui_menu_list_t
{
	struct ui_menu_item_t *head;
	struct ui_menu_item_t *cur;
};

struct ui_menu_param_t
{
	struct ui_view_layout_t layout;
	struct ui_menu_list_t list;
};



#endif
