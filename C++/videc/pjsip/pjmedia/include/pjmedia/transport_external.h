/* $Id: transport_external.h 2845 2009-07-29 12:19:25Z bennylp $ */
/* 
 * Copyright (C) 2008-2009 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#ifndef __PJMEDIA_TRANSPORT_EXTERNAL_H__
#define __PJMEDIA_TRANSPORT_EXTERNAL_H__


/**
 * @file transport_external.h
 * @brief External transport
 */

#include <pjmedia/stream.h>


/**
 * @defgroup PJMEDIA_TRANSPORT_EXTERNAL External Media Transport
 * @ingroup PJMEDIA_TRANSPORT
 * @brief External transport for AVCON.
 * @{
 *
 * This is the external media transport, where packets sent to this transport
 * will be sent back to the streams attached to this transport. Unlike the
 * other PJMEDIA transports, the external transport may be attached to multiple
 * streams (in other words, application should specify the same external transport
 * instance when calling #pjmedia_stream_create()). Any RTP or RTCP packets
 * sent by one stream to this transport by default will be sent back to all 
 * streams that are attached to this transport, including to the stream that
 * sends the packet. 
 */

PJ_BEGIN_DECL


/**
 * Create the external transport.
 *
 * @param endpt	    The media endpoint instance.
 * @param p_tp	    Pointer to receive the transport instance.
 *
 * @return	    PJ_SUCCESS on success.
 */
PJ_DECL(pj_status_t) pjmedia_transport_external_create(pjmedia_endpt *endpt,
													   void *ext_user_data,
													   void (*ext_rtp_cb)(void*, void*, pj_ssize_t),
													   void (*ext_rtcp_cb)(void*, void*, pj_ssize_t),
													   pjmedia_transport **p_tp);


PJ_DECL(pj_status_t) pjmedia_transport_external_on_rcvd_rtp(pjmedia_transport *tp,
						       const void *pkt,
								pj_size_t size);

PJ_DECL(pj_status_t) pjmedia_transport_external_on_rcvd_rtcp(pjmedia_transport *tp,
						       const void *pkt,
								pj_size_t size);

PJ_END_DECL


/**
 * @}
 */


#endif	/* __PJMEDIA_TRANSPORT_EXTERNAL_H__ */


