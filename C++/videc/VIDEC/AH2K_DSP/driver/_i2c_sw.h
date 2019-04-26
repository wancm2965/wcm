
#ifndef _I2C_H
#define _I2C_H

#include <csl.h>
#include <csl_gpio.h>

#define I2C_WRITE         0         /* write operation */
#define I2C_READ          1         /* read operation */

typedef struct  {
	//Bool mode;
	GPIO_Handle hGpio;
		
	Uint32 address;
	Uint32 freq;
	
	Uint32 pinSCL;
	Uint32 pinSDA;
	
} I2C_SW, *I2C_SW_Handle;

Bool _i2c_sw_write(I2C_SW_Handle hI2C, Uint8 reg, Uint8 *data, Uint32 len);
Bool _i2c_sw_read(I2C_SW_Handle hI2C, Uint8 reg, Uint8 *data, Uint32 len);
I2C_SW_Handle _i2c_sw_create(Uint32 addr, Uint32 pinSCL, Uint32 pinSDA, Int freq);

#endif
