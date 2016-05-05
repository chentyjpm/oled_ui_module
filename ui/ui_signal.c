

#include "string.h"
#include "ui_signal.h"
#include "stdio.h"

#define UI_VIEWTARGET_SIZEX		48
#define UI_VIEWTARGET_STRPOS	20

const uint8_t signallevelchar[9] = 
{
	0 ,0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF
};

//level from 0 - 255
void ui_viewtarget_draw_signalbar(struct ui_view_buf_t *viewbuf, uint8_t level)
{
	uint8_t i;
	//draw rect
	for( i = 0 ; i < 6 ; i ++)
	{
		viewbuf->linebuf[i][0] = 0xFF;
		viewbuf->linebuf[i][11] = 0xFF;
	}
	
	viewbuf->linebuf[0][1] = 0x01;
	viewbuf->linebuf[0][10] = 0x01;
	for( i = 1 ; i < 5 ; i ++)
	{
		viewbuf->linebuf[i][1] = 0;
		viewbuf->linebuf[i][10] = 0;
	}
	viewbuf->linebuf[5][1] = 0x80;
	viewbuf->linebuf[5][10] = 0x80;
	
	//draw signal
	//signal level = 0 - 48
	{
		uint16_t signallevel = (level * 48) / 100;
		
		if(signallevel > 8)
		{
			memset(&viewbuf->linebuf[5][2], 0xFF & 0xBF, 8);
			signallevel -= 8;
		}
		else
		{
			memset(&viewbuf->linebuf[5][2], (signallevelchar[signallevel] & 0xBF) | 0x80, 8);	
			signallevel = 0;
		}
		
		for( i = 4 ; i > 0 ; i --)
		{
			if(signallevel > 8)
			{
				memset(&viewbuf->linebuf[i][2], 0xFF, 8);
				signallevel -= 8;
			}
			else
			{
				memset(&viewbuf->linebuf[i][2], signallevelchar[signallevel], 8);
				signallevel = 0;
			}
		}
		
		if(signallevel > 8)
		{
			memset(&viewbuf->linebuf[0][2], 0xFF & 0xFD, 8);
			signallevel -= 8;
		}
		else
		{
			memset(&viewbuf->linebuf[0][2], (signallevelchar[signallevel] & 0xFD) | 0x01, 8);	
			signallevel = 0;
		}
	}
	
	
}

vsf_err_t 	ui_viewtarget_ondraw(void *param,  struct ui_view_buf_t *viewbuf)
{
	struct ui_viewsignal_param_t *view = (struct ui_viewsignal_param_t *)param;
	if(!(view->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	//draw signalbar
	ui_viewtarget_draw_signalbar(viewbuf, view->value);
	
	//draw string
	{
		char tmpstr[12];
		if(view->value < 100)
			sprintf(tmpstr, "%02d%%", view->value);
		else
			strcpy(tmpstr, "99%");
		ui_view_draw_ansiubig(viewbuf->linebuf[1] + UI_VIEWTARGET_STRPOS, 
							viewbuf->linebuf[2] + UI_VIEWTARGET_STRPOS, 
							viewbuf->linebuf[3] + UI_VIEWTARGET_STRPOS, 
							viewbuf->sizex - UI_VIEWTARGET_STRPOS, tmpstr);
		
		ui_view_draw_ansibig(viewbuf->linebuf[4] + UI_VIEWTARGET_STRPOS, 
							viewbuf->linebuf[5] + UI_VIEWTARGET_STRPOS, 
							viewbuf->sizex - UI_VIEWTARGET_STRPOS, "пе╨е");
		
	}
	
	return VSFERR_NONE;
	
}

const struct ui_view_op_t ui_viewsignal_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewtarget_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};


