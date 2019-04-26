/*
 * Provides registration of all codecs for libavcodec.
 * Copyright (c) 2002 Fabrice Bellard.
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

/**
 * @file allcodecs.c
 * Provides registration of all codecs for libavcodec.
 */

#include "avcodec.h"

#define REGISTER_ENCODER(X,x) { \
          extern AVCodec x##_encoder; \
          avcodec_register(&x##_encoder); }
#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(1)  avcodec_register(&x##_decoder); }
#define REGISTER_ENCDEC(X,x)  REGISTER_ENCODER(X,x); REGISTER_DECODER(X,x)

#define REGISTER_PARSER(X,x) { \
          extern AVCodecParser x##_parser; \
          if(1)  av_register_codec_parser(&x##_parser); }

/**
 * Register all the codecs, parsers and bitstream filters which were enabled at
 * configuration time. If you do not call this function you can select exactly
 * which formats you want to support, by using the individual registration
 * functions.
 *
 * @see register_avcodec
 * @see av_register_codec_parser
 */
void avcodec_register_all(void)
{
    static int initialized;

    if (initialized)
        return;
    initialized = 1;

    /* video codecs */
	{
		extern AVCodec h263_encoder;
		avcodec_register(&h263_encoder);
	}
	{
		extern AVCodec h263p_encoder;
		avcodec_register(&h263p_encoder);
	}
	{
		extern AVCodec h263_decoder;
		avcodec_register(&h263_decoder);
	}
	{
		extern AVCodec h264_decoder;
		avcodec_register(&h264_decoder);
	}
	{
		extern AVCodec mjpeg_encoder;
		avcodec_register(&mjpeg_encoder);
	}
	{
		extern AVCodec mjpeg_decoder;
		avcodec_register(&mjpeg_decoder);
	}
}
