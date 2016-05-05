#ifndef BSP_UI_H
#define BSP_UI_H

#include "ui_view.h"
#include "ansichar.h"
#include "gb2312.h"
#include "string.h"

vsf_err_t 	ui_viewdummy_init(void *param)
{
	return VSFERR_NONE;
}

vsf_err_t 	ui_viewdummy_fini(void *param)
{
	return VSFERR_NONE;
}

struct ui_view_layout_t* 	ui_viewdefault_onlayout(void *param)
{
	struct ui_view_param_t *viewbasic = (struct ui_view_param_t *)param;
	
	return &viewbasic->layout;
}

vsf_err_t ui_view_draw_ansismall(uint8_t *line1, uint8_t size, char *str)
{
	uint8_t *dpptr = line1;
	uint32_t value;
	
	if(strlen(str)*6 > size)
		return VSFERR_FAIL;
	
	while(*str != 0)
	{
		uint8_t *ptr;
		value = (*str - 0x20)*5;
		if(value < sizeof(ansichar_table))
		{
			ptr = (uint8_t *)&ansichar_table[value];
		}
		else
		{
			ptr = (uint8_t *)&ansichar_table[(' ' - 0x20) * 5];
		}
		
		memcpy(dpptr, ptr, 5);
		dpptr[5] = 0;
		dpptr += 6;
		str++;
	}
	
	return VSFERR_NONE;
}

vsf_err_t ui_view_draw_ansibig(uint8_t *line1, uint8_t *line2, uint8_t size, char *str)
{
	uint8_t *dpptr = line1;
	uint8_t *dpptr2 = line2;
	
	uint32_t value;
	
	if(strlen(str)*8 > size)
		return VSFERR_FAIL;
	
	while(*str != 0)
	{
		char *ptr;
		value = (*str - 0x20)*16;
		if(value < sizeof(ansichar_table_big))
		{
			ptr = (char *)&ansichar_table_big[value];
			
			{
				uint8_t i;
				for(i = 0 ; i < 8 ; i++)
				{
					*dpptr = *ptr;
					dpptr++;
					
					
					*dpptr2 = *(ptr + 8);
					dpptr2++;
					
					ptr++;
				}
			}
			str++;
		}
		else
		{
			//this is a gb2312 char
			{
				uint8_t i;
				const struct gb16_t *gbchar;
				ptr = (char *)&ansichar_table_big[(' ' - 0x20) * 16];
				for(i = 0 ; i < (sizeof(gb2312_table)/sizeof(struct gb16_t)) ; i++)
				{
					gbchar = &gb2312_table[i];
					if((gbchar->code[0] == *str)  && (gbchar->code[1] == *(str + 1)))
					{
						//march
						ptr = (char *)gbchar->dat;
						{
							uint8_t i;
							for(i = 0 ; i < 16 ; i++)
							{
								*dpptr = *ptr;
								dpptr++;
					
					
								*dpptr2 = *(ptr + 16);
								dpptr2++;
					
								ptr++;
							}
						}
						str+=2;
						
						break;
					}
				}
			}
		}
	}
	
	return VSFERR_NONE;
}

vsf_err_t ui_view_draw_ansiubig(uint8_t *line1, uint8_t *line2, uint8_t *line3, uint8_t size, char *str)
{
	uint8_t *dpptr = line1;
	uint8_t *dpptr2 = line2;
	uint8_t *dpptr3 = line3;
	
	uint32_t value;
	
	if(strlen(str)*12 > size)
		return VSFERR_FAIL;
	
	while(*str != 0)
	{
		uint8_t *ptr;
		value = (*str - 0x20)*36;
		if(value < sizeof(ansichar_table_ubig))
		{
			ptr = (uint8_t *)&ansichar_table_ubig[value];
		}
		else
		{
			ptr = (uint8_t *)&ansichar_table_ubig[(' ' - 0x20) * 36];
		}
		
		{
			uint8_t i;
			for(i = 0 ; i < 12 ; i++)
			{
				*dpptr = *ptr;
				dpptr++;
				*dpptr2 = *(ptr + 12);
				dpptr2++;
				*dpptr3 = *(ptr + 24);
				dpptr3++;
				ptr++;
			}
		}
		str++;
	}
	
	return VSFERR_NONE;
}

void ui_view_inventbuf(struct ui_view_buf_t *viewbuf)
{
	uint8_t i,j;
	
	for( j = 0 ; j < viewbuf->sizey ; j++)
	{
		uint8_t *ptr = viewbuf->linebuf[j];
		for( i = 0 ; i < viewbuf->sizex ; i ++)
		{
			*ptr = ~*ptr;
			ptr++;
		}
	}
}

vsf_err_t 	ui_viewstr_ondraw(void *param, struct ui_view_buf_t *viewbuf)
{
	struct ui_viewstr_param_t *viewstr = (struct ui_viewstr_param_t *)param;
	vsf_err_t err;
	if(!(viewstr->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	if(viewstr->font == UI_VIEW_STRUBIG)
	{	
		err = ui_view_draw_ansiubig(viewbuf->linebuf[0], viewbuf->linebuf[1], viewbuf->linebuf[2], viewbuf->sizex, viewstr->str);
	}
	else if(viewstr->font == UI_VIEW_STRBIG)
	{
		err = ui_view_draw_ansibig(viewbuf->linebuf[0], viewbuf->linebuf[1], viewbuf->sizex, viewstr->str);
	}
	else if(viewstr->font == UI_VIEW_STRSMALL)
		err = ui_view_draw_ansismall(viewbuf->linebuf[0], viewbuf->sizex, viewstr->str);
	else
		err = VSFERR_FAIL;
	
	if(err != VSFERR_NONE)
		return err;
	
	if((viewstr->flags & UI_VIEW_FLAG_FOCUSEABLE)
		&&(viewstr->flags & UI_VIEW_FLAG_ISFOCUSED))
	{
		ui_view_inventbuf(viewbuf);
	}
	
	return VSFERR_NONE;
}

vsf_err_t 	ui_viewimg_ondraw(void *param,  struct ui_view_buf_t *viewbuf)
{
	struct ui_viewimg_param_t *viewimg = (struct ui_viewimg_param_t *)param;
	char *imgptr = viewimg->img;
	
	if(!(viewimg->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	if((viewbuf->sizex * viewbuf->sizey) < viewimg->imgsize)
		return VSFERR_FAIL;
	{
		uint8_t i;
		for( i = 0 ; i < viewimg->layout.ysize ; i++)
		{
			memcpy(viewbuf->linebuf[viewimg->layout.ysize - i - 1], imgptr, viewimg->layout.xsize);
			imgptr += viewimg->layout.xsize;
		}
	}

	
	if((viewimg->flags & UI_VIEW_FLAG_FOCUSEABLE)
		&&(viewimg->flags & UI_VIEW_FLAG_ISFOCUSED))
	{
		ui_view_inventbuf(viewbuf);
	}
	
	return VSFERR_NONE;
}


vsf_err_t	ui_viewdefault_onfocus(void *param, uint8_t isfocused)
{
	struct ui_view_param_t *viewbasic = (struct ui_view_param_t *)param;
	if(!(viewbasic->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_FAIL;
	
	if(viewbasic->flags & UI_VIEW_FLAG_FOCUSEABLE)
	{
		if(isfocused)
			viewbasic->flags = viewbasic->flags | UI_VIEW_FLAG_ISFOCUSED;
		else
			viewbasic->flags = viewbasic->flags & (~UI_VIEW_FLAG_ISFOCUSED);
		return VSFERR_NONE;
	}
	return VSFERR_FAIL;
}

vsf_err_t	ui_viewdefault_onkey(void *param, uint8_t key)
{
	struct ui_view_param_t *viewbasic = (struct ui_view_param_t *)param;
	if(!(viewbasic->flags & UI_VIEW_FLAG_ENABLE))
		return VSFERR_NONE;
	
	return VSFERR_NONE;
}

const struct ui_view_op_t ui_viewstr_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewstr_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};

const struct ui_view_op_t ui_viewimg_op = 
{
	ui_viewdummy_init,
	ui_viewdummy_fini,
	ui_viewdefault_onlayout,
	ui_viewimg_ondraw,
	ui_viewdefault_onfocus,
	ui_viewdefault_onkey
};



#endif
