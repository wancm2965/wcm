/*++
本模块支持1个GSPI口的IO控制
本模块和硬件的接口为：
GSPI_DSPCLK 
GSPI_CS		
GSPI_SCLK	
GSPI_SDOUT	
GSPI_SDIN	

!!本模块需要引用外部的GPIO句柄-ghGpio
--*/
#ifndef _GSPI_H_
#define _GSPI_H_

#include <std.h>
#include <assert.h>
#include <csl_gpio.h>
#include <csl_vphal.h>
#include <csl_vp.h>


/*++
GSPI comprised of 4 pins
cs# 	:chip select, active low,host output
sclk	:burst clock, host output
sdout	:serial data out,host output
sdin	:serial data in,host input
--*/
#define GSPI_SDIN	GPIO_PIN4
#define GSPI_SDOUT	GPIO_PIN5
#define GSPI_SCLK	GPIO_PIN6
#define GSPI_CS		GPIO_PIN7



/*++
DSP clock
--*/
#define GSPI_DSPCLK 720*1000*1000

/*++
interface.
--*/



extern Bool	GSPI_init();
extern Bool	GSPI_write(Uns addr,Uns* bufptr,Uns bufcnt);
extern Bool	GSPI_read (Uns addr,Uns* bufptr,Uns bufcnt);


#endif /*_GSPI_H_*/



