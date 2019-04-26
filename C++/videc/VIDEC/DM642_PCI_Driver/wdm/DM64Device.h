// DM64Device.h
//
// Generated by DriverWizard 3.2.0 (Build 2485)
// Requires DDK and DriverWorks
// File created on 9/29/2007
//
// This include file contains the definition of a subclass of KPnpDevice.
// WDM drivers declare a subclass of KPnpDevice and override member
// functions to handle requests (IRPs) from the system.
//
#ifndef __DM64DEVICE_H__
#define __DM64DEVICE_H__

#include "KC6000.h"

class DM64Device : public KPnpDevice
{
public:
	SAFE_DESTRUCTORS;
	DM64Device(PDEVICE_OBJECT Pdo, ULONG Unit);
	~DM64Device();

	// Member functions
	DEVMEMBER_DISPATCHERS

	virtual NTSTATUS OnStartDevice(KIrp I);
	virtual NTSTATUS OnStopDevice(KIrp I);
	virtual NTSTATUS OnRemoveDevice(KIrp I);
	virtual NTSTATUS OnDevicePowerUp(KIrp I);
	virtual NTSTATUS OnDeviceSleep(KIrp I);
	virtual NTSTATUS DefaultPnp(KIrp I);
	virtual NTSTATUS DefaultPower(KIrp I);

	MEMBER_ISR(DM64Device,Isr);		// ISR (Interrupt Service Routine)
	MEMBER_DPC(DM64Device,IsrDpc);	// DPC (Deferred Procedure Call)
	
	DEVMEMBER_DMAREADY(DM64Device, OnMasterDmaReady);

	void SerialRead(KIrp I);
	void SerialWrite(KIrp I);
	NTSTATUS IOCTL_WARMRESET_Handler(KIrp I);
	NTSTATUS IOCTL_RUN_Handler(KIrp I);
	NTSTATUS IOCTL_INITEMIF_Handler(KIrp I);
	NTSTATUS IOCTL_LOAD_Handler(KIrp I);
	NTSTATUS IOCTL_RETRIEVE_Handler(KIrp I);
	NTSTATUS IOCTL_POST_Handler(KIrp I);
	NTSTATUS Hook(HANDLE hUserEvent);
	NTSTATUS Unhook();

protected:
	/////////////////////////////////////////////////////
	// Helper Function to Stop Device and Free Resources
	VOID FreeResources(void);

	// Member data
protected:
	KC6000				m_Chip;
	KPnpLowerDevice		m_Lower;
	KDeferredCall		m_DM64IsrDpc;	// Deferred Procedure Call
	
	typedef enum _DM64_ISR_REASON
	{
		IsrReasonNone				= 0x0,
		IsrReasonReadComplete		= 0x1,
		IsrReasonWriteComplete		= 0x2,
		IsrReasonCommandComplete	= 0x4,
		IsrReasonDmaComplete		= 0x8
				
	} DM64_ISR_REASON, *PDM64_ISR_REASON;
	
	//KDmaAdapter		m_MasterDma;			// DMA adapter object
	//KDmaTransfer		m_MasterDmaTransfer;	// DMA transfer object
	//KCommonDmaBuffer	m_DmaBuffer;			// Common buffer

	PUCHAR				m_ReceiveLogicalAddress;
	PUCHAR				m_ReceivePhysicalAddress;
	PUCHAR				m_SendLogicalAddress;
	PUCHAR				m_SendPhysicalAddress;

	//BOOLEAN				m_bHasControl;
	KEvent*				m_AppNotificationEvent;
	BOOLEAN				m_ReadPended;
	KLockableFifo<UCHAR>	m_ReadFifo;

	// TODO:	Review the driver managed queues.  The wizard uses one or
	//			more instances of the same DM64Queue class. 
	//			Review that the number of queue instances is correct.
	//			You might choose to derive another class.
	DM64Queue			m_ReadQueue;
	DM64Queue			m_WriteQueue;

#ifdef __COMMENT_ONLY
		// The following member functions are actually defined by 
		// a DEVMEMBER_XXX or MEMBER_XXX macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Visual Studio IDE using the class browser.
	virtual NTSTATUS Create(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS Close(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS Read(KIrp I);			  		// COMMENT_ONLY
	virtual NTSTATUS Write(KIrp I);					// COMMENT_ONLY
	virtual NTSTATUS DeviceControl(KIrp I);			// COMMENT_ONLY
	virtual NTSTATUS CleanUp(KIrp I); 				// COMMENT_ONLY
	virtual NTSTATUS SystemControl(KIrp I);			// COMMENT_ONLY
	BOOLEAN Isr(void);								// COMMENT_ONLY
	VOID IsrDpc(PVOID Arg1, PVOID Arg2);			// COMMENT_ONLY
#endif // __COMMENT_ONLY
};

#endif // __DM64DEVICE_H__