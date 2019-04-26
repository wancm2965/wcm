
#include "avconmkvlib.h"
#include "avconcodec.h"
#include "common.h"
#include "mathematics.h"
#include "riff.h"
#include "rational.h"
#include <string.h>
#include <math.h>

AVOutputFormat *first_oformat = NULL;

typedef struct InternalBuffer{
    int last_pic_num;
    uint8_t *base[4];
    uint8_t *data[4];
    int linesize[4];
}InternalBuffer;

typedef struct PixFmtInfo {
    const char *name;
    uint8_t nb_channels;     /* number of channels (including alpha) */
    uint8_t color_type;      /* color type (see FF_COLOR_xxx constants) */
    uint8_t pixel_type;      /* pixel storage type (see FF_PIXEL_xxx constants) */
    uint8_t is_alpha : 1;    /* true if alpha can be specified */
    uint8_t x_chroma_shift;  /* X chroma subsampling factor is 2 ^ shift */
    uint8_t y_chroma_shift;  /* Y chroma subsampling factor is 2 ^ shift */
    uint8_t depth;           /* bit depth of the color components */
} PixFmtInfo;
AVCodec *first_avcodec = NULL;
//#define assert(x) (void(0))
#define INTERNAL_BUFFER_SIZE 32
#define EDGE_WIDTH 16
#define INT_MAX 2147483647
#define FF_COLOR_RGB      0 /* RGB color space */
#define FF_COLOR_GRAY     1 /* gray color space */
#define FF_COLOR_YUV      2 /* YUV color space. 16 <= Y <= 235, 16 <= U, V <= 240 */
#define FF_COLOR_YUV_JPEG 3 /* YUV color space. 0 <= Y <= 255, 0 <= U, V <= 255 */

#define FF_PIXEL_PLANAR   0 /* each channel has one component in AVPicture */
#define FF_PIXEL_PACKED   1 /* only one components containing all the channels */
#define FF_PIXEL_PALETTE  2  /* one components containing indexes for a palette */

static void av_frac_init(AVFrac *f, int64_t val, int64_t num, int64_t den);
static void av_frac_add(AVFrac *f, int64_t incr);
static const PixFmtInfo pix_fmt_info[PIX_FMT_NB] = {
	{"yuv420p",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,1,1,8},//0
	{"yuv422",1,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,1,0,8},//1
	{"rgb24",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,0,0,8},//2
	{"bgr24",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,0,0,8},//3
	{"yuv422p",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,1,0,8},//4
	{"yuv444p",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,0,0,8},//5
	{"rgba32",4,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,0,0,8},//6
	{"yuv410p",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,2,2,8},//7
	{"yuv411p",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,2,0,8},//8
	{"rgb565",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,0,0,5},//9
	{"rgb555",3,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,0,0,5},//10
	{"gray",1,FF_COLOR_GRAY,FF_PIXEL_PLANAR,0,0,0,8},//11
	{"monow",1,FF_COLOR_GRAY,FF_PIXEL_PLANAR,0,0,0,1},//12
	{"monob",1,FF_COLOR_GRAY,FF_PIXEL_PLANAR,0,0,0,1},//13
	{"pal8",4,FF_COLOR_RGB,FF_PIXEL_PALETTE,1,0,0,8},//14
	{"yuvj420p",3,FF_COLOR_YUV_JPEG,FF_PIXEL_PLANAR,0,1,1,8},//15
	{"yuvj422p",3,FF_COLOR_YUV_JPEG,FF_PIXEL_PLANAR,0,1,0,8},//16
	{"yuvj444p",3,FF_COLOR_YUV_JPEG,FF_PIXEL_PLANAR,0,0,0,8},//17
	{"xvmcmc",0,0,0,0,0,0,0},//18
	{"xvmcidct",0,0,0,0,0,0,0},//19
	{"uyvy422",1,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,1,0,8},//20
	{"uyvy411",1,FF_COLOR_YUV,FF_PIXEL_PACKED,0,2,0,8},//21
	{"bgr32",4,FF_COLOR_RGB,FF_PIXEL_PACKED,1,0,0,8},//22
	{"bgr565",3,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,5},//23
	{"bgr555",3,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,5},//24
	{"bgr8",1,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,8},//25
	{"bgr4",1,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,4},//26
	{"bgr4_byte",1,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,8},//27
	{"rgb8",1,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,8},//28
	{"rgb4",1,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,4},//29
	{"rgb4_byte",1,FF_COLOR_RGB,FF_PIXEL_PACKED,0,0,0,8},//30
	{"nv12",2,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,1,1,8},//31
	{"nv12",2,FF_COLOR_YUV,FF_PIXEL_PLANAR,0,1,1,8},//32
	{"rgb32_1",4,FF_COLOR_RGB,FF_PIXEL_PACKED,0,1,1,8},//33
	{"bgr32_1",4,FF_COLOR_RGB,FF_PIXEL_PACKED,1,0,0,8},//34
	{"gray16be",1,FF_COLOR_GRAY,FF_PIXEL_PLANAR,0,0,0,16},//35
	{"gray16le",1,FF_COLOR_GRAY,FF_PIXEL_PLANAR,0,0,0,16},//36
};
#define FF_PIXEL_PACKED   1 /* only one components containing all the channels */
#define FF_PIXEL_PALETTE  2  /* one components containing indexes for a palette */
static int volatile entangled_thread_counter=0;

typedef struct AVMD5{
    uint8_t  block[64];
    uint32_t ABCD[4];
    uint64_t len;
    int      b_used;
} AVMD5;

const int av_md5_size= sizeof(AVMD5);
#define le2me_32(x) (x)
static const uint8_t S[4][4] = {
    { 7, 12, 17, 22 },  /* Round 1 */
    { 5,  9, 14, 20 },  /* Round 2 */
    { 4, 11, 16, 23 },  /* Round 3 */
    { 6, 10, 15, 21 }   /* Round 4 */
};

static const uint32_t T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,   /* Round 1 */
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,   /* Round 2 */
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,   /* Round 3 */
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,   /* Round 4 */
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

#define CORE(i, a, b, c, d) \
        t = S[i>>4][i&3];\
        a += T[i];\
\
        switch(i>>4){\
        case 0: a += (d ^ (b&(c^d))) + X[      i &15 ]; break;\
        case 1: a += (c ^ (d&(c^b))) + X[ (1+5*i)&15 ]; break;\
        case 2: a += (b^c^d)         + X[ (5+3*i)&15 ]; break;\
        case 3: a += (c^(b|~d))      + X[ (  7*i)&15 ]; break;\
        }\
        a = b + (( a << t ) | ( a >> (32 - t) ));

static void body(uint32_t ABCD[4], uint32_t X[16]){

    int t;
//    int i;
    unsigned int a= ABCD[3];
    unsigned int b= ABCD[2];
    unsigned int c= ABCD[1];
    unsigned int d= ABCD[0];

#ifdef WORDS_BIGENDIAN
    for(i=0; i<16; i++)
        X[i]= bswap_32(X[i]);
#endif

#ifdef CONFIG_SMALL
    for( i = 0; i < 64; i++ ){
        CORE(i,a,b,c,d)
        t=d; d=c; c=b; b=a; a=t;
    }
#else
#define CORE2(i) CORE(i,a,b,c,d) CORE((i+1),d,a,b,c) CORE((i+2),c,d,a,b) CORE((i+3),b,c,d,a)
#define CORE4(i) CORE2(i) CORE2((i+4)) CORE2((i+8)) CORE2((i+12))
CORE4(0) CORE4(16) CORE4(32) CORE4(48)
#endif

    ABCD[0] += d;
    ABCD[1] += c;
    ABCD[2] += b;
    ABCD[3] += a;
}

void av_md5_init(AVMD5 *ctx){
    ctx->len    = 0;
    ctx->b_used = 0;

    ctx->ABCD[0] = 0x10325476;
    ctx->ABCD[1] = 0x98badcfe;
    ctx->ABCD[2] = 0xefcdab89;
    ctx->ABCD[3] = 0x67452301;
}

void av_md5_update(AVMD5 *ctx, const uint8_t *src, const int len){
    int i;

    ctx->len += len;

    for( i = 0; i < len; i++ ){
        ctx->block[ ctx->b_used++ ] = src[i];
        if( 64 == ctx->b_used ){
            body(ctx->ABCD, (uint32_t*) ctx->block);
            ctx->b_used = 0;
        }
    }
}

void av_md5_final(AVMD5 *ctx, uint8_t *dst){
    int i;

    ctx->block[ctx->b_used++] = 0x80;

    memset(&ctx->block[ctx->b_used], 0, 64 - ctx->b_used);

    if( 56 < ctx->b_used ){
        body( ctx->ABCD, (uint32_t*) ctx->block );
        memset(ctx->block, 0, 64);
    }

    for(i=0; i<8; i++)
        ctx->block[56+i] = (ctx->len << 3) >> (i<<3);

    body(ctx->ABCD, (uint32_t*) ctx->block);

    for(i=0; i<4; i++)
        ((uint32_t*)dst)[i]= le2me_32(ctx->ABCD[3-i]);
}

void av_md5_sum(uint8_t *dst, const uint8_t *src, const int len){
    AVMD5 ctx[1];

    av_md5_init(ctx);
    av_md5_update(ctx, src, len);
    av_md5_final(ctx, dst);
}

void avcodec_set_dimensions(AVCodecContext *s, int width, int height){
    s->coded_width = width;
    s->coded_height= height;
    s->width = -((-width )>>s->lowres);
    s->height= -((-height)>>s->lowres);
}

void av_destruct_packet_nofree(AVPacket *pkt)
{
    pkt->data = NULL; pkt->size = 0;
}

void av_set_pts_info(AVStream *s, int pts_wrap_bits,
                     int pts_num, int pts_den)
{
    s->pts_wrap_bits = pts_wrap_bits;
    s->time_base.num = pts_num;
    s->time_base.den = pts_den;
}
void pstrcpy(char *buf, int buf_size, const char *str)
{
    int c;
    char *q = buf;
	
    if (buf_size <= 0)
        return;
	
    for(;;) {
        c = *str++;
        if (c == 0 || q >= buf + buf_size - 1)
            break;
        *q++ = c;
    }
    *q = '\0';
}
/* fraction handling */

/**
 * f = val + (num / den) + 0.5.
 *
 * 'num' is normalized so that it is such as 0 <= num < den.
 *
 * @param f fractional number
 * @param val integer value
 * @param num must be >= 0
 * @param den must be >= 1
 */
static void av_frac_init(AVFrac *f, int64_t val, int64_t num, int64_t den)
{
    num += (den >> 1);
    if (num >= den) {
        val += num / den;
        num = num % den;
    }
    f->val = val;
    f->num = num;
    f->den = den;
}

/**
 * Fractionnal addition to f: f = f + (incr / f->den).
 *
 * @param f fractional number
 * @param incr increment, can be positive or negative
 */
static void av_frac_add(AVFrac *f, int64_t incr)
{
    int64_t num, den;

    num = f->num + incr;
    den = f->den;
    if (num < 0) {
        f->val += num / den;
        num = num % den;
        if (num < 0) {
            num += den;
            f->val--;
        }
    } else if (num >= den) {
        f->val += num / den;
        num = num % den;
    }
    f->num = num;
}

void *av_fast_realloc(void *ptr, unsigned int *size, unsigned int min_size)
{
    if(min_size < *size)
        return ptr;
	
    *size= FFMAX(17*min_size/16 + 32, min_size);
	
    return av_realloc(ptr, *size);
}

void av_init_packet(AVPacket *pkt)
{
    pkt->pts   = AV_NOPTS_VALUE;
    pkt->dts   = AV_NOPTS_VALUE;
    pkt->pos   = -1;
    pkt->duration = 0;
    pkt->flags = 0;
    pkt->stream_index = 0;
    pkt->destruct= av_destruct_packet_nofree;
}
enum PixelFormat avcodec_default_get_format(struct AVCodecContext *s, const enum PixelFormat * fmt){
    return fmt[0];
}
int match_ext(const char *filename, const char *extensions)
{
    const char *ext, *p;
    char ext1[32], *q;
	
    if(!filename)
        return 0;
	
    ext = strrchr(filename, '.');
    if (ext) {
        ext++;
        p = extensions;
        for(;;) {
            q = ext1;
            while (*p != '\0' && *p != ',' && q-ext1<sizeof(ext1)-1)
                *q++ = *p++;
            *q = '\0';
            if (!stricmp(ext1, ext))
                return 1;
            if (*p == '\0')
                break;
            p++;
        }
    }
    return 0;
}
int avcodec_check_dimensions(void *av_log_ctx, unsigned int w, unsigned int h){
    if((int)w>0 && (int)h>0 && (w+128)*(uint64_t)(h+128) < INT_MAX/4)
        return 0;
	
    av_log(av_log_ctx, AV_LOG_ERROR, "picture size invalid (%ux%u)\n", w, h);
    return -1;
}
static const char* format_to_name(void* ptr)
{
    AVFormatContext* fc = (AVFormatContext*) ptr;
    if(fc->iformat) return fc->iformat->name;
    else if(fc->oformat) return fc->oformat->name;
    else return "NULL";
}
void avcodec_get_chroma_sub_sample(int pix_fmt, int *h_shift, int *v_shift)
{
    *h_shift = pix_fmt_info[pix_fmt].x_chroma_shift;
    *v_shift = pix_fmt_info[pix_fmt].y_chroma_shift;
}
#define ALIGN(x, a) (((x)+(a)-1)&~((a)-1))
void avcodec_align_dimensions(AVCodecContext *s, int *width, int *height){
    int w_align= 1;
    int h_align= 1;
	
    switch(s->pix_fmt){
    case PIX_FMT_YUV420P:
    case PIX_FMT_YUV422:
    case PIX_FMT_UYVY422:
    case PIX_FMT_YUV422P:
    case PIX_FMT_YUV444P:
    case PIX_FMT_GRAY8:
    case PIX_FMT_GRAY16BE:
    case PIX_FMT_GRAY16LE:
    case PIX_FMT_YUVJ420P:
    case PIX_FMT_YUVJ422P:
    case PIX_FMT_YUVJ444P:
        w_align= 16; //FIXME check for non mpeg style codecs and use less alignment
        h_align= 16;
        break;
    case PIX_FMT_YUV411P:
    case PIX_FMT_UYVY411:
        w_align=32;
        h_align=8;
        break;
    case PIX_FMT_YUV410P:
        if(s->codec_id == CODEC_ID_SVQ1){
            w_align=64;
            h_align=64;
        }
    case PIX_FMT_RGB555:
        if(s->codec_id == CODEC_ID_RPZA){
            w_align=4;
            h_align=4;
        }
    case PIX_FMT_PAL8:
        if(s->codec_id == CODEC_ID_SMC){
            w_align=4;
            h_align=4;
        }
        break;
    case PIX_FMT_BGR24:
        if((s->codec_id == CODEC_ID_MSZH) || (s->codec_id == CODEC_ID_ZLIB)){
            w_align=4;
            h_align=4;
        }
        break;
    default:
        w_align= 1;
        h_align= 1;
        break;
    }
	
    *width = ALIGN(*width , w_align);
    *height= ALIGN(*height, h_align);
}

int avcodec_default_execute(AVCodecContext *c, int (*func)(AVCodecContext *c2, void *arg2),void **arg, int *ret, int count){
    int i;
	
    for(i=0; i<count; i++){
        int r= func(c, arg[i]);
        if(ret) ret[i]= r;
    }
    return 0;
}

static const AVOption options[]={
//	{"probesize", NULL, OFFSET(probesize), FF_OPT_TYPE_INT, 32000, 32, INT_MAX, D}, /* 32000 from mpegts.c: 1.0 second at 24Mbit/s */
/*	{"muxrate", "set mux rate", OFFSET(mux_rate), FF_OPT_TYPE_INT, DEFAULT, 0, INT_MAX, E},
	{"packetsize", "set packet size", OFFSET(packet_size), FF_OPT_TYPE_INT, DEFAULT, 0, INT_MAX, E},
	{"fflags", NULL, OFFSET(flags), FF_OPT_TYPE_FLAGS, DEFAULT, INT_MIN, INT_MAX, D|E, "fflags"},
	{"ignidx", "ignore index", 0, FF_OPT_TYPE_CONST, AVFMT_FLAG_IGNIDX, INT_MIN, INT_MAX, D, "fflags"},
	{"genpts", "generate pts", 0, FF_OPT_TYPE_CONST, AVFMT_FLAG_GENPTS, INT_MIN, INT_MAX, D, "fflags"},
	{"track", " set the track number", OFFSET(track), FF_OPT_TYPE_INT, DEFAULT, 0, INT_MAX, E},
	{"year", "set the year", OFFSET(year), FF_OPT_TYPE_INT, DEFAULT, INT_MIN, INT_MAX, E},
	{"analyzeduration", NULL, OFFSET(max_analyze_duration), FF_OPT_TYPE_INT, 3*AV_TIME_BASE, 0, INT_MAX, D},*/
	{NULL},
};
static const AVClass av_format_context_class = { "AVFormatContext", format_to_name, options };

static void avformat_get_context_defaults(AVFormatContext *s)
{
    memset(s, 0, sizeof(AVFormatContext));
	
    s->av_class = &av_format_context_class;
	
    av_opt_set_defaults(s);
}
void avcodec_get_frame_defaults(AVFrame *pic){
    memset(pic, 0, sizeof(AVFrame));
	
    pic->pts= AV_NOPTS_VALUE;
    pic->key_frame= 1;
	
}

AVFormatContext *av_alloc_format_context(void)
{
    AVFormatContext *ic;
    ic = av_malloc(sizeof(AVFormatContext));
    if (!ic) return ic;
    avformat_get_context_defaults(ic);
    ic->av_class = &av_format_context_class;
    return ic;
}
static const char* context_to_name(void* ptr) {
    AVCodecContext *avc= ptr;
	
    if(avc && avc->codec && avc->codec->name)
        return avc->codec->name;
    else
        return "NULL";
}
int avpicture_fill(AVPicture *picture, uint8_t *ptr,
                   int pix_fmt, int width, int height)
{
    int size, w2, h2, size2;
    const PixFmtInfo *pinfo;

    if(avcodec_check_dimensions(NULL, width, height))
        goto fail;

    pinfo = &pix_fmt_info[pix_fmt];
    size = width * height;
    switch(pix_fmt) {
    case PIX_FMT_YUV420P:
    case PIX_FMT_YUV422P:
    case PIX_FMT_YUV444P:
    case PIX_FMT_YUV410P:
    case PIX_FMT_YUV411P:
    case PIX_FMT_YUVJ420P:
    case PIX_FMT_YUVJ422P:
    case PIX_FMT_YUVJ444P:
        w2 = (width + (1 << pinfo->x_chroma_shift) - 1) >> pinfo->x_chroma_shift;
        h2 = (height + (1 << pinfo->y_chroma_shift) - 1) >> pinfo->y_chroma_shift;
        size2 = w2 * h2;
        picture->data[0] = ptr;
        picture->data[1] = picture->data[0] + size;
        picture->data[2] = picture->data[1] + size2;
        picture->linesize[0] = width;
        picture->linesize[1] = w2;
        picture->linesize[2] = w2;
        return size + 2 * size2;
    case PIX_FMT_NV12:
    case PIX_FMT_NV21:
        w2 = (width + (1 << pinfo->x_chroma_shift) - 1) >> pinfo->x_chroma_shift;
        h2 = (height + (1 << pinfo->y_chroma_shift) - 1) >> pinfo->y_chroma_shift;
        size2 = w2 * h2 * 2;
        picture->data[0] = ptr;
        picture->data[1] = picture->data[0] + size;
        picture->data[2] = NULL;
        picture->linesize[0] = width;
        picture->linesize[1] = w2;
        picture->linesize[2] = 0;
        return size + 2 * size2;
    case PIX_FMT_RGB24:
    case PIX_FMT_BGR24:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width * 3;
        return size * 3;
    case PIX_FMT_RGBA32:
    case PIX_FMT_BGR32:
    case PIX_FMT_RGB32_1:
    case PIX_FMT_BGR32_1:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width * 4;
        return size * 4;
    case PIX_FMT_GRAY16BE:
    case PIX_FMT_GRAY16LE:
    case PIX_FMT_BGR555:
    case PIX_FMT_BGR565:
    case PIX_FMT_RGB555:
    case PIX_FMT_RGB565:
    case PIX_FMT_YUV422:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width * 2;
        return size * 2;
    case PIX_FMT_UYVY422:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width * 2;
        return size * 2;
    case PIX_FMT_UYVY411:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width + width/2;
        return size + size/2;
    case PIX_FMT_RGB8:
    case PIX_FMT_BGR8:
    case PIX_FMT_RGB4_BYTE:
    case PIX_FMT_BGR4_BYTE:
    case PIX_FMT_GRAY8:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width;
        return size;
    case PIX_FMT_RGB4:
    case PIX_FMT_BGR4:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = width / 2;
        return size / 2;
    case PIX_FMT_MONOWHITE:
    case PIX_FMT_MONOBLACK:
        picture->data[0] = ptr;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->linesize[0] = (width + 7) >> 3;
        return picture->linesize[0] * height;
    case PIX_FMT_PAL8:
        size2 = (size + 3) & ~3;
        picture->data[0] = ptr;
        picture->data[1] = ptr + size2; /* palette is stored here as 256 32 bit words */
        picture->data[2] = NULL;
        picture->linesize[0] = width;
        picture->linesize[1] = 4;
        return size2 + 256 * 4;
    default:
fail:
        picture->data[0] = NULL;
        picture->data[1] = NULL;
        picture->data[2] = NULL;
        picture->data[3] = NULL;
        return -1;
    }
}

int avpicture_get_size(int pix_fmt, int width, int height)
{
    AVPicture dummy_pict;
    return avpicture_fill(&dummy_pict, NULL, pix_fmt, width, height);
}
void ff_img_copy_plane(uint8_t *dst, int dst_wrap,
                           const uint8_t *src, int src_wrap,
                           int width, int height)
{
    if((!dst) || (!src))
        return;
    for(;height > 0; height--) {
        memcpy(dst, src, width);
        dst += dst_wrap;
        src += src_wrap;
    }
}

/**
 * Copy image 'src' to 'dst'.
 */
void img_copy(AVPicture *dst, const AVPicture *src,
              int pix_fmt, int width, int height)
{
    int bwidth, bits, i;
    const PixFmtInfo *pf = &pix_fmt_info[pix_fmt];

    pf = &pix_fmt_info[pix_fmt];
    switch(pf->pixel_type) {
    case FF_PIXEL_PACKED:
        switch(pix_fmt) {
        case PIX_FMT_YUV422:
        case PIX_FMT_UYVY422:
        case PIX_FMT_RGB565:
        case PIX_FMT_RGB555:
        case PIX_FMT_BGR565:
        case PIX_FMT_BGR555:
            bits = 16;
            break;
        case PIX_FMT_UYVY411:
            bits = 12;
            break;
        default:
            bits = pf->depth * pf->nb_channels;
            break;
        }
        bwidth = (width * bits + 7) >> 3;
        ff_img_copy_plane(dst->data[0], dst->linesize[0],
                       src->data[0], src->linesize[0],
                       bwidth, height);
        break;
    case FF_PIXEL_PLANAR:
        for(i = 0; i < pf->nb_channels; i++) {
            int w, h;
            w = width;
            h = height;
            if (i == 1 || i == 2) {
                w >>= pf->x_chroma_shift;
                h >>= pf->y_chroma_shift;
            }
            bwidth = (w * pf->depth + 7) >> 3;
            ff_img_copy_plane(dst->data[i], dst->linesize[i],
                           src->data[i], src->linesize[i],
                           bwidth, h);
        }
        break;
    case FF_PIXEL_PALETTE:
        ff_img_copy_plane(dst->data[0], dst->linesize[0],
                       src->data[0], src->linesize[0],
                       width, height);
        /* copy the palette */
        ff_img_copy_plane(dst->data[1], dst->linesize[1],
                       src->data[1], src->linesize[1],
                       4, 256);
        break;
    }
}
int avcodec_default_reget_buffer(AVCodecContext *s, AVFrame *pic){
//    AVFrame temp_pic;
   // int i;

    /* If no picture return a new buffer */
    if(pic->data[0] == NULL) {
        /* We will copy from buffer, so must be readable */
        pic->buffer_hints |= FF_BUFFER_HINTS_READABLE;
        return s->get_buffer(s, pic);
    }

    /* If internal buffer type return the same buffer */
    if(pic->type == FF_BUFFER_TYPE_INTERNAL)
        return 0;

#if !defined(CONFIG_MSVC) && !defined(__SYMBIAN32__)
		/*
     * Not internal type and reget_buffer not overridden, emulate cr buffer
     */
    temp_pic = *pic;
    for(i = 0; i < 4; i++)
        pic->data[i] = pic->base[i] = NULL;
    pic->opaque = NULL;
    /* Allocate new frame */
    if (s->get_buffer(s, pic))
        return -1;
    /* Copy image data from old buffer to new buffer */
    img_copy((AVPicture*)pic, (AVPicture*)&temp_pic, s->pix_fmt, s->width,
             s->height);
    s->release_buffer(s, &temp_pic); // Release old frame
#endif
    return 0;
}
static AVClass av_codec_context_class = { "AVCodecContext", context_to_name, options };

int avcodec_default_get_buffer(AVCodecContext *s, AVFrame *pic){
    int i;
    int w= s->width;
    int h= s->height;
    InternalBuffer *buf;
    int *picture_number;

//    assert(pic->data[0]==NULL);
//    assert(INTERNAL_BUFFER_SIZE > s->internal_buffer_count);

    if(avcodec_check_dimensions(s,w,h))
        return -1;

    if(s->internal_buffer==NULL){
        s->internal_buffer= av_mallocz(INTERNAL_BUFFER_SIZE*sizeof(InternalBuffer));
    }
#if 0
    s->internal_buffer= av_fast_realloc(
        s->internal_buffer,
        &s->internal_buffer_size,
        sizeof(InternalBuffer)*FFMAX(99,  s->internal_buffer_count+1)/*FIXME*/
        );
#endif

    buf= &((InternalBuffer*)s->internal_buffer)[s->internal_buffer_count];
    picture_number= &(((InternalBuffer*)s->internal_buffer)[INTERNAL_BUFFER_SIZE-1]).last_pic_num; //FIXME ugly hack
    (*picture_number)++;

    if(buf->base[0]){
        pic->age= *picture_number - buf->last_pic_num;
        buf->last_pic_num= *picture_number;
    }else{
        int h_chroma_shift, v_chroma_shift;
        int pixel_size, size[3];
        AVPicture picture;

        avcodec_get_chroma_sub_sample(s->pix_fmt, &h_chroma_shift, &v_chroma_shift);

        avcodec_align_dimensions(s, &w, &h);

        if(!(s->flags&CODEC_FLAG_EMU_EDGE)){
            w+= EDGE_WIDTH*2;
            h+= EDGE_WIDTH*2;
        }
        avpicture_fill(&picture, NULL, s->pix_fmt, w, h);
        pixel_size= picture.linesize[0]*8 / w;
//av_log(NULL, AV_LOG_ERROR, "%d %d %d %d\n", (int)picture.data[1], w, h, s->pix_fmt);
//        assert(pixel_size>=1);
            //FIXME next ensures that linesize= 2^x uvlinesize, thats needed because some MC code assumes it
        if(pixel_size == 3*8)
            w= ALIGN(w, STRIDE_ALIGN<<h_chroma_shift);
        else
            w= ALIGN(pixel_size*w, STRIDE_ALIGN<<(h_chroma_shift+3)) / pixel_size;
        size[1] = avpicture_fill(&picture, NULL, s->pix_fmt, w, h);
        size[0] = picture.linesize[0] * h;
        size[1] -= size[0];
        if(picture.data[2])
            size[1]= size[2]= size[1]/2;
        else
            size[2]= 0;

        buf->last_pic_num= -256*256*256*64;
        memset(buf->base, 0, sizeof(buf->base));
        memset(buf->data, 0, sizeof(buf->data));

        for(i=0; i<3 && size[i]; i++){
            const int h_shift= i==0 ? 0 : h_chroma_shift;
            const int v_shift= i==0 ? 0 : v_chroma_shift;

            buf->linesize[i]= picture.linesize[i];

            buf->base[i]= av_malloc(size[i]+16); //FIXME 16
            if(buf->base[i]==NULL) {
			    av_log(s, AV_LOG_ERROR, "get buffer: error allocating %d bytes\n", size[i]+16);
				if (size[i]+16 > (INT_MAX-16) ) {
					av_log(s, AV_LOG_ERROR, "get buffer: mismatch in alloc sizes: %d vs %d\n", size[i]+16, (INT_MAX-16) );
				}

				return -1;
			}
            memset(buf->base[i], 128, size[i]);

            // no edge if EDEG EMU or not planar YUV, we check for PAL8 redundantly to protect against a exploitable bug regression ...
            if((s->flags&CODEC_FLAG_EMU_EDGE) || (s->pix_fmt == PIX_FMT_PAL8) || !size[2])
                buf->data[i] = buf->base[i];
            else
                buf->data[i] = buf->base[i] + ALIGN((buf->linesize[i]*EDGE_WIDTH>>v_shift) + (EDGE_WIDTH>>h_shift), STRIDE_ALIGN);
        }
        pic->age= 256*256*256*64;
    }
    pic->type= FF_BUFFER_TYPE_INTERNAL;

    for(i=0; i<4; i++){
        pic->base[i]= buf->base[i];
        pic->data[i]= buf->data[i];
        pic->linesize[i]= buf->linesize[i];
    }
    s->internal_buffer_count++;

    return 0;
}

void avcodec_default_release_buffer(AVCodecContext *s, AVFrame *pic){
    int i;
    InternalBuffer *buf, *last, temp;

//    assert(pic->type==FF_BUFFER_TYPE_INTERNAL);
//    assert(s->internal_buffer_count);

    buf = NULL; /* avoids warning */
    for(i=0; i<s->internal_buffer_count; i++){ //just 3-5 checks so is not worth to optimize
        buf= &((InternalBuffer*)s->internal_buffer)[i];
        if(buf->data[0] == pic->data[0])
            break;
    }
  //  assert(i < s->internal_buffer_count);
    s->internal_buffer_count--;
    last = &((InternalBuffer*)s->internal_buffer)[s->internal_buffer_count];

    temp= *buf;
    *buf= *last;
    *last= temp;

    for(i=0; i<3; i++){
        pic->data[i]=NULL;
//        pic->base[i]=NULL;
    }
//printf("R%X\n", pic->opaque);
}
const AVOption *av_next_option(void *obj, const AVOption *last){
    if(last && last[1].name) return ++last;
    else if(last)            return NULL;
    else                     return (*(AVClass**)obj)->option;
}

int av_set_parameters(AVFormatContext *s, AVFormatParameters *ap)
{
  int ret;

    if (s->oformat->priv_data_size > 0) {
	s->priv_data = av_mallocz(s->oformat->priv_data_size);
	if (!s->priv_data)
		return AVERROR_NOMEM;
	} else
    s->priv_data = NULL;

    if (s->oformat->set_parameters) {
	       ret = s->oformat->set_parameters(s, ap);
	
	if (ret < 0)
		return ret;
}
return 0;
}
static const AVOption *find_opt(void *v, const char *name, const char *unit){
    AVClass *c= *(AVClass**)v; //FIXME silly way of storing AVClass
    const AVOption *o= c->option;
	
    for(;o && o->name; o++){
        if(!strcmp(o->name, name) && (!unit || !strcmp(o->unit, unit)) )
            return o;
    }
    return NULL;
}

static const AVOption *av_set_number(void *obj, const char *name, double num, int den, int64_t intnum){
    const AVOption *o= find_opt(obj, name, NULL);
    void *dst;
    if(!o || o->offset<=0)
        return NULL;
	
    if(o->max*den < num*intnum || o->min*den > num*intnum) {
        av_log(NULL, AV_LOG_ERROR, "Value %lf for parameter '%s' out of range.\n", num, name);
        return NULL;
    }
	
    dst= ((uint8_t*)obj) + o->offset;
	
    switch(o->type){
    case FF_OPT_TYPE_FLAGS:
    case FF_OPT_TYPE_INT:   *(int       *)dst= (int)(num/den)*intnum; break;
    case FF_OPT_TYPE_INT64: *(int64_t   *)dst= (int)(num/den)*intnum; break;
    case FF_OPT_TYPE_FLOAT: *(float     *)dst= num*intnum/den;         break;
    case FF_OPT_TYPE_DOUBLE:*(double    *)dst= num*intnum/den;         break;
    case FF_OPT_TYPE_RATIONAL:
        if((int)num == num) *(AVRational*)dst= _AVRational(num*intnum, den);
        else                *(AVRational*)dst= av_d2q(num*intnum/den, 1<<24);
    default:
        return NULL;
    }
    return o;
}
const AVOption *av_set_double(void *obj, const char *name, double n){
    return av_set_number(obj, name, n, 1, 1);
}

const AVOption *av_set_q(void *obj, const char *name, AVRational n){
    return av_set_number(obj, name, n.num, n.den, 1);
}

const AVOption *av_set_int(void *obj, const char *name, int64_t n){
    return av_set_number(obj, name, 1, 1, n);
}

void av_opt_set_defaults(void *s)
{
    const AVOption *opt = NULL;
    while ((opt = av_next_option(s, opt)) != NULL) {
        switch(opt->type) {
		case FF_OPT_TYPE_CONST:
			/* Nothing to be done here */
            break;
		case FF_OPT_TYPE_FLAGS:
		case FF_OPT_TYPE_INT: {
			int val;
			val = opt->default_val;
			av_set_int(s, opt->name, val);
							  }
            break;
		case FF_OPT_TYPE_FLOAT: {
			double val;
			val = opt->default_val;
			av_set_double(s, opt->name, val);
								}
            break;
		case FF_OPT_TYPE_RATIONAL: {
			AVRational val;
			val = av_d2q(opt->default_val, INT_MAX);
			av_set_q(s, opt->name, val);
								   }
            break;
		case FF_OPT_TYPE_STRING:
			/* Cannot set default for string as default_val is of type * double */
            break;
		default:
			av_log(s, AV_LOG_DEBUG, "AVOption type %d of option %s not implemented yet\n", opt->type, opt->name);
        }
    }
}
void avcodec_get_context_defaults(AVCodecContext *s){
    memset(s, 0, sizeof(AVCodecContext));

    s->av_class= &av_codec_context_class;

//    av_opt_set_defaults(s);

    s->rc_eq= "tex^qComp";
    s->time_base= _AVRational(0,1);
    s->get_buffer= avcodec_default_get_buffer;
    s->release_buffer= avcodec_default_release_buffer;
    s->get_format= avcodec_default_get_format;
    s->execute= avcodec_default_execute;
    s->sample_aspect_ratio= _AVRational(1,1);
    s->pix_fmt= PIX_FMT_NONE;
    s->sample_fmt= SAMPLE_FMT_S16; // FIXME: set to NONE

    s->palctrl = NULL;
    s->reget_buffer= avcodec_default_reget_buffer;
}

/**
 * allocates a AVCodecContext and set it to defaults.
 * this can be deallocated by simply calling free()
 */
AVCodecContext *avcodec_alloc_context(void){
    AVCodecContext *avctx= av_malloc(sizeof(AVCodecContext));

    if(avctx==NULL) return NULL;

    avcodec_get_context_defaults(avctx);

    return avctx;
}
AVStream *av_new_stream(AVFormatContext *s, int id)
{
    AVStream *st;
    int i;
	
    if (s->nb_streams >= MAX_STREAMS)
        return NULL;
	
    st = av_mallocz(sizeof(AVStream));
    if (!st)
        return NULL;
	
    st->codec= avcodec_alloc_context();
    if (s->iformat) {
        /* no default bitrate if decoding */
        st->codec->bit_rate = 0;
    }
    st->index = s->nb_streams;
    st->id = id;
    st->start_time = AV_NOPTS_VALUE;
    st->duration = AV_NOPTS_VALUE;
    st->cur_dts = AV_NOPTS_VALUE;
	
    /* default pts settings is MPEG like */
    av_set_pts_info(st, 33, 1, 90000);
    st->last_IP_pts = AV_NOPTS_VALUE;
    for(i=0; i<MAX_REORDER_DELAY+1; i++)
        st->pts_buffer[i]= AV_NOPTS_VALUE;
	
    s->streams[s->nb_streams++] = st;
    return st;
}

#define AV_RL32(x) ((((uint8_t*)(x))[3] << 24) | \
	(((uint8_t*)(x))[2] << 16) | \
	(((uint8_t*)(x))[1] << 8) | \
                    ((uint8_t*)(x))[0])
#define AV_WL32(p, d) do {                   \
	((uint8_t*)(p))[0] = (d);               \
	((uint8_t*)(p))[1] = (d)>>8;            \
	((uint8_t*)(p))[2] = (d)>>16;           \
	((uint8_t*)(p))[3] = (d)>>24;           \
    } while(0)
void av_lfg_init(AVLFG1 *c, unsigned int seed){
    uint8_t tmp[16]={0};
    int i;
	
    for(i=8; i<64; i+=4){
        AV_WL32(tmp, seed);
		tmp[4]=i;
        av_md5_sum(tmp, tmp,  16);
        c->state[i  ]= AV_RL32(tmp);
        c->state[i+1]= AV_RL32(tmp+4);
        c->state[i+2]= AV_RL32(tmp+8);
        c->state[i+3]= AV_RL32(tmp+12);
    }
    c->index=0;
}

AVOutputFormat *guess_format(const char *short_name, const char *filename,
                             const char *mime_type)
{
AVOutputFormat *fmt, *fmt_found;
int score_max, score;

/* specific test for image sequences */
#ifdef CONFIG_IMAGE2_MUXER
if (!short_name && filename &&
	av_filename_number_test(filename) &&
	av_guess_image2_codec(filename) != CODEC_ID_NONE) {
	return guess_format("image2", NULL, NULL);
}
#endif
/* find the proper file type */
fmt_found = NULL;
score_max = 0;
fmt = first_oformat;

while (fmt != NULL) {
	
	score = 0;
	if (fmt->name && short_name && !strcmp(fmt->name, short_name))
		score += 100;
	if (fmt->mime_type && mime_type && !strcmp(fmt->mime_type, mime_type))
		score += 10;
	if (filename && fmt->extensions &&
		match_ext(filename, fmt->extensions)) {
		score += 5;
	}
	if (score > score_max) {
		score_max = score;
		fmt_found = fmt;
	}
	fmt = fmt->next;
}
return fmt_found;
}


extern unsigned int av_codec_get_tag(const AVCodecTag *tags[4], enum CodecID id);

int av_write_header(AVFormatContext *s)
{
    int ret;
	unsigned int i;
    AVStream *st;
	
    // some sanity checks
    for(i=0;i<s->nb_streams;i++) {
        st = s->streams[i];
		
        switch (st->codec->codec_type) {
        case CODEC_TYPE_AUDIO:
            if(st->codec->sample_rate<=0){
                av_log(s, AV_LOG_ERROR, "sample rate not set\n");
                return -1;
            }
            break;
        case CODEC_TYPE_VIDEO:
            if(st->codec->time_base.num<=0 || st->codec->time_base.den<=0){ //FIXME audio too?
                av_log(s, AV_LOG_ERROR, "time base not set\n");
                return -1;
            }
            break;
        }
		
        if(s->oformat->codec_tag){
            if(st->codec->codec_tag){
                //FIXME
                //check that tag + id is in the table
                //if neither is in the table -> ok
                //if tag is in the table with another id -> FAIL
                //if id is in the table with another tag -> FAIL unless strict < ?
            }else
                st->codec->codec_tag= av_codec_get_tag(s->oformat->codec_tag, st->codec->codec_id);
        }
    }
	
    if (!s->priv_data && s->oformat->priv_data_size > 0) {
        s->priv_data = av_mallocz(s->oformat->priv_data_size);
        if (!s->priv_data)
            return AVERROR_NOMEM;
    }
	
    if(s->oformat->write_header){
        ret = s->oformat->write_header(s);
        if (ret < 0)
            return ret;
    }
	
    /* init PTS generation */
    for(i=0;i<s->nb_streams;i++) {
        int64_t den = AV_NOPTS_VALUE;
        st = s->streams[i];
		
        switch (st->codec->codec_type) {
        case CODEC_TYPE_AUDIO:
            den = (int64_t)st->time_base.num * st->codec->sample_rate;
            break;
        case CODEC_TYPE_VIDEO:
            den = (int64_t)st->time_base.num * st->codec->time_base.den;
            break;
        default:
            break;
        }
        if (den != AV_NOPTS_VALUE) {
            if (den <= 0)
                return AVERROR_INVALIDDATA;
            av_frac_init(&st->pts, 0, 0, den);
        }
    }
    return 0;
}
int av_get_bits_per_sample(enum CodecID codec_id){
    switch(codec_id){
    case CODEC_ID_ADPCM_SBPRO_2:
        return 2;
    case CODEC_ID_ADPCM_SBPRO_3:
        return 3;
    case CODEC_ID_ADPCM_SBPRO_4:
    case CODEC_ID_ADPCM_CT:
        return 4;
    case CODEC_ID_PCM_ALAW:
    case CODEC_ID_PCM_MULAW:
    case CODEC_ID_PCM_S8:
    case CODEC_ID_PCM_U8:
        return 8;
    case CODEC_ID_PCM_S16BE:
    case CODEC_ID_PCM_S16LE:
    case CODEC_ID_PCM_U16BE:
    case CODEC_ID_PCM_U16LE:
        return 16;
    case CODEC_ID_PCM_S24DAUD:
    case CODEC_ID_PCM_S24BE:
    case CODEC_ID_PCM_S24LE:
    case CODEC_ID_PCM_U24BE:
    case CODEC_ID_PCM_U24LE:
        return 24;
    case CODEC_ID_PCM_S32BE:
    case CODEC_ID_PCM_S32LE:
    case CODEC_ID_PCM_U32BE:
    case CODEC_ID_PCM_U32LE:
        return 32;
    default:
        return 0;
    }
}

static int get_audio_frame_size(AVCodecContext *enc, int size)
{
    int frame_size;
	
    if (enc->frame_size <= 1) {
        int bits_per_sample = av_get_bits_per_sample(enc->codec_id);
		
        if (bits_per_sample) {
            if (enc->channels == 0)
                return -1;
            frame_size = (size << 3) / (bits_per_sample * enc->channels);
        } else {
            /* used for example by ADPCM codecs */
            if (enc->bit_rate == 0)
                return -1;
            frame_size = (size * 8 * enc->sample_rate) / enc->bit_rate;
        }
    } else {
        frame_size = enc->frame_size;
    }
    return frame_size;
}
/**
 * Return the frame duration in seconds, return 0 if not available.
 */
static void compute_frame_duration(int *pnum, int *pden, AVStream *st,
                                   AVCodecParserContext *pc, AVPacket *pkt)
{
    int frame_size;

    *pnum = 0;
    *pden = 0;
    switch(st->codec->codec_type) {
    case CODEC_TYPE_VIDEO:
        if(st->time_base.num*INT64_C(1000) > st->time_base.den){
            *pnum = st->time_base.num;
            *pden = st->time_base.den;
        }else if(st->codec->time_base.num*INT64_C(1000) > st->codec->time_base.den){
            *pnum = st->codec->time_base.num;
            *pden = st->codec->time_base.den;
            if (pc && pc->repeat_pict) {
                *pden *= 2;
                *pnum = (*pnum) * (2 + pc->repeat_pict);
            }
        }
        break;
    case CODEC_TYPE_AUDIO:
        frame_size = get_audio_frame_size(st->codec, pkt->size);
        if (frame_size < 0)
            break;
        *pnum = frame_size;
        *pden = st->codec->sample_rate;
        break;
    default:
        break;
    }
}
//FIXME merge with compute_pkt_fields
static int compute_pkt_fields2(AVStream *st, AVPacket *pkt){
    int delay = FFMAX(st->codec->has_b_frames, !!st->codec->max_b_frames);
    int num, den, frame_size, i;

//    av_log(st->codec, AV_LOG_DEBUG, "av_write_frame: pts:%"PRId64" dts:%"PRId64" cur_dts:%"PRId64" b:%d size:%d st:%d\n", pkt->pts, pkt->dts, st->cur_dts, delay, pkt->size, pkt->stream_index);

/*    if(pkt->pts == AV_NOPTS_VALUE && pkt->dts == AV_NOPTS_VALUE)
        return -1;*/

    /* duration field */
    if (pkt->duration == 0) {
        compute_frame_duration(&num, &den, st, NULL, pkt);
        if (den && num) {
            pkt->duration = av_rescale(1, num * (int64_t)st->time_base.den, den * (int64_t)st->time_base.num);
        }
    }

    //XXX/FIXME this is a temporary hack until all encoders output pts
    if((pkt->pts == 0 || pkt->pts == AV_NOPTS_VALUE) && pkt->dts == AV_NOPTS_VALUE && !delay){
        pkt->dts=
//        pkt->pts= st->cur_dts;
        pkt->pts= st->pts.val;
    }

    //calculate dts from pts
    if(pkt->pts != AV_NOPTS_VALUE && pkt->dts == AV_NOPTS_VALUE){
        st->pts_buffer[0]= pkt->pts;
        for(i=1; i<delay+1 && st->pts_buffer[i] == AV_NOPTS_VALUE; i++)
            st->pts_buffer[i]= (i-delay-1) * pkt->duration;
        for(i=0; i<delay && st->pts_buffer[i] > st->pts_buffer[i+1]; i++)
            FFSWAP(__int64, st->pts_buffer[i], st->pts_buffer[i+1]);

        pkt->dts= st->pts_buffer[0];
    }

    if(st->cur_dts && st->cur_dts != AV_NOPTS_VALUE && st->cur_dts >= pkt->dts){
        av_log(NULL, AV_LOG_ERROR, "error, non monotone timestamps %"PRId64" >= %"PRId64"\n", st->cur_dts, pkt->dts);
        return -1;
    }
    if(pkt->dts != AV_NOPTS_VALUE && pkt->pts != AV_NOPTS_VALUE && pkt->pts < pkt->dts){
        av_log(NULL, AV_LOG_ERROR, "error, pts < dts\n");
        return -1;
    }

//    av_log(NULL, AV_LOG_DEBUG, "av_write_frame: pts2:%"PRId64" dts2:%"PRId64"\n", pkt->pts, pkt->dts);
    st->cur_dts= pkt->dts;
    st->pts.val= pkt->dts;

    /* update pts */
    switch (st->codec->codec_type) {
    case CODEC_TYPE_AUDIO:
        frame_size = get_audio_frame_size(st->codec, pkt->size);

        /* HACK/FIXME, we skip the initial 0-size packets as they are most likely equal to the encoder delay,
           but it would be better if we had the real timestamps from the encoder */
        if (frame_size >= 0 && (pkt->size || st->pts.num!=st->pts.den>>1 || st->pts.val)) {
            av_frac_add(&st->pts, (int64_t)st->time_base.den * frame_size);
        }
        break;
    case CODEC_TYPE_VIDEO:
        av_frac_add(&st->pts, (int64_t)st->time_base.den * st->codec->time_base.num);
        break;
    default:
        break;
    }
    return 0;
}

static void truncate_ts(AVStream *st, AVPacket *pkt){
    int64_t pts_mask = (INT64_C(2) << (st->pts_wrap_bits-1)) - 1;
	
	
    if (pkt->pts != AV_NOPTS_VALUE)
        pkt->pts &= pts_mask;
    if (pkt->dts != AV_NOPTS_VALUE)
        pkt->dts &= pts_mask;
}

int av_rewrite_audio_header(AVFormatContext *s,int64_t codecid,int64_t samplerate,int bit_depath,int channels)
{
	int ret;
	if(s->oformat->rewrite_audio_header)
             ret = s->oformat->rewrite_audio_header(s,codecid,samplerate,bit_depath,channels);
	if(!ret)
        ret= url_ferror(&s->pb);
    return ret;
}
int av_rewrite_video_header(AVFormatContext *s,int64_t video_width,int64_t video_height,int64_t codecid,const unsigned char *extra_data,int64_t extra_data_len)
{
	int ret;
	if(s->oformat->rewrite_video_header)
             ret = s->oformat->rewrite_video_header(s,video_width,video_height,codecid,extra_data,extra_data_len);
	if(!ret)
        ret= url_ferror(&s->pb);
    return ret;

}
int av_write_frame(AVFormatContext *s, AVPacket *pkt)
{
    int ret;

    ret=compute_pkt_fields2(s->streams[pkt->stream_index], pkt);
	if(ret<0 && !(s->oformat->flags & AVFMT_NOTIMESTAMPS))
	{	
		return ret;
	}

    truncate_ts(s->streams[pkt->stream_index], pkt);
	
    ret= s->oformat->write_packet(s, pkt);
    if(!ret)
        ret= url_ferror(&s->pb);
    return ret;
}
void av_destruct_packet(AVPacket *pkt)
{
    av_free(pkt->data);
    pkt->data = NULL; pkt->size = 0;
}
int av_dup_packet(AVPacket *pkt)
{
    if (pkt->destruct != av_destruct_packet) {
        uint8_t *data;
        /* we duplicate the packet and don't forget to put the padding
           again */
        if((unsigned)pkt->size > (unsigned)pkt->size + FF_INPUT_BUFFER_PADDING_SIZE)
            return AVERROR_NOMEM;
        data = av_malloc(pkt->size + FF_INPUT_BUFFER_PADDING_SIZE);
        if (!data) {
            return AVERROR_NOMEM;
        }
        memcpy(data, pkt->data, pkt->size);
        memset(data + pkt->size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
        pkt->data = data;
        pkt->destruct = av_destruct_packet;
    }
    return 0;
}
/**
 * Interleave a packet per DTS in an output media file.
 *
 * Packets with pkt->destruct == av_destruct_packet will be freed inside this function,
 * so they cannot be used after it, note calling av_free_packet() on them is still safe.
 *
 * @param s media file handle
 * @param out the interleaved packet will be output here
 * @param in the input packet
 * @param flush 1 if no further packets are available as input and all
 *              remaining packets should be output
 * @return 1 if a packet was output, 0 if no packet could be output,
 *         < 0 if an error occured
 */
int av_interleave_packet_per_dts(AVFormatContext *s, AVPacket *out, AVPacket *pkt, int flush){
    AVPacketList *pktl, **next_point, *this_pktl;
    int stream_count=0;
    int streams[MAX_STREAMS];

    if(pkt){
        AVStream *st= s->streams[ pkt->stream_index];

//        assert(pkt->destruct != av_destruct_packet); //FIXME

        this_pktl = av_mallocz(sizeof(AVPacketList));
        this_pktl->pkt= *pkt;
        if(pkt->destruct == av_destruct_packet)
            pkt->destruct= NULL; // non shared -> must keep original from being freed
        else
            av_dup_packet(&this_pktl->pkt);  //shared -> must dup

        next_point = &s->packet_buffer;
        while(*next_point){
            AVStream *st2= s->streams[ (*next_point)->pkt.stream_index];
            int64_t left=  st2->time_base.num * (int64_t)st ->time_base.den;
            int64_t right= st ->time_base.num * (int64_t)st2->time_base.den;
            if((*next_point)->pkt.dts * left > pkt->dts * right) //FIXME this can overflow
                break;
            next_point= &(*next_point)->next;
        }
        this_pktl->next= *next_point;
        *next_point= this_pktl;
    }

    memset(streams, 0, sizeof(streams));
    pktl= s->packet_buffer;
    while(pktl){
//av_log(s, AV_LOG_DEBUG, "show st:%d dts:%"PRId64"\n", pktl->pkt.stream_index, pktl->pkt.dts);
        if(streams[ pktl->pkt.stream_index ] == 0)
            stream_count++;
        streams[ pktl->pkt.stream_index ]++;
        pktl= pktl->next;
    }

    if(s->nb_streams == stream_count || (flush && stream_count)){
        pktl= s->packet_buffer;
        *out= pktl->pkt;

        s->packet_buffer= pktl->next;
        av_freep(&pktl);
        return 1;
    }else{
        av_init_packet(out);
        return 0;
    }
}
static int av_interleave_packet(AVFormatContext *s, AVPacket *out, AVPacket *in, int flush){
    if(s->oformat->interleave_packet)
        return s->oformat->interleave_packet(s, out, in, flush);
    else
        return av_interleave_packet_per_dts(s, out, in, flush);
}
int av_write_trailer(AVFormatContext *s)
{
    int ret;unsigned int i;
	
    for(;;){
        AVPacket pkt;
        ret= av_interleave_packet(s, &pkt, NULL, 1);
        if(ret<0) //FIXME cleanup needed for ret<0 ?
            goto fail;
        if(!ret)
            break;
		
        truncate_ts(s->streams[pkt.stream_index], &pkt);
        ret= s->oformat->write_packet(s, &pkt);
		
        av_free_packet(&pkt);
		
        if(ret<0)
            goto fail;
        if(url_ferror(&s->pb))
            goto fail;
    }
	
    if(s->oformat->write_trailer)
        ret = s->oformat->write_trailer(s);
fail:
    if(ret == 0)
		ret=url_ferror(&s->pb);
    for(i=0;i<s->nb_streams;i++)
        av_freep(&s->streams[i]->priv_data);
    av_freep(&s->priv_data);
    return ret;
}
int strstart(const char *str, const char *val, const char **ptr)
{
    const char *p, *q;
    p = str;
    q = val;
    while (*q != '\0') {
        if (*p != *q)
            return 0;
        p++;
        q++;
    }
    if (ptr)
        *ptr = p;
    return 1;
}
static int file_open(URLContext *h, const char *filename, int flags)
{
    char *access;
    FILE *fd;

    strstart(filename, "file:", &filename);
	
    if (flags & URL_RDWR) {
        access = "w+b";
    } else if (flags & URL_WRONLY) {
        access = "wb";
    } else {
        access = "rb";
    }
    fd = fopen(filename, access);
    if (fd == NULL)
        return -ENOENT;

    h->priv_data = (void *) fd;
    return 0;
}

static int file_read(URLContext *h, unsigned char *buf, int size)
{
    FILE *fd = h->priv_data;
    return fread(buf, 1, size, fd);
}

static int file_write(URLContext *h, unsigned char *buf, int size)
{
    FILE * fd = (FILE *)h->priv_data;
    return fwrite(buf, 1, size, fd);
}

/* XXX: use llseek */
static offset_t file_seek(URLContext *h, offset_t pos, int whence)
{
    FILE *fd = (FILE *)h->priv_data;
    return fseek(fd, pos, whence);
}

static int file_close(URLContext *h)
{
    FILE *fd = (FILE *)h->priv_data;
    return fclose(fd);
}

URLProtocol file_protocol = {
    "file",
		file_open,
		file_read,
		file_write,
		file_seek,
		file_close,
};

void av_register_output_format(AVOutputFormat *format)
{
    AVOutputFormat **p;
    p = &first_oformat;
    while (*p != NULL) p = &(*p)->next;
    *p = format;
    format->next = NULL;
	
	
}