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

#pragma once


// Codecs supported in the LAV Video configuration
// Codecs not listed here cannot be turned off. You can request codecs to be added to this list, if you wish.
typedef enum LAVVideoCodec {
  Codec_H264,
  Codec_VC1,
  Codec_MPEG1,
  Codec_MPEG2,
  Codec_MPEG4,
  Codec_MSMPEG4,
  Codec_VP8,
  Codec_WMV3,
  Codec_WMV12,
  Codec_MJPEG,
  Codec_Theora,
  Codec_FLV1,
  Codec_VP6,
  Codec_SVQ,
  Codec_H261,
  Codec_H263,
  Codec_Indeo,
  Codec_TSCC,
  Codec_Fraps,
  Codec_HuffYUV,
  Codec_QTRle,
  Codec_DV,
  Codec_Bink,
  Codec_Smacker,
  Codec_RV12,
  Codec_RV34,
  Codec_Lagarith,
  Codec_Cinepak,
  Codec_Camstudio,
  Codec_QPEG,
  Codec_ZLIB,
  Codec_QTRpza,
  Codec_PNG,
  Codec_MSRLE,
  Codec_ProRes,
  Codec_UtVideo,
  Codec_Dirac,
  Codec_DNxHD,
  Codec_MSVideo1,
  Codec_8BPS,
  Codec_LOCO,
  Codec_ZMBV,
  Codec_VCR1,
  Codec_Snow,
  Codec_FFV1,
  Codec_v210,

  Codec_NB            // Number of entrys (do not use when dynamically linking)
};

// Codecs with hardware acceleration
typedef enum LAVVideoHWCodec {
  HWCodec_H264  = Codec_H264,
  HWCodec_VC1   = Codec_VC1,
  HWCodec_MPEG2 = Codec_MPEG2,
  HWCodec_MPEG4 = Codec_MPEG4,

  HWCodec_NB    = HWCodec_MPEG4 + 1
};

// Type of hardware accelerations
typedef enum LAVHWAccel {
  HWAccel_None,
  HWAccel_CUDA,
  HWAccel_QuickSync,
  HWAccel_DXVA2,
  HWAccel_DXVA2CopyBack = HWAccel_DXVA2,
  HWAccel_DXVA2Native
};

// Deinterlace algorithms offered by the hardware decoders
typedef enum LAVHWDeintModes {
  HWDeintMode_Weave,
  HWDeintMode_BOB,
  HWDeintMode_Hardware
};

// Software deinterlacing algorithms
typedef enum LAVSWDeintModes {
  SWDeintMode_None,
  SWDeintMode_YADIF
};

// Type of deinterlacing to perform
// - FramePerField re-constructs one frame from every field, resulting in 50/60 fps.
// - FramePer2Field re-constructs one frame from every 2 fields, resulting in 25/30 fps.
// Note: Weave will always use FramePer2Field
typedef enum LAVDeintOutput {
  DeintOutput_FramePerField,
  DeintOutput_FramePer2Field
};

// Control the field order of the deinterlacer
typedef enum LAVDeintFieldOrder {
  DeintFieldOrder_Auto,
  DeintFieldOrder_TopFieldFirst,
  DeintFieldOrder_BottomFieldFirst,
};

// Supported output pixel formats
typedef enum LAVOutPixFmts {
  LAVOutPixFmt_None = -1,
  LAVOutPixFmt_YV12,            // 4:2:0, 8bit, planar
  LAVOutPixFmt_NV12,            // 4:2:0, 8bit, Y planar, U/V packed
  LAVOutPixFmt_YUY2,            // 4:2:2, 8bit, packed
  LAVOutPixFmt_UYVY,            // 4:2:2, 8bit, packed
  LAVOutPixFmt_AYUV,            // 4:4:4, 8bit, packed
  LAVOutPixFmt_P010,            // 4:2:0, 10bit, Y planar, U/V packed
  LAVOutPixFmt_P210,            // 4:2:2, 10bit, Y planar, U/V packed
  LAVOutPixFmt_Y410,            // 4:4:4, 10bit, packed
  LAVOutPixFmt_P016,            // 4:2:0, 16bit, Y planar, U/V packed
  LAVOutPixFmt_P216,            // 4:2:2, 16bit, Y planar, U/V packed
  LAVOutPixFmt_Y416,            // 4:4:4, 16bit, packed
  LAVOutPixFmt_RGB32,           // 32-bit RGB (BGRA)
  LAVOutPixFmt_RGB24,           // 24-bit RGB (BGR)

  LAVOutPixFmt_v210,            // 4:2:2, 10bit, packed
  LAVOutPixFmt_v410,            // 4:4:4, 10bit, packed

  LAVOutPixFmt_YV16,            // 4:2:2, 8-bit, planar
  LAVOutPixFmt_YV24,            // 4:4:4, 8-bit, planar

  LAVOutPixFmt_I420,            // 4:2:0, 8bit, planar

  LAVOutPixFmt_NB               // Number of formats
} LAVOutPixFmts;

typedef enum LAVDitherMode {
  LAVDither_Ordered,
  LAVDither_Random
} LAVDitherMode;

// LAV Video configuration interface
[uuid("FA40D6E9-4D38-4761-ADD2-71A9EC5FD32F")]
interface ILAVVideoSettings : public IUnknown
{
   // Get the number of threads to use for Multi-Threaded decoding (where available)
  //  0 = Auto Detect (based on number of CPU cores)
  //  1 = 1 Thread -- No Multi-Threading
  // >1 = Multi-Threading with the specified number of threads
  STDMETHOD_(DWORD,GetNumThreads)() = 0;


  // Get wether the aspect ratio encoded in the stream should be forwarded to the renderer,
  // or the aspect ratio specified by the source filter should be kept.
  // TRUE  = AR from the Stream
  // FALSE = AR from the source filter
  STDMETHOD_(BOOL,GetStreamAR)() = 0;

  // Configure which pixel formats are enabled for output
  // If pixFmt is invalid, Get will return FALSE and Set E_FAIL
  STDMETHOD_(BOOL,GetPixelFormat)(LAVOutPixFmts pixFmt) = 0;


  // Get the RGB output range for the YUV->RGB conversion
  // 0 = Auto (same as input), 1 = Limited (16-235), 2 = Full (0-255)
  STDMETHOD_(DWORD,GetRGBOutputRange)() = 0;


  // get the deinterlacing field order of the hardware decoder
  STDMETHOD_(LAVDeintFieldOrder, GetDeintFieldOrder)() = 0;


  // Get wether all frames should be deinterlaced if the stream is flagged interlaced
  STDMETHOD_(BOOL, GetDeintAggressive)() = 0;


  // Get wether all frames should be deinterlaced, even ones marked as progressive
  STDMETHOD_(BOOL, GetDeintForce)() = 0;


  // Get which HW Accel method is active
  STDMETHOD_(LAVHWAccel, GetHWAccel)() = 0;


  // Get which codecs should use HW Acceleration
  STDMETHOD_(BOOL, GetHWAccelCodec)(LAVVideoHWCodec hwAccelCodec) = 0;


  // Get the deinterlacing mode used by the hardware decoder
  STDMETHOD_(LAVHWDeintModes, GetHWAccelDeintMode)() = 0;


  // Get the deinterlacing output for the hardware decoder
  STDMETHOD_(LAVDeintOutput, GetHWAccelDeintOutput)() = 0;


  // Get wether the hardware decoder should force high-quality deinterlacing
  // Note: this option is not supported on all decoder implementations and/or all operating systems
  STDMETHOD_(BOOL, GetHWAccelDeintHQ)() = 0;


  // Get the software deinterlacing mode used
  STDMETHOD_(LAVSWDeintModes, GetSWDeintMode)() = 0;


  // Get the software deinterlacing output
  STDMETHOD_(LAVDeintOutput, GetSWDeintOutput)() = 0;


  // Get wether all content is treated as progressive, and any interlaced flags are ignored
  STDMETHOD_(BOOL, GetDeintTreatAsProgressive)() = 0;


  // Get the dithering mode used
  STDMETHOD_(LAVDitherMode, GetDitherMode)() = 0;


  // Get if the MS WMV9 DMO Decoder should be used for VC-1/WMV3
  STDMETHOD_(BOOL, GetUseMSWMV9Decoder)() = 0;
};

// LAV Video status interface
[uuid("1CC2385F-36FA-41B1-9942-5024CE0235DC")]
interface ILAVVideoStatus : public IUnknown
{
  // Get the name of the active decoder (can return NULL if none is active)
  STDMETHOD_(const WCHAR *, GetActiveDecoderName)() = 0;
};
