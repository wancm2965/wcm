/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
#ifndef __IIC_H  
#define __IIC_H

#include <csl.h>
#include <csl_i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	Uint8 addr;
	I2C_Handle handle;
} I2C_HW, *I2C_HW_Handle;

I2C_HW_Handle _i2c_hw_init(Uint32 addr);
void _i2c_hw_write(I2C_HW_Handle ih, Uint8 reg, Uint8 *data, Uint16 numBytes8);
void _i2c_hw_read(I2C_HW_Handle ih, Uint8 reg, Uint8 *data);

#ifdef __cplusplus
}
#endif

#endif

