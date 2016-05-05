

#include "string.h"
#include "ui_altitude.h"
#include "stdio.h"

#define UI_VIEWALTITUDE_SIZEX		72
#define UI_VIEWALTITUDE_STRPOS		16


const char gImage_arrowup[48] = { /* 0X42,0X01,0X10,0X00,0X18,0X00, */
0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0X7F,0X7F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X38,0X3C,0X1E,0X0F,0X07,0XFF,0XFF,0XFF,0X07,0X0F,0X1E,0X3C,0X38,0X00,0X00,
0X0E,0X0E,0X0E,0X0E,0X0E,0X8E,0XCE,0XCE,0XCE,0X8E,0X0E,0X0E,0X0E,0X0E,0X0E,0X00,
};

const char gImage_arrowno[48] = { /* 0X42,0X01,0X10,0X00,0X18,0X00, */
0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X00,
};

const char gImage_arrow_littleup[48] = { /* 0X42,0X01,0X10,0X00,0X18,0X00, */
0X72,0X73,0X73,0X73,0X73,0X7F,0X7F,0X7F,0X7F,0X7F,0X73,0X73,0X73,0X73,0X72,0X00,
0X00,0X00,0X80,0XC0,0XE0,0XF0,0XF8,0XFC,0XF8,0XF0,0XE0,0XC0,0X80,0X00,0X00,0X00,
0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X0E,0X00,
};

const char gImage_arrow_littledown[48] = { /* 0X42,0X01,0X10,0X00,0X18,0X00, */
0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X70,0X00,
0X00,0X00,0X01,0X03,0X07,0X0F,0X1F,0X3F,0X1F,0X0F,0X07,0X03,0X01,0X00,0X00,0X00,
0X4E,0XCE,0XCE,0XCE,0XCE,0XFE,0XFE,0XFE,0XFE,0XFE,0XCE,0XCE,0XCE,0XCE,0X4E,0X00,
};


const char gImage_arrowdown[48] = { /* 0X42,0X01,0X10,0X00,0X18,0X00, */
0X70,0X70,0X70,0X70,0X70,0X71,0X73,0X73,0X73,0X71,0X70,0X70,0X70,0X70,0X70,0X00,
0X00,0X1C,0X3C,0X78,0XF0,0XE0,0XFF,0XFF,0XFF,0XE0,0XF0,0X78,0X3C,0X1C,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0XFE,0XFE,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

//level from 0 - 255

void ui_viewaltitude_draw_arrow(struct ui_view_buf_t *viewbuf, const char *img)
{
	memcpy(viewbuf->linebuf[1], img + 32, 16);
	memcpy(viewbuf->linebuf[2], img + 16, 16);
	memcpy(viewbuf->linebuf[3], img + 0, 16);
	
}

vsf_err_t 	ui_viewaltitude_ondraw(void *param,  struct ui_view_buf_t *viewbuf)
{
	struct ui_viewaltitude_param_t *view = (struct ui_viewaltitude_param_t *)param;
	int16_t err_altitude;
	if(!(view->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	if(view->isvalid)
	{
	
		err_altitude = view->value;//client->altitude - local->altitude;
		//draw arrow
		if(err_altitude > 0)
			if(err_altitude > ALTITUDE_FLOOR)	
				ui_viewaltitude_draw_arrow(viewbuf, gImage_arrowup);
			else
				ui_viewaltitude_draw_arrow(viewbuf, gImage_arrow_littleup);
		else
		{
			if(err_altitude < - ALTITUDE_FLOOR)
				ui_viewaltitude_draw_arrow(viewbuf, gImage_arrowdown);
			else
				ui_viewaltitude_draw_arrow(viewbuf, gImage_arrow_littledown);
		}
		
		if(err_altitude < 0)
			err_altitude = - err_altitude;
		//draw string
		{
			char tmpstr[12];
			if(err_altitude < 100)
			{
				int8_t meter;
				int8_t point;
				meter = err_altitude/10;
				point = err_altitude - meter * 10;
					
				sprintf(tmpstr, "%01d.%01dm", meter, point);
			}
			else
			{
				sprintf(tmpstr, "%03dm", err_altitude/10);
			}
			ui_view_draw_ansiubig(viewbuf->linebuf[1] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->linebuf[2] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->linebuf[3] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->sizex - UI_VIEWALTITUDE_STRPOS, tmpstr);
			
			ui_view_draw_ansibig(viewbuf->linebuf[4] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->linebuf[5] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->sizex - UI_VIEWALTITUDE_STRPOS, "高度差");
			
		}
	}
	else
	{
		//no client
		//draw string
		{

			ui_view_draw_ansiubig(viewbuf->linebuf[1], 
								viewbuf->linebuf[2], 
								viewbuf->linebuf[3], 
								viewbuf->sizex, "  ---");
			
			ui_view_draw_ansibig(viewbuf->linebuf[4] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->linebuf[5] + UI_VIEWALTITUDE_STRPOS, 
								viewbuf->sizex - UI_VIEWALTITUDE_STRPOS, "高度差");
		}
	}
		
	
	return VSFERR_NONE;
	
}

const struct ui_view_op_t ui_viewaltitude_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewaltitude_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};


