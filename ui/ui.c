
#include "bspconfig.h"

#include "ui.h"
#include "string.h"

#define UI_REFREACH_TICK	10

vsf_err_t ui_init(struct ui_param_t *param)
{
	memset(param, 0, sizeof(struct ui_param_t));
	return VSFERR_NONE;
}


vsf_err_t ui_ondisp(struct ui_param_t *param, struct vsf_buffer_t *dpbuf)
{
	struct ui_view_layout_t *layout;
	struct ui_cell_t *cell;
	struct ui_view_buf_t viewbuf;
	uint8_t cnt,i;
	
	for(cnt = 0 ; cnt < UI_CELL_MAX ; cnt++)
	{
		cell = &param->cell[cnt];
		if(cell->ref)
		{
			const struct ui_view_op_t *op = cell->op;
			
			if(op->ondraw == NULL||op->onlayout == NULL)
				return VSFERR_FAIL;
			
			layout = op->onlayout(cell->param);
			
			//check is valid
			if(layout->xpos + layout->xsize > DISP_SIZE_H
				||layout->ypos + layout->ysize > DISP_SIZE_V)
				return VSFERR_FAIL;
			
			//getviewbuf
			viewbuf.sizex = layout->xsize;
			viewbuf.sizey = layout->ysize;
			for(i = 0 ; i < layout->ysize ; i++)
			{
				
				viewbuf.linebuf[i] = dpbuf->buffer + layout->xpos + (layout->ypos + i) * DISP_SIZE_H;
				//clear buf
				memset(viewbuf.linebuf[i], 0, layout->xsize);
			}
			
			if(op->ondraw(cell->param, &viewbuf) != VSFERR_NONE)
				return VSFERR_FAIL;
		}
	}
		
	return VSFERR_NONE;
}

vsf_err_t ui_onkey(struct ui_param_t *param, uint8_t key)
{
	struct 	ui_cell_t *view;
	uint8_t	curfocus = param->focus;
	
	if(!key)
		return VSFERR_NONE;
	
	view = &param->cell[curfocus];
	
	if (key == KEY_VALUE_MENU)
	{
		//clear current focus
		view->op->onfocus(view->param, false);
		
		do
		{
			param->focus++;
			if (param->focus >= UI_CELL_MAX)
			{
				param->focus = 0;
				return VSFERR_NONE;
			}
			view = &param->cell[param->focus];
			if (view->ref != 0)
			{
				if(view->op->onfocus(view->param, true) == VSFERR_NONE)
					return VSFERR_NONE;
			}
			
		}while(param->focus != curfocus);
		return VSFERR_FAIL;
	}
	else
	if (key == KEY_VALUE_ENTER)
	{
		view->op->onevt(view->param, UI_VIEW_EVT_CLICK);
		return VSFERR_NONE;
	}
	return VSFERR_NONE;
}

vsf_err_t	ui_addview(struct ui_param_t *param, void *view, const struct ui_view_op_t *type)
{
	uint8_t cnt;
	for(cnt = 0 ; cnt < UI_CELL_MAX ; cnt++)
	{
		if(param->cell[cnt].ref == 0)
		{
			struct ui_cell_t *cell = &param->cell[cnt];
			cell->op = type;
			cell->param = view;
			cell->ref++;
			return VSFERR_NONE;
		}
	}
	return VSFERR_FAIL;
}

vsf_err_t	ui_delview(struct ui_param_t *param, void *view)
{
	uint8_t cnt;
	for(cnt = 0 ; cnt < UI_CELL_MAX ; cnt++)
	{
		if(param->cell[cnt].param == view)
		{
			param->cell[cnt].ref = 0;
			return VSFERR_NONE;
		}
	}
	return VSFERR_FAIL;
}


const struct ui_op_t ui_op = 
{
	ui_init,
	ui_init,
	ui_ondisp,
	ui_onkey,
	ui_addview,
	ui_delview
};
