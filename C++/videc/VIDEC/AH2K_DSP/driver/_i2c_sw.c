/*
	_i2c_sw.c

	create at 2009-10-21
*/

#include <std.h>
#include <mem.h>
#include <csl_gpio.h>

#include "_i2c_sw.h"

extern GPIO_Handle g_hGPIO;
extern Int INTERNALHEAP;

////////////////////////////////////////////////////////////////
void _idelay(int nDelay)
{
	volatile Uint32 i, n;

	n = 21 * nDelay;
	for (i = 0; i < n; i ++)
		asm("  nop");
}

inline Uint32 _GetSCL(I2C_SW_Handle hI2C)
{
	return GPIO_pinRead(hI2C->hGpio, hI2C->pinSCL);
}

inline Uint32 _GetSDA(I2C_SW_Handle hI2C)
{
	return GPIO_pinRead(hI2C->hGpio, hI2C->pinSDA);
}

inline void _SetSDA(I2C_SW_Handle hI2C, Uint32 level)
{

	if (level)
		GPIO_pinDirection(hI2C->hGpio, hI2C->pinSDA, GPIO_INPUT);
	else
	{
		GPIO_pinDirection(hI2C->hGpio, hI2C->pinSDA, GPIO_OUTPUT);
		GPIO_pinWrite(hI2C->hGpio, hI2C->pinSDA, level);
	}

	_idelay(hI2C->freq / 2);
}

inline void _SetSCL(I2C_SW_Handle hI2C, Uint32 level)
{
	GPIO_pinWrite(hI2C->hGpio, hI2C->pinSCL, level);

	_idelay(hI2C->freq / 2);
}

inline void _ClockStretching(I2C_SW_Handle hI2C)
{
	// wait for any SCL clock stretching
	GPIO_pinDirection(hI2C->hGpio, hI2C->pinSCL, GPIO_INPUT);
	while (_GetSCL(hI2C) == 0)
	{
		asm("  nop");
	}
	GPIO_pinDirection(hI2C->hGpio, hI2C->pinSCL, GPIO_OUTPUT);
	_SetSCL( hI2C, 1 );
}

void _Start(I2C_SW_Handle hI2C)
{
	GPIO_pinDirection(hI2C->hGpio, hI2C->pinSCL, GPIO_OUTPUT);

	_SetSDA(hI2C, 1);
	_SetSCL(hI2C, 1);
	_SetSDA(hI2C, 0);
	_SetSCL(hI2C, 0);
}

void _Stop(I2C_SW_Handle hI2C)
{
	GPIO_pinDirection(hI2C->hGpio, hI2C->pinSCL, GPIO_OUTPUT);

	_SetSCL(hI2C, 0);
	_SetSDA(hI2C, 0);
	_SetSCL(hI2C, 1);

	_ClockStretching(hI2C);
	_SetSDA(hI2C, 1);

}

static Bool _Write(I2C_SW_Handle hI2C, Uint8 val)
{
	volatile Uint8 mask;

	GPIO_pinDirection(hI2C->hGpio, hI2C->pinSCL, GPIO_OUTPUT);

	// generate bit patterns by setting SCL and SDA lines
	for ( mask = 0; mask < 8; mask++ ) {
		//Put data bit on pin.
		_SetSDA( hI2C, ( val & 0x80 ) ? 1 : 0 );
			                 
		val <<= 1;
	
		_ClockStretching(hI2C);
		_SetSCL( hI2C, 1 );
		_SetSCL( hI2C, 0 );
	}

	_SetSDA(hI2C, 1);
	_SetSCL( hI2C, 1 );

	// Wait ACK
	if ( _GetSDA(hI2C) == 0 ) {

		_SetSDA(hI2C, 0);
		_SetSCL(hI2C, 0);
		return TRUE;
	}

	// timeout
	_SetSCL(hI2C, 0);
	_Stop(hI2C);
	return FALSE;
}

static void _Read( I2C_SW_Handle hI2C, Uint8 * val, Bool ack )
{
	volatile Uint8 mask;

	*val = 0x00;   

	_SetSDA(hI2C, 1);
	_SetSCL(hI2C, 0);

	_ClockStretching(hI2C);

	GPIO_pinDirection(hI2C->hGpio, hI2C->pinSDA, GPIO_INPUT);
	for( mask = 0; mask < 8; mask++) {
		//--> Here data is output by the slave 
		*val <<= 1;
		_SetSCL(hI2C, 1);

		//--> Data from slave should be stable here
		*val |= _GetSDA(hI2C);

		_SetSCL(hI2C, 0);
		_idelay(1);
	}

	if (ack)
		_SetSDA(hI2C, 0);		// Set data pin to output/low to ACK the read
	else
		_SetSDA(hI2C, 1);		// Set data pin to input/high to NACK the read

	_SetSCL(hI2C, 1);								// Set SCLK high
	_SetSCL(hI2C, 0);								// Set SCLK low
	_SetSDA(hI2C, 0);
	_SetSDA(hI2C, 1);	
}

Bool _i2c_sw_write(I2C_SW_Handle hI2C, Uint8 reg, Uint8 *data, Uint32 len)
{
	int n;

	if (len) {
		// Send Start
		_Start( hI2C );

		// Send I2C Write Address
		if ( !_Write( hI2C, hI2C->address | I2C_WRITE)) {
			return FALSE;
		}

		_idelay(hI2C->freq);
		if ( !_Write( hI2C, reg )) {
			return FALSE;
		}

		_idelay(hI2C->freq);
		// Send Command
		for (n = 0; n < len; n++) {
			if ( !_Write(hI2C, data[n])) {
				return FALSE;
			}
			_idelay(hI2C->freq);
		}

		_Stop(hI2C);
		
	}

	return TRUE;
}

Bool _i2c_sw_read(I2C_SW_Handle hI2C,
              Uint8   reg,
              Uint8   *data,
              Uint32  len)
{
	int n;

	if (len) {
		// Send the (Repeated if WR) Start condition to start reception
		_Start(hI2C);

		// Send I2C Write Address
		if ( !_Write( hI2C, hI2C->address | I2C_WRITE)) {
			return FALSE;
		}

		_idelay(hI2C->freq);
		// Send register address
		if ( !_Write(hI2C, reg)) {
			return FALSE;
		}
		
		_idelay(hI2C->freq);
		_Start(hI2C);

		// Send I2C Read Address
		if ( !_Write(hI2C, hI2C->address | I2C_READ)) {
			return FALSE;
		}

		_idelay(hI2C->freq);
		// Receive response
		for (n = 0; n < len; n++) {

			_Read(hI2C, data + n, ((n == (len - 1)) ? 0 : 1));
			_idelay(hI2C->freq);
		}

		_Stop(hI2C);
	}

	return TRUE;
}

I2C_SW_Handle _i2c_sw_create(Uint32 addr, Uint32 pinSCL, Uint32 pinSDA, Int freq)
{
	I2C_SW_Handle pI2C = (I2C_SW_Handle )MEM_alloc(INTERNALHEAP, sizeof(I2C_SW), 64);

	if (g_hGPIO == INV)
		g_hGPIO = GPIO_open(GPIO_DEV0, GPIO_OPEN_RESET);

	GPIO_pinEnable(g_hGPIO, pinSCL | pinSDA);
			
	pI2C->address	= addr;
	pI2C->pinSCL	= pinSCL;
	pI2C->pinSDA	= pinSDA;
	pI2C->hGpio		= g_hGPIO;
	pI2C->freq		= freq;

	_Start(pI2C);
	_Stop(pI2C);

	return pI2C;
}
