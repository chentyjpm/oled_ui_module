

#include "string.h"
#include "ui_battery.h"
#include "ui_img.h"

vsf_err_t 	ui_viewbattery_ondraw(void *param,  struct ui_view_buf_t *viewbuf)
{
	struct ui_viewbattery_param_t *view = (struct ui_viewbattery_param_t *)param;
	uint8_t *line1 = viewbuf->linebuf[0];
	uint8_t *line2 = viewbuf->linebuf[1];
	uint16_t batlevel;
	uint8_t i;
	if(!(view->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	if((view->flags & UI_VIEW_FLAG_FOCUSEABLE)
		&&(view->flags & UI_VIEW_FLAG_ISFOCUSED))
	{
		view->flashtick ++;
		if(view->flashtick & 0x01)
		{
			uint8_t i;
			char *imgptr = (char *)gImage_set;
			for( i = 0 ; i < view->layout.ysize ; i++)
			{
				memcpy(viewbuf->linebuf[view->layout.ysize - i - 1], imgptr, view->layout.xsize);
				imgptr += view->layout.xsize;
			}
			
		}
	}
	else
	{
		batlevel = (view->level * view->layout.xsize)/100;
		batlevel = view->layout.xsize - batlevel;
		//head
		line1[0] = 0xF0;
		line1[1] = 0xF0;
		line2[0] = 0x03;
		line2[1] = 0x03;
		
		line1[2] = 0xFF;
		line2[2] = 0x3F;
		
		line1[3] = 0x01;
		line2[3] = 0x20;
		
		for( i = 4 ; i < view->layout.xsize - 2 ; i ++)
		{
			if(i < batlevel)
			{
				//drawempty
				line1[i] = 0x01;
				line2[i] = 0x20;
			}
			else
			{
				//drawfull
				line1[i] = 0xFD;
				line2[i] = 0x2F;
			}
		}
		
		line1[view->layout.xsize - 2] = 0x01;
		line2[view->layout.xsize - 2] = 0x20;
		line1[view->layout.xsize - 1] = 0xFF;
		line2[view->layout.xsize - 1] = 0x3F;
	}
	return VSFERR_NONE;
	
}

const struct ui_view_op_t ui_viewbattery_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewbattery_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};


