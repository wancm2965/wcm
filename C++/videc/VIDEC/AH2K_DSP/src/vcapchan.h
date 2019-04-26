#ifndef _AHXXX_INFO_H_
#define _AHXXX_INFO_H_

#include "video_proc.h"
#include <fvid.h>
/*
define all device id
*/

#define AH1000 		0xA010
#define AH2000 		0xA011
#define AH2000X		0xA012
#define AH2040		0xA040
#define AH2300		0xA013
#define AH2100L		0xA032
#define AH1100L		0xA031
#define AH2200L		0xA033
#define AH410		0xA020
#define AH420L		0xA021
#define AH210		0xA030
#define AH2210L		0xA036
#define AH2205L		0xA038
#define AH4220E		0xA043

#define AH3110L		0xB110
#define AH3102SE    0xD102
#define AH3102SEA   0xD10C

#define AH3101    	0xB101
#define AH3111    	0xB111
#define AH3111A    	0xB11B //add by feichao

#define AH3101A		0xB10B
#define AH3101EA 	0xC10B

#define AH3111E   	0xC111
#define AH3111EA    0xC11B

#define AH3210		0xB210
#define AH3210A		0xB21A

#define QAVP3400	0xF400
#define QAVP3210    0xF210

#define AH3002SE_T	0xC002
#define	AH3110ED_T	0xC110
#define	AH3110D_T	0xA11A


#define AH3201EA	0xC201
#define HAVP3201	0xF201	//add by hedaihua
#define HAVPM3002SM	0xF002	//add by hedaihua
#define	AH3210ED	0xD210	//add by hedaihua
#define AH3110ED	0xD110	
#define	AH3210D		0xA210
#define	AH3110D		0xA110

#define AHUNDEF     0xFFFF

/*
define ADC type
*/
#define ADC9880		1
#define ADC5150		2
#define ADC2971		3
#define ADC7180		4

/*
define VP type
*/
#define VP0 			"/VP0CAPTURE"
#define VP1 			"/VP1CAPTURE"
#define VP2 			"/VP2CAPTURE"

/*
define I2c info
*/
#define I2CHANDLE0	0
#define I2CHANDLE12	0
#define I2CHANDLE16	1
#define I2CHANDLE17	2
#define I2CHANDLE46	0
#define I2CHANDLE47	1

/*
define device infomation
*/

typedef enum {
	CAPTURE_STOP = 0,
	CAPTURE_TEST,
	CAPTURE_START,
	CAPTURE_RUN
} CAPTURE_STATE;

//////////////////////////////////////////////////////////
#define SYS_CLK_600MHZ	600000000
#define SYS_CLK_720MHZ	720000000

#define SYS_MEM_32M		0
#define SYS_MEM_64M		1

#define VPORT0			0
#define VPORT1			1
#define VPORT2			2

#define CHANNEL_A		'A'
#define CHANNEL_B		'B'

#define ENCRYPT_OFF		0
#define ENCRYPT_ON		1

//∂®“Â∞Âø®√Ë ˆ±Ì
typedef struct 
{
	int idPort;		// vport 0,1,2
	int idChannel;	// a,b
	int adcType;		// 9880, 5150, sdi
	int polarity;
	int limitFrame;
	int limitRes;    // 

	int i2cType;		// hw? sim?
 	int i2cAddr;		// 
	int i2cScl;		// gpio pin
	int i2cSda;		// gpio pin

	int numPCIControl;
} VIDEOINFO;

typedef struct 
{
	int channel;
} AUDIOINFO;

typedef struct 
{
	int idBoard;
	int sysClk;
	int memSize;
	int cpld;        // 
	int encrypt;     // 

	int audioNum;
	int videoNum;
	AUDIOINFO audios[1];
	VIDEOINFO videos[4];
} BOARDINFO;

typedef struct {
	Uint32 index;
	VIDEOINFO *video;
	PVID_CHAN_INFO vi;
} CHANNAL_DESCRIPT;


typedef struct 
{
	FVID_Handle	handle;
   	FVID_Frame *frame;
	CAPTURE_STATE runStatus;

} TSK_CAP;

BOARDINFO *findBoardInfo(Uint16 idBoard);
Uint32 CheckBoardClk(void);
Bool CheckBoardMem(void);


TSK_CAP *cap_open(Int id, VIDEOINFO *video, Ptr setting);
void cap_stop(TSK_CAP *cap);

#endif


