/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/*
 *  ======== teb6416_pci_devParams.c ========
 *   default driver parameters
 */
#include <std.h>

#include <c64xx_pci.h>

/*
 *  ======== pciDevParams ========
 *  This static initialization defines the device parameters.
 */
C64XX_PCI_DevParams pciDevParams = {
        C64XX_PCI_VERSION_1,          
        6,
        -1,               
        NULL,
        C64XX_PCI_INTR_MASK_DEFAULT   
};


