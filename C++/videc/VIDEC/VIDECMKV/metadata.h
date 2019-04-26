/*
 * copyright (c) 2009 Michael Niedermayer
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVFORMAT_METADATA_H
#define AVFORMAT_METADATA_H

/**
 * @file
 * internal metadata API header
 * see avformat.h or the public API!
 */


#include "avconmkvlib.h"



typedef struct AVMetadataConv{
    const char *native;
    const char *generic;
}AVMetadataConv;

//typedef struct AVMetadataConv AVMetadataConv;


#if FF_API_OLD_METADATA
void ff_metadata_demux_compat(AVFormatContext *s);
void ff_metadata_mux_compat(AVFormatContext *s);
#endif
AVMetadataTag *av_metadata_get(AVMetadata *m, const char *key, const AVMetadataTag *prev, int flags);
void ff_metadata_conv(AVMetadata **pm, const struct AVMetadataConv *d_conv,const struct AVMetadataConv *s_conv);
void ff_metadata_conv_ctx(AVFormatContext *ctx, const AVMetadataConv *d_conv,const AVMetadataConv *s_conv);
int ff_flac_write_header(ByteIOContext *pb, AVCodecContext *codec,int last_block);
#endif /* AVFORMAT_METADATA_H */
