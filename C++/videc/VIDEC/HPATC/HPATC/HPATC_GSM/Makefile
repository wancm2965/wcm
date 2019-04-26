TARGET		= ../libs/libgsm.a
INCPATH		= -I./include/
CC		= arm-linux-gcc
CFLAGS		= -o3 -w -c
AR		= arm-linux-ar
LFLAGS		= cr
RANLIB		= arm-linux-ranlib

SRC_GSM		= $(wildcard ./gsm/*.c)

OBJ_GSM		= $(patsubst %.c,%.o,$(SRC_GSM))


all:$(TARGET)

$(TARGET): \
		$(OBJ_GSM)			
	$(AR) $(LFLAGS) $(TARGET) \
		$(OBJ_GSM)
	$(RANLIB) $(TARGET)

$(OBJ_GSM):$(SRC_GSM)
	$(CC) $(CFLAGS) $*.c -o $@ $(INCPATH)
	
clean:
	rm -f $(OBJ_GSM)


