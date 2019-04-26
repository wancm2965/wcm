

#ifndef AVCONCODEC_H
#define AVCONCODEC_H

#include "inttypes.h"
#include "avconmkvlib.h"
#include "rational.h"
#include "log.h"
//typedef unsigned __int64 uint64_t

#define AV_NOPTS_VALUE			0x8000000000000000
#define AV_TIME_BASE            1000000

#ifdef __cplusplus
 extern "C" {
#endif

enum CodecType {
	CODEC_TYPE_UNKNOWN = -1,
	CODEC_TYPE_VIDEO,
	CODEC_TYPE_AUDIO,
	CODEC_TYPE_DATA,
	CODEC_TYPE_SUBTITLE,
};

enum CodecID {
    CODEC_ID_NONE,
    CODEC_ID_MPEG1VIDEO,
    CODEC_ID_MPEG2VIDEO, /* prefered ID for MPEG Video 1 or 2 decoding */
    CODEC_ID_MPEG2VIDEO_XVMC,
    CODEC_ID_H261,
    CODEC_ID_H263,
    CODEC_ID_RV10,
    CODEC_ID_RV20,
    CODEC_ID_MJPEG,
    CODEC_ID_MJPEGB,
    CODEC_ID_LJPEG,
    CODEC_ID_SP5X,
    CODEC_ID_JPEGLS,
    CODEC_ID_MPEG4,
    CODEC_ID_RAWVIDEO,
    CODEC_ID_MSMPEG4V1,
    CODEC_ID_MSMPEG4V2,
    CODEC_ID_MSMPEG4V3,
    CODEC_ID_WMV1,
    CODEC_ID_WMV2,
    CODEC_ID_H263P,
    CODEC_ID_H263I,
    CODEC_ID_FLV1,
    CODEC_ID_SVQ1,
    CODEC_ID_SVQ3,
    CODEC_ID_DVVIDEO,
    CODEC_ID_HUFFYUV,
    CODEC_ID_CYUV,
    CODEC_ID_H264,
    CODEC_ID_INDEO3,
    CODEC_ID_VP3,
    CODEC_ID_THEORA,
    CODEC_ID_ASV1,
    CODEC_ID_ASV2,
    CODEC_ID_FFV1,
    CODEC_ID_4XM,
    CODEC_ID_VCR1,
    CODEC_ID_CLJR,
    CODEC_ID_MDEC,
    CODEC_ID_ROQ,
    CODEC_ID_INTERPLAY_VIDEO,
    CODEC_ID_XAN_WC3,
    CODEC_ID_XAN_WC4,
    CODEC_ID_RPZA,
    CODEC_ID_CINEPAK,
    CODEC_ID_WS_VQA,
    CODEC_ID_MSRLE,
    CODEC_ID_MSVIDEO1,
    CODEC_ID_IDCIN,
    CODEC_ID_8BPS,
    CODEC_ID_SMC,
    CODEC_ID_FLIC,
    CODEC_ID_TRUEMOTION1,
    CODEC_ID_VMDVIDEO,
    CODEC_ID_MSZH,
    CODEC_ID_ZLIB,
    CODEC_ID_QTRLE,
    CODEC_ID_SNOW,
    CODEC_ID_TSCC,
    CODEC_ID_ULTI,
    CODEC_ID_QDRAW,
    CODEC_ID_VIXL,
    CODEC_ID_QPEG,
    CODEC_ID_XVID,
    CODEC_ID_PNG,
    CODEC_ID_PPM,
    CODEC_ID_PBM,
    CODEC_ID_PGM,
    CODEC_ID_PGMYUV,
    CODEC_ID_PAM,
    CODEC_ID_FFVHUFF,
    CODEC_ID_RV30,
    CODEC_ID_RV40,
    CODEC_ID_VC1,
    CODEC_ID_WMV3,
    CODEC_ID_LOCO,
    CODEC_ID_WNV1,
    CODEC_ID_AASC,
    CODEC_ID_INDEO2,
    CODEC_ID_FRAPS,
    CODEC_ID_TRUEMOTION2,
    CODEC_ID_BMP,
    CODEC_ID_CSCD,
    CODEC_ID_MMVIDEO,
    CODEC_ID_ZMBV,
    CODEC_ID_AVS,
    CODEC_ID_SMACKVIDEO,
    CODEC_ID_NUV,
    CODEC_ID_KMVC,
    CODEC_ID_FLASHSV,
    CODEC_ID_CAVS,
    CODEC_ID_JPEG2000,
    CODEC_ID_VMNC,
    CODEC_ID_VP5,
    CODEC_ID_VP6,
    CODEC_ID_VP6F,
    CODEC_ID_TARGA,
    CODEC_ID_DSICINVIDEO,
    CODEC_ID_TIERTEXSEQVIDEO,
    CODEC_ID_TIFF,
    CODEC_ID_GIF,
    CODEC_ID_FFH264,
	CODEC_ID_DXA,
    CODEC_ID_DNXHD,
    CODEC_ID_THP,
    CODEC_ID_SGI,
    CODEC_ID_C93,
    CODEC_ID_BETHSOFTVID,
    CODEC_ID_PTX,
    CODEC_ID_TXD,
    CODEC_ID_VP6A,
    CODEC_ID_AMV,
    CODEC_ID_VB,
    CODEC_ID_PCX,
    CODEC_ID_SUNRAST,
    CODEC_ID_INDEO4,
    CODEC_ID_INDEO5,
    CODEC_ID_MIMIC,
    CODEC_ID_RL2,
    CODEC_ID_8SVX_EXP,
    CODEC_ID_8SVX_FIB,
    CODEC_ID_ESCAPE124,
    CODEC_ID_DIRAC,
    CODEC_ID_BFI,
    CODEC_ID_CMV,
    CODEC_ID_MOTIONPIXELS,
    CODEC_ID_TGV,
    CODEC_ID_TGQ,
    CODEC_ID_TQI,
    CODEC_ID_AURA,
    CODEC_ID_AURA2,
    CODEC_ID_V210X,
    CODEC_ID_TMV,
    CODEC_ID_V210,
    CODEC_ID_DPX,
    CODEC_ID_MAD,
    CODEC_ID_FRWU,
    CODEC_ID_FLASHSV2,
    CODEC_ID_CDGRAPHICS,
    CODEC_ID_R210,
    CODEC_ID_ANM,
    CODEC_ID_BINKVIDEO,
    CODEC_ID_IFF_ILBM,
    CODEC_ID_IFF_BYTERUN1,
    CODEC_ID_KGV1,
    CODEC_ID_YOP,
    CODEC_ID_VP8,
    CODEC_ID_PICTOR,
    CODEC_ID_ANSI,
    CODEC_ID_A64_MULTI,
    CODEC_ID_A64_MULTI5,
    CODEC_ID_R10K,
    CODEC_ID_MXPEG,
    CODEC_ID_LAGARITH,

    /* various pcm "codecs" */
    CODEC_ID_PCM_S16LE= 0x10000,
    CODEC_ID_PCM_S16BE,
    CODEC_ID_PCM_U16LE,
    CODEC_ID_PCM_U16BE,
    CODEC_ID_PCM_S8,
    CODEC_ID_PCM_U8,
    CODEC_ID_PCM_MULAW,
    CODEC_ID_PCM_ALAW,
    CODEC_ID_PCM_S32LE,
    CODEC_ID_PCM_S32BE,
    CODEC_ID_PCM_U32LE,
    CODEC_ID_PCM_U32BE,
    CODEC_ID_PCM_S24LE,
    CODEC_ID_PCM_S24BE,
    CODEC_ID_PCM_U24LE,
    CODEC_ID_PCM_U24BE,
    CODEC_ID_PCM_S24DAUD,
	CODEC_ID_PCM_ZORK,
    CODEC_ID_PCM_S16LE_PLANAR,
    CODEC_ID_PCM_DVD,
    CODEC_ID_PCM_F32BE,
    CODEC_ID_PCM_F32LE,
    CODEC_ID_PCM_F64BE,
    CODEC_ID_PCM_F64LE,
    CODEC_ID_PCM_BLURAY,
    CODEC_ID_PCM_LXF,

    /* various adpcm codecs */
    CODEC_ID_ADPCM_IMA_QT= 0x11000,
    CODEC_ID_ADPCM_IMA_WAV,
    CODEC_ID_ADPCM_IMA_DK3,
    CODEC_ID_ADPCM_IMA_DK4,
    CODEC_ID_ADPCM_IMA_WS,
    CODEC_ID_ADPCM_IMA_SMJPEG,
    CODEC_ID_ADPCM_MS,
    CODEC_ID_ADPCM_4XM,
    CODEC_ID_ADPCM_XA,
    CODEC_ID_ADPCM_ADX,
    CODEC_ID_ADPCM_EA,
    CODEC_ID_ADPCM_G726,
    CODEC_ID_ADPCM_CT,
    CODEC_ID_ADPCM_SWF,
    CODEC_ID_ADPCM_YAMAHA,
    CODEC_ID_ADPCM_SBPRO_4,
    CODEC_ID_ADPCM_SBPRO_3,
    CODEC_ID_ADPCM_SBPRO_2,

    /* AMR */
    CODEC_ID_AMR_NB= 0x12000,
    CODEC_ID_AMR_WB,

    /* RealAudio codecs*/
    CODEC_ID_RA_144= 0x13000,
    CODEC_ID_RA_288,

    /* various DPCM codecs */
    CODEC_ID_ROQ_DPCM= 0x14000,
    CODEC_ID_INTERPLAY_DPCM,
    CODEC_ID_XAN_DPCM,
    CODEC_ID_SOL_DPCM,

    CODEC_ID_MP2= 0x15000,
    CODEC_ID_MP3, /* prefered ID for MPEG Audio layer 1, 2 or3 decoding */
    CODEC_ID_AAC,
//#if LIBAVCODEC_VERSION_INT < ((52<<16)+(0<<8)+0)
    CODEC_ID_MPEG4AAC,
//#endif
    CODEC_ID_AC3,
    CODEC_ID_DTS,
    CODEC_ID_VORBIS,
    CODEC_ID_DVAUDIO,
    CODEC_ID_WMAV1,
    CODEC_ID_WMAV2,
    CODEC_ID_MACE3,
    CODEC_ID_MACE6,
    CODEC_ID_VMDAUDIO,
    CODEC_ID_SONIC,
    CODEC_ID_SONIC_LS,
    CODEC_ID_FLAC,
    CODEC_ID_MP3ADU,
    CODEC_ID_MP3ON4,
    CODEC_ID_SHORTEN,
    CODEC_ID_ALAC,
    CODEC_ID_WESTWOOD_SND1,
    CODEC_ID_GSM,
    CODEC_ID_QDM2,
    CODEC_ID_COOK,
    CODEC_ID_TRUESPEECH,
    CODEC_ID_TTA,
    CODEC_ID_SMACKAUDIO,
    CODEC_ID_QCELP,
    CODEC_ID_WAVPACK,
    CODEC_ID_DSICINAUDIO,
    CODEC_ID_IMC,
    CODEC_ID_MUSEPACK7,
    CODEC_ID_MLP,
	CODEC_ID_GSM_MS, /* as found in WAV */
    CODEC_ID_ATRAC3,
    CODEC_ID_VOXWARE,
    CODEC_ID_APE,
    CODEC_ID_NELLYMOSER,
    CODEC_ID_MUSEPACK8,
    CODEC_ID_SPEEX,
    CODEC_ID_WMAVOICE,
    CODEC_ID_WMAPRO,
    CODEC_ID_WMALOSSLESS,
    CODEC_ID_ATRAC3P,
    CODEC_ID_EAC3,
    CODEC_ID_SIPR,
    CODEC_ID_MP1,
    CODEC_ID_TWINVQ,
    CODEC_ID_TRUEHD,
    CODEC_ID_MP4ALS,
    CODEC_ID_ATRAC1,
    CODEC_ID_BINKAUDIO_RDFT,
    CODEC_ID_BINKAUDIO_DCT,
    CODEC_ID_AAC_LATM,

    /* subtitle codecs */
    CODEC_ID_DVD_SUBTITLE= 0x17000,
	CODEC_ID_DVB_SUBTITLE,
    CODEC_ID_TEXT,  ///< raw UTF-8 text
    CODEC_ID_XSUB,
    CODEC_ID_SSA,
    CODEC_ID_MOV_TEXT,
    CODEC_ID_HDMV_PGS_SUBTITLE,
    CODEC_ID_DVB_TELETEXT,
    CODEC_ID_SRT,

	CODEC_ID_TTF= 0x18000,
	
    CODEC_ID_PROBE= 0x19000, ///< codec_id is not known (like CODEC_ID_NONE) but lavf should attempt to identify it

    CODEC_ID_MPEG2TS= 0x20000, /* _FAKE_ codec to indicate a raw MPEG2 transport
                         stream (only used by libavformat) */
    CODEC_ID_FFMETADATA=0x21000,   ///< Dummy codec for streams containing only metadata information.
};
enum SampleFormat {
    SAMPLE_FMT_NONE = -1,
		SAMPLE_FMT_U8,              ///< unsigned 8 bits
		SAMPLE_FMT_S16,             ///< signed 16 bits
		SAMPLE_FMT_S24,             ///< signed 24 bits
		SAMPLE_FMT_S32,             ///< signed 32 bits
		SAMPLE_FMT_FLT,             ///< float
};
/* CODEC_ID_MP3LAME is absolete */
#define CODEC_ID_MP3LAME CODEC_ID_MP3

#define STRIDE_ALIGN 8

#define CODEC_FLAG_QSCALE 0x0002  ///< use fixed qscale
#define CODEC_FLAG_4MV    0x0004  ///< 4 MV per MB allowed / Advanced prediction for H263
#define CODEC_FLAG_QPEL   0x0010  ///< use qpel MC
#define CODEC_FLAG_GMC    0x0020  ///< use GMC
#define CODEC_FLAG_MV0    0x0040  ///< always try a MB with MV=<0,0>
#define CODEC_FLAG_PART   0x0080  ///< use data partitioning
/* parent program gurantees that the input for b-frame containing streams is not written to
   for at least s->max_b_frames+1 frames, if this is not set than the input will be copied */
#define CODEC_FLAG_INPUT_PRESERVED 0x0100
#define CODEC_FLAG_PASS1 0x0200   ///< use internal 2pass ratecontrol in first  pass mode
#define CODEC_FLAG_PASS2 0x0400   ///< use internal 2pass ratecontrol in second pass mode
#define CODEC_FLAG_EXTERN_HUFF 0x1000 ///< use external huffman table (for mjpeg)
#define CODEC_FLAG_GRAY  0x2000   ///< only decode/encode grayscale
#define CODEC_FLAG_EMU_EDGE 0x4000///< don't draw edges
#define CODEC_FLAG_PSNR           0x8000 ///< error[?] variables will be set during encoding
#define CODEC_FLAG_TRUNCATED  0x00010000 /** input bitstream might be truncated at a random location instead
                                            of only at frame boundaries */
#define CODEC_FLAG_NORMALIZE_AQP  0x00020000 ///< normalize adaptive quantization
#define CODEC_FLAG_INTERLACED_DCT 0x00040000 ///< use interlaced dct
#define CODEC_FLAG_LOW_DELAY      0x00080000 ///< force low delay
#define CODEC_FLAG_ALT_SCAN       0x00100000 ///< use alternate scan
#define CODEC_FLAG_TRELLIS_QUANT  0x00200000 ///< use trellis quantization
#define CODEC_FLAG_GLOBAL_HEADER  0x00400000 ///< place global headers in extradata instead of every keyframe
#define CODEC_FLAG_BITEXACT       0x00800000 ///< use only bitexact stuff (except (i)dct)
/* Fx : Flag for h263+ extra options */
#define CODEC_FLAG_H263P_AIC      0x01000000 ///< H263 Advanced intra coding / MPEG4 AC prediction (remove this)
#define CODEC_FLAG_AC_PRED        0x01000000 ///< H263 Advanced intra coding / MPEG4 AC prediction
#define CODEC_FLAG_H263P_UMV      0x02000000 ///< Unlimited motion vector
#define CODEC_FLAG_CBP_RD         0x04000000 ///< use rate distortion optimization for cbp
#define CODEC_FLAG_QP_RD          0x08000000 ///< use rate distortion optimization for qp selectioon
#define CODEC_FLAG_H263P_AIV      0x00000008 ///< H263 Alternative inter vlc
#define CODEC_FLAG_OBMC           0x00000001 ///< OBMC
#define CODEC_FLAG_LOOP_FILTER    0x00000800 ///< loop filter
#define CODEC_FLAG_H263P_SLICE_STRUCT 0x10000000
#define CODEC_FLAG_INTERLACED_ME  0x20000000 ///< interlaced motion estimation
#define CODEC_FLAG_SVCD_SCAN_OFFSET 0x40000000 ///< will reserve space for SVCD scan offset user data
#define CODEC_FLAG_CLOSED_GOP     ((int)0x80000000)
#define CODEC_FLAG2_FAST          0x00000001 ///< allow non spec compliant speedup tricks
#define CODEC_FLAG2_STRICT_GOP    0x00000002 ///< strictly enforce GOP size
#define CODEC_FLAG2_NO_OUTPUT     0x00000004 ///< skip bitstream encoding
#define CODEC_FLAG2_LOCAL_HEADER  0x00000008 ///< place global headers at every keyframe instead of in extradata
#define CODEC_FLAG2_BPYRAMID      0x00000010 ///< H.264 allow b-frames to be used as references
#define CODEC_FLAG2_WPRED         0x00000020 ///< H.264 weighted biprediction for b-frames
#define CODEC_FLAG2_MIXED_REFS    0x00000040 ///< H.264 one reference per partition, as opposed to one reference per macroblock
#define CODEC_FLAG2_8X8DCT        0x00000080 ///< H.264 high profile 8x8 transform
#define CODEC_FLAG2_FASTPSKIP     0x00000100 ///< H.264 fast pskip
#define CODEC_FLAG2_AUD           0x00000200 ///< H.264 access unit delimiters
#define CODEC_FLAG2_BRDO          0x00000400 ///< b-frame rate-distortion optimization
#define CODEC_FLAG2_INTRA_VLC     0x00000800 ///< use MPEG-2 intra VLC table
#define CODEC_FLAG2_MEMC_ONLY     0x00001000 ///< only do ME/MC (I frames -> ref, P frame -> ME+MC)
#define CODEC_FLAG2_DROP_FRAME_TIMECODE 0x00002000 ///< timecode is in drop frame format
#define CODEC_FLAG2_SKIP_RD       0x00004000 ///< RD optimal MB level residual skiping


enum Motion_Est_ID {
		 ME_ZERO = 1,
			 ME_FULL,
			 ME_LOG,
			 ME_PHODS,
			 ME_EPZS,
			 ME_X1,
			 ME_HEX,
			 ME_UMH,
			 ME_ITER,
	 };
	 
	 enum AVDiscard{
		 //we leave some space between them for extensions (drop some keyframes for intra only or drop just some bidir frames)
		 AVDISCARD_NONE   =-16, ///< discard nothing
			 AVDISCARD_DEFAULT=  0, ///< discard useless packets like 0 size packets in avi
			 AVDISCARD_NONREF =  8, ///< discard all non reference
			 AVDISCARD_BIDIR  = 16, ///< discard all bidirectional frames
			 AVDISCARD_NONKEY = 32, ///< discard all frames except keyframes
			 AVDISCARD_ALL    = 48, ///< discard all
	 };
	 enum AVSampleFormat {
		 AV_SAMPLE_FMT_NONE = -1,
			 AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
			 AV_SAMPLE_FMT_S16,         ///< signed 16 bits
			 AV_SAMPLE_FMT_S32,         ///< signed 32 bits
			 AV_SAMPLE_FMT_FLT,         ///< float
			 AV_SAMPLE_FMT_DBL,         ///< double
			 AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if dynamically linking to libavcore
	 };
	 typedef struct RcOverride{
		 int start_frame;
		 int end_frame;
		 int qscale; // if this is 0 then quality_factor will be used instead
		 float quality_factor;
	 } RcOverride;
/**
 * Pan Scan area.
 * this specifies the area which should be displayed. Note there may be multiple such areas for one frame
 */
typedef struct AVPanScan{
    /**
     * id.
     * - encoding: set by user.
     * - decoding: set by lavc
     */
    int id;

    /**
     * width and height in 1/16 pel
     * - encoding: set by user.
     * - decoding: set by lavc
     */
    int width;
    int height;

    /**
     * position of the top left corner in 1/16 pel for up to 3 fields/frames.
     * - encoding: set by user.
     * - decoding: set by lavc
     */
    int16_t position[3][2];
}AVPanScan;

#define FF_COMMON_FRAME \
    /**\
     * pointer to the picture planes.\
     * this might be different from the first allocated byte\
     * - encoding: \
     * - decoding: \
     */\
    uint8_t *data[4];\
    int linesize[4];\
    /**\
     * pointer to the first allocated byte of the picture. can be used in get_buffer/release_buffer\
     * this isn't used by lavc unless the default get/release_buffer() is used\
     * - encoding: \
     * - decoding: \
     */\
    uint8_t *base[4];\
    /**\
     * 1 -> keyframe, 0-> not\
     * - encoding: set by lavc\
     * - decoding: set by lavc\
     */\
    int key_frame;\
\
    /**\
     * picture type of the frame, see ?_TYPE below.\
     * - encoding: set by lavc for coded_picture (and set by user for input)\
     * - decoding: set by lavc\
     */\
    int pict_type;\
\
    /**\
     * presentation timestamp in time_base units (time when frame should be shown to user)\
     * if AV_NOPTS_VALUE then frame_rate = 1/time_base will be assumed\
     * - encoding: MUST be set by user\
     * - decoding: set by lavc\
     */\
    int64_t pts;\
\
    /**\
     * picture number in bitstream order.\
     * - encoding: set by\
     * - decoding: set by lavc\
     */\
    int coded_picture_number;\
    /**\
     * picture number in display order.\
     * - encoding: set by\
     * - decoding: set by lavc\
     */\
    int display_picture_number;\
\
    /**\
     * quality (between 1 (good) and FF_LAMBDA_MAX (bad)) \
     * - encoding: set by lavc for coded_picture (and set by user for input)\
     * - decoding: set by lavc\
     */\
    int quality; \
\
    /**\
     * buffer age (1->was last buffer and dint change, 2->..., ...).\
     * set to INT_MAX if the buffer has not been used yet \
     * - encoding: unused\
     * - decoding: MUST be set by get_buffer()\
     */\
    int age;\
\
    /**\
     * is this picture used as reference\
     * - encoding: unused\
     * - decoding: set by lavc (before get_buffer() call))\
     */\
    int reference;\
\
    /**\
     * QP table\
     * - encoding: unused\
     * - decoding: set by lavc\
     */\
    int8_t *qscale_table;\
    /**\
     * QP store stride\
     * - encoding: unused\
     * - decoding: set by lavc\
     */\
    int qstride;\
\
    /**\
     * mbskip_table[mb]>=1 if MB didnt change\
     * stride= mb_width = (width+15)>>4\
     * - encoding: unused\
     * - decoding: set by lavc\
     */\
    uint8_t *mbskip_table;\
\
    /**\
     * Motion vector table.\
     * @code\
     * example:\
     * int mv_sample_log2= 4 - motion_subsample_log2;\
     * int mb_width= (width+15)>>4;\
     * int mv_stride= (mb_width << mv_sample_log2) + 1;\
     * motion_val[direction][x + y*mv_stride][0->mv_x, 1->mv_y];\
     * @endcode\
     * - encoding: set by user\
     * - decoding: set by lavc\
     */\
    int16_t (*motion_val[2])[2];\
\
    /**\
     * Macroblock type table\
     * mb_type_base + mb_width + 2\
     * - encoding: set by user\
     * - decoding: set by lavc\
     */\
    uint32_t *mb_type;\
\
    /**\
     * log2 of the size of the block which a single vector in motion_val represents: \
     * (4->16x16, 3->8x8, 2-> 4x4, 1-> 2x2)\
     * - encoding: unused\
     * - decoding: set by lavc\
     */\
    uint8_t motion_subsample_log2;\
\
    /**\
     * for some private data of the user\
     * - encoding: unused\
     * - decoding: set by user\
     */\
    void *opaque;\
\
    /**\
     * error\
     * - encoding: set by lavc if flags&CODEC_FLAG_PSNR\
     * - decoding: unused\
     */\
    uint64_t error[4];\
\
    /**\
     * type of the buffer (to keep track of who has to dealloc data[*])\
     * - encoding: set by the one who allocs it\
     * - decoding: set by the one who allocs it\
     * Note: user allocated (direct rendering) & internal buffers can not coexist currently\
     */\
    int type;\
    \
    /**\
     * when decoding, this signal how much the picture must be delayed.\
     * extra_delay = repeat_pict / (2*fps)\
     * - encoding: unused\
     * - decoding: set by lavc\
     */\
    int repeat_pict;\
    \
    /**\
     * \
     */\
    int qscale_type;\
    \
    /**\
     * The content of the picture is interlaced.\
     * - encoding: set by user\
     * - decoding: set by lavc (default 0)\
     */\
    int interlaced_frame;\
    \
    /**\
     * if the content is interlaced, is top field displayed first.\
     * - encoding: set by user\
     * - decoding: set by lavc\
     */\
    int top_field_first;\
    \
    /**\
     * Pan scan.\
     * - encoding: set by user\
     * - decoding: set by lavc\
     */\
    AVPanScan *pan_scan;\
    \
    /**\
     * tell user application that palette has changed from previous frame.\
     * - encoding: ??? (no palette-enabled encoder yet)\
     * - decoding: set by lavc (default 0)\
     */\
    int palette_has_changed;\
    \
    /**\
     * Codec suggestion on buffer type if != 0\
     * - encoding: unused\
     * - decoding: set by lavc (before get_buffer() call))\
     */\
    int buffer_hints;\
\
    /**\
     * DCT coeffitients\
     * - encoding: unused\
     * - decoding: set by lavc\
     */\
    short *dct_coeff;\
\
    /**\
     * Motion referece frame index\
     * - encoding: set by user\
     * - decoding: set by lavc\
     */\
    int8_t *ref_index[2];\
\
    /**\
     * Motion referece frame index\
     * - encoding: set by user\
     * - decoding: set by lavc\
     */\
    int64_t pkt_pts;\
\
    /**\
     * dts from the last AVPacket that has been input into the decoder\
     * - encoding: unused\
     * - decoding: Read by user.\
     */\
    int64_t pkt_dts;\

#define FF_QSCALE_TYPE_MPEG1 0
#define FF_QSCALE_TYPE_MPEG2 1
#define FF_QSCALE_TYPE_H264  2

#define FF_BUFFER_TYPE_INTERNAL 1
#define FF_BUFFER_TYPE_USER     2 ///< Direct rendering buffers (image is (de)allocated by user)
#define FF_BUFFER_TYPE_SHARED   4 ///< buffer from somewhere else, don't dealloc image (data/base), all other tables are not shared
#define FF_BUFFER_TYPE_COPY     8 ///< just a (modified) copy of some other buffer, don't dealloc anything


#define FF_I_TYPE 1 // Intra
#define FF_P_TYPE 2 // Predicted
#define FF_B_TYPE 3 // Bi-dir predicted
#define FF_S_TYPE 4 // S(GMC)-VOP MPEG4
#define FF_SI_TYPE 5
#define FF_SP_TYPE 6

#define FF_INPUT_BUFFER_PADDING_SIZE 8
#define FF_BUFFER_HINTS_VALID    0x01 // Buffer hints value is meaningful (if 0 ignore)
#define FF_BUFFER_HINTS_READABLE 0x02 // Codec will read from buffer
#define FF_BUFFER_HINTS_PRESERVE 0x04 // User must not alter buffer content
#define FF_BUFFER_HINTS_REUSABLE 0x08 // Codec will reuse the buffer (update)

/**
 * Audio Video Frame.
 */
typedef struct AVFrame {
    FF_COMMON_FRAME
}AVFrame;


typedef struct AVPacket1 {
		int64_t pts;                            ///< presentation time stamp in time_base units
		int64_t dts;                            ///< decompression time stamp in time_base units
		uint8_t *data;
		int   size;
		int   stream_index;
		int   flags;
		int   duration;                         ///< presentation duration in time_base units (0 if not available)
		void  (*destruct)(struct AVPacket *);
		void  *priv;
		int64_t pos;                            ///< byte position in stream, -1 if unknown
} AVPacket1;

/**
 * main external api structure.
 */
typedef struct AVCodecContext {
	    int rc_lookahead;
    /**
     * Info on struct for av_log
     * - set by avcodec_alloc_context
     */
       AVClass *av_class;
    /**
     * the average bitrate.
     * - encoding: set by user. unused for constant quantizer encoding
     * - decoding: set by lavc. 0 or some bitrate if this info is available in the stream
     */
      int bit_rate;

    /**
     * number of bits the bitstream is allowed to diverge from the reference.
     *           the reference can be CBR (for CBR pass1) or VBR (for pass2)
     * - encoding: set by user. unused for constant quantizer encoding
     * - decoding: unused
     */
    int bit_rate_tolerance;

    /**
     * CODEC_FLAG_*.
     * - encoding: set by user.
     * - decoding: set by user.
     */
    int flags;

    /**
     * some codecs needs additionnal format info. It is stored here
     * - encoding: set by user.
     * - decoding: set by lavc. (FIXME is this ok?)
     */
    int sub_id;

    /**
     * motion estimation algorithm used for video coding.
     * 1 (zero), 2 (full), 3 (log), 4 (phods), 5 (epzs), 6 (x1), 7 (hex),
     * 8 (umh), 9 (iter) [7, 8 are x264 specific, 9 is snow specific]
     * - encoding: MUST be set by user.
     * - decoding: unused
     */
    int me_method;

    /**
     * some codecs need / can use extra-data like huffman tables.
     * mjpeg: huffman tables
     * rv10: additional flags
     * mpeg4: global headers (they can be in the bitstream or here)
     * the allocated memory should be FF_INPUT_BUFFER_PADDING_SIZE bytes larger
     * then extradata_size to avoid prolems if its read with the bitstream reader
     * the bytewise contents of extradata must not depend on the architecture or cpu endianness
     * - encoding: set/allocated/freed by lavc.
     * - decoding: set/allocated/freed by user.
     */
    uint8_t *extradata;
    int extradata_size;
        int bits_per_raw_sample;
    /**
     * this is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. for fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identically 1.
     * - encoding: MUST be set by user
     * - decoding: set by lavc.
     */
    AVRational time_base;

    /* video only */
    /**
     * picture width / height.
     * - encoding: MUST be set by user.
     * - decoding: set by lavc.
     * Note, for compatibility its possible to set this instead of
     * coded_width/height before decoding
     */
    int width, height;

#define FF_ASPECT_EXTENDED 15

    /**
     * the number of pictures in a group of pitures, or 0 for intra_only.
     * - encoding: set by user.
     * - decoding: unused
     */
    int gop_size;

    /**
     * pixel format, see PIX_FMT_xxx.
     * - encoding: set by user.
     * - decoding: set by lavc.
     */
    enum PixelFormat pix_fmt;

    /**
     * Frame rate emulation. If not zero lower layer (i.e. format handler)
     * has to read frames at native frame rate.
     * - encoding: set by user.
     * - decoding: unused.
     */
    int rate_emu;

    /**
     * if non NULL, 'draw_horiz_band' is called by the libavcodec
     * decoder to draw an horizontal band. It improve cache usage. Not
     * all codecs can do that. You must check the codec capabilities
     * before
     * - encoding: unused
     * - decoding: set by user.
     * @param height the height of the slice
     * @param y the y position of the slice
     * @param type 1->top field, 2->bottom field, 3->frame
     * @param offset offset into the AVFrame.data from which the slice should be read
     */
    void (*draw_horiz_band)(struct AVCodecContext *s,
                            const AVFrame *src, int offset[4],
                            int y, int type, int height);

    /* audio only */
    int sample_rate; ///< samples per sec
    int channels;

    /**
     * audio sample format.
     * - encoding: set by user.
     * - decoding: set by lavc.
     */
    enum SampleFormat sample_fmt;  ///< sample format, currenly unused

    /* the following data should not be initialized */
    /**
     * samples per packet. initialized when calling 'init'
     */
    int frame_size;
    int frame_number;   ///< audio or video frame number
    int real_pict_num;  ///< returns the real picture number of previous encoded frame

    /**
     * number of frames the decoded output will be delayed relative to
     * the encoded input.
     * - encoding: set by lavc.
     * - decoding: unused
     */
    int delay;

    /* - encoding parameters */
    float qcompress;  ///< amount of qscale change between easy & hard scenes (0.0-1.0)
    float qblur;      ///< amount of qscale smoothing over time (0.0-1.0)

    /**
     * minimum quantizer.
     * - encoding: set by user.
     * - decoding: unused
     */
    int qmin;

    /**
     * maximum quantizer.
     * - encoding: set by user.
     * - decoding: unused
     */
    int qmax;

    /**
     * maximum quantizer difference between frames.
     * - encoding: set by user.
     * - decoding: unused
     */
    int max_qdiff;

    /**
     * maximum number of b frames between non b frames.
     * note: the output will be delayed by max_b_frames+1 relative to the input
     * - encoding: set by user.
     * - decoding: unused
     */
    int max_b_frames;

    /**
     * qscale factor between ip and b frames.
     * - encoding: set by user.
     * - decoding: unused
     */
    float b_quant_factor;

    /** obsolete FIXME remove */
    int rc_strategy;
#define FF_RC_STRATEGY_XVID 1

    int b_frame_strategy;

    /**
     * hurry up amount.
     * deprecated in favor of skip_idct and skip_frame
     * - encoding: unused
     * - decoding: set by user. 1-> skip b frames, 2-> skip idct/dequant too, 5-> skip everything except header
     */
    int hurry_up;

    struct AVCodec *codec;

    void *priv_data;

    /* unused, FIXME remove*/
    int rtp_mode;

    int rtp_payload_size;   /* The size of the RTP payload: the coder will  */
                            /* do it's best to deliver a chunk with size    */
                            /* below rtp_payload_size, the chunk will start */
                            /* with a start code on some codecs like H.263  */
                            /* This doesn't take account of any particular  */
                            /* headers inside the transmited RTP payload    */


    /* The RTP callback: This function is called   */
    /* every time the encoder has a packet to send */
    /* Depends on the encoder if the data starts   */
    /* with a Start Code (it should) H.263 does.   */
    /* mb_nb contains the number of macroblocks    */
    /* encoded in the RTP payload                  */
    void (*rtp_callback)(struct AVCodecContext *avctx, void *data, int size, int mb_nb);

    /* statistics, used for 2-pass encoding */
    int mv_bits;
    int header_bits;
    int i_tex_bits;
    int p_tex_bits;
    int i_count;
    int p_count;
    int skip_count;
    int misc_bits;

    /**
     * number of bits used for the previously encoded frame.
     * - encoding: set by lavc
     * - decoding: unused
     */
    int frame_bits;

    /**
     * private data of the user, can be used to carry app specific stuff.
     * - encoding: set by user
     * - decoding: set by user
     */
    void *opaque;

    char codec_name[32];
    enum CodecType codec_type; /* see CODEC_TYPE_xxx */
    enum CodecID codec_id; /* see CODEC_ID_xxx */

    /**
     * fourcc (LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
     * this is used to workaround some encoder bugs
     * - encoding: set by user, if not then the default based on codec_id will be used
     * - decoding: set by user, will be converted to upper case by lavc during init
     */
    unsigned int codec_tag;

    /**
     * workaround bugs in encoders which sometimes cannot be detected automatically.
     * - encoding: set by user
     * - decoding: set by user
     */
    int workaround_bugs;
#define FF_BUG_AUTODETECT       1  ///< autodetection
#define FF_BUG_OLD_MSMPEG4      2
#define FF_BUG_XVID_ILACE       4
#define FF_BUG_UMP4             8
#define FF_BUG_NO_PADDING       16
#define FF_BUG_AMV              32
#define FF_BUG_AC_VLC           0  ///< will be removed, libavcodec can now handle these non compliant files by default
#define FF_BUG_QPEL_CHROMA      64
#define FF_BUG_STD_QPEL         128
#define FF_BUG_QPEL_CHROMA2     256
#define FF_BUG_DIRECT_BLOCKSIZE 512
#define FF_BUG_EDGE             1024
#define FF_BUG_HPEL_CHROMA      2048
#define FF_BUG_DC_CLIP          4096
#define FF_BUG_MS               8192 ///< workaround various bugs in microsofts broken decoders
//#define FF_BUG_FAKE_SCALABILITY 16 //autodetection should work 100%

    /**
     * luma single coeff elimination threshold.
     * - encoding: set by user
     * - decoding: unused
     */
    int luma_elim_threshold;

    /**
     * chroma single coeff elimination threshold.
     * - encoding: set by user
     * - decoding: unused
     */
    int chroma_elim_threshold;

    /**
     * strictly follow the std (MPEG4, ...).
     * - encoding: set by user
     * - decoding: unused
     */
    int strict_std_compliance;
#define FF_COMPLIANCE_VERY_STRICT   2 ///< strictly conform to a older more strict version of the spec or reference software
#define FF_COMPLIANCE_STRICT        1 ///< strictly conform to all the things in the spec no matter what consequences
#define FF_COMPLIANCE_NORMAL        0
#define FF_COMPLIANCE_INOFFICIAL   -1 ///< allow inofficial extensions
#define FF_COMPLIANCE_EXPERIMENTAL -2 ///< allow non standarized experimental things

    /**
     * qscale offset between ip and b frames.
     * if > 0 then the last p frame quantizer will be used (q= lastp_q*factor+offset)
     * if < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset)
     * - encoding: set by user.
     * - decoding: unused
     */
    float b_quant_offset;

    /**
     * error resilience higher values will detect more errors but may missdetect
     * some more or less valid parts as errors.
     * - encoding: unused
     * - decoding: set by user
     */
    int error_resilience;
#define FF_ER_CAREFUL         1
#define FF_ER_COMPLIANT       2
#define FF_ER_AGGRESSIVE      3
#define FF_ER_VERY_AGGRESSIVE 4

    /**
     * called at the beginning of each frame to get a buffer for it.
     * if pic.reference is set then the frame will be read later by lavc
     * avcodec_align_dimensions() should be used to find the required width and
     * height, as they normally need to be rounded up to the next multiple of 16
     * - encoding: unused
     * - decoding: set by lavc, user can override
     */
    int (*get_buffer)(struct AVCodecContext *c, AVFrame *pic);

    /**
     * called to release buffers which where allocated with get_buffer.
     * a released buffer can be reused in get_buffer()
     * pic.data[*] must be set to NULL
     * - encoding: unused
     * - decoding: set by lavc, user can override
     */
    void (*release_buffer)(struct AVCodecContext *c, AVFrame *pic);

    /**
     * if 1 the stream has a 1 frame delay during decoding.
     * - encoding: set by lavc
     * - decoding: set by lavc
     */
    int has_b_frames;

    /**
     * number of bytes per packet if constant and known or 0
     * used by some WAV based audio codecs
     */
    int block_align;

    int parse_only; /* - decoding only: if true, only parsing is done
                       (function avcodec_parse_frame()). The frame
                       data is returned. Only MPEG codecs support this now. */

    /**
     * 0-> h263 quant 1-> mpeg quant.
     * - encoding: set by user.
     * - decoding: unused
     */
    int mpeg_quant;

    /**
     * pass1 encoding statistics output buffer.
     * - encoding: set by lavc
     * - decoding: unused
     */
    char *stats_out;

    /**
     * pass2 encoding statistics input buffer.
     * concatenated stuff from stats_out of pass1 should be placed here
     * - encoding: allocated/set/freed by user
     * - decoding: unused
     */
    char *stats_in;

    /**
     * ratecontrol qmin qmax limiting method.
     * 0-> clipping, 1-> use a nice continous function to limit qscale wthin qmin/qmax
     * - encoding: set by user.
     * - decoding: unused
     */
    float rc_qsquish;

    float rc_qmod_amp;
    int rc_qmod_freq;

    /**
     * ratecontrol override, see RcOverride.
     * - encoding: allocated/set/freed by user.
     * - decoding: unused
     */
    RcOverride *rc_override;
    int rc_override_count;

    /**
     * rate control equation.
     * - encoding: set by user
     * - decoding: unused
     */
    char *rc_eq;

    /**
     * maximum bitrate.
     * - encoding: set by user.
     * - decoding: unused
     */
    int rc_max_rate;

    /**
     * minimum bitrate.
     * - encoding: set by user.
     * - decoding: unused
     */
    int rc_min_rate;

    /**
     * decoder bitstream buffer size.
     * - encoding: set by user.
     * - decoding: unused
     */
    int rc_buffer_size;
    float rc_buffer_aggressivity;

    /**
     * qscale factor between p and i frames.
     * if > 0 then the last p frame quantizer will be used (q= lastp_q*factor+offset)
     * if < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset)
     * - encoding: set by user.
     * - decoding: unused
     */
    float i_quant_factor;

    /**
     * qscale offset between p and i frames.
     * - encoding: set by user.
     * - decoding: unused
     */
    float i_quant_offset;

    /**
     * initial complexity for pass1 ratecontrol.
     * - encoding: set by user.
     * - decoding: unused
     */
    float rc_initial_cplx;

    /**
     * dct algorithm, see FF_DCT_* below.
     * - encoding: set by user
     * - decoding: unused
     */
    int dct_algo;
#define FF_DCT_AUTO    0
#define FF_DCT_FASTINT 1
#define FF_DCT_INT     2
#define FF_DCT_MMX     3
#define FF_DCT_MLIB    4
#define FF_DCT_ALTIVEC 5
#define FF_DCT_FAAN    6

    /**
     * luminance masking (0-> disabled).
     * - encoding: set by user
     * - decoding: unused
     */
    float lumi_masking;

    /**
     * temporary complexity masking (0-> disabled).
     * - encoding: set by user
     * - decoding: unused
     */
    float temporal_cplx_masking;

    /**
     * spatial complexity masking (0-> disabled).
     * - encoding: set by user
     * - decoding: unused
     */
    float spatial_cplx_masking;

    /**
     * p block masking (0-> disabled).
     * - encoding: set by user
     * - decoding: unused
     */
    float p_masking;

    /**
     * darkness masking (0-> disabled).
     * - encoding: set by user
     * - decoding: unused
     */
    float dark_masking;


    /* for binary compatibility */
    int unused;

    /**
     * idct algorithm, see FF_IDCT_* below.
     * - encoding: set by user
     * - decoding: set by user
     */
    int idct_algo;
#define FF_IDCT_AUTO         0
#define FF_IDCT_INT          1
#define FF_IDCT_SIMPLE       2
#define FF_IDCT_SIMPLEMMX    3
#define FF_IDCT_LIBMPEG2MMX  4
#define FF_IDCT_PS2          5
#define FF_IDCT_MLIB         6
#define FF_IDCT_ARM          7
#define FF_IDCT_ALTIVEC      8
#define FF_IDCT_SH4          9
#define FF_IDCT_SIMPLEARM    10
#define FF_IDCT_H264         11
#define FF_IDCT_VP3          12
#define FF_IDCT_IPP          13
#define FF_IDCT_XVIDMMX      14
#define FF_IDCT_CAVS         15
#define FF_IDCT_SIMPLEARMV5TE 16
#define FF_IDCT_SIMPLEARMV6  17

    /**
     * slice count.
     * - encoding: set by lavc
     * - decoding: set by user (or 0)
     */
    int slice_count;
    /**
     * slice offsets in the frame in bytes.
     * - encoding: set/allocated by lavc
     * - decoding: set/allocated by user (or NULL)
     */
    int *slice_offset;

    /**
     * error concealment flags.
     * - encoding: unused
     * - decoding: set by user
     */
    int error_concealment;
#define FF_EC_GUESS_MVS   1
#define FF_EC_DEBLOCK     2

    /**
     * dsp_mask could be add used to disable unwanted CPU features
     * CPU features (i.e. MMX, SSE. ...)
     *
     * with FORCE flag you may instead enable given CPU features
     * (Dangerous: usable in case of misdetection, improper usage however will
     * result into program crash)
     */
    unsigned dsp_mask;
#define FF_MM_FORCE    0x80000000 /* force usage of selected flags (OR) */
    /* lower 16 bits - CPU features */
//#ifdef HAVE_MMX
#define FF_MM_MMX      0x0001 /* standard MMX */
#define FF_MM_3DNOW    0x0004 /* AMD 3DNOW */
#define FF_MM_MMXEXT   0x0002 /* SSE integer functions or AMD MMX ext */
#define FF_MM_SSE      0x0008 /* SSE functions */
#define FF_MM_SSE2     0x0010 /* PIV SSE2 functions */
#define FF_MM_3DNOWEXT 0x0020 /* AMD 3DNowExt */
//#endif /* HAVE_MMX */
#ifdef HAVE_IWMMXT
#define FF_MM_IWMMXT   0x0100 /* XScale IWMMXT */
#endif /* HAVE_IWMMXT */

    /**
     * bits per sample/pixel from the demuxer (needed for huffyuv).
     * - encoding: set by lavc
     * - decoding: set by user
     */
     int bits_per_sample;

    /**
     * prediction method (needed for huffyuv).
     * - encoding: set by user
     * - decoding: unused
     */
     int prediction_method;
#define FF_PRED_LEFT   0
#define FF_PRED_PLANE  1
#define FF_PRED_MEDIAN 2

    /**
     * sample aspect ratio (0 if unknown).
     * numerator and denominator must be relative prime and smaller then 256 for some video standards
     * - encoding: set by user.
     * - decoding: set by lavc.
     */
    AVRational sample_aspect_ratio;

    /**
     * the picture in the bitstream.
     * - encoding: set by lavc
     * - decoding: set by lavc
     */
    AVFrame *coded_frame;

    /**
     * debug.
     * - encoding: set by user.
     * - decoding: set by user.
     */
    int debug;
#define FF_DEBUG_PICT_INFO 1
#define FF_DEBUG_RC        2
#define FF_DEBUG_BITSTREAM 4
#define FF_DEBUG_MB_TYPE   8
#define FF_DEBUG_QP        16
#define FF_DEBUG_MV        32
#define FF_DEBUG_DCT_COEFF 0x00000040
#define FF_DEBUG_SKIP      0x00000080
#define FF_DEBUG_STARTCODE 0x00000100
#define FF_DEBUG_PTS       0x00000200
#define FF_DEBUG_ER        0x00000400
#define FF_DEBUG_MMCO      0x00000800
#define FF_DEBUG_BUGS      0x00001000
#define FF_DEBUG_VIS_QP    0x00002000
#define FF_DEBUG_VIS_MB_TYPE 0x00004000

    /**
     * debug.
     * - encoding: set by user.
     * - decoding: set by user.
     */
    int debug_mv;
#define FF_DEBUG_VIS_MV_P_FOR  0x00000001 //visualize forward predicted MVs of P frames
#define FF_DEBUG_VIS_MV_B_FOR  0x00000002 //visualize forward predicted MVs of B frames
#define FF_DEBUG_VIS_MV_B_BACK 0x00000004 //visualize backward predicted MVs of B frames

    /**
     * error.
     * - encoding: set by lavc if flags&CODEC_FLAG_PSNR
     * - decoding: unused
     */
    uint64_t error[4];

    /**
     * minimum MB quantizer.
     * - encoding: unused
     * - decoding: unused
     */
    int mb_qmin;

    /**
     * maximum MB quantizer.
     * - encoding: unused
     * - decoding: unused
     */
    int mb_qmax;

    /**
     * motion estimation compare function.
     * - encoding: set by user.
     * - decoding: unused
     */
    int me_cmp;
    /**
     * subpixel motion estimation compare function.
     * - encoding: set by user.
     * - decoding: unused
     */
    int me_sub_cmp;
    /**
     * macroblock compare function (not supported yet).
     * - encoding: set by user.
     * - decoding: unused
     */
    int mb_cmp;
    /**
     * interlaced dct compare function
     * - encoding: set by user.
     * - decoding: unused
     */
    int ildct_cmp;
#define FF_CMP_SAD  0
#define FF_CMP_SSE  1
#define FF_CMP_SATD 2
#define FF_CMP_DCT  3
#define FF_CMP_PSNR 4
#define FF_CMP_BIT  5
#define FF_CMP_RD   6
#define FF_CMP_ZERO 7
#define FF_CMP_VSAD 8
#define FF_CMP_VSSE 9
#define FF_CMP_NSSE 10
#define FF_CMP_W53  11
#define FF_CMP_W97  12
#define FF_CMP_DCTMAX 13
#define FF_CMP_DCT264 14
#define FF_CMP_CHROMA 256

    /**
     * ME diamond size & shape.
     * - encoding: set by user.
     * - decoding: unused
     */
    int dia_size;

    /**
     * amount of previous MV predictors (2a+1 x 2a+1 square).
     * - encoding: set by user.
     * - decoding: unused
     */
    int last_predictor_count;

    /**
     * pre pass for motion estimation.
     * - encoding: set by user.
     * - decoding: unused
     */
    int pre_me;

    /**
     * motion estimation pre pass compare function.
     * - encoding: set by user.
     * - decoding: unused
     */
    int me_pre_cmp;

    /**
     * ME pre pass diamond size & shape.
     * - encoding: set by user.
     * - decoding: unused
     */
    int pre_dia_size;

    /**
     * subpel ME quality.
     * - encoding: set by user.
     * - decoding: unused
     */
    int me_subpel_quality;

    /**
     * callback to negotiate the pixelFormat.
     * @param fmt is the list of formats which are supported by the codec,
     * its terminated by -1 as 0 is a valid format, the formats are ordered by quality
     * the first is allways the native one
     * @return the choosen format
     * - encoding: unused
     * - decoding: set by user, if not set then the native format will always be choosen
     */
    enum PixelFormat (*get_format)(struct AVCodecContext *s, const enum PixelFormat * fmt);

    /**
     * DTG active format information (additionnal aspect ratio
     * information only used in DVB MPEG2 transport streams). 0 if
     * not set.
     *
     * - encoding: unused.
     * - decoding: set by decoder
     */
    int dtg_active_format;
#define FF_DTG_AFD_SAME         8
#define FF_DTG_AFD_4_3          9
#define FF_DTG_AFD_16_9         10
#define FF_DTG_AFD_14_9         11
#define FF_DTG_AFD_4_3_SP_14_9  13
#define FF_DTG_AFD_16_9_SP_14_9 14
#define FF_DTG_AFD_SP_4_3       15

    /**
     * Maximum motion estimation search range in subpel units.
     * if 0 then no limit
     *
     * - encoding: set by user.
     * - decoding: unused.
     */
    int me_range;

    /**
     * intra quantizer bias.
     * - encoding: set by user.
     * - decoding: unused
     */
    int intra_quant_bias;
#define FF_DEFAULT_QUANT_BIAS 999999

    /**
     * inter quantizer bias.
     * - encoding: set by user.
     * - decoding: unused
     */
    int inter_quant_bias;

    /**
     * color table ID.
     * - encoding: unused.
     * - decoding: which clrtable should be used for 8bit RGB images
     *             table have to be stored somewhere FIXME
     */
    int color_table_id;

    /**
     * internal_buffer count.
     * Don't touch, used by lavc default_get_buffer()
     */
    int internal_buffer_count;

    /**
     * internal_buffers.
     * Don't touch, used by lavc default_get_buffer()
     */
    void *internal_buffer;

#define FF_LAMBDA_SHIFT 7
#define FF_LAMBDA_SCALE (1<<FF_LAMBDA_SHIFT)
#define FF_QP2LAMBDA 118 ///< factor to convert from H.263 QP to lambda
#define FF_LAMBDA_MAX (256*128-1)

#define FF_QUALITY_SCALE FF_LAMBDA_SCALE //FIXME maybe remove
    /**
     * global quality for codecs which cannot change it per frame.
     * this should be proportional to MPEG1/2/4 qscale.
     * - encoding: set by user.
     * - decoding: unused
     */
    int global_quality;

#define FF_CODER_TYPE_VLC   0
#define FF_CODER_TYPE_AC    1
    /**
     * coder type
     * - encoding: set by user.
     * - decoding: unused
     */
    int coder_type;

    /**
     * context model
     * - encoding: set by user.
     * - decoding: unused
     */
    int context_model;
#if 0
    /**
     *
     * - encoding: unused
     * - decoding: set by user.
     */
    uint8_t * (*realloc)(struct AVCodecContext *s, uint8_t *buf, int buf_size);
#endif

    /**
     * slice flags
     * - encoding: unused
     * - decoding: set by user.
     */
    int slice_flags;
#define SLICE_FLAG_CODED_ORDER    0x0001 ///< draw_horiz_band() is called in coded order instead of display
#define SLICE_FLAG_ALLOW_FIELD    0x0002 ///< allow draw_horiz_band() with field slices (MPEG2 field pics)
#define SLICE_FLAG_ALLOW_PLANE    0x0004 ///< allow draw_horiz_band() with 1 component at a time (SVQ1)

    /**
     * XVideo Motion Acceleration
     * - encoding: forbidden
     * - decoding: set by decoder
     */
    int xvmc_acceleration;

    /**
     * macroblock decision mode
     * - encoding: set by user.
     * - decoding: unused
     */
    int mb_decision;
#define FF_MB_DECISION_SIMPLE 0        ///< uses mb_cmp
#define FF_MB_DECISION_BITS   1        ///< chooses the one which needs the fewest bits
#define FF_MB_DECISION_RD     2        ///< rate distoration

    /**
     * custom intra quantization matrix
     * - encoding: set by user, can be NULL
     * - decoding: set by lavc
     */
    uint16_t *intra_matrix;

    /**
     * custom inter quantization matrix
     * - encoding: set by user, can be NULL
     * - decoding: set by lavc
     */
    uint16_t *inter_matrix;

    /**
     * fourcc from the AVI stream header (LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
     * this is used to workaround some encoder bugs
     * - encoding: unused
     * - decoding: set by user, will be converted to upper case by lavc during init
     */
    unsigned int stream_codec_tag;

    /**
     * scene change detection threshold.
     * 0 is default, larger means fewer detected scene changes
     * - encoding: set by user.
     * - decoding: unused
     */
    int scenechange_threshold;

    /**
     * minimum lagrange multipler
     * - encoding: set by user.
     * - decoding: unused
     */
    int lmin;

    /**
     * maximum lagrange multipler
     * - encoding: set by user.
     * - decoding: unused
     */
    int lmax;

    /**
     * Palette control structure
     * - encoding: ??? (no palette-enabled encoder yet)
     * - decoding: set by user.
     */
    struct AVPaletteControl *palctrl;

    /**
     * noise reduction strength
     * - encoding: set by user.
     * - decoding: unused
     */
    int noise_reduction;

    /**
     * called at the beginning of a frame to get cr buffer for it.
     * buffer type (size, hints) must be the same. lavc won't check it.
     * lavc will pass previous buffer in pic, function should return
     * same buffer or new buffer with old frame "painted" into it.
     * if pic.data[0] == NULL must behave like get_buffer().
     * - encoding: unused
     * - decoding: set by lavc, user can override
     */
    int (*reget_buffer)(struct AVCodecContext *c, AVFrame *pic);

    /**
     * number of bits which should be loaded into the rc buffer before decoding starts
     * - encoding: set by user.
     * - decoding: unused
     */
    int rc_initial_buffer_occupancy;

    /**
     *
     * - encoding: set by user.
     * - decoding: unused
     */
    int inter_threshold;

    /**
     * CODEC_FLAG2_*.
     * - encoding: set by user.
     * - decoding: set by user.
     */
    int flags2;

    /**
     * simulates errors in the bitstream to test error concealment.
     * - encoding: set by user.
     * - decoding: unused.
     */
    int error_rate;

    /**
     * MP3 antialias algorithm, see FF_AA_* below.
     * - encoding: unused
     * - decoding: set by user
     */
    int antialias_algo;
#define FF_AA_AUTO    0
#define FF_AA_FASTINT 1 //not implemented yet
#define FF_AA_INT     2
#define FF_AA_FLOAT   3
    /**
     * Quantizer noise shaping.
     * - encoding: set by user
     * - decoding: unused
     */
    int quantizer_noise_shaping;

    /**
     * Thread count.
     * is used to decide how many independant tasks should be passed to execute()
     * - encoding: set by user
     * - decoding: set by user
     */
    int thread_count;

    /**
     * the codec may call this to execute several independant things. it will return only after
     * finishing all tasks, the user may replace this with some multithreaded implementation, the
     * default implementation will execute the parts serially
     * @param count the number of things to execute
     * - encoding: set by lavc, user can override
     * - decoding: set by lavc, user can override
     */
    int (*execute)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg), void **arg2, int *ret, int count);

    /**
     * Thread opaque.
     * can be used by execute() to store some per AVCodecContext stuff.
     * - encoding: set by execute()
     * - decoding: set by execute()
     */
    void *thread_opaque;

    /**
     * Motion estimation threshold. under which no motion estimation is
     * performed, but instead the user specified motion vectors are used
     *
     * - encoding: set by user
     * - decoding: unused
     */
     int me_threshold;

    /**
     * Macroblock threshold. under which the user specified macroblock types will be used
     * - encoding: set by user
     * - decoding: unused
     */
     int mb_threshold;

    /**
     * precision of the intra dc coefficient - 8.
     * - encoding: set by user
     * - decoding: unused
     */
     int intra_dc_precision;

    /**
     * noise vs. sse weight for the nsse comparsion function.
     * - encoding: set by user
     * - decoding: unused
     */
     int nsse_weight;

    /**
     * number of macroblock rows at the top which are skipped.
     * - encoding: unused
     * - decoding: set by user
     */
     int skip_top;

    /**
     * number of macroblock rows at the bottom which are skipped.
     * - encoding: unused
     * - decoding: set by user
     */
     int skip_bottom;

    /**
     * profile
     * - encoding: set by user
     * - decoding: set by lavc
     */
     int profile;
#define FF_PROFILE_UNKNOWN -99

    /**
     * level
     * - encoding: set by user
     * - decoding: set by lavc
     */
     int level;
#define FF_LEVEL_UNKNOWN -99

    /**
     * low resolution decoding. 1-> 1/2 size, 2->1/4 size
     * - encoding: unused
     * - decoding: set by user
     */
     int lowres;

    /**
     * bitsream width / height. may be different from width/height if lowres
     * or other things are used
     * - encoding: unused
     * - decoding: set by user before init if known, codec should override / dynamically change if needed
     */
    int coded_width, coded_height;

    /**
     * frame skip threshold
     * - encoding: set by user
     * - decoding: unused
     */
    int frame_skip_threshold;

    /**
     * frame skip factor
     * - encoding: set by user
     * - decoding: unused
     */
    int frame_skip_factor;

    /**
     * frame skip exponent
     * - encoding: set by user
     * - decoding: unused
     */
    int frame_skip_exp;

    /**
     * frame skip comparission function
     * - encoding: set by user.
     * - decoding: unused
     */
    int frame_skip_cmp;

    /**
     * border processing masking. raises the quantizer for mbs on the borders
     * of the picture.
     * - encoding: set by user
     * - decoding: unused
     */
    float border_masking;

    /**
     * minimum MB lagrange multipler.
     * - encoding: set by user.
     * - decoding: unused
     */
    int mb_lmin;

    /**
     * maximum MB lagrange multipler.
     * - encoding: set by user.
     * - decoding: unused
     */
    int mb_lmax;

    /**
     *
     * - encoding: set by user.
     * - decoding: unused
     */
    int me_penalty_compensation;

    /**
     *
     * - encoding: unused
     * - decoding: set by user.
     */
    enum AVDiscard skip_loop_filter;

    /**
     *
     * - encoding: unused
     * - decoding: set by user.
     */
    enum AVDiscard skip_idct;

    /**
     *
     * - encoding: unused
     * - decoding: set by user.
     */
    enum AVDiscard skip_frame;

    /**
     *
     * - encoding: set by user.
     * - decoding: unused
     */
    int bidir_refine;

    /**
     *
     * - encoding: set by user.
     * - decoding: unused
     */
    int brd_scale;

    /**
     * constant rate factor - quality-based VBR - values ~correspond to qps
     * - encoding: set by user.
     * - decoding: unused
     */
    float crf;

    /**
     * constant quantization parameter rate control method
     * - encoding: set by user.
     * - decoding: unused
     */
    int cqp;

    /**
     * minimum gop size
     * - encoding: set by user.
     * - decoding: unused
     */
    int keyint_min;

    /**
     * number of reference frames
     * - encoding: set by user.
     * - decoding: unused
     */
    int refs;

    /**
     * chroma qp offset from luma
     * - encoding: set by user.
     * - decoding: unused
     */
    int chromaoffset;

    /**
     * influences how often b-frames are used
     * - encoding: set by user.
     * - decoding: unused
     */
    int bframebias;

    /**
     * trellis RD quantization
     * - encoding: set by user.
     * - decoding: unused
     */
    int trellis;

    /**
     * reduce fluctuations in qp (before curve compression)
     * - encoding: set by user.
     * - decoding: unused
     */
    float complexityblur;

    /**
     * in-loop deblocking filter alphac0 parameter
     * alpha is in the range -6...6
     * - encoding: set by user.
     * - decoding: unused
     */
    int deblockalpha;

    /**
     * in-loop deblocking filter beta parameter
     * beta is in the range -6...6
     * - encoding: set by user.
     * - decoding: unused
     */
    int deblockbeta;

    /**
     * macroblock subpartition sizes to consider - p8x8, p4x4, b8x8, i8x8, i4x4
     * - encoding: set by user.
     * - decoding: unused
     */
    int partitions;
#define X264_PART_I4X4 0x001  /* Analyse i4x4 */
#define X264_PART_I8X8 0x002  /* Analyse i8x8 (requires 8x8 transform) */
#define X264_PART_P8X8 0x010  /* Analyse p16x8, p8x16 and p8x8 */
#define X264_PART_P4X4 0x020  /* Analyse p8x4, p4x8, p4x4 */
#define X264_PART_B8X8 0x100  /* Analyse b16x8, b8x16 and b8x8 */

    /**
     * direct mv prediction mode - 0 (none), 1 (spatial), 2 (temporal)
     * - encoding: set by user.
     * - decoding: unused
     */
    int directpred;

    /**
     * audio cutoff bandwidth (0 means "automatic") . Currently used only by FAAC
     * - encoding: set by user.
     * - decoding: unused
     */
    int cutoff;

    /**
     * multiplied by qscale for each frame and added to scene_change_score
     * - encoding: set by user.
     * - decoding: unused
     */
    int scenechange_factor;

    /**
     *
     * note: value depends upon the compare functin used for fullpel ME
     * - encoding: set by user.
     * - decoding: unused
     */
    int mv0_threshold;

    /**
     * adjusts sensitivity of b_frame_strategy 1
     * - encoding: set by user.
     * - decoding: unused
     */
    int b_sensitivity;

    /**
     * - encoding: set by user.
     * - decoding: unused
     */
    int compression_level;
#define FF_COMPRESSION_DEFAULT -1

    /**
     * sets whether to use LPC mode - used by FLAC encoder
     * - encoding: set by user.
     * - decoding: unused.
     */
    int use_lpc;

    /**
     * LPC coefficient precision - used by FLAC encoder
     * - encoding: set by user.
     * - decoding: unused.
     */
    int lpc_coeff_precision;

    /**
     * - encoding: set by user.
     * - decoding: unused.
     */
    int min_prediction_order;

    /**
     * - encoding: set by user.
     * - decoding: unused.
     */
    int max_prediction_order;

    /**
     * search method for selecting prediction order
     * - encoding: set by user.
     * - decoding: unused.
     */
    int prediction_order_method;

    /**
     * - encoding: set by user.
     * - decoding: unused.
     */
    int min_partition_order;

    /**
     * - encoding: set by user.
     * - decoding: unused.
     */
    int max_partition_order;

    /**
     * GOP timecode frame start number, in non drop frame format
     * - encoding: set by user.
     * - decoding: unused.
     */
    int64_t timecode_frame_start;
	AVPacket1 *pkt;
} AVCodecContext;

/**
 * AVCodec.
 */
typedef struct AVCodec {
    const char *name;
    enum CodecType type;
    enum CodecID id;
    int priv_data_size;
    int (*init)(AVCodecContext *);
    int (*encode)(AVCodecContext *, uint8_t *buf, int buf_size, void *data);
    int (*close)(AVCodecContext *);
    int (*decode)(AVCodecContext *, void *outdata, int *outdata_size,uint8_t *buf, int buf_size);
	int (*decode2)(AVCodecContext *, void *outdata, int *outdata_size, AVPacket1 *avpkt);
    int capabilities;
    struct AVCodec *next;
    void (*flush)(AVCodecContext *);
    const AVRational *supported_framerates; ///array of supported framerates, or NULL if any, array is terminated by {0,0}
    const enum PixelFormat *pix_fmts;       ///array of supported pixel formats, or NULL if unknown, array is terminanted by -1
	char *long_name;
} AVCodec;
typedef struct AVCodecParserContext {
    void *priv_data;
    struct AVCodecParser *parser;
    int64_t frame_offset; /* offset of the current frame */
    int64_t cur_offset; /* current offset
                           (incremented by each av_parser_parse()) */
    int64_t last_frame_offset; /* offset of the last frame */
    /* video info */
    int pict_type; /* XXX: put it back in AVCodecContext */
    int repeat_pict; /* XXX: put it back in AVCodecContext */
    int64_t pts;     /* pts of the current frame */
    int64_t dts;     /* dts of the current frame */

    /* private data */
    int64_t last_pts;
    int64_t last_dts;
    int fetch_timestamp;

#define AV_PARSER_PTS_NB 4
    int cur_frame_start_index;
    int64_t cur_frame_offset[AV_PARSER_PTS_NB];
    int64_t cur_frame_pts[AV_PARSER_PTS_NB];
    int64_t cur_frame_dts[AV_PARSER_PTS_NB];

    int flags;
#define PARSER_FLAG_COMPLETE_FRAMES           0x0001
} AVCodecParserContext;
/**
 * four components are given, that's all.
 * the last component is alpha
 */
typedef struct AVPicture {
    uint8_t *data[4];
    int linesize[4];       ///< number of bytes per line
} AVPicture;
extern AVCodec *first_avcodec;
int av_get_bits_per_sample(enum CodecID codec_id);
int avpicture_get_size(int pix_fmt, int width, int height);
void avcodec_get_context_defaults(AVCodecContext *s);
AVCodecContext *avcodec_alloc_context(void);
void avcodec_get_frame_defaults(AVFrame *pic);


#ifdef __cplusplus
}
#endif

#endif