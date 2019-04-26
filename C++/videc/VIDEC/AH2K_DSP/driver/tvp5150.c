
/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/**************************************************************************/
/* TVP5150.c file */
/**************************************************************************/
#include <std.h>
#ifdef _DEBUG
#include <stdio.h>
#endif
#include <iom.h>
#include <fvid.h>                       
#include <edc.h>
#include <vport.h>
#include <vportcap.h>
#include <csl_edma.h>
#include <csl_i2c.h> 
#include <csl_gpio.h>
#include <assert.h> 

#include "tvp5150.h"
#include "_tvp5150.h"
#include "i2c.h"
#include "hdedc.h"

extern Int INTERNALHEAP;
//////////////////////////////////////

typedef struct TVP5150_DEV {
	Int id;
	I2C i2c;
} TVP5150_DEV;

//static TVP5150_DEV tvp5150 = {0, _TVP5150_IIC_ADDR0, TVP5150_AFMT_COMPOSITE};
//extern I2C_Handle DM642_I2C_hI2C;

//typedef enum {INTERLACED = TRUE, PROGRESSIVE = FALSE} FrameType ;   

//static void configTVP5150(Int decNum, TVP5150_Mode inMode,                     
   // TVP5150_Mode outMode, TVP5150_AnalogFormat inFormat);

//static void enableIPortOutput(Int channel, Bool iPortOutputEnable);
//static void enableTask(Uns channel, Uns task, Uns enableFlag);
static Int TVP5150_GetInputMode(EDC_Handle handle);
static void configTVP5150(EDC_Handle handle);
static void initTVP5150Def(EDC_Handle handle);

static Int TVP5150_ctrl(EDC_Handle handle, TVP5150_Cmd cmd, Arg arg);
static Int TVP5150_close(EDC_Handle handle);
static EDC_Handle TVP5150_open(String devName, Arg optArg);

EDC_Fxns TVP5150_Fxns = {
    TVP5150_open,
    TVP5150_close,
    TVP5150_ctrl                   
};

/*
 * ======== configTVP5150 ========
 */
static void configTVP5150(EDC_Handle handle)
{
	initTVP5150Def(handle);
}

static Int TVP5150_close(EDC_Handle handle)
{
	return 0;
}

/*
 * ======== initTVP5150Def ========
 */
void initTVP5150Def(EDC_Handle handle)
{
	const struct i2c_reg_value *regs = tvp5150_init_default;
	TVP5150_DEV *dev = (TVP5150_DEV *)handle;

	while (regs->reg != 0xFF)
	{
		writeReg(&dev->i2c, regs->reg, regs->value);
		regs++;
	}

	regs = tvp5150_init_enable;
	while (regs->reg != 0xFF)
	{
		writeReg(&dev->i2c, regs->reg, regs->value);
		regs++;
	}

#ifdef _DEBUG
	{	
		Uint8 data[6];
		Uint8 reg = 0x80;

		readMultiReg(&dev->i2c, reg, data, 6);
		printf("TVP5150 register 0x%02x~0x%02x is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", 
			reg, reg + 5, data[0], data[1], data[2], data[3], data[4], data[5]);
	}

#endif
}

/*
 * ======== TVP5150_ctrl ========
 */
static Int TVP5150_ctrl(EDC_Handle handle, TVP5150_Cmd cmd, Arg arg)
{
	TVP5150_DEV *dev = (TVP5150_DEV *)handle;

    switch(cmd) {
		case EDC_EXT_INIT:
        {
			if (INV != (Ptr)arg)
			{
				I2C_INFO *p = (I2C_INFO *) arg;

				dev->i2c.mode = p->mode;
				if (p->mode == I2CMODE_SW)
				{
					dev->i2c.handle = (Ptr)_i2c_sw_create(p->addr, p->pinSCL, p->pinSDA, 4);

					writeReg(&dev->i2c, 0x05, 1);
					//writeReg(&dev->i2c, 0x05, 0);
				}
				else
				{
					dev->i2c.handle = (Ptr)_i2c_hw_create(p->addr);
				}
			}
			//TVP5150_ConfParams *TVP5150Params = (TVP5150_ConfParams *)arg;
            configTVP5150(handle);
            //setTVP5150SyncMode(devId, TVP5150Params->enableBT656Sync);
           	break;
		}
#if 0
        case EDC_RESET:
			//resetTVP5150(devId);
        	break;
        case TVP5150_POWERDOWN:
        	//powerdownTVP5150(devId, TRUE);
			break;                 
        case TVP5150_POWERUP:
        	//powerdownTVP5150(devId, FALSE);
        	break;
        case TVP5150_SET_AMUXMODE:
        	break;
#endif
		//case TVP5150_GET_INPUTMODE:
		case EDC_EXT_GETINPUT:
		{
			//TVP5150_ConfParams *TVP5150Params = (TVP5150_ConfParams *)arg;
            *(Int *)arg = TVP5150_GetInputMode(handle);
			break;
		}
    }

    return EDC_SUCCESS;
}

/*
 * ======== TVP5150_open ========
 */
static EDC_Handle TVP5150_open(String devName, Arg optArg)
{
    Int devId;
	TVP5150_DEV *handle;

    /* Check if the addresse is correct */
    if(devName[0] != '/') {
        devId = -1;
    } else  {
        devId = devName[1] - '0';
    } 

	handle = MEM_alloc(INTERNALHEAP, sizeof(TVP5150_DEV), 64);
	handle->id = devId;
    return (EDC_Handle)handle;
}


/**************************************************************************/
/* End of file                                                                            */
/**************************************************************************/

/*************************************************************************/
//TVP5150 registers setting: louise add
/**************************************************************************/
//alter the channel 1 source seletction
Int TVP5150_GetInputMode(EDC_Handle handle)
{
	unsigned char InputMode;
	Int mode;
	TVP5150_DEV *dev = (TVP5150_DEV *)handle;

	readReg(&dev->i2c, 0x8C, &InputMode);
#ifdef _DEBUG
	printf("InputMode is 0x%x\n", InputMode);
#endif

	switch (InputMode & 0x0F)
	{
		case 1:   // ??
		case 3:
		case 5:
		case 7:
			mode = MODE_PAL;
			break;
		case 9:
			mode = MODE_NTSC;
			break;
		case 11:
			mode = MODE_SECAM;
			break;
		default:
			mode = MODE_NONE;
			break;
	}

	//_IIC_sim_read(devId, devId, TVP5150_IPUTMODE_REG, &InputMode);
	return mode;                 
}

#if 0
void TVP5150_SetInputMode(Uns i2c_sim,unsigned int devId, unsigned char InputMode)
{
	//I2CDEVWrite(devId, TVP5150_IPUTMODE_REG, &InputMode,1);     
	//_IIC_sim_write(devId, devId,TVP5150_IPUTMODE_REG,&InputMode,1);
}



unsigned char TVP5150_GetSignalSrc(Uns i2c_sim, unsigned int devId)
{
	unsigned char SigSrc;
	//I2CDEVRead(devId, TVP5150_INTSTATUS_REG, &SigSrc,1);
	//_IIC_sim_read(devId, devId, TVP5150_INTSTATUS_REG, &SigSrc);
	return (SigSrc&0x80);                 
}


unsigned int TVP5150_GetScanLine(Uns i2c_sim,unsigned int devId)
{
	unsigned char slh,sll;
	unsigned int ScanLine;
	
	//I2CDEVRead(devId, TVP5150_SCANLINEH_REG, &slh,1);
	//I2CDEVRead(devId, TVP5150_SCANLINEL_REG, &sll,1);
	//_IIC_sim_read(devId, devId, TVP5150_SCANLINEH_REG, &slh);
	//_IIC_sim_read(devId, devId, TVP5150_SCANLINEL_REG, &sll);
	ScanLine = (((unsigned int)slh&0x03)<<8)+sll;
	
	return ScanLine;                 
}

unsigned char TVP5150_VIDEOSTD(Uns i2c_sim,unsigned int devId)
{
	unsigned char Vd;
	
	//I2CDEVRead(devId, TVP5150_VIDEOSTD_REG, &Vd,1);
	//_IIC_sim_read(devId, devId, TVP5150_VIDEOSTD_REG, &Vd);
	return Vd;
}


#endif

