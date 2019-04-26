/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
#include "_i2c_hw.h"      

#define I2CDELAY(iterations)  {      \
    volatile Int j;                  \
    for(j = 0; j < iterations; j ++); \
}   
#define DELAY_TIME 1000

I2C_Handle DM642_I2C_hI2C = INV;
extern Int INTERNALHEAP;

static const I2C_Config EVM642VIDEOIIC_Config = {
    0,  /* master mode,  i2coar;   */
    0,  /* no interrupt, i2cimr;   */
    (20-5), /* scl low time, i2cclkl;  */
    (20-5), /* scl high time,i2cclkh;  */
    1,  /* configure later, i2ccnt;*/
    0,  /* configure later, i2csar;*/
    0x4620, /* master tx mode,     */
            /* i2c runs free,      */
            /* 8-bit data + NACK   */
            /* no repeat mode      */
    (75-1), /* 4MHz clock, i2cpsc  */
};

I2C_HW_Handle _i2c_hw_create(Uint32 addr)
{
	I2C_HW_Handle pI2C;
	I2C_Config i2cCfg = {
		0x0000007f, /* I2COAR -    Not used if master */
		0x00000000, /* I2CIER -    Disable interrupts, use polling */
		0x0000001b, /* I2CCLKL -   Low period for 100KHz operation */
		0x0000001b, /* I2CCLKH -   High period for 100KHz operation */
		0x00000002, /* I2CCNT -    Data words per transmission */
		0x0000001a, /* I2CSAR -    Slave address */
		0x00004680, /* I2CMDR -    Mode */
		0x00000019  /* I2CPSC -    Prescale 300MHz to 12MHz */
	};

	if (DM642_I2C_hI2C == INV)
	{
		/* Open I2C handle */
		DM642_I2C_hI2C = I2C_open(I2C_PORT0, I2C_OPEN_RESET);

		/* Configure I2C controller */
		I2C_config(DM642_I2C_hI2C, &i2cCfg);

		/* Take I2C out of reset */        
		I2C_outOfReset(DM642_I2C_hI2C);
	}

	pI2C = (I2C_HW_Handle )MEM_alloc(INTERNALHEAP, sizeof(I2C_HW), 64);
	pI2C->addr = addr >> 1;
	pI2C->handle = DM642_I2C_hI2C;

	return pI2C;
}

/*
 * ======== _IIC_hw_write ========
 * This function performs write operation via I2C bus.
 */
void _i2c_hw_write(I2C_HW_Handle ih,
					Uint8 reg,
					Uint8 *data,
					Uint16 numBytes)
{             
    Int i;
	Uint8 devAddress;
	I2C_Handle hI2C;
    
	hI2C = ih->handle;
	devAddress = ih->addr;

    /* Wait until bus is free */
    while (I2C_bb(hI2C));

    /* save old settings */
    //I2C_getConfig(hI2C, &prevIICConfig);

    /* set I2C mode register */
    I2C_RSETH(hI2C, I2CMDR, EVM642VIDEOIIC_Config.i2cmdr);
    
    /* set I2C imr register  */
    I2C_RSETH(hI2C, I2CIMR, EVM642VIDEOIIC_Config.i2cimr);
    
    /* configure the I2C slave address register */
    I2C_RSETH(hI2C, I2CSAR, devAddress);
    
    /* set I2C count register */
   	I2C_RSETH(hI2C, I2CCNT, numBytes + 1);
    
    /* write the sub address */
    I2C_RSETH(hI2C, I2CDXR, reg);
    
    /* Generate start condition */
    I2C_start(hI2C);
    
    I2CDELAY(DELAY_TIME);

    /* write the data */ 
    for(i = 0; i < numBytes; i ++) {
        while(!I2C_xrdy(hI2C));
        I2C_writeByte(hI2C, *data ++);
        I2CDELAY(DELAY_TIME);
    }

    /* Generate stop condition */
    I2C_sendStop(hI2C); 
    
    I2CDELAY(DELAY_TIME);        

    /* Wait until bus is free */
    while (I2C_bb(hI2C));
    
    I2CDELAY(DELAY_TIME);        
    /* now restore the previous I2C settings */
    
    /* set I2C mode register */
    //I2C_RSETH(hI2C, I2CMDR, prevIICConfig.i2cmdr);
    
    /* set I2C imr register  */
    //I2C_RSETH(hI2C, I2CIMR, prevIICConfig.i2cimr);
    
    /* configure the I2C slave address register */
    //I2C_RSETH(hI2C, I2CSAR, prevIICConfig.i2csar);
    
    /* set I2C count register */
    //I2C_RSETH(hI2C, I2CCNT, prevIICConfig.i2ccnt);

    I2CDELAY(DELAY_TIME);        
}     

void _i2c_hw_read(I2C_HW_Handle ih, Uint8 reg, Uint8 *data)
{
	int i = 0;
	Uint8 devAddress;
	I2C_Handle hI2C;
    
	hI2C = ih->handle;
	devAddress = ih->addr;

	for(i = 0;i < 0x100;i++)
	{
		if(I2C_bb(hI2C) == 0)
			break;
		else
			I2CDELAY(DELAY_TIME);
	}
	I2C_FSETH(hI2C, I2CSTR, BB, 1);
	I2C_RSETH(hI2C, I2CSAR, devAddress);
	I2C_RSETH(hI2C, I2CMDR, 0x0000c6a0);
	I2C_writeByte(hI2C, reg);
	I2C_start(hI2C);
	for(i = 0;i < 0x100;i++)
	{
		if(I2C_xrdy(hI2C) == 1)
			break;
		else
			I2CDELAY(DELAY_TIME);
			//DM642_waitusec(1);
	}
	//I2C_sendStop(hI2C);
	I2C_RSETH(hI2C, I2CMDR, 0x0000c4a0);
	I2C_start(hI2C);
	for(i = 0;i < 0x100;i++)
	{
		if(I2C_rrdy(hI2C) == 1)
			break;
		else
			//DM642_waitusec(1);
			I2CDELAY(DELAY_TIME);
	}
	I2C_sendStop(hI2C);
	//DM642_waitusec(100);
	I2CDELAY(DELAY_TIME);
	*data = I2C_readByte(hI2C);
	for(i = 0;i < 0x100;i++)
	{
		if(I2C_bb(hI2C) == 0)
			break;
		else
			//DM642_waitusec(1);
			I2CDELAY(DELAY_TIME);
	}
}
