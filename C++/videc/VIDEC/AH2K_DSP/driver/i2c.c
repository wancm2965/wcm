/*
	2008.8.21
*/
#include <stdio.h>
#include "i2c.h"

/*
 * ======== defnine register r/w function ========
 */
void writeReg(Ptr handle, Uint8 reg, Uint8 data)
{
	I2C *i2c = (I2C *)handle;
	if (i2c->mode == I2CMODE_SW)
	{
		int status = _i2c_sw_write(i2c->handle, reg, &data, 1);
#ifdef _DEBUG
		if (status == FALSE)
			printf("i2c write error\n");
#endif
	}
	else
	{
		_i2c_hw_write(i2c->handle, reg, &data, 1);
	}
}

void readReg(Ptr handle, Uint8 reg, Uint8 *data)
{
	I2C *i2c = (I2C *)handle;
	if (i2c->mode == I2CMODE_SW)
	{
		_i2c_sw_read(i2c->handle, reg, data, 1);
	}
	else
	{
		_i2c_hw_read(i2c->handle, reg, data);
	}
}

void writeMultiReg(Ptr handle, Uint8 reg, Uint8 *data, Uint16 numBytes)
{
	I2C *i2c = (I2C *)handle;
	if (i2c->mode == I2CMODE_SW)
	{
		int status = _i2c_sw_write(i2c->handle, reg, data, numBytes);
#ifdef _DEBUG
		if (status == FALSE)
			printf("i2c write error\n");
#endif
	}
	else
	{
		_i2c_hw_write(i2c->handle, reg, data, numBytes);
	}
}

void readMultiReg(Ptr handle, Uint8 reg, Uint8 *data, Uint16 numBytes)
{
	I2C *i2c = (I2C *)handle;
	if (i2c->mode == I2CMODE_SW)
	{
		_i2c_sw_read(i2c->handle, reg, data, numBytes);
	}
	else
	{
     	Int i;
     	
		for (i = 0; i < numBytes; i++)
		{
			_i2c_hw_read(i2c->handle, reg + i, data + i);
		}
	}
}
