
#ifndef AVCONMKVLIB_H
#define AVCONMKVLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "inttypes.h"
#include <time.h>
#include <stdio.h>  /* FILE */
#include "avconcodec.h"
//#include "metadata.h"
#include "avio.h"


#define ENOENT 2
#define EINTR  4
#define EIO    5
#define EAGAIN 11
#define ENOMEM 12
#define EINVAL 22
#define EPIPE  32

#define CONFIG_ENCODERS 1
#define CONFIG_DECODERS 1
#define CONFIG_MUXERS 1
#define CONFIG_DEMUXERS 1
#define CONFIG_FFMPEG 0
#define CONFIG_FFPLAY 0
#define CONFIG_MEMALIGN_HACK 1
#define CONFIG_MPEGAUDIO_HP 1
#define CONFIG_PROTOCOLS 0
#define TARGET_MMX 0

#ifndef PRId64
#define PRId64 "lld"
#endif
	
#ifndef PRIu64
#define PRIu64 "llu"
#endif
	
#ifndef PRIx64
#define PRIx64 "llx"
#endif
	
#ifndef PRIX64
#define PRIX64 "llX"
#endif
	
#ifndef PRId32
#define PRId32 "d"
#endif

typedef struct {
		char *key;
		char *value;
}AVMetadataTag;

typedef struct AVMetadata{
    int count;
    AVMetadataTag *elems;
}AVMetadata;
/* packet functions */
enum PixelFormat {
    PIX_FMT_NONE= -1,
		PIX_FMT_YUV420P,   ///< Planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
		PIX_FMT_YUYV422,   ///< Packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
		PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
		PIX_FMT_BGR24,     ///< Packed RGB 8:8:8, 24bpp, BGRBGR...
		PIX_FMT_YUV422P,   ///< Planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
		PIX_FMT_YUV444P,   ///< Planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
		PIX_FMT_RGB32,     ///< Packed RGB 8:8:8, 32bpp, (msb)8A 8R 8G 8B(lsb), in cpu endianness
		PIX_FMT_YUV410P,   ///< Planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
		PIX_FMT_YUV411P,   ///< Planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
		PIX_FMT_RGB565,    ///< Packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), in cpu endianness
		PIX_FMT_RGB555,    ///< Packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), in cpu endianness most significant bit to 0
		PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
		PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 1 is white
		PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black
		PIX_FMT_PAL8,      ///< 8 bit with PIX_FMT_RGB32 palette
		PIX_FMT_YUVJ420P,  ///< Planar YUV 4:2:0, 12bpp, full scale (jpeg)
		PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
		PIX_FMT_YUVJ444P,  ///< Planar YUV 4:4:4, 24bpp, full scale (jpeg)
		PIX_FMT_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing(xvmc_render.h)
		PIX_FMT_XVMC_MPEG2_IDCT,
		PIX_FMT_UYVY422,   ///< Packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
		PIX_FMT_UYYVYY411, ///< Packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
		PIX_FMT_BGR32,     ///< Packed RGB 8:8:8, 32bpp, (msb)8A 8B 8G 8R(lsb), in cpu endianness
		PIX_FMT_BGR565,    ///< Packed RGB 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), in cpu endianness
		PIX_FMT_BGR555,    ///< Packed RGB 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), in cpu endianness most significant bit to 1
		PIX_FMT_BGR8,      ///< Packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
		PIX_FMT_BGR4,      ///< Packed RGB 1:2:1,  4bpp, (msb)1B 2G 1R(lsb)
		PIX_FMT_BGR4_BYTE, ///< Packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
		PIX_FMT_RGB8,      ///< Packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
		PIX_FMT_RGB4,      ///< Packed RGB 1:2:1,  4bpp, (msb)2R 3G 3B(lsb)
		PIX_FMT_RGB4_BYTE, ///< Packed RGB 1:2:1,  8bpp, (msb)2R 3G 3B(lsb)
		PIX_FMT_NV12,      ///< Planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 for UV
		PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped
		
		PIX_FMT_RGB32_1,   ///< Packed RGB 8:8:8, 32bpp, (msb)8R 8G 8B 8A(lsb), in cpu endianness
		PIX_FMT_BGR32_1,   ///< Packed RGB 8:8:8, 32bpp, (msb)8B 8G 8R 8A(lsb), in cpu endianness
		
		PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
		PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
		PIX_FMT_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
};
#define PIX_FMT_RGBA PIX_FMT_BGR32
#define PIX_FMT_BGRA PIX_FMT_RGB32
#define PIX_FMT_ARGB PIX_FMT_BGR32_1
#define PIX_FMT_ABGR PIX_FMT_RGB32_1
#define PIX_FMT_GRAY16 PIX_FMT_GRAY16LE

#ifdef WORDS_BIGENDIAN
#define PIX_FMT_RGBA PIX_FMT_RGB32_1
#define PIX_FMT_BGRA PIX_FMT_BGR32_1
#define PIX_FMT_ARGB PIX_FMT_RGB32
#define PIX_FMT_ABGR PIX_FMT_BGR32
#define PIX_FMT_GRAY16 PIX_FMT_GRAY16BE
#else
#define PIX_FMT_RGBA PIX_FMT_BGR32
#define PIX_FMT_BGRA PIX_FMT_RGB32
#define PIX_FMT_ARGB PIX_FMT_BGR32_1
#define PIX_FMT_ABGR PIX_FMT_RGB32_1
#define PIX_FMT_GRAY16 PIX_FMT_GRAY16LE
#endif

#if LIBAVUTIL_VERSION_INT < (50<<16)
#define PIX_FMT_UYVY411 PIX_FMT_UYYVYY411
#define PIX_FMT_RGBA32  PIX_FMT_RGB32
#define PIX_FMT_YUV422  PIX_FMT_YUYV422
#endif

typedef struct AVPacket {
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
	
											/**
											* Time difference in AVStream->time_base units from the pts of this
											* packet to the point at which the output from the decoder has converged
											* independent from the availability of previous frames. That is, the
											* frames are virtually identical no matter if decoding started from
											* the very first frame or from this keyframe.
											* Is AV_NOPTS_VALUE if unknown.
											* This field is not the display duration of the current packet.
											* This field has no meaning if the packet does not have AV_PKT_FLAG_KEY
											* set.
											*
											* The purpose of this field is to allow seeking in streams that have no
											* keyframes in the conventional sense. It corresponds to the
											* recovery point SEI in H.264 and match_time_delta in NUT. It is also
											* essential for some types of subtitle streams to ensure that all
     * subtitles are correctly displayed after seeking.
     */
    int64_t convergence_duration;
} AVPacket;
#define PKT_FLAG_KEY   0x0001

void av_destruct_packet_nofree(AVPacket *pkt);
void av_destruct_packet(AVPacket *pkt);

/* initialize optional fields of a packet */

 void av_init_packet(AVPacket *pkt);
/*************************************************/
/* input/output formats */

struct AVCodecTag;

struct AVFormatContext;

/* this structure contains the data a format has to probe a file */
typedef struct AVProbeData {
    const char *filename;
    unsigned char *buf;
    int buf_size;
} AVProbeData;

#define AVPROBE_SCORE_MAX 100               ///< max score, half of that is used for file extension based detection

typedef struct AVFormatParameters {
    AVRational time_base;
    int sample_rate;
    int channels;
    int width;
    int height;
    enum PixelFormat pix_fmt;
    int channel; /* used to select dv channel */
    const char *device; /* video, audio or DV device */
    const char *standard; /* tv standard, NTSC, PAL, SECAM */
    int mpeg2ts_raw:1;  /* force raw MPEG2 transport stream output, if possible */
    int mpeg2ts_compute_pcr:1; /* compute exact PCR for each transport
                                  stream packet (only meaningful if
                                  mpeg2ts_raw is TRUE */
    int initial_pause:1;       /* do not begin to play the stream
                                  immediately (RTSP only) */
    int prealloced_context:1;
    enum CodecID video_codec_id;
    enum CodecID audio_codec_id;
} AVFormatParameters;

typedef struct AVFrac {
    int64_t val, num, den;
} AVFrac;
//! demuxer will use url_fopen, no opened file should be provided by the caller
#define AVFMT_NOFILE        0x0001
#define AVFMT_NEEDNUMBER    0x0002 /**< Needs '%d' in filename. */
#define AVFMT_SHOW_IDS      0x0008 /**< Show format stream IDs numbers. */
#define AVFMT_RAWPICTURE    0x0020 /**< Format wants AVPicture structure for
                                      raw picture data. */
#define AVFMT_GLOBALHEADER  0x0040 /**< Format wants global header. */
#define AVFMT_NOTIMESTAMPS  0x0080 /**< Format does not need / have any timestamps. */
#define AVFMT_GENERIC_INDEX 0x0100 /**< Use generic index building code. */
#define AVFMT_TS_DISCONT    0x0200 /**< Format allows timestamp discontinuities. Note, muxers always require valid (monotone) timestamps */
#define AVFMT_VARIABLE_FPS  0x0400 /**< Format allows variable fps. */
#define AVFMT_NODIMENSIONS  0x0800 /**< Format does not need width/height */
#define AVFMT_NOSTREAMS     0x1000 /**< Format does not require any streams */

typedef struct AVOutputFormat {
    const char *name;
    const char *long_name;
    const char *mime_type;
    const char *extensions; /* comma separated extensions */
    /* size of private data so that it can be allocated in the wrapper */
    int priv_data_size;
    /* output support */
    enum CodecID audio_codec; /* default audio codec */
    enum CodecID video_codec; /* default video codec */
    int (*write_header)(struct AVFormatContext *);
    int (*write_packet)(struct AVFormatContext *, AVPacket *pkt);
    int (*write_trailer)(struct AVFormatContext *);

	int (*rewrite_audio_header)(struct AVFormatContext *s,int64_t codecid,int64_t samplerate,int bit_depath,int channels);
	int (*rewrite_video_header)(struct AVFormatContext *,int64_t video_width,int64_t video_height,int64_t codecid,const unsigned char extra_data,int64_t extra_data_len);
    /* can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER, AVFMT_GLOBALHEADER */
    int flags;
    /* currently only used to set pixel format if not YUV420P */
    int (*set_parameters)(struct AVFormatContext *, AVFormatParameters *);
    int (*interleave_packet)(struct AVFormatContext *, AVPacket *out, AVPacket *in, int flush);

    /**
     * list of supported codec_id-codec_tag pairs, ordered by "better choice first"
     * the arrays are all CODEC_ID_NONE terminated
     */
    const struct AVCodecTag **codec_tag;

    /* private fields */
    struct AVOutputFormat *next;
} AVOutputFormat;

typedef struct AVInputFormat {
    const char *name;
    const char *long_name;
    /* size of private data so that it can be allocated in the wrapper */
    int priv_data_size;
    /* tell if a given file has a chance of being parsing by this format */
    int (*read_probe)(AVProbeData *);
    /* read the format header and initialize the AVFormatContext
       structure. Return 0 if OK. 'ap' if non NULL contains
       additionnal paramters. Only used in raw format right
       now. 'av_new_stream' should be called to create new streams.  */
    int (*read_header)(struct AVFormatContext *,AVFormatParameters *ap);
    /* read one packet and put it in 'pkt'. pts and flags are also
       set. 'av_new_stream' can be called only if the flag
       AVFMTCTX_NOHEADER is used. */
    int (*read_packet)(struct AVFormatContext *, AVPacket *pkt);
    /* close the stream. The AVFormatContext and AVStreams are not
       freed by this function */
    int (*read_close)(struct AVFormatContext *);
    /**
     * seek to a given timestamp relative to the frames in
     * stream component stream_index
     * @param stream_index must not be -1
     * @param flags selects which direction should be preferred if no exact
     *              match is available
     */
    int (*read_seek)(struct AVFormatContext *,
                     int stream_index, int64_t timestamp, int flags);
    /**
     * gets the next timestamp in AV_TIME_BASE units.
     */
    int64_t (*read_timestamp)(struct AVFormatContext *s, int stream_index,
                              int64_t *pos, int64_t pos_limit);
    /* can use flags: AVFMT_NOFILE, AVFMT_NEEDNUMBER */
    int flags;
    /* if extensions are defined, then no probe is done. You should
       usually not use extension format guessing because it is not
       reliable enough */
    const char *extensions;
    /* general purpose read only value that the format can use */
    int value;

    /* start/resume playing - only meaningful if using a network based format
       (RTSP) */
    int (*read_play)(struct AVFormatContext *);

    /* pause playing - only meaningful if using a network based format
       (RTSP) */
    int (*read_pause)(struct AVFormatContext *);

    const struct AVCodecTag **codec_tag;

    /* private fields */
    struct AVInputFormat *next;
} AVInputFormat;

typedef struct AVIndexEntry {
    int64_t pos;
    int64_t timestamp;
#define AVINDEX_KEYFRAME 0x0001
    int flags:2;
    int size:30; //yeah trying to keep the size of this small to reduce memory requirements (its 24 vs 32 byte due to possible 8byte align)
    int min_distance;         /* min distance between this and the previous keyframe, used to avoid unneeded searching */
} AVIndexEntry;

#define AV_METADATA_MATCH_CASE      1
#define AV_METADATA_IGNORE_SUFFIX   2
#define AV_METADATA_DONT_STRDUP_KEY 4
#define AV_METADATA_DONT_STRDUP_VAL 8
#define AV_METADATA_DONT_OVERWRITE 16   ///< Don't overwrite existing tags.

#define AV_DISPOSITION_DEFAULT   0x0001
#define AV_DISPOSITION_DUB       0x0002
#define AV_DISPOSITION_ORIGINAL  0x0004
#define AV_DISPOSITION_COMMENT   0x0008
#define AV_DISPOSITION_LYRICS    0x0010
#define AV_DISPOSITION_KARAOKE   0x0020

typedef struct AVStream {
    int index;    /* stream index in AVFormatContext */
    int id;       /* format specific stream id */
    AVCodecContext *codec; /* codec context */
    /**
     * real base frame rate of the stream.
     * this is the lowest framerate with which all timestamps can be
     * represented accurately (its the least common multiple of all
     * framerates in the stream), Note, this value is just a guess!
     * for example if the timebase is 1/90000 and all frames have either
     * approximately 3600 or 1800 timer ticks then r_frame_rate will be 50/1
     */
    AVRational r_frame_rate;
	AVRational sample_aspect_ratio;
    void *priv_data;
    /* internal data used in av_find_stream_info() */
    int64_t codec_info_duration;
    int codec_info_nb_frames;
    /* encoding: PTS generation when outputing stream */
    AVFrac pts;

    /**
     * this is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. for fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identically 1.
     */
    AVRational time_base;
    int pts_wrap_bits; /* number of bits in pts (used for wrapping control) */
    /* ffmpeg.c private use */
    int stream_copy; /* if TRUE, just copy stream */
    enum AVDiscard discard; ///< selects which packets can be discarded at will and dont need to be demuxed
    //FIXME move stuff to a flags field?
    /* quality, as it has been removed from AVCodecContext and put in AVVideoFrame
     * MN:dunno if thats the right place, for it */
    float quality;
    /* decoding: position of the first frame of the component, in
       AV_TIME_BASE fractional seconds. */
    int64_t start_time;
    /* decoding: duration of the stream, in AV_TIME_BASE fractional
       seconds. */
    int64_t duration;

    char language[4]; /* ISO 639 3-letter language code (empty string if undefined) */

    /* av_read_frame() support */
    int need_parsing;                  ///< 1->full parsing needed, 2->only parse headers dont repack
    struct AVCodecParserContext *parser;

    int64_t cur_dts;
    int last_IP_duration;
    int64_t last_IP_pts;
    /* av_seek_frame() support */
    AVIndexEntry *index_entries; /* only used if the format does not
                                    support seeking natively */
    int nb_index_entries;
    unsigned int index_entries_allocated_size;

    int64_t nb_frames;                 ///< number of frames in this stream if known or 0
	int disposition; /**< AV_DISPOSITION_* bit field */
    AVMetadata *metadata;
#define MAX_REORDER_DELAY 4
    int64_t pts_buffer[MAX_REORDER_DELAY+1];
} AVStream;

/**
 * New fields can be added to the end with minor version bumps.
 * Removal, reordering and changes to existing fields require a major
 * version bump.
 * sizeof(AVProgram) must not be used outside libav*.
 */
typedef struct AVProgram {
    int            id;
#if FF_API_OLD_METADATA
    attribute_deprecated char           *provider_name; ///< network name for DVB streams
    attribute_deprecated char           *name;          ///< service name for DVB streams
#endif
    int            flags;
    enum AVDiscard discard;        ///< selects which program to discard and which to feed to the caller
    unsigned int   *stream_index;
    unsigned int   nb_stream_indexes;
    AVMetadata *metadata;
} AVProgram;

typedef struct AVChapter {
    int id;                 ///< unique ID to identify the chapter
    AVRational time_base;   ///< time base in which the start/end timestamps are specified
    int64_t start, end;     ///< chapter start/end time in time_base units
#if FF_API_OLD_METADATA
    attribute_deprecated char *title;            ///< chapter title
#endif
    AVMetadata *metadata;
} AVChapter;

#define AV_STRINGIFY(s)         AV_TOSTRING(s)
#define AV_TOSTRING(s) #s
#define LIBAVFORMAT_IDENT       "AVCON MKV"
#define AVFMTCTX_NOHEADER      0x0001 /* signal that no header is present
                                         (streams are added dynamically) */
#define MAX_STREAMS 20
enum AVOptionType{
    FF_OPT_TYPE_FLAGS,
		FF_OPT_TYPE_INT,
		FF_OPT_TYPE_INT64,
		FF_OPT_TYPE_DOUBLE,
		FF_OPT_TYPE_FLOAT,
		FF_OPT_TYPE_STRING,
		FF_OPT_TYPE_RATIONAL,
		FF_OPT_TYPE_CONST=128,
};
/* format I/O context */
typedef struct AVFormatContext {
    const AVClass *av_class; /* set by av_alloc_format_context */
    /* can only be iformat or oformat, not both at the same time */
    struct AVInputFormat *iformat;
    struct AVOutputFormat *oformat;
    void *priv_data;
    ByteIOContext pb;
    unsigned int nb_streams;
    AVStream *streams[MAX_STREAMS];
    char filename[1024]; /* input or output filename */
    /* stream info */
    int64_t timestamp;
    char title[512];
    char author[512];
    char copyright[512];
    char comment[512];
    char album[512];
    int year;  /* ID3 year, 0 if none */
    int track; /* track number, 0 if none */
    char genre[32]; /* ID3 genre */

    int ctx_flags; /* format specific flags, see AVFMTCTX_xx */
    /* private data for pts handling (do not modify directly) */
    /* This buffer is only needed when packets were already buffered but
       not decoded, for example to get the codec parameters in mpeg
       streams */
    struct AVPacketList *packet_buffer;

    /* decoding: position of the first frame of the component, in
       AV_TIME_BASE fractional seconds. NEVER set this value directly:
       it is deduced from the AVStream values.  */
    int64_t start_time;
    /* decoding: duration of the stream, in AV_TIME_BASE fractional
       seconds. NEVER set this value directly: it is deduced from the
       AVStream values.  */
    int64_t duration;
    /* decoding: total file size. 0 if unknown */
    int64_t file_size;
    /* decoding: total stream bitrate in bit/s, 0 if not
       available. Never set it directly if the file_size and the
       duration are known as ffmpeg can compute it automatically. */
    int bit_rate;

    /* av_read_frame() support */
    AVStream *cur_st;
    const uint8_t *cur_ptr;
    int cur_len;
    AVPacket cur_pkt;

    /* av_seek_frame() support */
    int64_t data_offset; /* offset of the first packet */
    int index_built;

    int mux_rate;
    int packet_size;
    int preload;
    int max_delay;

#define AVFMT_NOOUTPUTLOOP -1
#define AVFMT_INFINITEOUTPUTLOOP 0
    /* number of times to loop output in formats that support it */
    int loop_output;

    int flags;
#define AVFMT_FLAG_GENPTS       0x0001 ///< generate pts if missing even if it requires parsing future frames
#define AVFMT_FLAG_IGNIDX       0x0002 ///< ignore index
#define AVFMT_FLAG_NONBLOCK     0x0004 ///< Do not block when reading packets from input.
#define AVFMT_FLAG_IGNDTS       0x0008 ///< Ignore DTS on frames that contain both DTS & PTS
#define AVFMT_FLAG_NOFILLIN     0x0010 ///< Do not infer any values from other values, just return what is stored in the container
#define AVFMT_FLAG_NOPARSE      0x0020 ///< Do not use AVParsers, you also must set AVFMT_FLAG_NOFILLIN as the fillin code works on frames and no parsing -> no frames. Also seeking to frames can not work if parsing to find frame boundaries has been disabled
#define AVFMT_FLAG_RTP_HINT     0x0040 ///< Add RTP hinting to the output file

    int loop_input;
    /* decoding: size of data to probe; encoding unused */
    unsigned int probesize;

    /**
     * maximum duration in AV_TIME_BASE units over which the input should be analyzed in av_find_stream_info()
     */
    int max_analyze_duration;

	    const uint8_t *key;
    int keylen;

    unsigned int nb_programs;
    AVProgram **programs;

    /**
     * Forced video codec_id.
     * Demuxing: Set by user.
     */
    enum CodecID video_codec_id;

    /**
     * Forced audio codec_id.
     * Demuxing: Set by user.
     */
    enum CodecID audio_codec_id;

    /**
     * Forced subtitle codec_id.
     * Demuxing: Set by user.
     */
    enum CodecID subtitle_codec_id;

    /**
     * Maximum amount of memory in bytes to use for the index of each stream.
     * If the index exceeds this size, entries will be discarded as
     * needed to maintain a smaller size. This can lead to slower or less
     * accurate seeking (depends on demuxer).
     * Demuxers for which a full in-memory index is mandatory will ignore
     * this.
     * muxing  : unused
     * demuxing: set by user
     */
    unsigned int max_index_size;

    /**
     * Maximum amount of memory in bytes to use for buffering frames
     * obtained from realtime capture devices.
     */
    unsigned int max_picture_buffer;

    unsigned int nb_chapters;
    AVChapter **chapters;

    /**
     * Flags to enable debugging.
     */
    int debug;
#define FF_FDEBUG_TS        0x0001

    /**
     * Raw packets from the demuxer, prior to parsing and decoding.
     * This buffer is used for buffering packets until the codec can
     * be identified, as parsing cannot be done without knowing the
     * codec.
     */
    struct AVPacketList *raw_packet_buffer;
    struct AVPacketList *raw_packet_buffer_end;

    struct AVPacketList *packet_buffer_end;

    AVMetadata *metadata;

    /**
     * Remaining size available for raw_packet_buffer, in bytes.
     * NOT PART OF PUBLIC API
     */
#define RAW_PACKET_BUFFER_SIZE 2500000
    int raw_packet_buffer_remaining_size;

    /**
     * Start time of the stream in real world time, in microseconds
     * since the unix epoch (00:00 1st January 1970). That is, pts=0
     * in the stream was captured at this real world time.
     * - encoding: Set by user.
     * - decoding: Unused.
     */
    int64_t start_time_realtime;
} AVFormatContext;

typedef struct AVPacketList {
    AVPacket pkt;
    struct AVPacketList *next;
} AVPacketList;

typedef struct AVOption {
    const char *name;

    /**
     * short English text help.
     * @fixme what about other languages
     */
    const char *help;
    int offset;             ///< offset to context structure where the parsed value should be stored
    enum AVOptionType type;

    double default_val;
    double min;
    double max;

    int flags;
#define AV_OPT_FLAG_ENCODING_PARAM  1   ///< a generic parameter which can be set by the user for muxing or encoding
#define AV_OPT_FLAG_DECODING_PARAM  2   ///< a generic parameter which can be set by the user for demuxing or decoding
#define AV_OPT_FLAG_METADATA        4   ///< some data extracted or inserted into the file like title, comment, ...
#define AV_OPT_FLAG_AUDIO_PARAM     8
#define AV_OPT_FLAG_VIDEO_PARAM     16
#define AV_OPT_FLAG_SUBTITLE_PARAM  32
//FIXME think about enc-audio, ... style flags
    const char *unit;
} AVOption;

static __inline void av_free_packet(AVPacket *pkt)
{
    if (pkt && pkt->destruct) {
        pkt->destruct(pkt);
    }
}

#define AVERROR_UNKNOWN     (-1)  /* unknown error */
#define AVERROR_IO          (-2)  /* i/o error */
#define AVERROR_NUMEXPECTED (-3)  /* number syntax expected in filename */
#define AVERROR_INVALIDDATA (-4)  /* invalid data found */
#define AVERROR_NOMEM       (-5)  /* not enough memory */
#define AVERROR_NOFMT       (-6)  /* unknown format */
#define AVERROR_NOTSUPP     (-7)  /* operation not supported */

extern AVOutputFormat matroska_muxer;
/* media file output */
typedef struct {
    unsigned int state[64];
    int index;
} AVLFG1;

void av_lfg_init(AVLFG1 *c, unsigned int seed);
int av_set_parameters(AVFormatContext *s, AVFormatParameters *ap);
int av_write_header(AVFormatContext *s);
int av_write_frame(AVFormatContext *s, AVPacket *pkt);
int av_rewrite_video_header(AVFormatContext *s,int64_t video_width,int64_t video_height,int64_t codecid,const unsigned char *extra_data,int64_t extra_data_len);
int av_rewrite_audio_header(AVFormatContext *s,int64_t codecid,int64_t samplerate,int bit_depath,int channels);
int av_interleave_packet_per_dts(AVFormatContext *s, AVPacket *out, AVPacket *pkt, int flush);
int av_write_trailer(AVFormatContext *s);
void img_copy(AVPicture *dst, const AVPicture *src,int pix_fmt, int width, int height);

void *av_fast_realloc(void *ptr, unsigned int *size, unsigned int min_size);
void av_set_pts_info(AVStream *s, int pts_wrap_bits,int pts_num, int pts_den);
int av_dup_packet(AVPacket *pkt);

AVFormatContext *av_alloc_format_context(void);

void pstrcpy(char *buf, int buf_size, const char *str);
void av_register_output_format(AVOutputFormat *format);
AVOutputFormat *guess_format(const char *short_name,const char *filename, const char *mime_type);
AVStream *av_new_stream(AVFormatContext *s, int id);
int avpicture_fill(AVPicture *picture, uint8_t *ptr,int pix_fmt, int width, int height);
void *av_malloc(unsigned int size);
void *av_realloc(void *ptr, unsigned int size);
void av_free(void *ptr);
void av_opt_set_defaults(void *s);
void *av_mallocz(unsigned int size);
char *av_strdup(const char *s);
void av_freep(void *ptr);

#ifdef __cplusplus
}
#endif

#endif