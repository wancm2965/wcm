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
#include "LAVPixFmtConverter.h"
#include "Media.h"

#include "decoders/ILAVDecoder.h"

#include <MMReg.h>
#include "moreuuids.h"

#include <time.h>
#include "rand_sse.h"

/*
 * Availability of custom high-quality converters
 * x = formatter available, s = swscale converter used, - = format conversion not available (fallback to sws if we get tricked into it)
 * 1 = up to 10-bit (fallback to sws), 2 = only 16-bit
 * in/out       YV12    NV12    YV16     YUY2    UYVY    YV24   AYUV    P010    P210    v210    Y410    v410    P016    P216    Y416   RGB24   RGB32
 * YUV420         x       x       -       x       x       -       -       -       -       -       -       -       -       -       -      x       x
 * YUV420bX       x       x       -       x       x       -       -       x       -       -       -       -       x2      -       -      x1      x1
 * YUV422         s       s       x       x       x       -       s       -       -       -       -       -       -       -       -      x       x
 * YUV422bX       s       s       x       x       x       -       s       -       x       -       -       -       -       x2      -      x1      x1
 * YUV444         s       s       -       s       s       x       x       -       -       -       -       -       -       -       -      x       x
 * YUV444bX       s       s       -       s       s       x       x       -       -       -       x       -       -       -       x2     x1      x1
 * NV12           x       x       -       x       x       -       -       -       -       -       -       -       -       -       -      x       x
 * YUY2           s       s       s       s       s       -       -       -       -       -       -       -       -       -       -      s       s
 * RGB24          s       s       -       s       s       -       -       -       -       -       -       -       -       -       -      x       s
 * RGB32          s       s       -       s       s       -       -       -       -       -       -       -       -       -       -      s       x
 * ARGB32         s       s       -       s       s       -       -       -       -       -       -       -       -       -       -      s       x
 *
 * Every processing path has a swscale fallback (even those with a "-" above).
 * The "bX" formats usually only support 9/10 or 16-bit, 11-15 are not supported due to lack of samples.
 */


typedef struct {
  LAVPixelFormat in_pix_fmt;
  int maxbpp;
  int num_pix_fmt;
  LAVOutPixFmts lav_pix_fmts[LAVOutPixFmt_NB];
} LAV_INOUT_PIXFMT_MAP;

static LAV_INOUT_PIXFMT_MAP lav_pixfmt_map[] = {
  // Default
  { LAVPixFmt_None, 8, 6, { LAVOutPixFmt_NV12, LAVOutPixFmt_YV12, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24 } },

  // 4:2:0
  { LAVPixFmt_YUV420, 8, 6, { LAVOutPixFmt_NV12, LAVOutPixFmt_YV12, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24 } },
  { LAVPixFmt_NV12,   8, 6, { LAVOutPixFmt_NV12, LAVOutPixFmt_YV12, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24 } },

  { LAVPixFmt_YUV420bX, 10, 7, { LAVOutPixFmt_P010, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24 } },
  { LAVPixFmt_YUV420bX, 16, 8, { LAVOutPixFmt_P016, LAVOutPixFmt_P010, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24 } },

  // 4:2:2
  { LAVPixFmt_YUV422, 8, 7, { LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  { LAVPixFmt_YUY2,   8, 7, { LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },

  { LAVPixFmt_YUV422bX, 10, 9, { LAVOutPixFmt_P210, LAVOutPixFmt_v210, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  { LAVPixFmt_YUV422bX, 16, 10, { LAVOutPixFmt_P216, LAVOutPixFmt_P210, LAVOutPixFmt_v210, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  
  // 4:4:4
  { LAVPixFmt_YUV444,    8, 8, { LAVOutPixFmt_YV24, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  { LAVPixFmt_YUV444bX, 10, 10, { LAVOutPixFmt_Y410, LAVOutPixFmt_v410, LAVOutPixFmt_YV24, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  { LAVPixFmt_YUV444bX, 16, 11, { LAVOutPixFmt_Y416, LAVOutPixFmt_Y410, LAVOutPixFmt_v410, LAVOutPixFmt_YV24, LAVOutPixFmt_AYUV, LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },

  // RGB
  { LAVPixFmt_RGB24,  8, 6, { LAVOutPixFmt_RGB24, LAVOutPixFmt_RGB32, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  { LAVPixFmt_RGB32,  8, 6, { LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },
  { LAVPixFmt_ARGB32, 8, 6, { LAVOutPixFmt_RGB32, LAVOutPixFmt_RGB24, LAVOutPixFmt_YUY2, LAVOutPixFmt_UYVY, LAVOutPixFmt_NV12, LAVOutPixFmt_YV12 } },

  { LAVPixFmt_DXVA2, 8, 1, { LAVOutPixFmt_NV12 } },
};

LAVOutPixFmtDesc lav_pixfmt_desc[] = {
  { MEDIASUBTYPE_YV12,  12, 1, 3, { 1, 2, 2 }, { 1, 2, 2 } },        // YV12
  { MEDIASUBTYPE_NV12,  12, 1, 2, { 1, 2 }, { 1, 1 } },              // NV12
  { MEDIASUBTYPE_YUY2,  16, 2, 0, { 1 }, { 1 } },                    // YUY2 (packed)
  { MEDIASUBTYPE_UYVY,  16, 2, 0, { 1 }, { 1 } },                    // UYVY (packed)
  { MEDIASUBTYPE_AYUV,  32, 4, 0, { 1 }, { 1 } },                    // AYUV (packed)
  { MEDIASUBTYPE_P010,  24, 2, 2, { 1, 2 }, { 1, 1 } },              // P010
  { MEDIASUBTYPE_P210,  32, 2, 2, { 1, 1 }, { 1, 1 } },              // P210
  { FOURCCMap('014Y'),  32, 4, 0, { 1 }, { 1 }  },                   // Y410 (packed)
  { MEDIASUBTYPE_P016,  24, 2, 2, { 1, 2 }, { 1, 1 } },              // P016
  { MEDIASUBTYPE_P216,  32, 2, 2, { 1, 1 }, { 1, 1 } },              // P216
  { FOURCCMap('614Y'),  64, 8, 0, { 1 }, { 1 } },                    // Y416 (packed)
  { MEDIASUBTYPE_RGB32, 32, 4, 0, { 1 }, { 1 } },                    // RGB32
  { MEDIASUBTYPE_RGB24, 24, 3, 0, { 1 }, { 1 } },                    // RGB24
  { FOURCCMap('012v'),  24, 4, 0, { 1 }, { 1 } },                    // v210 (packed)
  { FOURCCMap('014v'),  32, 4, 0, { 1 }, { 1 }  },                   // v410 (packed)
  { MEDIASUBTYPE_YV16,  16, 1, 3, { 1, 1, 1 }, { 1, 2, 2 } },        // YV16
  { MEDIASUBTYPE_YV24,  24, 1, 3, { 1, 1, 1 }, { 1, 1, 1 } },        // YV24
};

static LAV_INOUT_PIXFMT_MAP *lookupFormatMap(LAVPixelFormat informat, int bpp, BOOL bFallbackToDefault = TRUE)
{
  for (int i = 0; i < countof(lav_pixfmt_map); ++i) {
    if (lav_pixfmt_map[i].in_pix_fmt == informat && bpp <= lav_pixfmt_map[i].maxbpp) {
      return &lav_pixfmt_map[i];
    }
  }
  if (bFallbackToDefault)
    return &lav_pixfmt_map[0];
  return NULL;
}

CLAVPixFmtConverter::CLAVPixFmtConverter()
  : m_pSettings(NULL)
  , m_InputPixFmt(LAVPixFmt_None)
  , m_OutputPixFmt(LAVOutPixFmt_YV12)
  , m_pSwsContext(NULL)
  , swsWidth(0), swsHeight(0)
  , m_RequiredAlignment(0)
  , m_nAlignedBufferSize(0)
  , m_pAlignedBuffer(NULL)
  , m_rgbCoeffs(NULL)
  , m_bRGBConverter(FALSE)
  , m_pRandomDithers(NULL)
  , m_ditherWidth(0)
  , m_ditherHeight(0)
  , m_ditherBits(0)
  , m_bSwapUV(false)
{
  convert = &CLAVPixFmtConverter::convert_generic;

  SYSTEM_INFO systemInfo;
  GetSystemInfo(&systemInfo);
  m_NumThreads = min(8, max(1, systemInfo.dwNumberOfProcessors / 2));

  ZeroMemory(&m_ColorProps, sizeof(m_ColorProps));
}

CLAVPixFmtConverter::~CLAVPixFmtConverter()
{
  DestroySWScale();
  av_freep(&m_pAlignedBuffer);
}

LAVOutPixFmts CLAVPixFmtConverter::GetOutputBySubtype(const GUID *guid)
{
  for (int i = 0; i < countof(lav_pixfmt_desc); ++i) {
    if (lav_pixfmt_desc[i].subtype == *guid) {
      return (LAVOutPixFmts)i;
    }
  }
  return LAVOutPixFmt_None;
}

int CLAVPixFmtConverter::GetFilteredFormatCount()
{
  LAV_INOUT_PIXFMT_MAP *pixFmtMap = lookupFormatMap(m_InputPixFmt, m_InBpp);
  int count = 0;
  for (int i = 0; i < pixFmtMap->num_pix_fmt; ++i) {
    if (m_pSettings->GetPixelFormat(pixFmtMap->lav_pix_fmts[i]))
      count++;
  }

  if (count == 0)
    count = lav_pixfmt_map[0].num_pix_fmt;

  return count;
}

LAVOutPixFmts CLAVPixFmtConverter::GetFilteredFormat(int index)
{
  LAV_INOUT_PIXFMT_MAP *pixFmtMap = lookupFormatMap(m_InputPixFmt, m_InBpp);
  int actualIndex = -1;
  for (int i = 0; i < pixFmtMap->num_pix_fmt; ++i) {
    if (m_pSettings->GetPixelFormat(pixFmtMap->lav_pix_fmts[i]))
      actualIndex++;
    if (index == actualIndex)
      return pixFmtMap->lav_pix_fmts[i];
  }

  // If no format is enabled, we use the fallback formats to avoid catastrophic failure
  if (index >= lav_pixfmt_map[0].num_pix_fmt)
    index = 0;
  return lav_pixfmt_map[0].lav_pix_fmts[index];
}

LAVOutPixFmts CLAVPixFmtConverter::GetPreferredOutput()
{
  return GetFilteredFormat(0);
}

int CLAVPixFmtConverter::GetNumMediaTypes()
{
  return GetFilteredFormatCount();
}

void CLAVPixFmtConverter::GetMediaType(CMediaType *mt, int index, LONG biWidth, LONG biHeight, DWORD dwAspectX, DWORD dwAspectY, REFERENCE_TIME rtAvgTime, BOOL bInterlaced, BOOL bVIH1)
{
  if (index < 0 || index >= GetFilteredFormatCount())
    index = 0;

  LAVOutPixFmts pixFmt = GetFilteredFormat(index);

  GUID guid = lav_pixfmt_desc[pixFmt].subtype;

  mt->SetType(&MEDIATYPE_Video);
  mt->SetSubtype(&guid);

  BITMAPINFOHEADER *pBIH = NULL;
  if (bVIH1) {
    mt->SetFormatType(&FORMAT_VideoInfo);

    VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)mt->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
    memset(vih, 0, sizeof(VIDEOINFOHEADER));

    vih->rcSource.right = vih->rcTarget.right = biWidth;
    vih->rcSource.bottom = vih->rcTarget.bottom = biHeight;
    vih->AvgTimePerFrame = rtAvgTime;

    pBIH = &vih->bmiHeader;
  } else {
    mt->SetFormatType(&FORMAT_VideoInfo2);

    VIDEOINFOHEADER2 *vih2 = (VIDEOINFOHEADER2 *)mt->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER2));
    memset(vih2, 0, sizeof(VIDEOINFOHEADER2));

    // Validate the Aspect Ratio - an AR of 0 crashes VMR-9
    if (dwAspectX == 0 || dwAspectY == 0) {
      int dwX = 0;
      int dwY = 0;
      av_reduce(&dwX, &dwY, biWidth, biHeight, max(biWidth, biHeight));

      dwAspectX = dwX;
      dwAspectY = dwY;
    }

    vih2->rcSource.right = vih2->rcTarget.right = biWidth;
    vih2->rcSource.bottom = vih2->rcTarget.bottom = biHeight;
    vih2->AvgTimePerFrame = rtAvgTime;
    vih2->dwPictAspectRatioX = dwAspectX;
    vih2->dwPictAspectRatioY = dwAspectY;

    if (bInterlaced)
      vih2->dwInterlaceFlags = AMINTERLACE_IsInterlaced | AMINTERLACE_DisplayModeBobOrWeave;

    pBIH = &vih2->bmiHeader;
  }

  pBIH->biSize = sizeof(BITMAPINFOHEADER);
  pBIH->biWidth = biWidth;
  pBIH->biHeight = biHeight;
  pBIH->biBitCount = lav_pixfmt_desc[pixFmt].bpp;
  pBIH->biPlanes = lav_pixfmt_desc[pixFmt].planes;
  pBIH->biSizeImage = (biWidth * biHeight * pBIH->biBitCount) >> 3;
  pBIH->biCompression = guid.Data1;

  if (!pBIH->biPlanes) {
    pBIH->biPlanes = 1;
  }

  if (guid == MEDIASUBTYPE_RGB32 || guid == MEDIASUBTYPE_RGB24) {
    pBIH->biCompression = BI_RGB;
  }

  // Correct size for v210
  if (guid == FOURCCMap('012v')) {
    pBIH->biWidth = FFALIGN(biWidth, 48);
    pBIH->biSizeImage = (pBIH->biWidth / 48) * 128 * biHeight;
  }

  mt->SetSampleSize(pBIH->biSizeImage);
  mt->SetTemporalCompression(0);
}


BOOL CLAVPixFmtConverter::IsAllowedSubtype(const GUID *guid)
{
  for (int i = 0; i < GetFilteredFormatCount(); ++i) {
    if (lav_pixfmt_desc[GetFilteredFormat(i)].subtype == *guid)
      return TRUE;
  }

  return FALSE;
}

#define OUTPUT_RGB (m_OutputPixFmt == LAVOutPixFmt_RGB32 || m_OutputPixFmt == LAVOutPixFmt_RGB24)

void CLAVPixFmtConverter::SelectConvertFunction()
{
  m_RequiredAlignment = 16;
  m_bRGBConverter = FALSE;
  convert = NULL;

  int cpu = av_get_cpu_flags();
  if (m_OutputPixFmt == LAVOutPixFmt_v210 || m_OutputPixFmt == LAVOutPixFmt_v410) {
    // We assume that every filter that understands v210 will also properly handle it
    m_RequiredAlignment = 0;
  } else if ((m_OutputPixFmt == LAVOutPixFmt_RGB32 && (m_InputPixFmt == LAVPixFmt_RGB32 || m_InputPixFmt == LAVPixFmt_ARGB32))
    || (m_OutputPixFmt == LAVOutPixFmt_RGB24 && m_InputPixFmt == LAVPixFmt_RGB24)) {
    convert = &CLAVPixFmtConverter::plane_copy;
    m_RequiredAlignment = 0;
  } else if (cpu & AV_CPU_FLAG_SSE2) {
    if (m_OutputPixFmt == LAVOutPixFmt_AYUV && m_InputPixFmt == LAVPixFmt_YUV444bX) {
      convert = &CLAVPixFmtConverter::convert_yuv444_ayuv_dither_le;
    } else if (m_OutputPixFmt == LAVOutPixFmt_AYUV && m_InputPixFmt == LAVPixFmt_YUV444) {
      convert = &CLAVPixFmtConverter::convert_yuv444_ayuv;
    } else if (m_OutputPixFmt == LAVOutPixFmt_Y410 && m_InputPixFmt == LAVPixFmt_YUV444bX && m_InBpp <= 10) {
      convert = &CLAVPixFmtConverter::convert_yuv444_y410;
    } else if (((m_OutputPixFmt == LAVOutPixFmt_YV12 || m_OutputPixFmt == LAVOutPixFmt_I420 || m_OutputPixFmt == LAVOutPixFmt_NV12) && m_InputPixFmt == LAVPixFmt_YUV420bX)
             || (m_OutputPixFmt == LAVOutPixFmt_YV16 && m_InputPixFmt == LAVPixFmt_YUV422bX)
             || (m_OutputPixFmt == LAVOutPixFmt_YV24 && m_InputPixFmt == LAVPixFmt_YUV444bX)) {
      if (m_OutputPixFmt == LAVOutPixFmt_NV12) {
        convert = &CLAVPixFmtConverter::convert_yuv_yv_nv12_dither_le<TRUE>;
      } else {
        convert = &CLAVPixFmtConverter::convert_yuv_yv_nv12_dither_le<FALSE>;
      }
      m_RequiredAlignment = 32;
    } else if (((m_OutputPixFmt == LAVOutPixFmt_P010 || m_OutputPixFmt == LAVOutPixFmt_P016) && m_InputPixFmt == LAVPixFmt_YUV420bX)
            || ((m_OutputPixFmt == LAVOutPixFmt_P210 || m_OutputPixFmt == LAVOutPixFmt_P216) && m_InputPixFmt == LAVPixFmt_YUV422bX)) {
      if (m_InBpp == 10)
        convert = &CLAVPixFmtConverter::convert_yuv420_px1x_le<6>;
      else if (m_InBpp == 9)
        convert = &CLAVPixFmtConverter::convert_yuv420_px1x_le<7>;
      else if (m_InBpp == 16)
        convert = &CLAVPixFmtConverter::convert_yuv420_px1x_le<0>;
    } else if (m_OutputPixFmt == LAVOutPixFmt_NV12 && m_InputPixFmt == LAVPixFmt_YUV420) {
      convert = &CLAVPixFmtConverter::convert_yuv420_nv12;
      m_RequiredAlignment = 32;
    } else if (m_OutputPixFmt == LAVOutPixFmt_YUY2 && m_InputPixFmt == LAVPixFmt_YUV422) {
      convert = &CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy<0>;
      m_RequiredAlignment = 32;
    } else if (m_OutputPixFmt == LAVOutPixFmt_UYVY && m_InputPixFmt == LAVPixFmt_YUV422) {
      convert = &CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy<1>;
      m_RequiredAlignment = 32;
    } else if ((m_OutputPixFmt == LAVOutPixFmt_RGB32 || m_OutputPixFmt == LAVOutPixFmt_RGB24)
            && (m_InputPixFmt == LAVPixFmt_YUV420 || m_InputPixFmt == LAVPixFmt_YUV420bX
             || m_InputPixFmt == LAVPixFmt_YUV422 || m_InputPixFmt == LAVPixFmt_YUV422bX
             || m_InputPixFmt == LAVPixFmt_YUV444 || m_InputPixFmt == LAVPixFmt_YUV444bX
             || m_InputPixFmt == LAVPixFmt_NV12)
            && m_InBpp <= 10) {
      if (m_OutputPixFmt == LAVOutPixFmt_RGB32) {
        convert = &CLAVPixFmtConverter::convert_yuv_rgb<1>;
        m_RequiredAlignment = 4;
      } else {
        convert = &CLAVPixFmtConverter::convert_yuv_rgb<0>;
      }
      m_bRGBConverter = TRUE;
    } else if ((m_OutputPixFmt == LAVOutPixFmt_YV12 || m_OutputPixFmt == LAVOutPixFmt_I420)&& m_InputPixFmt == LAVPixFmt_NV12) {
      convert = &CLAVPixFmtConverter::convert_nv12_yv12;
      m_RequiredAlignment = 32;
    } else if ((m_OutputPixFmt == LAVOutPixFmt_YUY2 || m_OutputPixFmt == LAVOutPixFmt_UYVY) && (m_InputPixFmt == LAVPixFmt_YUV420 || m_InputPixFmt == LAVPixFmt_NV12 || m_InputPixFmt == LAVPixFmt_YUV420bX) && m_InBpp <= 10) {
      if (m_OutputPixFmt == LAVOutPixFmt_YUY2) {
        convert = &CLAVPixFmtConverter::convert_yuv420_yuy2<0>;
      } else {
        convert = &CLAVPixFmtConverter::convert_yuv420_yuy2<1>;
      }
      m_RequiredAlignment = 8; // Pixel alignment of 8 guarantees a byte alignment of 16
    } else if ((m_OutputPixFmt == LAVOutPixFmt_YUY2 || m_OutputPixFmt == LAVOutPixFmt_UYVY) && m_InputPixFmt == LAVPixFmt_YUV422bX) {
      if (m_OutputPixFmt == LAVOutPixFmt_YUY2) {
        convert = &CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy_dither_le<0>;
      } else {
        convert = &CLAVPixFmtConverter::convert_yuv422_yuy2_uyvy_dither_le<1>;
      }

      m_RequiredAlignment = 8; // Pixel alignment of 8 guarantees a byte alignment of 16
    } else if ((m_OutputPixFmt == LAVOutPixFmt_NV12 && m_InputPixFmt == LAVPixFmt_NV12)) {
      convert = &CLAVPixFmtConverter::convert_nv12_nv12;
      m_RequiredAlignment = 0;
    } else if (((m_OutputPixFmt == LAVOutPixFmt_YV12 || m_OutputPixFmt == LAVOutPixFmt_I420) && m_InputPixFmt == LAVPixFmt_YUV420)
            || (m_OutputPixFmt == LAVOutPixFmt_YV16 && m_InputPixFmt == LAVPixFmt_YUV422)
            || (m_OutputPixFmt == LAVOutPixFmt_YV24 && m_InputPixFmt == LAVPixFmt_YUV444)) {
      convert = &CLAVPixFmtConverter::convert_yuv_yv;
      m_RequiredAlignment = 0;
    }
  // Fallbacks only to be used when SSE2 is not available
  } else if ((m_OutputPixFmt == LAVOutPixFmt_NV12 && m_InputPixFmt == LAVPixFmt_NV12)) {
    convert = &CLAVPixFmtConverter::plane_copy;
    m_RequiredAlignment = 0;
  }

  if (convert == NULL) {
    convert = &CLAVPixFmtConverter::convert_generic;
  }
}

DECLARE_CONV_FUNC_IMPL(plane_copy)
{
  LAVOutPixFmtDesc desc = lav_pixfmt_desc[outputFormat];

  int plane, line;

  const int widthBytes = width * desc.codedbytes;
  const int dstStrideBytes = dstStride * desc.codedbytes;
  const int planes = max(desc.planes, 1);

  for (plane = 0; plane < planes; plane++) {
    const int planeWidth     = widthBytes     / desc.planeWidth[plane];
    const int planeHeight    = height         / desc.planeHeight[plane];
    const int dstPlaneStride = dstStrideBytes / desc.planeWidth[plane];
    const uint8_t *srcBuf = src[plane];
    for (line = 0; line < planeHeight; ++line) {
      memcpy(dst, srcBuf, planeWidth);
      srcBuf += srcStride[plane];
      dst += dstPlaneStride;
    }
  }

  return S_OK;
}

void CLAVPixFmtConverter::ChangeStride(const uint8_t* src, int srcStride, uint8_t *dst, int dstStride, int width, int height, LAVOutPixFmts format)
{
  LAVOutPixFmtDesc desc = lav_pixfmt_desc[format];

  int line = 0;

  // Copy first plane
  const int widthBytes = width * desc.codedbytes;
  const int srcStrideBytes = srcStride * desc.codedbytes;
  const int dstStrideBytes = dstStride * desc.codedbytes;
  for (line = 0; line < height; ++line) {
    memcpy(dst, src, widthBytes);
    src += srcStrideBytes;
    dst += dstStrideBytes;
  }

  for (int plane = 1; plane < desc.planes; ++plane) {
    const int planeWidth     = widthBytes     / desc.planeWidth[plane];
    const int planeHeight    = height         / desc.planeHeight[plane];
    const int srcPlaneStride = srcStrideBytes / desc.planeWidth[plane];
    const int dstPlaneStride = dstStrideBytes / desc.planeWidth[plane];
    for (line = 0; line < planeHeight; ++line) {
      memcpy(dst, src, planeWidth);
      src += srcPlaneStride;
      dst += dstPlaneStride;
    }
  }
}

const uint16_t* CLAVPixFmtConverter::GetRandomDitherCoeffs(int height, int coeffs, int bits, int line)
{
  if (m_pSettings->GetDitherMode() != LAVDither_Random)
    return NULL;

  int totalWidth = 8 * coeffs;
  if (!m_pRandomDithers || totalWidth > m_ditherWidth || height > m_ditherHeight || bits != m_ditherBits) {
    if (m_pRandomDithers)
      _aligned_free(m_pRandomDithers);
    m_pRandomDithers = NULL;
    m_ditherWidth = totalWidth;
    m_ditherHeight = height;
    m_ditherBits = bits;
    m_pRandomDithers = (uint16_t *)_aligned_malloc(m_ditherWidth * m_ditherHeight * 2, 16);

#ifdef DEBUG
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    DbgLog((LOG_TRACE, 10, L"Creating dither matrix"));
#endif

    // Seed random number generator
    time_t seed = time(NULL);
    seed >>= 1;
    srand_sse((unsigned int)seed);

    bits = (1 << bits);
    for (int i = 0; i < m_ditherHeight; i++) {
      uint16_t *ditherline = m_pRandomDithers + (m_ditherWidth * i);
      for (int j = 0; j < m_ditherWidth; j += 4) {
        int rnds[4];
        rand_sse(rnds);
        ditherline[j+0] = rnds[0] % bits;
        ditherline[j+1] = rnds[1] % bits;
        ditherline[j+2] = rnds[2] % bits;
        ditherline[j+3] = rnds[3] % bits;
      }
    }
#ifdef DEBUG
    QueryPerformanceCounter(&end);
    double diff = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    DbgLog((LOG_TRACE, 10, L"Finished creating dither matrix (took %2.3fms)", diff));
#endif

  }

  if (line < 0 || line >= m_ditherHeight)
    line = rand() % m_ditherHeight;

  return &m_pRandomDithers[line * m_ditherWidth];
}
