/*
 *      Copyright (C) 2010-2012 Hendrik Leppkes
 *      http://www.1f0.de
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "stdafx.h"

#include <emmintrin.h>

#include "pixconv_internal.h"
#include "pixconv_sse2_templates.h"

template <int nv12>
DECLARE_CONV_FUNC_IMPL(convert_yuv_yv_nv12_dither_le)
{
  const uint16_t *y = (const uint16_t *)src[0];
  const uint16_t *u = (const uint16_t *)src[1];
  const uint16_t *v = (const uint16_t *)src[2];

  const int inYStride = srcStride[0] >> 1;
  const int inUVStride = srcStride[1] >> 1;

  const int shift = bpp - 8;

  int outLumaStride    = dstStride;
  int outChromaStride  = dstStride;
  int chromaWidth      = width;
  int chromaHeight     = height;

  LAVDitherMode ditherMode = m_pSettings->GetDitherMode();
  const uint16_t *dithers = GetRandomDitherCoeffs(height, 4, 8, 0);
  if (dithers == NULL)
    ditherMode = LAVDither_Ordered;

  if (inputFormat == LAVPixFmt_YUV420bX)
    chromaHeight = chromaHeight >> 1;
  if (inputFormat == LAVPixFmt_YUV420bX || inputFormat == LAVPixFmt_YUV422bX) {
    chromaWidth = (chromaWidth + 1) >> 1;
    outChromaStride = outChromaStride >> 1;
  }

  int line, i;

  __m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;

  uint8_t *dstY = dst;
  uint8_t *dstV = dstY + outLumaStride * height;
  uint8_t *dstU = dstV + outChromaStride * chromaHeight;

  if (outputFormat==LAVOutPixFmt_I420)
  {
	  uint8_t*dstTemp=dstV;
	  dstV=dstU;
	  dstU=dstTemp;
  }

  _mm_sfence();

  // Process Y
  for (line = 0; line < height; ++line) {
    // Load dithering coefficients for this line
    if (ditherMode == LAVDither_Random) {
      xmm4 = _mm_load_si128((const __m128i *)(dithers + (line << 5) + 0));
      xmm5 = _mm_load_si128((const __m128i *)(dithers + (line << 5) + 8));
      xmm6 = _mm_load_si128((const __m128i *)(dithers + (line << 5) + 16));
      xmm7 = _mm_load_si128((const __m128i *)(dithers + (line << 5) + 24));
    } else {
      PIXCONV_LOAD_DITHER_COEFFS(xmm7,line,8,dithers);
      xmm4 = xmm5 = xmm6 = xmm7;
    }

    __m128i *dst128Y = (__m128i *)(dstY + line * outLumaStride);

    for (i = 0; i < width; i+=32) {
      // Load pixels into registers, and apply dithering
      PIXCONV_LOAD_PIXEL16_DITHER(xmm0, xmm4, (y+i+ 0), shift);  /* Y0Y0Y0Y0 */
      PIXCONV_LOAD_PIXEL16_DITHER(xmm1, xmm5, (y+i+ 8), shift);  /* Y0Y0Y0Y0 */
      PIXCONV_LOAD_PIXEL16_DITHER(xmm2, xmm6, (y+i+16), shift);  /* Y0Y0Y0Y0 */
      PIXCONV_LOAD_PIXEL16_DITHER(xmm3, xmm7, (y+i+24), shift);  /* Y0Y0Y0Y0 */
      xmm0 = _mm_packus_epi16(xmm0, xmm1);                       /* YYYYYYYY */
      xmm2 = _mm_packus_epi16(xmm2, xmm3);                       /* YYYYYYYY */

      // Write data back
      _mm_stream_si128(dst128Y++, xmm0);
      _mm_stream_si128(dst128Y++, xmm2);
    }

    // Process U/V for chromaHeight lines
    if (line < chromaHeight) {
      __m128i *dst128UV = (__m128i *)(dstV + line * outLumaStride);
      __m128i *dst128U = (__m128i *)(dstU + line * outChromaStride);
      __m128i *dst128V = (__m128i *)(dstV + line * outChromaStride);

       for (i = 0; i < chromaWidth; i+=16) {
        PIXCONV_LOAD_PIXEL16_DITHER(xmm0, xmm4, (u+i), shift);    /* U0U0U0U0 */
        PIXCONV_LOAD_PIXEL16_DITHER(xmm1, xmm5, (u+i+8), shift);  /* U0U0U0U0 */
        PIXCONV_LOAD_PIXEL16_DITHER(xmm2, xmm6, (v+i), shift);    /* V0V0V0V0 */
        PIXCONV_LOAD_PIXEL16_DITHER(xmm3, xmm7, (v+i+8), shift);  /* V0V0V0V0 */

        xmm0 = _mm_packus_epi16(xmm0, xmm1);                      /* UUUUUUUU */
        xmm2 = _mm_packus_epi16(xmm2, xmm3);                      /* VVVVVVVV */
        if (nv12) {
          xmm1 = xmm0;
          xmm0 = _mm_unpacklo_epi8(xmm0, xmm2);
          xmm1 = _mm_unpackhi_epi8(xmm1, xmm2);

          _mm_stream_si128(dst128UV++, xmm0);
          _mm_stream_si128(dst128UV++, xmm1);
        } else {
          _mm_stream_si128(dst128U++, xmm0);
          _mm_stream_si128(dst128V++, xmm2);
        }
      }

      u += inUVStride;
      v += inUVStride;
    }

    y += inYStride;
  }

  return S_OK;
}

// Force creation of these two variants
template HRESULT CLAVPixFmtConverter::convert_yuv_yv_nv12_dither_le<0>CONV_FUNC_PARAMS;
template HRESULT CLAVPixFmtConverter::convert_yuv_yv_nv12_dither_le<1>CONV_FUNC_PARAMS;

template <int shift>
DECLARE_CONV_FUNC_IMPL(convert_yuv420_px1x_le)
{
  const uint16_t *y = (const uint16_t *)src[0];
  const uint16_t *u = (const uint16_t *)src[1];
  const uint16_t *v = (const uint16_t *)src[2];

  const int inYStride = srcStride[0] >> 1;
  const int inUVStride = srcStride[1] >> 1;
  const int outStride = dstStride << 1;
  const int uvHeight = (outputFormat == LAVOutPixFmt_P010 || outputFormat == LAVOutPixFmt_P016) ? (height >> 1) : height;
  const int uvWidth = (width + 1) >> 1;

  int line, i;
  __m128i xmm0,xmm1,xmm2;

  _mm_sfence();

  // Process Y
  for (line = 0; line < height; ++line) {
    __m128i *dst128Y = (__m128i *)(dst + line * outStride);

    for (i = 0; i < width; i+=16) {
      // Load 8 pixels into register
      PIXCONV_LOAD_PIXEL16(xmm0, (y+i), shift); /* YYYY */
      PIXCONV_LOAD_PIXEL16(xmm1, (y+i+8), shift); /* YYYY */
      // and write them out
      _mm_stream_si128(dst128Y++, xmm0);
      _mm_stream_si128(dst128Y++, xmm1);
    }

    y += inYStride;
  }

  BYTE *dstUV = dst + (height * outStride);

  // Process UV
  for (line = 0; line < uvHeight; ++line) {
    __m128i *dst128UV = (__m128i *)(dstUV + line * outStride);

    for (i = 0; i < uvWidth; i+=8) {
      // Load 8 pixels into register
      PIXCONV_LOAD_PIXEL16(xmm0, (v+i), shift); /* VVVV */
      PIXCONV_LOAD_PIXEL16(xmm1, (u+i), shift); /* UUUU */

      xmm2 = xmm0;
      xmm0 = _mm_unpacklo_epi16(xmm1, xmm0);    /* UVUV */
      xmm2 = _mm_unpackhi_epi16(xmm1, xmm2);    /* UVUV */

      _mm_stream_si128(dst128UV++, xmm0);
      _mm_stream_si128(dst128UV++, xmm2);
    }

    u += inUVStride;
    v += inUVStride;
  }

  return S_OK;
}

// Force creation of these two variants
template HRESULT CLAVPixFmtConverter::convert_yuv420_px1x_le<0>CONV_FUNC_PARAMS;
template HRESULT CLAVPixFmtConverter::convert_yuv420_px1x_le<6>CONV_FUNC_PARAMS;
template HRESULT CLAVPixFmtConverter::convert_yuv420_px1x_le<7>CONV_FUNC_PARAMS;

DECLARE_CONV_FUNC_IMPL(convert_yuv_yv)
{
  const uint8_t *y = src[0];
  const uint8_t *u = src[1];
  const uint8_t *v = src[2];

  const int inLumaStride    = srcStride[0];
  const int inChromaStride  = srcStride[1];
  const int outLumaStride   = dstStride;
  int outChromaStride = dstStride;

  int line;
  int chromaWidth  = width;
  int chromaHeight = height;

  if (inputFormat == LAVPixFmt_YUV420)
    chromaHeight = chromaHeight >> 1;
  if (inputFormat == LAVPixFmt_YUV420 || inputFormat == LAVPixFmt_YUV422) {
    chromaWidth = (chromaWidth + 1) >> 1;
    outChromaStride = outChromaStride >> 1;
  }

  uint8_t *dstY = dst;
  uint8_t *dstV = dstY + height * outLumaStride;
  uint8_t *dstU = dstV + chromaHeight * outChromaStride;

  if (outputFormat==LAVOutPixFmt_I420)
  {
	  uint8_t*dstTemp=dstV;
	  dstV=dstU;
	  dstU=dstTemp;
  }

  // Copy planes

  _mm_sfence();

  // Y
  if ((outLumaStride % 16) == 0 && ((intptr_t)dst % 16u) == 0) {
    for(line = 0; line < height; ++line) {
      PIXCONV_MEMCPY_ALIGNED(dstY + outLumaStride * line, y, width);
      y += inLumaStride;
    }
  } else {
    for(line = 0; line < height; ++line) {
      memcpy(dstY + outLumaStride * line, y, width);
      y += inLumaStride;
    }
  }

  // U/V
  if ((outChromaStride % 16) == 0 && ((intptr_t)dst % 16u) == 0) {
    for(line = 0; line < chromaHeight; ++line) {
      PIXCONV_MEMCPY_ALIGNED_TWO(
        dstU + outChromaStride * line, u,
        dstV + outChromaStride * line, v,
        chromaWidth);
      u += inChromaStride;
      v += inChromaStride;
    }
  } else {
    for(line = 0; line < chromaHeight; ++line) {
      memcpy(dstU + outChromaStride * line, u, chromaWidth);
      memcpy(dstV + outChromaStride * line, v, chromaWidth);
      u += inChromaStride;
      v += inChromaStride;
    }
  }

  return S_OK;
}

DECLARE_CONV_FUNC_IMPL(convert_yuv420_nv12)
{
  const uint8_t *y = src[0];
  const uint8_t *u = src[1];
  const uint8_t *v = src[2];

  const int inLumaStride    = srcStride[0];
  const int inChromaStride  = srcStride[1];
  const int outStride       = dstStride;

  const int chromaWidth     = (width + 1) >> 1;
  const int chromaHeight    = height >> 1;

  uint8_t *dstY = dst;
  uint8_t *dstUV = dstY + height * outStride;

  int line,i;
  __m128i xmm0,xmm1,xmm2,xmm3;

  _mm_sfence();

  // Y
  for(line = 0; line < height; ++line) {
    PIXCONV_MEMCPY_ALIGNED32(dstY + outStride * line, y, width);
    y += inLumaStride;
  }

  // U/V
  for(line = 0; line < chromaHeight; ++line) {
    __m128i *dst128UV = (__m128i *)(dstUV + line * outStride);

    for (i = 0; i < chromaWidth; i+=16) {
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm0, (v+i));  /* VVVV */
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm1, (u+i));  /* UUUU */

      xmm2 = _mm_unpacklo_epi8(xmm1, xmm0);      /* UVUV */
      xmm3 = _mm_unpackhi_epi8(xmm1, xmm0);      /* UVUV */

      _mm_stream_si128(dst128UV++, xmm2);
      _mm_stream_si128(dst128UV++, xmm3);
    }

    u += inChromaStride;
    v += inChromaStride;
  }

  return S_OK;
}

template <int uyvy>
DECLARE_CONV_FUNC_IMPL(convert_yuv422_yuy2_uyvy)
{
  const uint8_t *y = src[0];
  const uint8_t *u = src[1];
  const uint8_t *v = src[2];

  const int inLumaStride    = srcStride[0];
  const int inChromaStride  = srcStride[1];
  const int outStride       = dstStride << 1;

  const int chromaWidth     = (width + 1) >> 1;

  int line,i;
  __m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5;

  _mm_sfence();

  for (line = 0;  line < height; ++line) {
    __m128i *dst128 = (__m128i *)(dst + line * outStride);

    for (i = 0; i < chromaWidth; i+=16) {
      // Load pixels
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm0, (y+(i*2)+0));  /* YYYY */
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm1, (y+(i*2)+16)); /* YYYY */
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm2, (u+i));        /* UUUU */
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm3, (v+i));        /* VVVV */

      // Interleave Us and Vs
      xmm4 = xmm2;
      xmm4 = _mm_unpacklo_epi8(xmm4,xmm3);
      xmm2 = _mm_unpackhi_epi8(xmm2,xmm3);

      // Interlave those with the Ys
      if (uyvy) {
        xmm3 = xmm4;
        xmm3 = _mm_unpacklo_epi8(xmm3, xmm0);
        xmm4 = _mm_unpackhi_epi8(xmm4, xmm0);
      } else {
        xmm3 = xmm0;
        xmm3 = _mm_unpacklo_epi8(xmm3, xmm4);
        xmm4 = _mm_unpackhi_epi8(xmm0, xmm4);
      }

      _mm_stream_si128(dst128++, xmm3);
      _mm_stream_si128(dst128++, xmm4);

      // Interlave those with the Ys
      if (uyvy) {
        xmm5 = xmm2;
        xmm5 = _mm_unpacklo_epi8(xmm5, xmm1);
        xmm2 = _mm_unpackhi_epi8(xmm2, xmm1);
      } else {
        xmm5 = xmm1;
        xmm5 = _mm_unpacklo_epi8(xmm5, xmm2);
        xmm2 = _mm_unpackhi_epi8(xmm1, xmm2);
      }

      _mm_stream_si128(dst128++, xmm5);
      _mm_stream_si128(dst128++, xmm2);
    }
    y += inLumaStride;
    u += inChromaStride;
    v += inChromaStride;
  }

  return S_OK;
}

// Force creation of these two variants
template HRESULT CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy<0>CONV_FUNC_PARAMS;
template HRESULT CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy<1>CONV_FUNC_PARAMS;

template <int uyvy>
DECLARE_CONV_FUNC_IMPL(convert_yuv422_yuy2_uyvy_dither_le)
{
  const uint16_t *y = (const uint16_t *)src[0];
  const uint16_t *u = (const uint16_t *)src[1];
  const uint16_t *v = (const uint16_t *)src[2];

  const int inLumaStride    = srcStride[0] >> 1;
  const int inChromaStride  = srcStride[1] >> 1;
  const int outStride       = dstStride << 1;
  const int chromaWidth     = (width + 1) >> 1;
  const int shift           = bpp - 8;

  LAVDitherMode ditherMode = m_pSettings->GetDitherMode();
  const uint16_t *dithers = GetRandomDitherCoeffs(height, 4, 8, 0);
  if (dithers == NULL)
    ditherMode = LAVDither_Ordered;

  int line,i;
  __m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;

  _mm_sfence();

  for (line = 0;  line < height; ++line) {
    __m128i *dst128 = (__m128i *)(dst + line * outStride);

    // Load dithering coefficients for this line
    if (ditherMode == LAVDither_Random) {
      xmm4 = _mm_load_si128((const __m128i *)(dithers + (line << 5) +  0));
      xmm5 = _mm_load_si128((const __m128i *)(dithers + (line << 5) +  8));
      xmm6 = _mm_load_si128((const __m128i *)(dithers + (line << 5) + 16));
      xmm7 = _mm_load_si128((const __m128i *)(dithers + (line << 5) + 24));
    } else {
      PIXCONV_LOAD_DITHER_COEFFS(xmm7,line,8,dithers);
      xmm4 = xmm5 = xmm6 = xmm7;
    }

    for (i = 0; i < chromaWidth; i+=8) {
      // Load pixels
      PIXCONV_LOAD_PIXEL16_DITHER(xmm0, xmm4, (y+(i*2)+0), shift);  /* YYYY */
      PIXCONV_LOAD_PIXEL16_DITHER(xmm1, xmm5, (y+(i*2)+8), shift);  /* YYYY */
      PIXCONV_LOAD_PIXEL16_DITHER(xmm2, xmm6, (u+i), shift);        /* UUUU */
      PIXCONV_LOAD_PIXEL16_DITHER(xmm3, xmm7, (v+i), shift);        /* VVVV */

      // Pack Ys
      xmm0 = _mm_packus_epi16(xmm0, xmm1);

      // Interleave Us and Vs
      xmm2 = _mm_packus_epi16(xmm2, xmm2);
      xmm3 = _mm_packus_epi16(xmm3, xmm3);
      xmm2 = _mm_unpacklo_epi8(xmm2, xmm3);

      // Interlave those with the Ys
      if (uyvy) {
        xmm3 = xmm2;
        xmm3 = _mm_unpacklo_epi8(xmm3, xmm0);
        xmm2 = _mm_unpackhi_epi8(xmm2, xmm0);
      } else {
        xmm3 = xmm0;
        xmm3 = _mm_unpacklo_epi8(xmm3, xmm2);
        xmm2 = _mm_unpackhi_epi8(xmm0, xmm2);
      }

      _mm_stream_si128(dst128++, xmm3);
      _mm_stream_si128(dst128++, xmm2);
    }
    y += inLumaStride;
    u += inChromaStride;
    v += inChromaStride;
  }

  return S_OK;
}

// Force creation of these two variants
template HRESULT CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy_dither_le<0>CONV_FUNC_PARAMS;
template HRESULT CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy_dither_le<1>CONV_FUNC_PARAMS;

DECLARE_CONV_FUNC_IMPL(convert_nv12_yv12)
{
  const uint8_t *y  = src[0];
  const uint8_t *uv = src[1];

  const int inStride = srcStride[0];
  const int outLumaStride = dstStride;
  const int outChromaStride = dstStride >> 1;

  const int chromaHeight = height >> 1;

  uint8_t *dstY = dst;
  uint8_t *dstV = dstY + height * outLumaStride;
  uint8_t *dstU = dstV + chromaHeight * outChromaStride;

  if (outputFormat==LAVOutPixFmt_I420)
  {
	  uint8_t*dstTemp=dstV;
	  dstV=dstU;
	  dstU=dstTemp;
  }

  int line, i;
  __m128i xmm0,xmm1,xmm2,xmm3,xmm7;

  xmm7 = _mm_set1_epi16(0x00FF);

  _mm_sfence();

  // Copy the y
  for (line = 0; line < height; line++) {
    PIXCONV_MEMCPY_ALIGNED(dstY + outLumaStride * line, y, width);
    y += inStride;
  }

  for (line = 0; line < chromaHeight; line++) {
    __m128i *dstV128 = (__m128i *)(dstV + outChromaStride * line);
    __m128i *dstU128 = (__m128i *)(dstU + outChromaStride * line);

    for (i = 0; i < width; i+=32) {
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm0, uv+i+0);
      PIXCONV_LOAD_PIXEL8_ALIGNED(xmm1, uv+i+16);
      xmm2 = xmm0;
      xmm3 = xmm1;

      // null out the high-order bytes to get the U values
      xmm0 = _mm_and_si128(xmm0, xmm7);
      xmm1 = _mm_and_si128(xmm1, xmm7);
      // right shift the V values
      xmm2 = _mm_srli_epi16(xmm2, 8);
      xmm3 = _mm_srli_epi16(xmm3, 8);
      // unpack the values
      xmm0 = _mm_packus_epi16(xmm0, xmm1);
      xmm2 = _mm_packus_epi16(xmm2, xmm3);

      _mm_stream_si128(dstU128++, xmm0);
      _mm_stream_si128(dstV128++, xmm2);
    }
    uv += inStride;
  }

  return S_OK;
}

DECLARE_CONV_FUNC_IMPL(convert_nv12_nv12)
{
  const uint8_t *y  = src[0];
  const uint8_t *uv = src[1];

  const int inStride = srcStride[0];
  const int outStride = dstStride;
  const int chromaHeight = (height >> 1);

  uint8_t *dstY = dst;
  uint8_t *dstUV = dstY + height * outStride;

  int line;

  _mm_sfence();

  // Use SSE2 copy when the stride is aligned
  if ((outStride % 16) == 0) {
    // Copy the data
    for (line = 0; line < height; line++) {
      PIXCONV_MEMCPY_ALIGNED(dstY + outStride * line, y, width);
      y += inStride;
    }

    for (line = 0; line < chromaHeight; line++) {
      PIXCONV_MEMCPY_ALIGNED(dstUV + outStride * line, uv, width);
      uv += inStride;
    }
  } else {
    // Copy the data
    for (line = 0; line < height; line++) {
      memcpy(dstY + outStride * line, y, width);
      y += inStride;
    }

    for (line = 0; line < chromaHeight; line++) {
      memcpy(dstUV + outStride * line, uv, width);
      uv += inStride;
    }
  }

  return S_OK;
}
