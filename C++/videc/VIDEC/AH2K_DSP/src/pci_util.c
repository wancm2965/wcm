/*
	2008.7.22
	create by ningwei
*/

#include <std.h>
#ifdef _DEBUG
#include <stdio.h>
#endif
#include <async.h>
#include <tsk.h>

#include <csl_cache.h>
#include <csl_pci.h>
#include <c64xx_pci.h>

#include "vcapchan.h"
#include "pci_util.h"

Uint32 CPU_CLK = 720000000;

typedef struct {
	Uint8	enable;//0:DISABLE 1:CAP,2:ENC
	Uint8	scale_type;//0
	Uint8	framerate;//1-60
	Uint8	channel_index;//0

	Uint16	width;
	Uint16	height;
}XAH_PCI2DSP,*PXAH_PCI2DSP;

Ptr 				g_gio = NULL;
Bool				g_TransOn = 0;

int pci_init()
{
    C64XX_PCI_Attrs pciAttrs;
    GIO_Attrs gioAttrs;
	C64XX_PCI_EEPROMData config;
    
    pciAttrs.queuePriority = C64XX_PCI_QUEUE_PRIORITY_HIGH;
    gioAttrs.nPackets = 5;
    gioAttrs.timeout = SYS_FOREVER; 

   	g_gio = ASYNC_create("/udevPci/1", IOM_INOUT, NULL, &pciAttrs, &gioAttrs);
    if (g_gio == NULL)
		return 0;

	ASYNC_control(g_gio, C64XX_PCI_EEPROM_IS_AUTO_CFG, &config);
	if (config.eeData == 1)
	{
		// read device id
		config.eeAddr = (void *)1;
		ASYNC_control(g_gio, C64XX_PCI_EEPROM_READ, &config);

		return config.eeData;
	}

	return 0;
}

static void pci_process(PVID_CHAN_INFO vi, Ptr data)
{
	PVID_CHAN_INFO pvi = vi;
	PXAH_PCI2DSP pdata = (PXAH_PCI2DSP) data;

	pvi->flag_enable_cap = pdata->enable;
	if (pdata->width > 1920)
		pdata->width = 1920;
	if (pdata->height > 1080)
		pdata->height = 1080;

	if (pvi->framerate != pdata->framerate ||
		pvi->scale_type != pdata->scale_type ||
		pvi->width != pdata->width ||
		pvi->height != pdata->height)
	{
		pvi->framerate = pdata->framerate;
		pvi->scale_type = pdata->scale_type;
		
		pvi->width = pdata->width;
		pvi->height = pdata->height;
		
		if (pvi->framerate >= 40)
			pvi->framerate = 100;
		else if(pvi->framerate >= 30)
			pvi->framerate = 30;
		else if (pvi->framerate >= 15)
			pvi->framerate = 15;
		else if (pvi->framerate >= 10)
			pvi->framerate = 10;
		else if (pvi->framerate >= 5)
			pvi->framerate = 5;
		else
			pvi->framerate = 1;
	}
#ifdef _DEBUG
	printf("chan %d Translate is %d\n", vi->channel, vi->flag_enable_cap);
#endif
	if(vi->flag_enable_cap==0)
	{
		int i=0;
		PPCI_CONTROL pControl;
		for(i=0;i<pvi->numPCIControl;++i)
		{
			pControl=(PPCI_CONTROL)pvi->pPCIControl[i];
			pControl->inUse=0;
		}
	}
}

Void pci_write_ex(PPCI_CONTROL pControl)
{
	Int size;

    pControl->request.dstAddr = 0;
    pControl->request.options = 0 ;   
    pControl->request.options = C64XX_PCI_SETXFERMODE(pControl->request.options, PCI_WRITE);
	size = sizeof(C64XX_PCI_Request);
	CACHE_wbInvL1d(pControl,1024,CACHE_WAIT);

//	CACHE_invL1p(pControl,1024,CACHE_WAIT);
	CACHE_wbAllL2(CACHE_WAIT);

	//CACHE_wbL2(pControl, 1024, CACHE_WAIT);
		
   	if(IOM_ENOPACKETS==ASYNC_write(g_gio, &(pControl->request), &size, &(pControl->callback)))
	{
		pControl->inUse=0;	
	}
}

void _SetTransStatus(CHANNAL_DESCRIPT *chan)
{
	g_TransOn = FALSE;
	while (chan->index != (Uint32)INV)
	{
		if (chan->vi->flag_enable_cap)
		{
			g_TransOn = TRUE;
			break;
		}
		chan++;
	}
	
	// 如果所有通道都被关闭，则清除发送队列
	if (!g_TransOn)
	{
		ASYNC_control(g_gio, C64XX_PCI_RESET_CHANNEL, NULL);
	}
}

void tskMgr(CHANNAL_DESCRIPT *chan)
{
	C64XX_PCI_Msg msg;
	Ptr data;

	data = (PXAH_PCI2DSP)MEM_alloc(0, sizeof(XAH_PCI2DSP), 128);
	memset(data, 0, sizeof(XAH_PCI2DSP));
	msg.data = data;
	msg.size = sizeof(XAH_PCI2DSP);

	while (1)
	{
		msg.status = 0;
		ASYNC_control(g_gio, C64XX_PCI_RETRIEVE_MSG, &msg);
		if (msg.status == 1)
		{
			//control msg
			int channel_index = ((PXAH_PCI2DSP )data)->channel_index;
			
			if (chan[channel_index].vi != NULL)
			{	
				pci_process(chan[channel_index].vi, data);
				/*if(chan[channel_index].video->limitFrame!=0&&chan[channel_index].vi->framerate > chan[channel_index].video->limitFrame)
					chan[channel_index].vi->framerate = chan[channel_index].video->limitFrame;
					*/
					
			}

			_SetTransStatus(chan);
		}

		// 如果所有通道都被关闭，则清除发送队列
		if (!g_TransOn)
		{
			ASYNC_control(g_gio, C64XX_PCI_RESET_CHANNEL, NULL);
		}
		TSK_sleep(1);
	}
}


