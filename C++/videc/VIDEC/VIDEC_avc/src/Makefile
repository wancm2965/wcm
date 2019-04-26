
CFLAGS+= -I../inc -DHAVE_AV_CONFIG_H -D_ISOC99_SOURCE -D_POSIX_C_SOURCE=200112 -std=gnu99

#CFLAGS+= -save-temps
#CFLAGS+= -fverbose-asm

SRCS_C=\
      ./avc.c aandcttab.c options.c\
      ./allcodecs.c \
      ./bitstream.c \
      ./cabac.c \
      ./dsputil.c \
      ./error_resilience.c \
      ./eval.c \
      ./faandct.c \
      ./golomb.c \
      ./h263.c \
      ./h263_parser.c \
      ./h263dec.c \
      ./h264.c \
      ./h264_parser.c \
      ./h264idct.c \
      ./h264pred.c \
      ./imgconvert.c \
      ./jfdctfst.c \
      ./jfdctint.c \
      ./jrevdct.c \
      ./motion_est.c \
      ./mpegvideo.c \
      ./mpegvideo_enc.c \
      ./parser.c \
      ./ratecontrol.c \
      ./simple_idct.c \
      ./utils.c \
      ./w32thread.c \
    \
      ./crc.c \
      ./log.c \
      ./lzo.c \
      ./mathematics.c \
      ./mem.c \
      ./random_seed.c \
      ./rational.c \
    \
      ./mjpeg.c \
      ./mjpegdec.c \
      ./mjpegenc.c \
    \
      ./x86/cpuid.c \
      ./x86/dsputil_mmx.c \
      ./x86/dsputilenc_mmx.c \
      ./x86/fdct_mmx.c \
      ./x86/idct_mmx.c \
      ./x86/motion_est_mmx.c \
      ./x86/mpegvideo_mmx.c \
      ./x86/simple_idct_mmx.c 

SRCS_YASM=\
      ./x86/dsputil_yasm.asm \
      ./x86/h264_idct_sse2.asm \
      ./x86/h264_deblock_sse2.asm 

SRCS_C+= postprocess.c aclib.c

ifeq ($(64BIT),yes)
    CFLAGS+=-DARCH_X86_64
else
    CFLAGS+=-DARCH_X86_32
endif

LIB=../lib/avc.a
SLIB=../bin/avc.dll
DEF=../win32/avc.def
CC=gcc

NOINTRIN=1

include ./makefile_c.inc

OPTFLAGS+= -fno-math-errno
#OPTFLAGS+= -fno-signed-zeros

.all: $(LIB) $(SLIB)

clean:
	rm -f *.o *.d ./*.o ./*.d ./x86/*.o ./x86/*.d \
    ./*.o ./*.d $(SLIB) $(LIB)

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)

#      ./x86/fft_3dn.c \
#      ./x86/fft_3dn2.c \
#      ./x86/fft_sse.c \
#      ./x86/fft_mmx.asm

#      ./x86/snowdsp_mmx.c \
#      ./x86/vc1dsp_mmx.c \
#      ./x86/vp3dsp_mmx.c \
#      ./x86/vp3dsp_sse2.c
#      ./x86/idct_mmx_xvid.c \
#      ./x86/idct_sse2_xvid.c \
#      ./x86/cavsdsp_mmx.c \
