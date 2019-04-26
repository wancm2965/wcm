/*
	pci_util.h

	2008.8.12
*/

#ifndef __PCI_UTIL_H__
#define __PCI_UTIL_H__

#include <c64xx_pci.h>

extern Uint32 CPU_CLK;

typedef struct 
{
	Uint32				inUse;
	C64XX_PCI_Request	request;
	GIO_AppCallback 	callback;

	Ptr					data;
} PCI_CONTROL, *PPCI_CONTROL;

int pci_init();
Void pci_write(int vid, Ptr pData, Int nLen, Ptr flag);
Void pci_write_ex(PPCI_CONTROL pControl);

#endif
