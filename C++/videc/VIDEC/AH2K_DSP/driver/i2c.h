

#include <csl.h>
#include <csl_i2c.h>
//#include "_i2c_sw.h"
//#include "_i2c_hw.h"
#define _AD9880_IIC_ADDR		(0x98)
#define _TVP5150_IIC_ADDR0		(0xB8)
#define _TVP5150_IIC_ADDR1		(0xBA)
#define _AD7180_IIC_ADDR		(0x42)
#define	_AD7180_IIC_ADDR1		(0x40)

#define UNDEF_VALUE			0

typedef enum {
	I2CMODE_HW,
	I2CMODE_SW
} I2CMODE;

typedef struct  {
	I2CMODE mode;
	Int addr;

	I2C_Handle hw;
	Uint32 pinSCL;
	Uint32 pinSDA;

} I2C_INFO;

typedef struct {
	Int mode;
	Ptr handle;	
} I2C;

void readMultiReg(Ptr handle, Uint8 reg, Uint8 *data, Uint16 numBytes);
void writeMultiReg(Ptr handle, Uint8 reg, Uint8 *data, Uint16 numBytes);
void writeReg(Ptr handle, Uint8 reg, Uint8 data);
void readReg(Ptr handle, Uint8 reg, Uint8 *data);
