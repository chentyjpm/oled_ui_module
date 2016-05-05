#ifndef BSP_UI_VIEW_H
#define BSP_UI_VIEW_H

#include "app_type.h"
#include "component/buffer/buffer.h"
#include "app/spidersens.h"

#define UI_VIEW_MAX_V			6

#define UI_VIEW_STRUBIG			2
#define UI_VIEW_STRBIG			1
#define UI_VIEW_STRSMALL		0

#define UI_SIGNAL_LEVEL_H	200
#define UI_SIGNAL_LEVEL_M	100
#define UI_SIGNAL_LEVEL_L	50

#define ALTITUDE_FLOOR		15




struct ui_view_layout_t
{
	uint8_t xpos;
	uint8_t ypos;
	uint8_t xsize;
	uint8_t ysize;
};

enum ui_view_flags
{
	UI_VIEW_FLAG_ENABLE = 1,
	UI_VIEW_FLAG_FOCUSEABLE = 2,
	UI_VIEW_FLAG_ISFOCUSED = 4,
	UI_VIEW_FLAG_CLICKABLE = 8
};

enum ui_view_evts
{
	UI_VIEW_EVT_CLICK = 1,
	UI_VIEW_EVT_DOWN = 2,
};

struct ui_view_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
	
};

struct ui_viewstr_param_t
{
	struct ui_view_layout_t layout;
	uint32_t	flags;
	char *str;
	uint8_t font;
};

struct ui_viewimg_param_t
{
	struct ui_view_layout_t layout;
	uint32_t				flags;
	char 					*img;
	uint32_t 				imgsize;
};

struct ui_view_buf_t
{
	uint8_t *linebuf[UI_VIEW_MAX_V];
	uint8_t sizex;
	uint8_t sizey;
};

struct ui_view_op_t
{
	vsf_err_t 					(*init)(void *param);
	vsf_err_t 					(*fini)(void *param);
	struct ui_view_layout_t* 	(*onlayout)(void *param);
	vsf_err_t 					(*ondraw)(void *param, struct ui_view_buf_t *viewbuf);
	vsf_err_t					(*onfocus)(void *param, uint8_t isfocused);
	vsf_err_t 					(*onevt)(void *param, uint8_t evt);
};


vsf_err_t 	ui_viewdummy_init(void *param);
vsf_err_t 	ui_viewdummy_fini(void *param);

void ui_view_inventbuf(struct ui_view_buf_t *viewbuf);

vsf_err_t ui_view_draw_ansismall(uint8_t *line1, uint8_t size, char *str);
vsf_err_t ui_view_draw_ansibig(uint8_t *line1, uint8_t *line2, uint8_t size, char *str);
vsf_err_t ui_view_draw_ansiubig(uint8_t *line1, uint8_t *line2, uint8_t *line3, uint8_t size, char *str);

struct ui_view_layout_t* 	ui_viewdefault_onlayout(void *param);
vsf_err_t	ui_viewdefault_onfocus(void *param, uint8_t isfocused);
vsf_err_t	ui_viewdefault_onkey(void *param, uint8_t isfocused);

vsf_err_t 	ui_viewimg_ondraw(void *param,  struct ui_view_buf_t *viewbuf);

extern const struct ui_view_op_t ui_viewstr_op;
extern const struct ui_view_op_t ui_viewimg_op;

#endif


