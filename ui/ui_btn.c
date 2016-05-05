

#include "string.h"
#include "ui_btn.h"



vsf_err_t	ui_viewbtn_onkey(void *param, uint8_t evt)
{
	struct ui_viewbtn_param_t *viewbtn = (struct ui_viewbtn_param_t *)param;
	if(!(viewbtn->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	if(!(viewbtn->flags & UI_VIEW_FLAG_ISFOCUSED))
		return VSFERR_NONE;
	
	if(evt == UI_VIEW_EVT_CLICK)
	{
		if(!(viewbtn->flags & UI_VIEW_FLAG_CLICKABLE))
			return VSFERR_NONE;
		
		if(viewbtn->onclick != NULL)
			viewbtn->onclick(viewbtn->onclickparam);
		return VSFERR_NONE;
	}		
	
	return VSFERR_NONE;
}

vsf_err_t 	ui_viewbtn_ondraw(void *param,  struct ui_view_buf_t *viewbuf)
{
	struct ui_viewbtn_param_t *viewbtn = (struct ui_viewbtn_param_t *)param;
	vsf_err_t err;
	
	if(!(viewbtn->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	
	if (viewbtn->img != NULL)
	{
		char *imgptr = viewbtn->img;
		if ((viewbuf->sizex * viewbuf->sizey) < viewbtn->imgsize)
			return VSFERR_FAIL;
		{
			uint8_t i;
			for( i = 0 ; i < viewbtn->layout.ysize ; i++)
			{
				memcpy(viewbuf->linebuf[viewbtn->layout.ysize - i - 1], imgptr, viewbtn->layout.xsize);
				imgptr += viewbtn->layout.xsize;
			}
		}
		err = VSFERR_NONE;
	}
	else
	if (viewbtn->str != NULL)
	{
		if(viewbtn->strfont == UI_VIEW_STRUBIG && viewbtn->layout.ysize > 2)
		{	
			err = ui_view_draw_ansiubig(viewbuf->linebuf[0], viewbuf->linebuf[1], viewbuf->linebuf[2], viewbuf->sizex, viewbtn->str);
		}
		else if(viewbtn->strfont == UI_VIEW_STRBIG && viewbtn->layout.ysize > 1)
		{
			err = ui_view_draw_ansibig(viewbuf->linebuf[0], viewbuf->linebuf[1], viewbuf->sizex, viewbtn->str);
		}
		else if(viewbtn->strfont == UI_VIEW_STRSMALL)
			err = ui_view_draw_ansismall(viewbuf->linebuf[0], viewbuf->sizex, viewbtn->str);
		else
			err = VSFERR_FAIL;
	}

	
	if((viewbtn->flags & UI_VIEW_FLAG_FOCUSEABLE)
		&&(viewbtn->flags & UI_VIEW_FLAG_ISFOCUSED))
	{
		viewbtn->flashtick ++;
		if(viewbtn->flashtick & 0x01)
			ui_view_inventbuf(viewbuf);
	}
	
	return err;
}

const struct ui_view_op_t ui_viewbtn_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewbtn_ondraw,
	ui_viewdefault_onfocus,
	ui_viewbtn_onkey
};



