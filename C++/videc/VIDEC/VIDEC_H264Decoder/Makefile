### FILES and DIRECTORIES ###

SRCS_C=\
    libavcodec/allcodecs.c \
    libavcodec/avpacket.c \
    libavcodec/bitstream.c \
    libavcodec/cabac.c \
    libavcodec/dsputil.c \
    libavcodec/error_resilience.c \
    libavcodec/golomb.c \
    libavcodec/h264.c \
    libavcodec/h264_cabac.c \
    libavcodec/h264_cavlc.c \
    libavcodec/h264_direct.c \
    libavcodec/h264_loopfilter.c \
    libavcodec/h264_parser.c \
    libavcodec/h264_ps.c \
    libavcodec/h264_refs.c \
    libavcodec/h264_sei.c \
    libavcodec/h264dsp.c \
    libavcodec/h264idct.c \
    libavcodec/h264pred.c \
    libavcodec/imgconvert.c \
    libavcodec/jrevdct.c \
    libavcodec/mpegvideo.c \
    libavcodec/options.c \
    libavcodec/parser.c \
    libavcodec/pthread.c \
    libavcodec/simple_idct.c \
    libavcodec/utils.c \
    libavcodec/vp3.c \
    libavcodec/vp3dsp.c \
    libavcodec/xiph.c \
    \
    libavcodec/x86/dsputil_mmx.c \
    libavcodec/x86/fdct_mmx.c \
    libavcodec/x86/h264_intrapred_init.c \
    libavcodec/x86/h264dsp_mmx.c \
    libavcodec/x86/idct_mmx.c \
    libavcodec/x86/idct_mmx_xvid.c \
    libavcodec/x86/idct_sse2_xvid.c \
    libavcodec/x86/mpegvideo_mmx.c \
    libavcodec/x86/simple_idct_mmx.c \
    \
    libavutil/cpu.c \
    libavutil/imgutils.c \
    libavutil/intfloat_readwrite.c \
    libavutil/inverse.c \
    libavutil/log.c \
    libavutil/mathematics.c \
    libavutil/mem.c \
    libavutil/pixdesc.c \
    libavutil/rational.c \
    \
    libavutil/x86/cpu.c \
    \
    libpostproc/postprocess.c \
    \


SRCS_YASM=\
    libavcodec/x86/dsputil_yasm.asm \
    libavcodec/x86/h264_chromamc.asm \
    libavcodec/x86/h264_deblock.asm \
    libavcodec/x86/h264_idct.asm \
    libavcodec/x86/h264_intrapred.asm \
    libavcodec/x86/h264_weight.asm \
    libavcodec/x86/vp3dsp.asm \
    libavcodec/x86/x86util.asm

ifeq ($(64BIT),yes)
    OBJ_DIR=../../obj/MinGW64/VIDEC_H264Decoder/
    TARGET_LIB_DIR=./
else
    OBJ_DIR=../../obj/MinGW32/VIDEC_H264Decoder/
    TARGET_LIB_DIR=./
endif

OBJ_DIRS=\
    $(OBJ_DIR) \
    $(OBJ_DIR)libavcodec \
    $(OBJ_DIR)libavcodec/x86 \
    $(OBJ_DIR)libavutil \
    $(OBJ_DIR)libpostproc \
    $(OBJ_DIR)libavutil/x86

TARGET_LIB=$(TARGET_LIB_DIR)libH264Decoder.a
CC=gcc


### SETTINGS ###

CFLAGS+=\
    -I. -Ilibavcodec -Ilibavutil -I../VIDEC_pthreads \
    -DHAVE_AV_CONFIG_H -D_ISOC99_SOURCE -std=gnu99

#CFLAGS+= -save-temps
#CFLAGS+= -fverbose-asm

YASMFLAGS+= -Pconfig.asm

ifeq ($(64BIT),yes)
    CFLAGS+=-DARCH_X86_64
else
    CFLAGS+=-DARCH_X86_32
endif


### INCLUDES ###

include ./makefile_c.inc


### TARGETS ###

.all: outdirs $(TARGET_LIB)

.PHONY: outdirs $(OBJ_DIRS)
outdirs: $(OBJ_DIRS)
$(OBJ_DIRS):
	$(shell test -d $(@) || mkdir -p $(@))


.PHONY: clean
clean:
	rm -f $(TARGET_LIB)
	rm -r -f $(OBJ_DIR)
