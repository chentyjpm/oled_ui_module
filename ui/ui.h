#ifndef BSP_UI_H
#define BSP_UI_H

#include "ui_view.h"
	
struct ui_cell_t
{
	void *param;
	const struct ui_view_op_t *op;
	uint8_t	ref;
};

#define UI_CELL_MAX		20

struct ui_param_t
{
	struct ui_cell_t 	cell[UI_CELL_MAX];
	uint8_t				focus;
};

struct ui_op_t
{
	vsf_err_t 	(*init)(struct ui_param_t *param);
	vsf_err_t 	(*fini)(struct ui_param_t *param);
	vsf_err_t 	(*ondisp)(struct ui_param_t *param, struct vsf_buffer_t *dpbuf);
	vsf_err_t 	(*onkey)(struct ui_param_t *param, uint8_t key);	
	vsf_err_t	(*addview)(struct ui_param_t *param, void *view, const struct ui_view_op_t *type);
	vsf_err_t	(*delview)(struct ui_param_t *param, void *view);
};

extern const struct ui_op_t ui_op;
#endif



