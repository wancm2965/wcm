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

#include "LAVAudioSettings.h"
#include "FloatingAverage.h"
#include "Media.h"
#include "BitstreamParser.h"
#include "PostProcessor.h"


//////////////////// Configuration //////////////////////////

// Buffer Size for decoded PCM: 1s of 192kHz 32-bit with 8 channels
// 192000 (Samples) * 4 (Bytes per Sample) * 8 (channels)
#define LAV_AUDIO_BUFFER_SIZE 6144000

#define REQUEST_FLOAT 1

// Maximum Durations (in reference time)
// 10ms (DTS has 10.6667 ms samples, don't want to queue them up)
#define PCM_BUFFER_MAX_DURATION 100000
// 4ms
#define PCM_BUFFER_MIN_DURATION 40000

// Maximum desync that we attribute to jitter before re-syncing (10ms)
#define MAX_JITTER_DESYNC 100000i64

//////////////////// End Configuration //////////////////////

#define CODEC_ID_PCM_SxxBE (CodecID)0x19001
#define CODEC_ID_PCM_SxxLE (CodecID)0x19002
#define CODEC_ID_PCM_UxxBE (CodecID)0x19003
#define CODEC_ID_PCM_UxxLE (CodecID)0x19004
#define CODEC_ID_PCM_QTRAW (CodecID)0x19005

#define LAVC_AUDIO_REGISTRY_KEY L"Software\\LAV\\Audio"
#define LAVC_AUDIO_REGISTRY_KEY_FORMATS L"Software\\LAV\\Audio\\Formats"
#define LAVC_AUDIO_LOG_FILE     L"LAVAudio.txt"

struct WAVEFORMATEX_HDMV_LPCM;

struct BufferDetails {
  GrowableArray<BYTE>   *bBuffer;         // PCM Buffer
  LAVAudioSampleFormat  sfFormat;         // Sample Format
  WORD                  wBitsPerSample;   // Bits per sample
  DWORD                 dwSamplesPerSec;  // Samples per second
  unsigned              nSamples;         // Samples in the buffer (every sample is sizeof(sfFormat) * nChannels in the buffer)
  WORD                  wChannels;        // Number of channels
  DWORD                 dwChannelMask;    // channel mask
  REFERENCE_TIME        rtStart;          // Start Time of the buffer


  BufferDetails() : bBuffer(NULL), sfFormat(SampleFormat_16), wBitsPerSample(0), dwSamplesPerSec(0), wChannels(0), dwChannelMask(0), nSamples(0), rtStart(AV_NOPTS_VALUE) {
    bBuffer = new GrowableArray<BYTE>();
  };
  ~BufferDetails() {
    delete bBuffer;
  }
};

// Copy the given data into our buffer, including padding, so broken decoders do not overread and crash
#define COPY_TO_BUFFER(data, size) { \
  if (size > m_nFFBufferSize) { \
    m_nFFBufferSize = size; \
    m_pFFBuffer = (BYTE*)av_realloc_f(m_pFFBuffer, m_nFFBufferSize + FF_INPUT_BUFFER_PADDING_SIZE, 1); \
    if (!m_pFFBuffer) { \
      m_nFFBufferSize = 0; \
      return E_FAIL; \
    } \
  }\
  memcpy(m_pFFBuffer, data, size); \
  memset(m_pFFBuffer+size, 0, FF_INPUT_BUFFER_PADDING_SIZE); \
}

struct DTSDecoder;

[uuid("E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491")]
class CLAVAudio : public CTransformFilter, public ILAVAudioSettings, public ILAVAudioStatus
{
public:
  CLAVAudio(LPUNKNOWN pUnk, HRESULT* phr);
  ~CLAVAudio();

  // IUnknown
  DECLARE_IUNKNOWN;
  STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);


  // ILAVAudioSettings
  STDMETHODIMP GetDRC(BOOL *pbDRCEnabled, int *piDRCLevel);
  STDMETHODIMP_(BOOL) GetFormatConfiguration(LAVAudioCodec aCodec);
  STDMETHODIMP_(BOOL) GetBitstreamConfig(LAVBitstreamCodec bsCodec);
  STDMETHODIMP_(BOOL) GetDTSHDFraming();
  STDMETHODIMP_(BOOL) GetAutoAVSync();
  STDMETHODIMP_(BOOL) GetOutputStandardLayout();
  STDMETHODIMP_(BOOL) GetExpandMono();
  STDMETHODIMP_(BOOL) GetExpand61();
  STDMETHODIMP_(BOOL) GetAllowRawSPDIFInput();
  STDMETHODIMP_(BOOL) GetSampleFormat(LAVAudioSampleFormat format);
  STDMETHODIMP GetAudioDelay(BOOL *pbEnabled, int *pDelay);

  // ILAVAudioStatus
  STDMETHODIMP_(BOOL) IsSampleFormatSupported(LAVAudioSampleFormat sfCheck);
  STDMETHODIMP GetDecodeDetails(const char **pCodec, const char **pDecodeFormat, int *pnChannels, int *pSampleRate, DWORD *pChannelMask);
  STDMETHODIMP GetOutputDetails(const char **pOutputFormat, int *pnChannels, int *pSampleRate, DWORD *pChannelMask);
  STDMETHODIMP EnableVolumeStats();
  STDMETHODIMP DisableVolumeStats();
  STDMETHODIMP GetChannelVolumeAverage(WORD nChannel, float *pfDb);

  // CTransformFilter
  HRESULT CheckInputType(const CMediaType* mtIn);
  HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
  HRESULT DecideBufferSize(IMemAllocator * pAllocator, ALLOCATOR_PROPERTIES *pprop);
  HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);

  HRESULT Receive(IMediaSample *pIn);

  // Optional Overrides
  HRESULT CheckConnect(PIN_DIRECTION dir, IPin *pPin);
  HRESULT SetMediaType(PIN_DIRECTION dir, const CMediaType *pmt);

  HRESULT EndOfStream();
  HRESULT BeginFlush();
  HRESULT EndFlush();
  HRESULT NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

  HRESULT StartStreaming();
  HRESULT StopStreaming();
  HRESULT BreakConnect(PIN_DIRECTION Dir);

public:
  // Pin Configuration
  const static AMOVIESETUP_MEDIATYPE    sudPinTypesIn[];
  const static int                      sudPinTypesInCount;
  const static AMOVIESETUP_MEDIATYPE    sudPinTypesOut[];
  const static int                      sudPinTypesOutCount;

private:
  HRESULT LoadDefaults();

  HRESULT ffmpeg_init(CodecID codec, const void *format, GUID format_type, DWORD formatlen);
  void ffmpeg_shutdown();

  CMediaType CreateMediaType(LAVAudioSampleFormat outputFormat, DWORD nSamplesPerSec, WORD nChannels, DWORD dwChannelMask, WORD wBitsPerSample = 0) const;
  HRESULT ReconnectOutput(long cbBuffer, CMediaType& mt);
  HRESULT ProcessBuffer(BOOL bEOF = FALSE);
  HRESULT Decode(const BYTE *p, int buffsize, int &consumed, HRESULT *hrDeliver);
  HRESULT PostProcess(BufferDetails *buffer);
  HRESULT GetDeliveryBuffer(IMediaSample **pSample, BYTE **pData);

  HRESULT QueueOutput(BufferDetails &buffer);
  HRESULT FlushOutput(BOOL bDeliver = TRUE);

  HRESULT Deliver(BufferDetails &buffer);

  void CreateBDLPCMHeader(BYTE *pBuf, const WAVEFORMATEX_HDMV_LPCM *wfex_lpcm) const;
  HRESULT ParseRealAudioHeader(const BYTE *extra, const size_t extralen);

  void UpdateVolumeStats(const BufferDetails &buffer);

  BOOL IsBitstreaming(CodecID codec);
  HRESULT InitBitstreaming();
  HRESULT ShutdownBitstreaming();
  static int BSWriteBuffer(void *opaque, uint8_t *buf, int buf_size);

  HRESULT CreateBitstreamContext(CodecID codec, WAVEFORMATEX *wfe);
  HRESULT UpdateBitstreamContext();
  HRESULT FreeBitstreamContext();

  HRESULT Bitstream(const BYTE *p, int buffsize, int &consumed, HRESULT *hrDeliver);
  HRESULT DeliverBitstream(CodecID codec, const BYTE *buffer, DWORD dwSize, DWORD dwFrameSize, REFERENCE_TIME rtStartInput, REFERENCE_TIME rtStopInput);

  CMediaType CreateBitstreamMediaType(CodecID codec, DWORD dwSampleRate);
  void ActivateDTSHDMuxing();

  HRESULT InitDTSDecoder();
  HRESULT FreeDTSDecoder();
  HRESULT FlushDTSDecoder(BOOL bReopen = FALSE);
  HRESULT DecodeDTS(const BYTE * const p, int buffsize, int &consumed, HRESULT *hrDeliver);
  int SafeDTSDecode(BYTE *pInput, int len, BYTE *pOutput, int unk1, int unk2, int *pBitdepth, int *pChannels, int *pCoreSampleRate, int *pUnk4, int *pHDSampleRate, int *pUnk5, int *pProfile);

  HRESULT CheckChannelLayoutConformity(DWORD dwLayout);
  HRESULT Create51Conformity(DWORD dwLayout);
  HRESULT Create61Conformity(DWORD dwLayout);
  HRESULT Create71Conformity(DWORD dwLayout);

  LAVAudioSampleFormat GetBestAvailableSampleFormat(LAVAudioSampleFormat inFormat, BOOL bNoFallback = FALSE);
  HRESULT ConvertSampleFormat(BufferDetails *pcm, LAVAudioSampleFormat outputFormat);
  HRESULT Truncate32Buffer(BufferDetails *buffer);

private:
  CodecID              m_nCodecId;       // FFMPEG Codec Id
  AVCodec              *m_pAVCodec;      // AVCodec reference
  AVCodecContext       *m_pAVCtx;        // AVCodecContext reference
  AVCodecParserContext *m_pParser;       // AVCodecParserContext reference

  AVFrame              *m_pFrame;        // AVFrame used for decoding

  BYTE                 *m_pFFBuffer;     // FFMPEG processing buffer (padded)
	int                  m_nFFBufferSize;  // Size of the FFMPEG processing buffer

  BOOL                 m_bDiscontinuity; // Discontinuity
  REFERENCE_TIME       m_rtStart;        // Start time
  double               m_dStartOffset;   // Start time offset (extra precision)
  double               m_dRate;

  REFERENCE_TIME       m_rtStartInput;   // rtStart of the last input package
  REFERENCE_TIME       m_rtStopInput;    // rtStop of the last input package

  BOOL                 m_bUpdateTimeCache;
  REFERENCE_TIME       m_rtStartInputCache;   // rtStart of the last input package
  REFERENCE_TIME       m_rtStopInputCache;    // rtStop of the last input package
  REFERENCE_TIME       m_rtBitstreamCache;    // Bitstreaming time cache

  GrowableArray<BYTE>  m_buff;           // Input Buffer
  LAVAudioSampleFormat     m_DecodeFormat;  // Number of bits in the samples
  BOOL                 m_bFallback16Int;

  BOOL                 m_bSampleSupport[SampleFormat_NB];

  // Settings
  struct AudioSettings {
    BOOL DRCEnabled;
    int DRCLevel;
    BOOL bFormats[AudioCodec_NB];
    BOOL bBitstream[Bitstream_NB];
    BOOL DTSHDFraming;
    BOOL AutoAVSync;
    BOOL ExpandMono;
    BOOL Expand61;
    BOOL OutputStandardLayout;
    BOOL AllowRawSPDIF;
    BOOL bSampleFormats[SampleFormat_NB];
    BOOL AudioDelayEnabled;
    int  AudioDelay;
  } m_settings;
  BOOL                m_bRuntimeConfig;

  BOOL                m_bVolumeStats;    // Volume Stats gathering enabled
  FloatingAverage<float> m_faVolume[8];     // Floating Average for volume (8 channels)

  BOOL                m_bQueueResync;
  BOOL                m_bNeedSyncpoint;
  BufferDetails       m_OutputQueue;

  AVIOContext *m_avioBitstream;
  GrowableArray<BYTE> m_bsOutput;

  AVFormatContext     *m_avBSContext;

  BOOL                m_bDTSHD;
  CBitstreamParser    m_bsParser;
  BOOL                m_bFindDTSInPCM;

  FloatingAverage<REFERENCE_TIME> m_faJitter;
  REFERENCE_TIME      m_JitterLimit;

  HMODULE             m_hDllExtraDecoder;

  DTSDecoder          *m_pDTSDecoderContext;
  unsigned            m_DTSBitDepth;
  unsigned            m_DTSDecodeChannels;

  DWORD               m_DecodeLayout;
  DWORD               m_DecodeLayoutSanified;
  BOOL                m_bChannelMappingRequired;

  ExtendedChannelMap  m_ChannelMap;
  int                 m_ChannelMapOutputChannels;
  DWORD               m_ChannelMapOutputLayout;

  struct {
    int flavor;
    int coded_frame_size;
    int audio_framesize;
    int sub_packet_h;
    int sub_packet_size;
    unsigned int deint_id;
  } m_raData;
};
