/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/*
 *  ======== c64xx_pci.c ========
 * 
 *  DSP/BIOS PCI IOM mini-driver for the c64xx DSPs
 *  Uses the  BIOS PCI Chip Support Library(CSL).
 */
 
/* add talker( handle_host_request() ) to handle request from host. 
 * 2006-12-27 yliqiang@gmail.com
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <std.h>
#include <csl_pci.h>
#include <csl_cache.h>
#include <hwi.h>
#include <mem.h>
#include <que.h>
#include <assert.h>

#include <iom.h>
#include <c64xx_pci.h>

extern Int EXTERNALHEAP;

#define PCI_MAXPACKETS			6
#define ADDR_READ_MAILBOX		0x1F400
#define ADDR_WRITE_MAILBOX		0x1F800
#define PCI_DSP_HANDSHAKE_ADDR	0x1FF00
#define PCI_DSP_COMPLETE_ADDR	0x1FF10
#define REQUEST_MAGIC 			0xBABEF00D
#define ACK_MAGIC				0xFEEDBEEF

#define Timer_Cnt(z) ( (z) = *(volatile unsigned int *)0x1980008 )
/* PCI Interrupt mapped to HWI Vector 4 */
#define ISR_VECTOR_ID   4

#define GET_BYTE_COUNT(count) \
        (((count)<< 16) & C64XX_PCI_MASK_PCIMC_CNT)

#define CALLBACK(chan, packet, result) { (packet)->status = (result);\
            ((chan)->callback)((Ptr)(chan)->callbackArg, (packet));}

typedef struct C64XX_PCI_MAILBOX {
	Uint32 magic;
	Uint32 size;
	Int8 data[1];
} C64XX_PCI_MAILBOX;

/* Channel variables */
typedef struct ChanObj {
	struct ChanObj		*next;
	Int					id;
    QUE_Handle          queue; /* priority queue */
    QUE_Obj				inQue;
    QUE_Obj				outQue;
    QUE_Obj				readRequestQue;
    QUE_Obj				writeRequestQue;
    Uns                 writeCount; /* count for number of IOM_WRITE IOPs */ 
    IOM_Packet          *flushAbortIop; /* hold IOM_FLUSH/IOM_ABORT IOP */ 
    /* callback used to notify client when I/O complete */ 
    IOM_TiomCallback    callback;
    Ptr                 callbackArg;
} ChanObj, *ChanHandle;

/* Device object */
typedef struct PCIDeviceObj 
{
	ChanObj						*chObjQue;
    IOM_Packet                  *curPacket; /* current packet in process */
    QUE_Obj                     highPrioQue;
    QUE_Obj                     lowPrioQue;

    QUE_Obj     				freeList;
    C64XX_PCI_Request			freeReq[PCI_MAXPACKETS]; // available frames for I/O operations

    C64XX_PCI_TerrCallback      errCallback; /* async error callback */
    Uns                         evtMask; /* registered events ored together */ 
    C64XX_PCI_ErrInfo           *errInfo; /* detailed info for async error */ 
    Int                         openCount; /* count number of open channels */
    Ptr							printkHostAddr;
    Uns							printkHostLen;

	//Ptr							controlReceiveAddr;
	//Uns							controlReceiveLen;

	//Ptr							controlSendAddr;
	//Uns							controlSendLen;
} PCIDeviceObj, PCIDeviceHandle;

/*
the protocal to exchange info. to dsp 
32 31 30 29 28 .... 16 15... 0 
request/ack magic
type        task_id    len	     
seq/data  (type related)
address/? (type related)
*/
enum PacketState{
	READ_REQUEST	= 1,
	READ_COMPLETE	= 2,
	WRITE_REQUEST	= 3,
	WRITE_COMPLETE	= 4
};

struct pci_dsp_handshake {
	Uint32	request;
	Uint32	seq:16,
			task_id:12,
			type:4;
	Uint32	len;
	Uint32 	addr;    	
};

/* static device object */
PCIDeviceObj device;

static Void doTransfer(C64XX_PCI_Request *request);
static Void isr();
#ifdef PCI_ERROR_ISR_IMPL
static Void pciErrorIsr();
#endif
static Void processQueue();
static Void removePackets(Ptr chanp, Int cmd);
static Void resetChannel(Ptr chanp);
static Void resetDevice();
static void writeMailBox(Ptr chanp, Ptr arg);
static void readMailBox(Ptr chanp, Ptr arg);
static void ack_host(C64XX_PCI_Request *oldReq);

//static void PCI_control_init();
//static void PCI_set_control(Ptr chanp, Ptr arg);

/*
 * Forward declaration of mini driver interface functions. They are only 
 * exposed via the IOM_Fxns function table to avoid namespace pollution.
 */
static Int      mdBindDev(Ptr *devp, Int devid, Ptr devParams);
static Int      mdControlChan(Ptr chanp, Uns command, Ptr arg);
static Int      mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
        						Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg);
static Int      mdDeleteChan(Ptr chanp);
static Int      mdSubmitChan(Ptr chanp, IOM_Packet* pPacket);
static Int      mdUnBindDev(Ptr devp);
static void 	PCI_printk_init();
/*
 * Public mini driver interface table.
 */
IOM_Fxns C64XX_PCI_FXNS = {
    &mdBindDev,
    &mdUnBindDev,
    &mdControlChan,
    &mdCreateChan,
    &mdDeleteChan,
    &mdSubmitChan
};

/*
 *  ======== mdBindDev ========
 *  This function will init the queues, set up interrupt.
 *  it does not allocate memory, so return NULL.  
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams)
{
    Int intrId;
    C64XX_PCI_DevParams *pciDevParams = (C64XX_PCI_DevParams *)devParams;
    HWI_Attrs hwiAttrs;
	int i;
	
	device.chObjQue = NULL;
	device.printkHostAddr = NULL;
	/* tell host the handshake address */
	PCI_RSET(DSPMA,PCI_DSP_HANDSHAKE_ADDR);
	PCI_RSET(PCIMA,REQUEST_MAGIC);

	/* Initialize queue structures */
    QUE_new(&device.highPrioQue);
    QUE_new(&device.lowPrioQue);

    QUE_new(&device.freeList);
    
    /* Initialize the freeList with packets */
    for (i = 0; i < PCI_MAXPACKETS; i++)  {
        QUE_put(&device.freeList, &device.freeReq[i]);
    }

    device.curPacket = NULL;
    device.openCount = 0;
    
    if (pciDevParams != NULL) {
        /* Check the version number */
        if (pciDevParams->versionId != C64XX_PCI_VERSION_1){
           /* Unsupported version */
           return(IOM_EBADARGS);
        }
        if (pciDevParams->pciIntrId > 0) {
           intrId = pciDevParams->pciIntrId;
        }
        else {
           intrId = ISR_VECTOR_ID;
        }
        
        hwiAttrs.intrMask = pciDevParams->intrMask;

        if (pciDevParams->errCallback != NULL) {
            device.errCallback = pciDevParams->errCallback->errFxn;
            device.evtMask = pciDevParams->errCallback->evtMask;
            device.errInfo = pciDevParams->errCallback->errInfo;
        }
        else {
            device.errCallback = NULL;
        }

#ifdef PCI_ERROR_ISR_IMPL
        /* 
         *  pciErrIntrId plug in depend on board hardware design.
         *  ERR_ISR_VECTOR_ID: TBD. 
         */
        
        Int pciErrIntrId;
        if (pciDevParams->pciErrIntrId > 0) {
            pciErrIntrId = pciDevParams->pciErrIntrId;
        }
        else {
            pciErrIntrId = ERR_ISR_VECTOR_ID; 
        }
#endif
    }
    else {
        intrId = ISR_VECTOR_ID;
        hwiAttrs.intrMask = C64XX_PCI_INTR_MASK_DEFAULT;
        device.errCallback = NULL;
    }
    
    hwiAttrs.ccMask = IRQ_CCMASK_NONE; /* the default value */
    hwiAttrs.arg = NULL;

    /* plug in normal pci isr routine */
    IRQ_map(IRQ_EVT_DSPINT, intrId); 
    /* dmachan parameter set to -1 since 6416 has EDMA instead of DMA */  
    HWI_dispatchPlug(intrId, (Fxn)isr, -1, &hwiAttrs);

#ifdef PCI_ERROR_ISR_IMPL
    /* plug in error pci isr routine if registered. IRQ_EVT_XXX: TBD.  
     * need to enable two bits in PCI Command Register: TBD 
     *          bit6 - parity error reporting enable bit
     *          bit8 - system error reporting ebable bit
     */
    IRQ_map(IRQ_EVT_XXX, pciErrIntrId); 
    HWI_dispatchPlug(pciErrIntrId, (Fxn)pciErrIsr, -1, NULL);
    IRQ_enable(IRQ_EVT_XXX);
#endif
	PCI_printk_init();
    return (IOM_COMPLETED);
    
}

/*
 *  ======== mdControlChan ========
 *  This function could be implemented by using function table, avoid complier
 *  link each function and also avoid using "switch" call. 
 *  return status  
 */
static Int mdControlChan(Ptr chanp, Uns command, Ptr arg)
{
    C64XX_PCI_EEPROMData *promData = (C64XX_PCI_EEPROMData *)arg;
    Uns                   result; 
    Uns                   imask;
    
#ifdef TEST_ERROR_HANDLER
    Bool report;
    C64XX_PCI_Request *req;
    IOM_Packet *packet;
    Uns error;
    Uns event;
#endif
    
    switch (command) {
        /* EEPROM call. */
        case C64XX_PCI_EEPROM_ERASE: 
            promData->eeData = PCI_eepromErase((Uns)promData->eeAddr);
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_ERASE_ALL:
            promData->eeData = PCI_eepromEraseAll();
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_IS_AUTO_CFG:
            promData->eeData = PCI_eepromIsAutoCfg();
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_READ:
            promData->eeData = PCI_eepromRead((Uns)promData->eeAddr);
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_SIZE:
            promData->eeData = PCI_eepromSize();
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_TEST:
            promData->eeData = PCI_eepromTest();
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_WRITE:
            result = PCI_eepromWrite((Uns)promData->eeAddr, promData->eeData);
            promData->eeData = result;
            return (IOM_COMPLETED); 
        case C64XX_PCI_EEPROM_WRITE_ALL:
            result = PCI_eepromWriteAll(promData->eeData);
            promData->eeData = result;
            return (IOM_COMPLETED); 

        /* Host interrupt call */
        case C64XX_PCI_DSP_INT_REQ_SET:
            PCI_dspIntReqSet();
            return (IOM_COMPLETED);
        case C64XX_PCI_DSP_INT_REQ_CLEAR:
            PCI_dspIntReqClear();
            return (IOM_COMPLETED);

        /* reset calls */
        case C64XX_PCI_RESET_CHANNEL:
            resetChannel(chanp);
            return (IOM_COMPLETED);      
        case C64XX_PCI_RESET_ALL_CHANNELS:
            resetDevice();
            return (IOM_COMPLETED);

		case C64XX_PCI_POST_MSG:
			writeMailBox(chanp, arg);
			return (IOM_COMPLETED);
			
		case C64XX_PCI_RETRIEVE_MSG:
			readMailBox(chanp, arg);
			return (IOM_COMPLETED);

#ifdef TEST_ERROR_HANDLER
        case C64XX_PCI_TEST_ERROR_HANDLER:
    		error = 0;
    		report = FALSE;
    		event = *((Uns *)arg);
    		imask = HWI_disable();
    		if (device.errCallback == NULL) {
        		HWI_restore(imask);
				return (IOM_COMPLETED);
    		}
    		if ((event == C64XX_PCI_EVT_PARITY_ERR) && 
            	(device.evtMask & C64XX_PCI_EVT_PARITY_ERR)) {
        		error = error | C64XX_PCI_EVT_PARITY_ERR;
        		report = TRUE;
    		} 
    		else if ((event == C64XX_PCI_EVT_SYSTEM_ERR) && 
            	(device.evtMask & C64XX_PCI_EVT_SYSTEM_ERR)) {
        		error = error | C64XX_PCI_EVT_SYSTEM_ERR;
        		report = TRUE;
    		}

    		if (!report) {
        		HWI_restore(imask);
        		return (IOM_COMPLETED);
    		}

    		if (device.curPacket != NULL) {
        		packet = device.curPacket;
        		req = (C64XX_PCI_Request *)packet->addr;
        		device.errInfo->inprogressDstAddr = req->dstAddr;
        		device.errInfo->inprogressSrcAddr = req->srcAddr;
        		/* device.errInfo->statusReg = PCI_STATUS_REGISTER; */
    		} 
    		else {
        		device.errInfo->inprogressDstAddr = NULL;
        		device.errInfo->inprogressSrcAddr = NULL;
        		/* device.errInfo->statusReg = PCI_STATUS_REGISTER; */
    		}

    		(device.errCallback)(error, (Ptr)device.errInfo);
    		HWI_restore(imask);
    		return (IOM_COMPLETED);
#endif 
        default:
            break;
    }
    
    return (IOM_ENOTIMPL);      
}


/*
 *  ======== mdCreateChan ========
 *  This function allocates memory for channel object and initializes it.
 *  return IOM_COMPLETED if success return IOM_EALLOC if fails
 */
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg)
{
    ChanHandle          chan;   
    C64XX_PCI_Attrs     *pciAttrs = (C64XX_PCI_Attrs *)chanParams;
    Uns                 imask;
    Int					chanNum = -1;
    
    if (mode != IOM_INOUT) {
        return (IOM_EBADMODE);
    }
    
    if(*name++ == '/') {
        chanNum = atoi(name);
    }
    	
    chan = MEM_alloc(EXTERNALHEAP, sizeof(ChanObj), 0);
    if ( chan == MEM_ILLEGAL ) { 
        return (IOM_EALLOC);
    }
    
    /* Initialize the channel structure */
    chan->id = chanNum;
    chan->next = device.chObjQue;
    device.chObjQue = chan;
    QUE_new(&chan->inQue);
    QUE_new(&chan->outQue);
    QUE_new(&chan->readRequestQue);
    QUE_new(&chan->writeRequestQue);
    
    if ( pciAttrs != NULL && 
            pciAttrs->queuePriority == C64XX_PCI_QUEUE_PRIORITY_HIGH)
	{
        chan->queue = &device.highPrioQue;
    }
    else
	{
        chan->queue = &device.lowPrioQue;    
    }
    
    chan->writeCount = 0;
    chan->flushAbortIop = NULL;
    chan->callback = cbFxn;
    chan->callbackArg = cbArg; 
         
    imask = HWI_disable();
    if (device.openCount == 0) {
        /* enable PCI events */
        /* clear all intr */
        PCI_RSET(PCIIS, 0x00000FFF);
        PCI_intEnable(PCI_EVT_PCIMASTER);
        PCI_intEnable(PCI_EVT_PCITARGET);
        PCI_intEnable(PCI_EVT_MASTEROK);
		PCI_intEnable(PCI_EVT_HOSTSW);
        IRQ_enable(IRQ_EVT_DSPINT);

        /* tell host we are ready for handshake */
		*(volatile Uint32 *)PCI_DSP_HANDSHAKE_ADDR = ACK_MAGIC;
		*(volatile Uint32 *)(PCI_DSP_COMPLETE_ADDR) = ACK_MAGIC;
		*(volatile Uint32 *)(ADDR_READ_MAILBOX) = ACK_MAGIC;
		*(volatile Uint32 *)(ADDR_WRITE_MAILBOX) = ACK_MAGIC;

		/* first ack host */
		{
		C64XX_PCI_Request request = { 0 };

    	request.options = PCI_WRITE;
		request.task_id = 1;
		request.byteSend = 1;
		ack_host(&request);
		}
    }
    device.openCount++;
    HWI_restore(imask);

    *chanp = chan;

    return (IOM_COMPLETED);
}

/*
 *  ======== mdDeleteChan ========
 *  frees memory
 *  return status
 */
static Int mdDeleteChan(Ptr chanp)
{
    ChanHandle  chan = (ChanHandle)chanp;
    Uns                 imask;

    MEM_free(0, chan, sizeof(ChanObj));

    imask = HWI_disable();
    device.openCount--;
    if (device.openCount == 0) {
        /* disable PCI events */
        PCI_intDisable(PCI_EVT_PCIMASTER);
        PCI_intDisable(PCI_EVT_PCITARGET);
        PCI_intDisable(PCI_EVT_MASTEROK);

		PCI_intDisable(PCI_EVT_HOSTSW);
        IRQ_disable(IRQ_EVT_DSPINT);
    }
    HWI_restore(imask);

    return (IOM_COMPLETED); 
}

int g_spin_count = 0;

/*
 *  ======== mdSubmitChan ========
 *  return submit status
 */
static Int mdSubmitChan(Ptr chanp, IOM_Packet *pPacket)
{
    ChanHandle          chan = (ChanHandle)chanp;
    IOM_Packet*         packet = pPacket;
    Uns                 imask;
    C64XX_PCI_Request  	*req;    
    C64XX_PCI_Request  	*curReq;
   	C64XX_PCI_Request   *host_req;
    Uns					match = 0; 

    req = (C64XX_PCI_Request *)packet->addr;
    req->reserved = chan;
    
    /* IOM_READ or IOM_WRITE command handle in the same way */ 
    if (packet->cmd == IOM_READ || packet->cmd == IOM_WRITE) {

        imask = HWI_disable();
        if(chan->id == -1)
        {
	        QUE_enqueue(chan->queue, packet);
	
	        if (device.curPacket == NULL)
	        {
#ifndef TEST_RESET
	            processQueue();
#endif
	        }
        }
        else
        {
        	if(packet->cmd == IOM_READ)
        	{
        		host_req = QUE_dequeue(&chan->writeRequestQue);

        		if( (QUE_Obj *)host_req == &chan->writeRequestQue )
        		{
        			QUE_enqueue(&chan->inQue, packet);
        		}
        		else
        		{
					req->srcAddr = host_req->srcAddr;

					if (req->byteCnt > host_req->byteCnt)
						req->byteRequest = host_req->byteCnt;
					else
						req->byteRequest = req->byteCnt;
						
					match = 1;

					//QUE_put(&device.freeList, host_req);
        		}
        	}
        	else if (packet->cmd == IOM_WRITE)
        	{
        	    host_req = QUE_dequeue(&chan->readRequestQue);
        	    
        	    if( (QUE_Obj *)host_req == &chan->readRequestQue )
        	    {
        			QUE_enqueue(&chan->outQue, packet);
        		}
        		else
        		{
        			req->dstAddr = host_req->dstAddr;
					//req->dstAddr = device.controlReceiveAddr;
					
        			if (req->byteCnt > host_req->byteCnt)
						req->byteRequest = host_req->byteCnt;
					else
						req->byteRequest = req->byteCnt;

        			match = 1;
        		}
        	}
        	
        	if (match)
        	{
        		req->task_id = host_req->task_id;
        		req->seq = host_req->seq;
        		req->byteSend = 0;
        		
			    /* Must put the packet back of the freeList */
				QUE_put(&device.freeList, host_req);
				        		
        		QUE_enqueue(chan->queue, packet);
        		if (device.curPacket == NULL)
        		{
	            	processQueue();
	        	}
        	}
        }
        
        HWI_restore(imask);
        return (IOM_PENDING);
    } 

    if (packet->cmd == IOM_FLUSH || packet->cmd == IOM_ABORT) { 
        
        imask = HWI_disable();
        chan->flushAbortIop = packet;

        /* check the channel of pending packet */ 
        if (device.curPacket != NULL) {
                curReq = (C64XX_PCI_Request *)device.curPacket->addr;

            /* The pending packet is this channel. Let isr handles it */
            if ((ChanHandle)curReq->reserved == chan) {
                HWI_restore(imask);         
                return (IOM_PENDING);
            } 
        }         
        
        /* the pending packet is not FLUSH/ABORT channel */
        removePackets(chan, packet->cmd);

        /* FLUSH/ABORT done */
        if (chan->writeCount == 0) {
            chan->flushAbortIop = NULL;
            HWI_restore(imask); 
            return (IOM_COMPLETED);
        }                   

        HWI_restore(imask);         
        return (IOM_PENDING);
    }
    
    return (IOM_EBADIO); 
}

/*
 *  ======== mdUnBindDev ========
 *  return status
 */

static Int mdUnBindDev(Ptr devp)
{
    /* empty for now */
    return (IOM_COMPLETED);
}

/*
 *  ======== C64XX_PCI_init ========
 */
Void C64XX_PCI_init(Void)
{
    /* empty for now */
}

static Void handle_host_request(Void)
{
	ChanHandle	chan;
	volatile struct pci_dsp_handshake *hs = (volatile struct pci_dsp_handshake *)PCI_DSP_HANDSHAKE_ADDR;
	Uint32 id, type, len, seq, addr, match;

	while (hs->request != REQUEST_MAGIC)
	{
#ifdef _DEBUG
		printf("host isr request != REQUEST_MAGIC\n");
#endif
		asm("  nop");
	}

	id = hs->task_id;
	type = hs->type;
	len = hs->len;
	seq = hs->seq;
	addr = hs->addr;
	match = 0;

	hs->request = ACK_MAGIC;
	
	chan = device.chObjQue;
	while (chan)
	{
		IOM_Packet			*iomPacket;
    	C64XX_PCI_Request   *request;
    	
		if (chan->id != id)
		{
			chan = chan->next;
			continue;
		}
    		
		switch (type)
		{
			case READ_REQUEST:
			{
   				iomPacket = QUE_dequeue(&chan->outQue);
   		
   				// iomPacket queue is empty
   				if ((QUE_Obj *)iomPacket == &chan->outQue)
   				{
					request = QUE_get(&device.freeList);

					if(request==NULL)
					{
					#ifdef _DEBUG
						printf("request==NULL\n");
					#endif	
						break;
					}
   					
   					QUE_enqueue(&chan->readRequestQue, request); 
   					//assert(!QUE_empty(&chan->readRequestQue));
   					
					request->byteCnt = len;
   				}
   				else
   				{
   					request = (C64XX_PCI_Request *)iomPacket->addr;
   					if (request->byteCnt > len)
						request->byteRequest = len;
					else
						request->byteRequest = request->byteCnt;

   					match = 1;
   				}
				
				request->dstAddr = (Ptr)addr;
   				break;
			}
			
			case WRITE_REQUEST:
			{
				iomPacket = QUE_dequeue(&chan->inQue);
				
				if ((QUE_Obj *)iomPacket == &chan->inQue)
				{
					request = QUE_get(&device.freeList);
					
					QUE_enqueue(&chan->writeRequestQue, request);
   					request->byteCnt = len;
				} 
				else
				{
					request = (C64XX_PCI_Request *)iomPacket->addr;
					
					if (request->byteCnt > len)
						request->byteRequest = len;
					else
						request->byteRequest = request->byteCnt;
						
					match = 1;
				}

				request->srcAddr = (Ptr)addr;
				break;
			}
			
			default:
				return;
		}
		
		request->task_id = id;
   		request->seq = seq;
   		
   		if (match)
		{
			request->byteSend = 0;
			
			QUE_enqueue(chan->queue, iomPacket);
	       	if (device.curPacket == NULL) 
	       	{
	           	processQueue();
			}
		}

		return;
	}
	
	if (id == 0 && type == READ_REQUEST)
	{
		device.printkHostAddr = (Ptr)addr;
		device.printkHostLen = len;
	}

	/* drop the request */
}

static void ack_host(C64XX_PCI_Request *oldReq)
{
	/* ack host we are done */
    volatile struct pci_dsp_handshake *hs = (struct pci_dsp_handshake *)PCI_DSP_HANDSHAKE_ADDR+1;
    Uns	xfrMode = C64XX_PCI_GETXFERMODE(oldReq->options);
        	
   	while (hs->request != ACK_MAGIC)
   		g_spin_count++;

   	hs->seq = oldReq->seq;
   	hs->task_id = oldReq->task_id;
   	hs->len = oldReq->byteSend;
        	
   	if (xfrMode == PCI_WRITE)
   	{
		hs->type = READ_COMPLETE;
        hs->addr = (Uint32)oldReq->dstAddr;
	}
    else if (xfrMode ==  PCI_READ_PREF || xfrMode ==  PCI_READ_NOPREF)
    {
    	hs->type = WRITE_COMPLETE;
    	hs->addr = (Uint32)oldReq->srcAddr;
    }
        	
    hs->request = REQUEST_MAGIC;
    PCI_dspIntReqClear();
    PCI_dspIntReqSet();
    
    /* normal. process next request in the queues */
    //while (hs->request != ACK_MAGIC);
}

/*
 *  ======== isr ========
 *  interrupt service routine is runtime pluged in
 */
static Void isr(Void)
{
    ChanHandle          chan;
    IOM_Packet          *oldPacket;
    C64XX_PCI_Request   *oldReq;         
    Int                 status = IOM_EBADIO;
    
    if (PCI_FGET(PCIIS, MASTEROK)) {
        /* pci master transfer complete */       
        //PCI_FSET(PCIIS, MASTEROK, 1);
        PCI_RSET(PCIIS, _PCI_PCIIS_MASTEROK_MASK); /* clear the MASTEROK only */
        oldPacket = device.curPacket;
        oldReq = (C64XX_PCI_Request *)oldPacket->addr;
        
        oldReq->byteSend += oldReq->byteCnt;
        if (oldReq->byteSend < oldReq->byteRequest)
        {
          	doTransfer(oldReq);
          	return;
        }
        	
        status = IOM_COMPLETED;
    } 
    else if (PCI_FGET(PCIIS, PCIMASTER)) { 
        /* pci master abort */
        //PCI_FSET(PCIIS, PCIMASTER, 1);
        PCI_RSET(PCIIS, _PCI_PCIIS_MASTEROK_MASK);
    } 
    else if (PCI_FGET(PCIIS, PCITARGET)) {
        /* pci target abort */
        //PCI_FSET(PCIIS, PCITARGET, 1);
        PCI_RSET(PCIIS, _PCI_PCIIS_PCITARGET_MASK);
    }
    else if(PCI_FGET(PCIIS, HOSTSW)) {
        //PCI_FSET(PCIIS, HOSTSW, 1); 
        PCI_RSET(PCIIS, _PCI_PCIIS_HOSTSW_MASK);
        handle_host_request();
        return;
    } 
    else {
        /* not registered interrupt, clear intr here */
        PCI_RSET(PCIIS, 0x00000FFF);
        /* error handling TBD */
        return;
    }

    if (device.curPacket != NULL)
    {
        oldPacket = device.curPacket;
        oldReq = (C64XX_PCI_Request *)oldPacket->addr; 
        chan = (ChanHandle)oldReq->reserved;
        Timer_Cnt(oldReq->stop_time); 
        device.curPacket = NULL;

        if (chan->flushAbortIop == NULL)
        {
			ack_host(oldReq);
            processQueue();
            
            oldReq->byteCnt = oldReq->byteSend;
            /* callback */
            CALLBACK(chan, oldPacket, status);   
            return;
        } 
        
        /* flush or abort cmd */
        /* first do callback */
        CALLBACK(chan, oldPacket, status); 
                     
        if (chan->writeCount) { 
           chan->writeCount--;              
        } 
        else {
           removePackets(chan, chan->flushAbortIop->cmd);
        }
            
        /* process next request in the queues */
        processQueue();    

        /* if all write IOPs are done, call the callback for the FLUSH IOP */
        if (chan->writeCount == 0) {
           CALLBACK(chan,chan->flushAbortIop,IOM_COMPLETED);
           chan->flushAbortIop = NULL;   
        } 
        return; 
    } 

    /* process next request in the queues */
    processQueue(); 
}

/*
 *  ======== pciErrorIsr ========
 *  interrupt service routine is runtime pluged in for asynchronous error TBD
 */
#ifdef PCI_ERROR_ISR_IMPL
static Void pciErrorIsr(Void)
{
    /* How to check PCI status register to identify the error source --
    parity/system: TBD (on C6416, not accessable) use 'event' here to 
    represent the status register related content */ 
    
    Bool report = FALSE;
    Uns error = 0;
    C64XX_PCI_Request *req;
    IOM_Packet *packet;

    if (device.errCallback == NULL) {
        return;
    }
    if ((event == C64XX_PCI_EVT_PARITY_ERR) && 
            (device.evtMask & C64XX_PCI_EVT_PARITY_ERR)) {
        error = error | C64XX_PCI_EVT_PARITY_ERR;
        report = TRUE;
    } 
    else if ((event == C64XX_PCI_EVT_SYSTEM_ERR) && 
            (device.evtMask & C64XX_PCI_EVT_SYSTEM_ERR)) {
        error = error | C64XX_PCI_EVT_SYSTEM_ERR;
        report = TRUE;
    } 

    if (!report) {
        return;
    }

    if (device.curPacket != NULL) {
        packet = device.curPacket;
        req = (C64XX_PCI_Request *)packet->addr;
        device.errInfo->inprogressDstAddr = req->dstAddr;
        device.errInfo->inprogressSrcAddr = req->srcAddr;
        device.errInfo->statusReg = PCI_STATUS_REGISTER;
    } 
    else {
        device.errInfo->inprogressDstAddr = NULL;
        device.errInfo->inprogressSrcAddr = NULL;
        device.errInfo->statusReg = PCI_STATUS_REGISTER;
    }

    (device.errCallback)(error, (Ptr)device.errorInfo);
     
}
#endif

//#define C64_SUPPORT 1
/*
 *  ======== doTransfer ========
 */
 #define DMA_MAX_TRANSFER ((64 * 1024) - 4)
static Void doTransfer(C64XX_PCI_Request *request)
{
    C64XX_PCI_Request   *req = request;
    PCI_ConfigXfr       config;
    Uns                 xfrMode;

	request->byteCnt =  request->byteRequest - request->byteSend;
	if (request->byteCnt > DMA_MAX_TRANSFER)
		request->byteCnt = DMA_MAX_TRANSFER;
	
    xfrMode = C64XX_PCI_GETXFERMODE(request->options);
    config.trctl = 0x14;
        
    if (xfrMode == PCI_WRITE) {
        config.dspma = (Uns)req->srcAddr + request->byteSend;
        config.pcima = (Uns)req->dstAddr + request->byteSend; 
        config.pcimc = GET_BYTE_COUNT(req->byteCnt);
                                  
    } 
    else if (xfrMode ==  PCI_READ_PREF || xfrMode ==  PCI_READ_NOPREF) {
        config.dspma = (Uns)req->dstAddr + request->byteSend;
        config.pcima = (Uns)req->srcAddr + request->byteSend; 
        config.pcimc = GET_BYTE_COUNT(req->byteCnt);                    
    }
        
    PCI_xfrConfig(&config);
    Timer_Cnt(req->start_time);
    PCI_xfrStart(xfrMode);      
}


/*
 *  ======== processQueue ========
 */
static Void processQueue()
{
    QUE_Handle          queue;
    C64XX_PCI_Request   *request;

    queue = &device.highPrioQue;
    /* process next packet in queue */  
    if (QUE_empty(queue))
    {
        queue = &device.lowPrioQue;
        if (QUE_empty(queue))
        {
            return;
        }
    }
    
    device.curPacket = QUE_dequeue(queue);
    request = (C64XX_PCI_Request *)device.curPacket->addr;
    doTransfer(request);                     
}

/*
 *  ======== removePackets ========
 */
static Void removePackets(Ptr chanp, Int cmd) 
{           
    ChanHandle          chan = (ChanHandle)chanp;
    QUE_Elem            *next;
    QUE_Elem            *current;
    Int                 status;
#ifdef _DEBUG
	Int					n = 0;
#endif

    if (cmd != IOM_FLUSH && cmd != IOM_ABORT) {
        return;
    } 
    
	// add by ningwei 2009/4/22
	current = QUE_head(&chan->outQue);

	while(&chan->outQue != current) {
		next = QUE_next(current);

		QUE_remove(current);
		CALLBACK(chan, (IOM_Packet *)current, status);

        current = next;

#ifdef _DEBUG
		n++;
#endif
	}

#ifdef _DEBUG
	printf("free out queue %d item\n", n);
	n = 0;
#endif

	// 清除未处理的主机请求
	while(!QUE_empty(&chan->readRequestQue)) {
		C64XX_PCI_Request *request;

		request = QUE_dequeue(&chan->readRequestQue);
		request->options = 0 ;   
    	request->options = C64XX_PCI_SETXFERMODE(request->options, PCI_WRITE);
		request->byteSend = 1;
		ack_host(request);

		QUE_enqueue(&device.freeList, request);

#ifdef _DEBUG
		n++;
#endif
	}

#ifdef _DEBUG
	printf("ack host read request %d item\n", n);
#endif
}

/*
 *  ======== resetChannel ========
 */
static Void resetChannel(Ptr chanp)
{
    Uns                 imask;
    
    imask = HWI_disable();
    removePackets(chanp, IOM_ABORT);
    HWI_restore(imask); 
}

/*
 *  ======== resetDevice ========
 */
static Void resetDevice()
{
    QUE_Elem            *next;
    QUE_Elem            *current;
    QUE_Handle          queue;
    C64XX_PCI_Request  *curReq;
    Int                 i;  
    Uns                 imask;
    
    imask = HWI_disable();

    /* assume reset async ? */
    if (device.curPacket != NULL) {       
        curReq = (C64XX_PCI_Request *)((IOM_Packet *)device.curPacket)->addr; 
         CALLBACK((ChanHandle)curReq->reserved, 
                    (IOM_Packet *)device.curPacket, IOM_ABORTED); 
        device.curPacket = NULL;      
    }
    
    for (i = 0; i < 2; i++) {
        if (i == 0 ) {
            queue = &device.highPrioQue;      
        } 
        else {
            queue = &device.lowPrioQue; 
        }
        /* remove all the elements in the high/low priority queue */
        current = QUE_head(queue);
        curReq = (C64XX_PCI_Request *)((IOM_Packet *)current)->addr;
    
        while (queue != current) {
            next = QUE_next(current);
          
            QUE_remove(current);
            CALLBACK((ChanHandle)curReq->reserved, 
                    (IOM_Packet *)current, IOM_ABORTED);
                 
            current = next;
            curReq = (C64XX_PCI_Request*)((IOM_Packet *)current)->addr; 
        } /* end while */        
    } /* end for */

    HWI_restore(imask); 
}

#define LOG_BUF_LEN 1024
static char log_buf[2][LOG_BUF_LEN];
C64XX_PCI_Request req[2];
IOM_Packet iom_packet[2];
ChanObj printkChan;

static char *cur_log_buf;
static int cur_log_pos;
static int cur_log_idx;

static Void printkChanCallback(Ptr arg, IOM_Packet *packet)
{

}

static void PCI_printk_init()
{
	cur_log_buf = log_buf[0];
	cur_log_pos = 0;
	cur_log_idx = 0;
	
    req[0].srcAddr = log_buf[0];
    req[0].options = 0;
    req[0].options = C64XX_PCI_SETXFERMODE(req[0].options, PCI_WRITE);
    req[0].task_id = 0;
    req[0].reserved = &printkChan;
    iom_packet[0].addr = &req[0];
	req[1].srcAddr = log_buf[1];
	req[1].options = 0;
	req[1].options = C64XX_PCI_SETXFERMODE(req[1].options, PCI_WRITE);
	req[1].task_id = 0;
	req[1].reserved = &printkChan;
	iom_packet[1].addr = &req[1];
	
	printkChan.queue = &device.highPrioQue;
	printkChan.callback = printkChanCallback;
	printkChan.callbackArg = 0;
	printkChan.flushAbortIop = NULL;
    printkChan.writeCount = 0;
}

int PCI_printk(char *fmt, ...)
{
	va_list args;
    Uns	old;

    va_start(args, fmt);
    old = HWI_disable();
	cur_log_pos += vsnprintf(cur_log_buf+cur_log_pos,
							LOG_BUF_LEN-cur_log_pos,
							fmt, args);
	if(cur_log_pos >= (LOG_BUF_LEN-256))
	{
		if(device.printkHostAddr)
		{
			req[cur_log_idx].dstAddr = device.printkHostAddr;
			req[cur_log_idx].byteCnt = (cur_log_pos > device.printkHostLen) ?
									device.printkHostLen : cur_log_pos;
	        QUE_enqueue(printkChan.queue, &iom_packet[cur_log_idx]);
	        if (device.curPacket == NULL) {
	            processQueue();
	        }
			
		}
		cur_log_idx = !cur_log_idx;
		cur_log_buf = log_buf[cur_log_idx];
		cur_log_pos = 0;
	}
	HWI_restore(old);
    va_end(args);
    return cur_log_pos;
}

void PCI_printk_flush()
{
    Uns	old;
    old = HWI_disable();
	if(cur_log_pos)
	{
		if(device.printkHostAddr)
		{
			req[cur_log_idx].dstAddr = device.printkHostAddr;
			req[cur_log_idx].byteCnt = (cur_log_pos > device.printkHostLen) ?
									device.printkHostLen : cur_log_pos;
	        QUE_enqueue(printkChan.queue, &iom_packet[cur_log_idx]);
	        if (device.curPacket == NULL) {
	            processQueue();
	        }
			
		}
		cur_log_idx = !cur_log_idx;
		cur_log_buf = log_buf[cur_log_idx];
		cur_log_pos = 0;
	}
	HWI_restore(old);	
}

static void writeMailBox(Ptr chanp, Ptr arg)
{
	Int32 size, i;
	volatile C64XX_PCI_MAILBOX *mbox;
	C64XX_PCI_Msg *msg;
	
	mbox = (volatile C64XX_PCI_MAILBOX *)ADDR_READ_MAILBOX;
	msg =  (C64XX_PCI_Msg *)arg;

	if (mbox->magic != ACK_MAGIC)
	{
		msg->status = 0;
		return;
	}
		
	size = msg->size;
	if (size > 1016)
		size = 1016;
		
	// copy all data
	mbox->size = size;
	for (i = 0; i < size; i++)
		mbox->data[i] = ((Uint8 *)(msg->data))[i];
	mbox->magic = REQUEST_MAGIC;
	
	msg->size = size;
	msg->status = 1;
	
	PCI_dspIntReqClear();
    PCI_dspIntReqSet();
}

static void readMailBox(Ptr chanp, Ptr arg)
{
	Int32 size, i;
	volatile C64XX_PCI_MAILBOX *mbox;
	C64XX_PCI_Msg *msg;
	
	mbox = (volatile C64XX_PCI_MAILBOX *)ADDR_WRITE_MAILBOX;
	msg =  (C64XX_PCI_Msg *)arg;

	if (mbox->magic != REQUEST_MAGIC)
	{
		msg->status = 0;
		return;
	}

	size = mbox->size;
	// copy all data
	for (i = 0; i < size; i++)
		((Uint8 *)(msg->data))[i]= mbox->data[i];

	msg->size = size;
	msg->status = 1;
	mbox->magic = ACK_MAGIC;
}

