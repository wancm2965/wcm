/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/**************************************************************************/
/* ad9880.c file */
/**************************************************************************/
#include <std.h>
#include <stdio.h>
#include <csl.h>
#include <csl_i2c.h>      
#include <edc.h>
#include <ad9880.h>
#include <assert.h>

#include "i2c.h"
#include "_ad9880.h"
#include "hdedc.h"

typedef struct AD9880_DEV {
	Uint8 id;
	Uint8 interface_select;
	Uint32 last_lines;
	Uint32 hSize;
	Uint32 vSize;

	AD9880_AnalogFormat	aFmt;
	AD9880_ConfParams1 praram;

	Uint32 is_rgb;
	Int brightness;

	I2C i2c;
} AD9880_DEV;

extern Int INTERNALHEAP;

static Int AD9880_ctrl(EDC_Handle handle, AD9880_Cmd cmd, Arg arg);
static Int AD9880_close(EDC_Handle handle);
static EDC_Handle AD9880_open(String devName, Arg optArg);

static void initAD9880Def(EDC_Handle handle);
static void powerdownAD9880(EDC_Handle handle, Bool powerDownEnable);
static void setBrightness(EDC_Handle handle, Int bright);
static void _init(EDC_Handle handle, Arg arg);

EDC_Fxns AD9880_Fxns = {
    AD9880_open,
    AD9880_close,
    AD9880_ctrl                   
};

/////////////////////////////////////////////////////////////////////////////////

void _init(EDC_Handle handle, Arg arg)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;
	if (INV != (Ptr)arg)
	{
		I2C_INFO *p = (I2C_INFO *) arg;

		dev->i2c.mode = p->mode;
		if (p->mode == I2CMODE_SW)
			dev->i2c.handle = (Ptr)_i2c_sw_create(p->addr, p->pinSCL, p->pinSDA, 2);
		else
			dev->i2c.handle = (Ptr)_i2c_hw_create(p->addr);
	}

	initAD9880Def(handle);

	// Auto Input Channel select 			
	writeReg(&dev->i2c, 0x11, 0);

#ifdef _DEBUG
	{
		Uint8 data[6];
		readMultiReg(&dev->i2c, 0x00, data, 6);
		printf("AD9880 register 0x00~0x02 is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", 
				data[0], data[1], data[2], data[3], data[4], data[5]);
	}
#endif
}

static void setBrightness(EDC_Handle handle, Int bright)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;
	Uint8 val = bright;

	dev->brightness = bright;
	writeReg(&dev->i2c, 0x0A, val);
	if (dev->is_rgb == 0)
	{
		val = 0;
	}
	
	//val = -127;
	writeReg(&dev->i2c, 0x08, val);

	//val = 127;
	writeReg(&dev->i2c, 0x0C, val);

}

/*
 * ======== initAD9880Def ========
 */
static void initAD9880Def(EDC_Handle handle)
{
	Uint8 *conf_base = _AD9880_Configure;
	AD9880_DEV *dev = (AD9880_DEV *)handle;

	//Uint8 reg;
	writeMultiReg(&dev->i2c, 0x01, conf_base + 0x01, 3);
	writeMultiReg(&dev->i2c, 0x08, conf_base + 0x08, 6);
	writeReg(&dev->i2c, 0x11, conf_base[0x11]);
	writeMultiReg(&dev->i2c, 0x1A, conf_base + 0x1a, 4);
	writeMultiReg(&dev->i2c, 0x21, conf_base + 0x21, 6);
	writeReg(&dev->i2c, 0x34, conf_base[0x34]);

	writeMultiReg(&dev->i2c, 0x4D, conf_base + 0x35, 4);
	writeReg(&dev->i2c, 0x53, conf_base[0x39]);
	writeReg(&dev->i2c, 0x56, conf_base[0x3A]);		
	writeReg(&dev->i2c, 0x59, conf_base[0x3B]);
		
	writeMultiReg(&dev->i2c, 0x35, _AD9880_CSC[5], 24);
	//writeReg(hI2C, addr, 0x20, 0x71);

	return;
}

/*
 * ======== powerdownAD9880 ========
 */
static void powerdownAD9880(EDC_Handle handle, Bool powerDownEnable)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;
    Uint8 powerdownSettings = 0x0d;

    /* Put _AD9880 in power down mode */
    if(!powerDownEnable)
    {
        powerdownSettings = 0xf0;
    }
    
    writeReg(&dev->i2c, 0x88, powerdownSettings);
}

static void _reset(EDC_Handle handle)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;

	dev->last_lines = 0;
	writeReg(&dev->i2c, 0x11, 0);
}

static Int _isChange(EDC_Handle handle)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;
	Uint16 lines = 0;
	
	readMultiReg(&dev->i2c, 0x17, (Uint8 *)&lines, 2);
	lines = ((lines >> 8) & 0x00FF) | ((lines << 8) & 0xFF00);
	if ((lines >= (dev->last_lines - 2)) && 
		(lines <= (dev->last_lines + 2)))   
	{
		return 0;
	}

	return 1;
}

static Int _getInput(EDC_Handle handle)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;
	Uint8 source_type = 0;

	readReg(&dev->i2c, 0x15, &source_type);   
	if ((source_type & 0x40) == 0x40)			// VGA1 is actived, activity is detected on the Hsync1.
	{
		dev->aFmt = AD9880_AFMT_VGA;
		dev->interface_select = 0x5d;
	}
	else if ((source_type & 0x80) == 0x80)		// VGA2 is actived, activity is detected on the Hsync0.
	{
		dev->aFmt = AD9880_AFMT_VGA;
		dev->interface_select = 0x55;

	}
	else if ((source_type & 0x08) == 0x08)		// YUV1 is actived, activity is detected on the SOG0.
	{
		dev->aFmt = AD9880_AFMT_COMPONENT;
		dev->interface_select = 0xF5;
	}
	else if ((source_type & 0x04) == 0x04)		// YUV2 is actived, activity is detected on the SOG0.
	{
		dev->aFmt = AD9880_AFMT_COMPONENT;
		dev->interface_select = 0xFD;
	}
	else
	{
		readReg(&dev->i2c, 0x2f, &source_type); 
		if ((source_type & 0x60) == 0x60)		// DVI is actived, the presence of a TMDS DE and clock.
		{
			dev->aFmt = AD9880_AFMT_DVI;
			dev->interface_select = 0x03;
		}
		else
		{
			dev->aFmt = AD9880_AFMT_NONE;
		}
	}

	return dev->aFmt;
}

static Int _detect(EDC_Handle handle, Arg arg)
{
	Uint16 lines = 0;
	AD9880_tblMode *ptbl;
	AD9880_ConfParams *params = (AD9880_ConfParams *)arg;
	Int rate = params->rate;
	AD9880_DEV *dev = (AD9880_DEV *)handle;
	Uint8 ret;

	readMultiReg(&dev->i2c, 0x17, (Uint8 *)&lines, 2);
	lines = ((lines >> 8) & 0x00FF) | ((lines << 8) & 0xFF00);
#ifdef _DEBUG
	printf("LCF is %d\n", lines);
#endif
	params->hSize = 0;
	params->vSize = 0;
	params->vProch = 0;
	params->hProch = 0;
	params->interlaced = 0;
	dev->last_lines = 0;
	params->lcf = lines;

	if (dev->aFmt == AD9880_AFMT_NONE)
	{
		return 0;
	}

	ptbl = tblMode[dev->aFmt];
	while (ptbl->min_lines != 0)
	{
		// for custom
		//if (ptbl->min_lines == 1)
		//{
		//	ptbl->params.vSize = (lines - 40 + 7) & (~7);
		//	ptbl->params.hSize = ((Int)(ptbl->params.vSize * 1.4) + 7) & (~7);		// 4:3
		//}

		if (((lines >= ptbl->min_lines) && 
			(lines <= ptbl->max_lines) && 
			(rate == ptbl->frame_rate))/* ||
			(ptbl->min_lines == 1)*/)
		{
			*params = ptbl->params;
			params->lcf=lines;
			params->rate=rate;

			if (ptbl->params.interlaced == 1)
				params->vProch = lines - ptbl->params.vSize / 2 - ptbl->params.vProch;
			else
				params->vProch = lines - ptbl->params.vSize - ptbl->params.vProch;
			
			if (params->vProch < 0)
				params->vProch = 0;
					
			dev->praram = ptbl->param1;
			dev->last_lines = lines;
			dev->hSize = params->hSize;
			dev->vSize = params->vSize;
			dev->praram.interlaced = params->interlaced;

			break;
		}

		ptbl++;
	}
	
	// ÊäÈëÐÅºÅÊÇRGB? YUV?
	dev->is_rgb = 1; //0;

	if (dev->aFmt == AD9880_AFMT_DVI)
	{
		readReg(&dev->i2c, 0x5B, &ret);
		if (ret & 0x08) // is HDMI
		{	
			//if (dev->hSize == 1920 || dev->hSize == 1280) //1080)
			{
				//Uint8 ret;
				readReg(&dev->i2c, 0x81, &ret);			// Active Format Information Status
				if ((ret & 0x60) != 0) //0x20)
					// is YUV
					dev->is_rgb = 0;
			}
		}
	}
	else if (dev->aFmt == AD9880_AFMT_COMPONENT) //1 || dev->aFmt == AD9880_AFMT_COMPONENT2)
		dev->is_rgb = 0;

	return 2;
}

/*
 * ======== AD9880_ctrl ========
 */
static Int AD9880_ctrl(EDC_Handle handle, AD9880_Cmd cmd, Arg arg)
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;

    switch (cmd)
    {
		case EDC_EXT_INIT:
			_init(handle, arg);
			break;
		case EDC_EXT_GETINPUT:
			*(Int *)arg = _getInput(handle);
			break;
		case EDC_EXT_GETSTATUS:
			*(Int *)arg = _isChange(handle);
			break;
		case EDC_EXT_GETRES:
			_detect(handle, arg);
			break;
		case EDC_EXT_RESET:
			_reset(handle);
			break;

		case EDC_EXT_CONFIG:
		//case AD9880_SET:
		{
			Uint8 val;
			//Uint32 Brightness = 0xf0;			/* Brightness Control, 0x80(-128)~0x7f(+127) */
			Uint32 Phase_Adjust = 16;		/* Phase Adjust Control, 0~31 */
			Uint8 ValueOfReg21 = 0xEC;		//720p 20080516			

			if (dev->aFmt == AD9880_AFMT_COMPONENT) //1 || AD9880_AFMT_COMPONENT2)
			{
				//{{720p 20080516
				if (dev->hSize == 1280 || (dev->hSize == 1920 && dev->praram.interlaced==0))
				{
					ValueOfReg21 = 0x6C;
				}
				//}}
			}
			
			writeReg(&dev->i2c, 0x21, ValueOfReg21);	     // 720p 20080516
			writeReg(&dev->i2c, 0x11, dev->interface_select);     // VGA Input Channel 1

			if (arg == NULL)
			{
				writeReg(&dev->i2c, 0x01, PLL_MSB(dev->praram.total_size));
				writeReg(&dev->i2c, 0x02, PLL_LSB(dev->praram.total_size));
				writeReg(&dev->i2c, 0x03, dev->praram.clock);
				writeReg(&dev->i2c, 0x19, dev->praram.camp);		
			}
			else
			{
				AD9880_ConfParams1 *param = (AD9880_ConfParams1 *)arg;
			
				writeReg(&dev->i2c, 0x01, PLL_MSB(param->total_size));
				writeReg(&dev->i2c, 0x02, PLL_LSB(param->total_size));
				writeReg(&dev->i2c, 0x03, param->clock);
				writeReg(&dev->i2c, 0x19, param->camp);
			}

			writeReg(&dev->i2c, 0x04, Phase_Adjust << 3);
			if (dev->is_rgb == 0)
			{
				writeReg(&dev->i2c, 0x34, 0x84);
				writeReg(&dev->i2c, 0x09, 0x80);
		 		writeReg(&dev->i2c, 0x0D, 0x80);
				writeReg(&dev->i2c, 0x1B, 0xa2);
			}
			else
			{
				writeReg(&dev->i2c, 0x34, 0x86);
				writeReg(&dev->i2c, 0x09, 0x04);
		 		writeReg(&dev->i2c, 0x0D, 0x04);
				writeReg(&dev->i2c, 0x1B, 0x02);
			}

			//setBrightness(handle, g_nbrightness);

			if (dev->aFmt == AD9880_AFMT_DVI)
			{
				val = 0xfa;
			}
			else
			{ 
				val = 0xea;
			}
			writeReg(&dev->i2c, 0x24, val);
			break;
		}

		//case AD9880_SET_BRIGHT:
		case EDC_EXT_SETBRIGHT:
			setBrightness(handle, (Int)arg);
			break;
    }
    
    return EDC_SUCCESS;
}

/*
 * ======== AD9880_open ========
 */
static EDC_Handle AD9880_open(String devName, Arg optArg)
{
	Int devId;
	AD9880_DEV *dev;
	
	if (devName[0] != '/')
		devId = -1;
	else
		devId = devName[1] - '0';

	dev = (AD9880_DEV *)MEM_alloc(INTERNALHEAP, sizeof(AD9880_DEV), 32);
	dev->id = devId;
    return (EDC_Handle)dev;
}

/*
 * ======== AD9880_close ========
 */
static Int AD9880_close(EDC_Handle handle) 
{
	AD9880_DEV *dev = (AD9880_DEV *)handle;

    /* First Check if the Handle is correct */
    if(dev != NULL)
    {
        /* Put _AD9880 in power down mode */
        powerdownAD9880(&dev->i2c, TRUE);
        return EDC_SUCCESS;    
    }

    return EDC_FAILED;
}

/**************************************************************************/
/* End of file                                                            */
/**************************************************************************/
