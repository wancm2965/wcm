#include "stdio.h"
#include "adv7180.h"

#include "tvp5150.h"
#include "hdedc.h"

#include "i2c.h"

/*AD_RESULT[2:0],address 0x10[6:4]
AD_RESULT[2:0]	Description
000	NTSM M/J
001	NTSC 4.43
010	PAL M
011 PAL60
100 PAL B/G/H/I/D
101 SECAM
110 PAL Combination N
111	SECAM 525
*/

typedef struct _ADV7180_DEV
{
	Int id;
	I2C i2c;

}ADV7180_DEV;

extern Int INTERNALHEAP;
static Int ADV7180_ctrl(EDC_Handle handle, Uns cmd, Arg arg);
static Int ADV7180_close(EDC_Handle handle);
static EDC_Handle ADV7180_open(String devName, Arg optArg);

EDC_Fxns ADV7180_Fxns = {
    ADV7180_open,
    ADV7180_close,
    ADV7180_ctrl                   
};


static Int ADV7180_ctrl(EDC_Handle handle, Uns cmd, Arg arg)
{
	ADV7180_DEV *dev = (ADV7180_DEV *)handle;

	switch(cmd)
	{
	case EDC_EXT_INIT:
	 {
		if (INV != (Ptr)arg)
		{
			I2C_INFO *p = (I2C_INFO *) arg;
			if (p->mode == I2CMODE_SW)
			{
				dev->i2c.mode = I2CMODE_SW;
    				dev->i2c.handle = (Ptr)_i2c_sw_create(p->addr, p->pinSCL, p->pinSDA, 4);

				//adv7180 init
				writeReg(&dev->i2c,0x00,0x00);	/*CVBS->Ain1*/
				writeReg(&dev->i2c,0x04,0x57);	/*enable SFL. */
				writeReg(&dev->i2c,0x17,0x41);	/*comb filter select,SH1*/
				writeReg(&dev->i2c,0x31,0x02);	/*clear newmodw sav,eav*/
				writeReg(&dev->i2c,0x3d,0xa2);	/*mwe enable manual window,color kill thresold*/
				writeReg(&dev->i2c,0x3e,0x6a);	
				writeReg(&dev->i2c,0x3f,0xa0);	
				writeReg(&dev->i2c,0x0e,0x80);	
				writeReg(&dev->i2c,0x55,0x81);
				writeReg(&dev->i2c,0x0e,0x00);
			}
			else
			{
				dev->i2c.mode = I2CMODE_HW;
			}
		}

       	break;
	}
	
	case EDC_EXT_RESET:
		writeReg(&dev->i2c,0x0f,0x80);/*复位需要2ms,复位位会自动清除(期间adv7180不可访问？？)*/

		break;
	case EDC_EXT_GETINPUT:
		{
			Uint8 data=0xff;
			Uint8 type;

			readReg(&dev->i2c,0x10,&data);

			type=(data&0x70)>>4;
			if(type==0 || type==1)
				*(int *)arg=MODE_NTSC;
			else if(type==2 ||type==3||type==4 ||type==6)
				*(int *)arg= MODE_PAL;
			else if(type==5 ||type==7)
				*(int *)arg= MODE_SECAM;
			else
				*(int *)arg= MODE_NONE;

			break;
		}
	}
	return EDC_SUCCESS;

}
static Int ADV7180_close(EDC_Handle handle)
{
	return EDC_SUCCESS;    
}
static EDC_Handle ADV7180_open(String devName, Arg optArg)
{
	Int devId;
	ADV7180_DEV *dev;
	
	if (devName[0] != '/')
		devId = -1;
	else
		devId = devName[1] - '0';

	dev = (ADV7180_DEV *)MEM_alloc(INTERNALHEAP, sizeof(ADV7180_DEV), 64);
	dev->id = devId;
    return (EDC_Handle)dev;
}




