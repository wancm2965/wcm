/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/* 
 *  ======== c64xx_pci.h ========
 * 
 *  This file declares an implementation of the mini driver interface for
 *  PCI peripheral.
 * 
 *  - This driver is typically used by another mini driver to R/W to the PCI.
 *  - This driver allows multiple devices on the PCI bus
 *  - channel direction: IOM_INPUT, IOM_OUTPUT, or IOM_INOUT  
 *  - channel queue priority: The driver maintains two global request queues,  
 *        which could be set by users when channel creates.
 *        C64XX_PCI_QUEUE_PRIORITY_LOW, C64XX_PCI_QUEUE_PRIORITY_HIGH
 *        The default queue priority is the low priority  
 *  - The PCI could generates interrupt to host through control function. 
 *  - EEPROM functions are one to one CSL calls. CSL call return through   
 *        C64XX_PCI_EEPROM. See CSL manual for definition of the
 *        returned data structure.
 *  -  -------- options for C64XX_PCI_Request ---------
 *     31 -  4  |3 2 1 0
 *     reserved |PCI_WRITE
 *              |PCI_READ_PREF
 *              |PCI_READ_NOPREF
 *     -------------------------------------------------
 */

#ifndef C64XX_PCI_
#define C64XX_PCI_

#include <iom.h>


#ifdef __cplusplus
extern "C" {
#endif 

/* Version Number definition */
#define C64XX_PCI_VERSION_1 0xAB01

#ifndef TEST_ERROR_HANDLER
#define TEST_ERROR_HANDLER
#endif

#ifdef TEST_ERROR_HANDLER
#define C64XX_PCI_TEST_ERROR_HANDLER            0xf001
#endif 

#define C64XX_PCI_CTRL_BASE                     0x5000
/* Control Function for EEPROM */
#define C64XX_PCI_EEPROM_ERASE          (C64XX_PCI_CTRL_BASE + 0x01)
#define C64XX_PCI_EEPROM_ERASE_ALL      (C64XX_PCI_CTRL_BASE + 0x02)    
#define C64XX_PCI_EEPROM_IS_AUTO_CFG    (C64XX_PCI_CTRL_BASE + 0x03)    
#define C64XX_PCI_EEPROM_READ           (C64XX_PCI_CTRL_BASE + 0x04)    
#define C64XX_PCI_EEPROM_SIZE           (C64XX_PCI_CTRL_BASE + 0x05)    
#define C64XX_PCI_EEPROM_TEST           (C64XX_PCI_CTRL_BASE + 0x06)    
#define C64XX_PCI_EEPROM_WRITE          (C64XX_PCI_CTRL_BASE + 0x07)    
#define C64XX_PCI_EEPROM_WRITE_ALL      (C64XX_PCI_CTRL_BASE + 0x08)    

/* Control Function for PCI to Host Interrupt */
#define C64XX_PCI_DSP_INT_REQ_SET       (C64XX_PCI_CTRL_BASE + 0x09)    
#define C64XX_PCI_DSP_INT_REQ_CLEAR     (C64XX_PCI_CTRL_BASE + 0x0A)    

#define C64XX_PCI_POST_MSG				(C64XX_PCI_CTRL_BASE + 0x0B)
#define C64XX_PCI_RETRIEVE_MSG			(C64XX_PCI_CTRL_BASE + 0x0C)

/* Control Function for Reset */
#define C64XX_PCI_RESET_CHANNEL         IOM_CHAN_RESET
#define C64XX_PCI_RESET_ALL_CHANNELS            IOM_DEVICE_RESET        
        
#define C64XX_PCI_MASK_TRANSFER_MODE    0x0000000F /* READ/WRITE option mask */
#define C64XX_PCI_MASK_DSPMA            0xFFFFFFFC  /* DSPMA reg addr mask */
#define C64XX_PCI_MASK_DSPMA_AINC       0x00000002  /* DSPMA reg ainc mask */
#define C64XX_PCI_MASK_PCIMA            0xFFFFFFFC  /* PCIMA register mask */
#define C64XX_PCI_MASK_PCIMC_CNT        0xFFFF0000  /* PCIMC CNT mask */

#define C64XX_PCI_QUEUE_PRIORITY_LOW    0  /* queue priority */
#define C64XX_PCI_QUEUE_PRIORITY_HIGH   1

#define C64XX_PCI_SETXFERMODE(xfrOption, mode) \
                ((xfrOption) | ((mode) & C64XX_PCI_MASK_TRANSFER_MODE))
#define C64XX_PCI_GETXFERMODE(xfrOption) \
                ((xfrOption) & C64XX_PCI_MASK_TRANSFER_MODE)

#define C64XX_PCI_INTR_MASK_DEFAULT  1
typedef struct C64XX_PCI_Request 
{
	QUE_Elem link;
    Ptr    srcAddr;  /* 32bits src address: read->pci addr, write->dsp addr */
    Ptr    dstAddr;  /* 32bits dst address: read->dsp addr, write->pci addr */ 
    Uns    byteCnt;  /* 16 bits size of transfer data in byte */
    /* options bit0-3: PCI_WRITE, PCI_READ_PREF or PCI_READ_NOPREF */  
    Uns    options; 
    Ptr    reserved; /* for mini driver usage */ 
    
    Uns task_id;
    Uns seq;
    Uns start_time;
    Uns stop_time;
    
    Uns byteSend;
    Uns	byteRequest;
    
} C64XX_PCI_Request;

typedef struct C64XX_PCI_Attrs 
{
    Uns    queuePriority;
} C64XX_PCI_Attrs;

typedef struct C64XX_PCI_Msg
{
	Uns status;
	Uns size;
	Ptr data;
} C64XX_PCI_Msg;

typedef struct C64XX_PCI_EEPROMData 
{
    Ptr    eeAddr; /* 32bits EEPROM address */
    Uns    eeData; /* eeData could either be input data or output data */
} C64XX_PCI_EEPROMData;

/* PCI Error Handler definition */
#define C64XX_PCI_EVT_PARITY_ERR        1
#define C64XX_PCI_EVT_SYSTEM_ERR        2

typedef Void ( *C64XX_PCI_TerrCallback)(Uns evtMask, Ptr arg);

typedef struct C64XX_PCI_ErrInfo 
{
    Ptr statusReg;  /* pointer of the status register */
    Ptr inprogressDstAddr; /* dst addr caused err */
    Ptr inprogressSrcAddr; /* src addr caused err */
} C64XX_PCI_ErrInfo;

typedef struct C64XX_PCI_ErrCallback
{
    C64XX_PCI_TerrCallback      errFxn;
    Uns                         evtMask; /* ored events */
    C64XX_PCI_ErrInfo           *errInfo;
} C64XX_PCI_ErrCallback;

typedef struct C64XX_PCI_DevParams
{ 
    Int versionId; /* Version number, set by the app */
    Int pciIntrId; /* interrupt id for normal pci interrupt */
    Int pciErrIntrId; /* interrupt id for pci parity/system error interrupt */
    C64XX_PCI_ErrCallback *errCallback;
    Uns intrMask;    /* Interrupt mask, set while executing EDMA int ISR */
} C64XX_PCI_DevParams;

#define KERN_EMERG      "<0>"   /* system is unusable                   */
#define KERN_ALERT      "<1>"   /* action must be taken immediately     */
#define KERN_CRIT       "<2>"   /* critical conditions                  */
#define KERN_ERR        "<3>"   /* error conditions                     */
#define KERN_WARNING    "<4>"   /* warning conditions                   */
#define KERN_NOTICE     "<5>"   /* normal but significant condition     */
#define KERN_INFO       "<6>"   /* informational                        */
#define KERN_DEBUG      "<7>"   /* debug-level messages                 */

int PCI_printk(char *fmt, ...);
#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* C64XX_PCI_ */


