#ifndef _TVP5150_H
#define _TVP5150_H

#include <csl_i2c.h>
#include <csl_gpio.h> 
#include <edc.h>

#define MODE_NONE		0
#define MODE_NTSC		1
#define MODE_PAL		2
#define MODE_SECAM		3

#define TVP5150_IPUTMODE_REG	0x00
#define INPUTMODE_AIP1A			0
#define INPUTMODE_AIP1B			2
#define INPUTMODE_SVIDEO		3

#define TVP5150_INTSTATUS_REG	0xC0
#define TVP5150_VIDEOSTD_REG	0x8C

#define TVP5150_SCANLINEH_REG	0x84
#define TVP5150_SCANLINEL_REG	0x85

typedef enum TVP5150_AnalogFormat {
    TVP5150_AFMT_SVIDEO,
    TVP5150_AFMT_COMPOSITE
} TVP5150_AnalogFormat;

typedef enum TVP5150_Mode {
    TVP5150_MODE_NTSC640,
    TVP5150_MODE_NTSC720,
    TVP5150_MODE_PAL720,
    TVP5150_MODE_PAL768,
    TVP5150_MODE_CIF,
    TVP5150_MODE_QCIF,
    TVP5150_MODE_SQCIF,
    TVP5150_MODE_SIF,     
    TVP5150_MODE_USER
}TVP5150_Mode;

typedef unsigned int TVP5150_Cmd;

typedef struct {
    TVP5150_Mode inMode;
    TVP5150_Mode outMode;
    TVP5150_AnalogFormat aFmt;
    Bool enableBT656Sync;
    Bool enableIPortOutput;
    I2C_Handle hI2C;

    // optional parameters for inMode == TVP5150_MODE_USER  

    Int hSize;
    Int vSize;
    Bool interlaced;   
    GPIO_Handle hGpio; 
} TVP5150_ConfParams;

extern EDC_Fxns TVP5150_Fxns;

//void TVP5150_SetInputMode(unsigned int i2c_sim, unsigned int devId, unsigned char InputMode);

//unsigned char TVP5150_GetInputMode(Uns i2c_sim,unsigned int devId);
//unsigned char TVP5150_GetInputMode(unsigned int devId, Uint8 devAddr);
//unsigned int TVP5150_GetScanLine(Uns i2c_sim,unsigned int devId);
//unsigned char TVP5150_VIDEOSTD(Uns i2c_sim,unsigned int devId);
//unsigned char TVP5150_GetSignalSrc(Uns i2c_sim,unsigned int devId);
//void initTVP5150Def(I2C_Handle devID, Uns i2c_sim, Uns i2cAddr );
#endif
