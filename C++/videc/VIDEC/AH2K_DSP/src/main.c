/*
*  Copyright 2003 by Texas Instruments Incorporated.
*  All rights reserved. Property of Texas Instruments Incorporated.
*  Restricted rights to use, duplicate or disclose this code are
*  granted through contract.
*  
*/
/* "@(#) DDK 1.10.00.21 06-26-03 (ddk-b10)" */
#include <std.h>
#include <stdio.h>
#include <tsk.h>  
#include <dev.h>
#include <csl.h>
#include <dio.h>
#include <csl_dat.h>
#include <csl_cache.h>
#include <csl_vphal.h>
#include <csl_gpio.h>
#include <assert.h>

#include "i2c.h"
#include "vcapchan.h"
#include "pci_util.h"
#include "vcapparams.h"
#include "vportcap.h"
#include "pcm1801a.h"
#include "pcm1801u.h"
#include "video_proc.h"


extern VPORT_PortParams vCapParamsPort_9880;
extern VPORT_PortParams vCapParamsPort_5150;

extern void tskHVideo(CHANNAL_DESCRIPT *chan);
extern void tskVideo(CHANNAL_DESCRIPT *chan);
extern void tskAudio();
extern void tskMgr(CHANNAL_DESCRIPT *chan);

extern EDC_Fxns GS2971_Fxns;
extern EDC_Fxns ADV7180_Fxns;

//add by chenzhenbao
//4:2:0;
int g_ImgFmt=0;


String taskName[] = { "tskCap1", "tskCap2", "tskCap3", "tskCap4" };
GPIO_Handle g_hGPIO = INV;
DIO_Params dio_params;
CHANNAL_DESCRIPT g_ChanBlock[5] = { { (Uint32)INV }, { (Uint32)INV }, { (Uint32)INV }, { (Uint32)INV }, { (Uint32)INV }};

static Int InstallVideoDevice(Int idPort, Uint32 adcType, Int polarity)
{
	Int status;
	String DevName[3] = {VP0, VP1, VP2};
	struct DEV_Attrs vportAttrs;

	DEV_Device *dev;
	char name[20];

	sprintf(name, "/VP%dCAPTURE", idPort);
	DEV_match(name, &dev);
	if (dev != NULL)
		return SYS_OK;

	vportAttrs.devid = idPort;
	vportAttrs.devp = NULL;
	vportAttrs.type = DEV_IOMTYPE;
	if (adcType == ADC9880)
	{
		vportAttrs.params = &vCapParamsPort_9880;
		vCapParamsPort_9880.edcTbl[0] = &AD9880_Fxns;
	}
	else if (adcType == ADC2971)
	{
		vportAttrs.params = &vCapParamsPort_9880;
		vCapParamsPort_9880.edcTbl[0] = &GS2971_Fxns;
	}
	else if (adcType == ADC7180)
	{
		vportAttrs.params = &vCapParamsPort_5150;
		vCapParamsPort_5150.edcTbl[0] = &ADV7180_Fxns;
		vCapParamsPort_5150.edcTbl[1] = &ADV7180_Fxns;
	}
	else 
	{
		vportAttrs.params = &vCapParamsPort_5150;
	}

	((VPORT_PortParams *)vportAttrs.params)->vc1Polarity = 
	((VPORT_PortParams *)vportAttrs.params)->vc2Polarity = 
	((VPORT_PortParams *)vportAttrs.params)->vc3Polarity = polarity;
	status = DEV_createDevice(DevName[idPort], &VPORTCAP_Fxns, NULL, &vportAttrs);
	return  status;
}

static Int InstallAudioDevice(Int audioNum, Int chan)
{
	Int status;
	String DevName = "/udevCodec";
	struct DEV_Attrs attrs;
	
	if(audioNum == 2)
	{
		attrs.devid = 0;
		attrs.devp	= NULL;
		attrs.type	= DEV_IOMTYPE;
		attrs.params= NULL;
		status = DEV_createDevice(DevName, &TEB6416_EDMA_PCM1801_FXNS, (Fxn)TEB6416_EDMA_PCM1801_init, &attrs);
	}
	else
	{	
		attrs.devid = chan;
		attrs.devp	= NULL;
		attrs.type	= DEV_IOMTYPE;
		attrs.params= NULL;
		status = DEV_createDevice(DevName, &TEB6416_EDMA_PCM3002_FXNS, (Fxn)TEB6416_EDMA_PCM3002_init, &attrs);
	}

	dio_params.name = DevName;
	dio_params.chanParams = 0;
	attrs.devid = 0;
	attrs.devp 	= NULL;
	attrs.type	= DEV_SIOTYPE;
	attrs.params= &dio_params;
	status = DEV_createDevice("/dioCodec", &DIO_tskDynamicFxns, NULL, &attrs);

	return  status;
}

void CPLD_switch(void)
{
//#define VP_PFUNC1		(*(volatile Uint32 *)0x01C44020)
//#define VP_PDIR1		(*(volatile Uint32 *)0x01C44024)
//#define VP_PDOUT1		(*(volatile Uint32 *)0x01C4402C)
	//VP_PFUNC1 = 0x00000400; // VP0D[19:10] as GPIO pin. 
	//VP_PDIR1  = 0x00000400; // VP0D[15:12] as output.

	//VP_PDOUT1 = 0x00000000; //(0x05 & 0x0f) << 12;  // 选择下接口
	_VP_PFUNC1_FSET(PFUNC10, VP_PFUNC_PFUNC0_VDATA0TO9);	/* VP0D[19:10] as GPIO pin */
	_VP_PDIR1_FSET(PDIR10, VP_PDIR_PDIR10_VDATA10TO11OUT);
	_VP_PDOUT1_FSET(PDOUT10, 1);

}

/* Spin in a delay loop for delay iterations */
void DM642_wait(Uint32 delay)
{
	volatile Uint32 i;

	for (i = 0; i < 21 * delay; i++)
		asm("  nop");
}

void dm642_init()
{
	/* Initialize CSL */
	CSL_init();
}

static void board_init(void)
{
	CACHE_wbInvAllL2(CACHE_WAIT);
	CACHE_setL2Mode(CACHE_128KCACHE);			// reserve 128k for ME

	CACHE_enableCaching(CACHE_EMIFA_CE00);
	CACHE_enableCaching(CACHE_EMIFA_CE01);		// 32M cache only

	// 检测物理内存大小
	if (CheckBoardMem())
	{
		MEM_redefine(EXTERNALHEAP, (Ptr)0x80100000, 0x03F00000);

	 	CACHE_enableCaching(CACHE_EMIFA_CE02);
		CACHE_enableCaching(CACHE_EMIFA_CE03);		// enable cache for 64M
	}

	DAT_open(DAT_CHAANY, DAT_PRI_LOW, 1); //DAT_OPEN_2D);
}
Uint16 idPci;
Bool g_bIsPCICard=0;

void main(void)
{
	BOARDINFO *board;
	//Uint16 idPci;
	Uint32 i = 0;
	struct TSK_Attrs attrs = {0};
	CHIP_Config percfg = { 0 };

    CHIP_config(&percfg);
	// 检测CPU时钟,在使能CACHE之前执行比较稳定
	CPU_CLK = CheckBoardClk();
	


	board_init();

	idPci = pci_init();
	assert(idPci != 0);
	board = findBoardInfo(idPci);
	assert(board != 0);

	if(idPci!=AH2000)
	{
		CPU_CLK = SYS_CLK_720MHZ;
	}


	if (idPci==AH3102SE ||
		idPci==AH3102SEA ||
		idPci==AH3101EA	||
		idPci==AH3111EA	||
		idPci==AH3201EA||
		idPci==HAVP3201||
		idPci==HAVPM3002SM||
		idPci==AH3110ED_T||
		idPci==AH3002SE_T||
		idPci==AH3210ED||
		idPci==AH3110ED)
	{
		g_bIsPCICard=0;
	}
	else
	{
		g_bIsPCICard=1;
	}

	// 打开DSP外设
	if (board->audioNum != 0)
	{
		if(board->audioNum == 2)
		{
			percfg.percfg |= CHIP_MCASP0;
		}
		else
		{
			if (board->audios[0].channel == 0)
				percfg.percfg |= CHIP_MCBSP0;
			else
				percfg.percfg |= CHIP_MCBSP1;
		}
	}

	for (i = 0; i < board->videoNum; i++)
	{
		if (board->videos[i].i2cType == I2CMODE_HW)
			percfg.percfg |= CHIP_I2C;

		if (board->videos[i].idPort == VPORT0)
			percfg.percfg |= CHIP_VP0;

		if (board->videos[i].idPort == VPORT1)
			percfg.percfg |= CHIP_VP1;

		if (board->videos[i].idPort == VPORT2)
			percfg.percfg |= CHIP_VP2;
	}
	CHIP_config(&percfg);

	video_proc_init();

	attrs.stackseg = INTERNALHEAP;
	attrs.priority = 5;
	attrs.stacksize = 0x600;
	for (i = 0; i < board->videoNum; i++)
	{
		VIDEOINFO *video;
		Fxn fxn;

		video = &board->videos[i];
		assert(InstallVideoDevice(video->idPort, video->adcType, video->polarity) == SYS_OK);

		g_ChanBlock[i].index = i;
		g_ChanBlock[i].video = video;
		attrs.name = taskName[i];

		if(video->adcType == ADC5150 ||video->adcType==ADC7180)
			fxn = (Fxn)tskVideo;
		else  /* AD9880 or GS2971 */
			fxn = (Fxn)tskHVideo;
			
		TSK_create(fxn, &attrs, &g_ChanBlock[i]);
	}

	attrs.name = "tskMgr";
	TSK_create((Fxn)tskMgr, &attrs, g_ChanBlock);

	if (board->audioNum != 0)
	{
		AUDIOINFO *audio = &board->audios[0];
		InstallAudioDevice(board->audioNum,audio->channel);

		attrs.priority = 6;
		attrs.stacksize = 0x400;
		attrs.name = "tskAudio";
		TSK_create((Fxn)tskAudio, &attrs, board->audioNum);
	}
}
