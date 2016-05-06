/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __STREAM_H_INCLUDED__
#define __STREAM_H_INCLUDED__

#include "component/buffer/buffer.h"

struct vsf_stream_t;
struct vsf_stream_op_t
{
	void (*init)(struct vsf_stream_t *stream);
	void (*fini)(struct vsf_stream_t *stream);
	uint32_t (*write)(struct vsf_stream_t *stream, struct vsf_buffer_t *buffer);
	uint32_t (*read)(struct vsf_stream_t *stream, struct vsf_buffer_t *buffer);
	uint32_t (*get_data_length)(struct vsf_stream_t *stream);
	uint32_t (*get_avail_length)(struct vsf_stream_t *stream);
};

struct vsf_stream_cb_t
{
	void *param;
	void (*on_inout)(void *param);
	void (*on_connect)(void *param);
	void (*on_disconnect)(void *param);
};

struct vsf_stream_t
{
	// user_mem points to user structure, eg queue/fifo
	struct vsf_stream_op_t const *op;

	// callback_tx is notification for tx end of the stream
	// when rx end read the data out, will notify the tx end
	struct vsf_stream_cb_t callback_tx;
	// callback_rx is notification for rx end of the stream
	// when tx end write the data in, will notify the rx end
	struct vsf_stream_cb_t callback_rx;
	bool tx_ready;
	bool rx_ready;
	bool overflow;
};

#define STREAM_INIT(s)			stream_init((struct vsf_stream_t *)(s))
#define STREAM_FINI(s)			stream_fini((struct vsf_stream_t *)(s))
#define STREAM_WRITE(s, b)		stream_write((struct vsf_stream_t *)(s), (b))
#define STREAM_READ(s, b)		stream_read((struct vsf_stream_t *)(s), (b))
#define STREAM_GET_DATA_SIZE(s)	stream_get_data_size((struct vsf_stream_t *)(s))
#define STREAM_GET_FREE_SIZE(s)	stream_get_free_size((struct vsf_stream_t *)(s))
#define STREAM_CONNECT_RX(s)	stream_connect_rx((struct vsf_stream_t *)(s))
#define STREAM_CONNECT_TX(s)	stream_connect_tx((struct vsf_stream_t *)(s))
#define STREAM_DISCONNECT_RX(s)	stream_disconnect_rx((struct vsf_stream_t *)(s))
#define STREAM_DISCONNECT_TX(s)	stream_disconnect_tx((struct vsf_stream_t *)(s))

vsf_err_t stream_init(struct vsf_stream_t *stream);
vsf_err_t stream_fini(struct vsf_stream_t *stream);
uint32_t stream_write(struct vsf_stream_t *stream, struct vsf_buffer_t *buffer);
uint32_t stream_read(struct vsf_stream_t *stream, struct vsf_buffer_t *buffer);
uint32_t stream_get_data_size(struct vsf_stream_t *stream);
uint32_t stream_get_free_size(struct vsf_stream_t *stream);
void stream_connect_rx(struct vsf_stream_t *stream);
void stream_connect_tx(struct vsf_stream_t *stream);
void stream_disconnect_rx(struct vsf_stream_t *stream);
void stream_disconnect_tx(struct vsf_stream_t *stream);

// fifo stream, user_mem is vsf_fifo_t: available in interrupt
struct vsf_fifostream_t
{
	struct vsf_stream_t stream;
	struct vsf_fifo_t mem;
};
extern const struct vsf_stream_op_t fifostream_op;
// multibuf stream, user_mem is vsf_multibuf_stream_t: unavailable in interrupt
struct vsf_mbufstream_mem_t
{
	struct vsf_multibuf_t multibuf;
	// private
	uint32_t rpos, wpos;
};
struct vsf_mbufstream_t
{
	struct vsf_stream_t stream;
	struct vsf_mbufstream_mem_t mem;
};
extern const struct vsf_stream_op_t mbufstream_op;
// buffer stream, user_mem is vsf_buffer_stream_t: unavailable in interrupt
struct vsf_bufstream_mem_t
{
	struct vsf_buffer_t buffer;
	bool read;
	// private
	uint32_t pos;
};
struct vsf_bufstream_t
{
	struct vsf_stream_t stream;
	struct vsf_bufstream_mem_t mem;
};
extern const struct vsf_stream_op_t bufstream_op;

#endif	// __STREAM_H_INCLUDED__
