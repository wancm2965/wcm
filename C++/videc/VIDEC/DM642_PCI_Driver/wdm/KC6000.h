/***********************************************************
*	
*	Copyright (C) 2004 Intelligent Motion Co. Ltd(IMDSP)
*                All Rights Reserved
* ==============================================
*	File Name: 			Kc6000.h
*	Project Name:		IMDSP PCI LIB	
*	Description:		KC6000 class declaration and implementation
*	Create Time:		2004.6.10
*	Last Modify:		2004.12.13
*
***********************************************************/

#ifndef __KC6000_H__
#define __KC6000_H__

#include "KC6000Reg.h"

extern KDebugOnlyTrace T;

struct pci_dsp_handshake {
	ULONG	request;
	ULONG	sta_id_seq;
	ULONG	len;
	ULONG	addr;
};

enum PacketState {
	READ_REQUEST	= 1,
	READ_COMPLETE	= 2,
	WRITE_REQUEST	= 3,
	WRITE_COMPLETE	= 4 
};

#define MAGIC_REQUEST		0xBABEF00D
#define MAGIC_ACK			0xFEEDBEEF

#define ADDR_READ_MAILBOX	0x1F400
#define ADDR_WRITE_MAILBOX	0x1F800
#define ADDR_HOST_REQ		0x1FF00
#define ADDR_DSP_NOTIFY		0x1FF10
#define MAILBOX_MAX_LEN		0x3F8
#define MSG_CHAN			128

#define KC6000_MK_HANDSHAKE(state, id, seq) ((state << 28) | (id << 16 ) | (seq & 0x00FF))

///////////////////////////////////////////////////////////////////////////////////
// Local Bus Access Mode
//
typedef enum _C6000_ACCESS_MODE
{
	C6000BitSize8 = 0,
	C6000BitSize16 = 1,
	C6000BitSize32 = 3
} C6000_ACCESS_MODE;

///////////////////////////////////////////////////////////////////////////////////
// Reason for ISR
//
typedef enum _C6000_ISR_REASON
{
	C6000ReasonNone = 0,
	C6000ReasonHOSTSW = 1,
	C6000ReasonMASTEROK = 2,
	C6000ReasonOutboundPost = 4

} C6000_ISR_REASON;

///////////////////////////////////////////////////////////////////////////////////
// There is no |= operator for enum type
//
inline C6000_ISR_REASON operator |= (C6000_ISR_REASON& a, C6000_ISR_REASON b)
{
	return (a = (C6000_ISR_REASON)(ULONG(a) | ULONG(b)));
}

enum DSPPBits {
	DSPP_PAGE_SHIFT = 22, 
	DSPP_PAGE_SIZE = (1UL << DSPP_PAGE_SHIFT),
	DSPP_PAGE_MASK = (~(DSPP_PAGE_SIZE - 1)),
	MAP = (1UL << 10),
};

///////////////////////////////////////////////////////////////////////////////////
// KC6000 class 
//		This class abstracts C6000 Chipset
//
class KC6000
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	KC6000() : m_bValid(false), m_PciConfig(NULL) { T << "KC6000::KC6000()\n" ; }
	~KC6000() { T << "KC6000::~KC6000()\n" ; Invalidate(); }

	// init/deinit/validate methods
	NTSTATUS Initialize(PDEVICE_OBJECT TopOfStack, PCM_RESOURCE_LIST pTranslated, PCM_RESOURCE_LIST pRaw, PKSERVICE_ROUTINE pIsr, PVOID pContext);

	VOID Invalidate();
	bool IsValid() { return m_bValid; }

public:
	////////////////////////////////////////
	// Interrupts

	// this context is used to synchronize
	// access to registers with ISR
	typedef struct _SET_CLEAR_CONTEXT
	{
		KC6000*			m_pPlx;
		ULONG			m_reg;
		ULONG			m_set;
		ULONG			m_clear;

		_SET_CLEAR_CONTEXT(KC6000* pPlx, ULONG reg, ULONG set, ULONG clear) :
		m_pPlx(pPlx), m_reg(reg), m_set(set), m_clear(clear) {}
	} SET_CLEAR_CONTEXT, *PSET_CLEAR_CONTEXT;

	void EnableInterrupts(bool bIsr = false);
	void DisableInterrupts(bool bIsr = true);
	C6000_ISR_REASON GetIsrReason();
	VOID ClearIsrReason();

	/////////////////////////////////
	// Direct Slave Mode
	void SetRemap(ULONG Address);
	void ReadPage0(ULONG Address, PVOID buf, ULONG size);
	void WritePage0(ULONG Address, PVOID buf, ULONG size);
	VOID BusIop(ULONG LocalAddress, PVOID Buffer, ULONG Size, bool bRead);

	////////////////////////////////////////
	// Dma
	bool StartMasterDma(ULONG LocalAddress, ULONG size, DMA_DIRECTION dir);

	////////////////////////////////////////
	// Reset the chip
	void SetEmif(PVOID emif, ULONG size);	
	VOID Reset();
	VOID Unreset();

	////////////////////////////////////////
	// Hardware registers lock
	void Lock(BOOLEAN bDpcLevel = FALSE) { m_HwLock.Lock(bDpcLevel); }
	void Unlock() { m_HwLock.Unlock(); }

protected:
	static BOOLEAN _set_clear_isr(PSET_CLEAR_CONTEXT p)
	{
		ULONG v = p->m_pPlx->m_MemConfig[p->m_reg];
		v |= p->m_set;
		v &= ~p->m_clear;
		p->m_pPlx->m_MemConfig[p->m_reg] = v;

		return TRUE;
	}

	void set_bit(ULONG reg, ULONG set)
	{
		SET_CLEAR_CONTEXT context(this, reg, set, 0);
		m_Interrupt.Synchronize((PKSYNCHRONIZE_ROUTINE)_set_clear_isr, &context);
	}

	void clear_bit(ULONG reg, ULONG clear)
	{
		SET_CLEAR_CONTEXT context(this, reg, 0, clear);
		//========================================================================
		//Synchronize-->This member gives the driver writer a means to execute 
		//non-ISR code in a state where there is no possibility of the ISR running, 
		//thereby allowing atomic access to shared data.
        //========================================================================
		m_Interrupt.Synchronize((PKSYNCHRONIZE_ROUTINE)_set_clear_isr, &context);
	}

public:
	KMemoryRange		m_MemConfig;		// Memory mapped Configuration Registers
	KIoRange			m_IoConfig;			// I/O Mapped Configuration Registers
	KMemoryRange		m_LocalAddrSpace0;	// Local Address Space 0
	KInterrupt			m_Interrupt;		// Interrupt

	KPciConfiguration*	m_PciConfig;				// PCI Configuration Registers

	KSpinLock			m_HwLock;			// Lock to synchronize register access
	bool				m_bValid;			// track valid state

	ULONG				m_ReadSequence;
	ULONG				m_WriteSequence;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
//

////////////////////////////////////////////////////////////////////////
//  KC6000::Initialize
//
//	Routine Description:
//		This routine is initialized the class
//
//	Parameters:
//		[in] pTranslated
//			pointer to translated resources
//
//		[in] pRaw
//			pointer to raw resources
//
//		[in] pIsr
//			pointer to ISR routine
//
//		[in] pContext
//			pointer to context for ISR
//
//	Return Value:
//		status
//
//	Comments:
//		This function should be called from OnStartDevice
//
// pResourceList : 翻译资源表指针
// Ordinal: 翻译资源表序号
// bSaveFlag : 表示是否保存浮点处理器环境，X86必须为FALSE
inline NTSTATUS KC6000::Initialize(PDEVICE_OBJECT TopOfStack, PCM_RESOURCE_LIST pTranslated, PCM_RESOURCE_LIST pRaw, PKSERVICE_ROUTINE pIsr, PVOID pContext)
{
	NTSTATUS status;

	do 
	{
		// Initialize PCI Configuration
		m_PciConfig = new KPciConfiguration(TopOfStack);
		if ( !m_PciConfig )
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		// Initialize Local Space 0
		status = m_LocalAddrSpace0.Initialize(pTranslated, pRaw, 0);
		if ( !NT_SUCCESS(status) )
			break;

		// Initialize configuration register memory space 
		status = m_MemConfig.Initialize(pTranslated, pRaw, 1);			
		if ( !NT_SUCCESS(status) )
			break;

		// Initialize configuration register i/o space
		status = m_IoConfig.Initialize(pTranslated, pRaw, 0);
		if ( !NT_SUCCESS(status) )
			break;

		// make sure that interrupts are disabled
		DisableInterrupts(true);

		// Initialize Interrupt
		status = m_Interrupt.InitializeAndConnect(pTranslated, pIsr, pContext);
		if ( !NT_SUCCESS(status) )
		{
			T.Trace(TraceWarning,__FUNCTION__" Failed to initialize m_DM64Interrupt STATUS %x\n", status);
			break;
		}

        // track valid state
		m_bValid = true;

	} while ( false );

	return status;
}

////////////////////////////////////////////////////////////////////////
//  KC6000::Invalidate
//
//	Routine Description:
//		This routine frees allocated resources
//
//	Parameters:
//		[in] bPresent
//			true if hardware is still present
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KC6000::Invalidate()
{
	m_bValid = false;

	m_Interrupt.Invalidate();
	m_LocalAddrSpace0.Invalidate();
	m_MemConfig.Invalidate();
	m_IoConfig.Invalidate();

	if ( m_PciConfig )
	{
		delete m_PciConfig;
		m_PciConfig = NULL;
	}
}

////////////////////////////////////////////////////////////////////////
//  KC6000::EnableInterrupts
//
//	Routine Description:
//		enables interrupts
//
//	Parameters:
//		[in] bIsr
//			true if routine is called from ISR
//
//	Return Value:
//		None
//
//	Comments:
//		None
inline void KC6000::EnableInterrupts(bool bIsr)
{
	if ( bIsr )
	{
		m_MemConfig[C6000_HSR] &= ~C6000_HSR_INTAM;
	}
	else
	{
		clear_bit(C6000_HSR, C6000_HSR_INTAM);
	}
}

////////////////////////////////////////////////////////////////////////
//  KC6000::DisableInterrupts
//
//	Routine Description:
//		disables interrupts
//
//	Parameters:
//		[in] bIsr
//			true if routine is called from ISR
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KC6000::DisableInterrupts(bool bIsr)
{
	if ( bIsr )
		//====================================================================================
		//m_MemConfig被初始化指向PCIBAR0
		//用KMemoryRegister operator [ ] ( int index )进行重载，也可用inb outb;
		//C6000_INTCSR_IE-->PCI Interrupt Enable. Writing 1 enables PCI interrupts (INTA#).
		//====================================================================================
		m_MemConfig[C6000_HSR] |= C6000_HSR_INTAM;
	else
		//===============================================================================
        //用同步函数，把C6000_INTCSR读出后，与上C6000_INTCSR_IE，在写入C6000_INTCSR
		//===============================================================================
		set_bit(C6000_HSR, C6000_HSR_INTAM);
}

////////////////////////////////////////////////////////////////////////
//  KC6000::GetIsrReason
//
//	Routine Description:
//		Examines the reason of ISR
//
//	Parameters:
//		None
//
//	Return Value:
//		ISR Reason
//
//	Comments:
//		None
//
inline C6000_ISR_REASON KC6000::GetIsrReason() 
{
	C6000_ISR_REASON Reason = C6000ReasonNone;

	ULONG IntCS = m_MemConfig[C6000_HSR];
	if ((IntCS & C6000_HSR_INTAVAL) == C6000_HSR_INTAVAL)
		Reason = C6000ReasonMASTEROK;

	return Reason;
}

////////////////////////////////////////////////////////////////////////
//  KC6000::ClearIsrReason
//
//	Routine Description:
//		Clears the ISR reason
//
//	Parameters:
//		[in] Reason
//			ISR Reason
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KC6000::ClearIsrReason()
{
	ULONG reg;

	Lock();
	reg = m_MemConfig[C6000_HSR];
	m_MemConfig[C6000_HSR] = (reg | C6000_HSR_INTSRC) & 0x1F;
	Unlock();
}

////////////////////////////////////////////////////////////////////////
//  KC6000::BusIop
//
//	Routine Description:
//		Reads Local Bus in Direct Slave Mode
//
//	Parameters:
//		[in] LocalAddress
//			Address to read from. If bRemap is true, then it is an
//			absolute offset on local bus, otherwise it is a relative
//			offset to the current mapping
//
//		[out] Buffer
//			buffer to hold the data
//
//		[in] Size
//			size of the buffer
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KC6000::BusIop(ULONG LocalAddress, PVOID Buffer, ULONG Size, bool bRead)
{
	PUCHAR tmp_buf;
	ULONG remain, offset;
	ULONG page_offset, numBytes;

	Lock();

	tmp_buf = (PUCHAR) Buffer;
	remain = Size;
	offset = LocalAddress;

	while (remain)
	{
		page_offset = offset & (DSPP_PAGE_SIZE - 1);
		if (page_offset + remain > DSPP_PAGE_SIZE)
			numBytes = DSPP_PAGE_SIZE - page_offset;
		else
			numBytes = remain;

		SetRemap(offset);
		if (bRead)
		{
			if (((ULONG_PTR)tmp_buf & 0x1) || (numBytes & 0x1))
				m_LocalAddrSpace0.inb(page_offset, (PUCHAR)tmp_buf, numBytes);
			else if (((ULONG_PTR)tmp_buf & 0x2) || (numBytes & 0x2))
				m_LocalAddrSpace0.inw(page_offset, (PUSHORT)tmp_buf, numBytes / sizeof(USHORT));	
			else
				m_LocalAddrSpace0.ind(page_offset, (PULONG)tmp_buf, numBytes / sizeof(ULONG));
		}
		else
		{
			if (((ULONG_PTR)tmp_buf & 0x1) || (numBytes & 0x1))
				m_LocalAddrSpace0.outb(page_offset, (PUCHAR)tmp_buf, numBytes);
			else if (((ULONG_PTR)tmp_buf & 0x2) || (numBytes & 0x2))
				m_LocalAddrSpace0.outw(page_offset, (PUSHORT)tmp_buf, numBytes / sizeof(USHORT));	
			else
				m_LocalAddrSpace0.outd(page_offset, (PULONG)tmp_buf, numBytes / sizeof(ULONG));
		}

		offset += numBytes;
		remain -= numBytes;
		tmp_buf += numBytes;
	}

	SetRemap(0);
	Unlock();
}

////////////////////////////////////////////////////////////////////////
//  KC6000::Reset
//
//	Routine Description:
//		Reset chip
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline VOID KC6000::Reset()
{
	ULONG boot = m_MemConfig[C6000_HDCR];

	if (boot & C6000_HDCR_PCIBOOT)
		m_MemConfig[C6000_HDCR] = boot | C6000_HDCR_WARMRESET;
}

inline VOID KC6000::Unreset()
{
	m_MemConfig[C6000_HDCR] |= C6000_HDCR_DSPINT;
}

////////////////////////////////////////////////////////////////////////
//  KPlx9056::SetRemap
//
//	Routine Description:
//		Set Local Address Mapping Address
//
//	Parameters:
//		[in] Address
//			New local mapping address
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
inline void KC6000::SetRemap(ULONG Address)
{
	m_MemConfig[C6000_DSPP] = (Address & DSPP_PAGE_MASK) >> DSPP_PAGE_SHIFT;
}

inline void KC6000::SetEmif(PVOID emif, ULONG size)
{
	m_MemConfig.outd(0, (PULONG)emif, size / sizeof(ULONG));
}

inline void KC6000::ReadPage0(ULONG Address, PVOID buf, ULONG size)
{
	if (((ULONG_PTR)buf & 0x1) || (size & 0x1))
		m_LocalAddrSpace0.inb(Address, (PUCHAR)buf, size);
	else if (((ULONG_PTR)buf & 0x2) || (size & 0x2))
		m_LocalAddrSpace0.inw(Address, (PUSHORT)buf, size / sizeof(USHORT));	
	else
		m_LocalAddrSpace0.ind(Address, (PULONG)buf, size / sizeof(ULONG));
}

inline void KC6000::WritePage0(ULONG Address, PVOID buf, ULONG size)
{
	if (((ULONG_PTR)buf & 0x1) || (size & 0x1))
		m_LocalAddrSpace0.outb(Address, (PUCHAR)buf, size);
	else if (((ULONG_PTR)buf & 0x2) || (size & 0x2))
		m_LocalAddrSpace0.outw(Address, (PUSHORT)buf, size / sizeof(USHORT));	
	else
		m_LocalAddrSpace0.outd(Address, (PULONG)buf, size / sizeof(ULONG));
}

inline bool KC6000::StartMasterDma(ULONG LocalAddress, ULONG size, DMA_DIRECTION dir)
{
	struct pci_dsp_handshake hs;
	ULONG magic;

	ReadPage0(ADDR_HOST_REQ, (PVOID)&magic, sizeof(magic));
	if  (magic != MAGIC_ACK)
	{
		T.Trace(TraceInfo, __FUNCTION__"-- Error!\n");
		return false;
	}

	if (dir == FromDeviceToMemory)
		hs.sta_id_seq = KC6000_MK_HANDSHAKE(READ_REQUEST, 1, m_ReadSequence);
	else
		hs.sta_id_seq = KC6000_MK_HANDSHAKE(WRITE_REQUEST, 1, m_WriteSequence);
	hs.len = size;
	hs.addr = LocalAddress;
	hs.request = MAGIC_REQUEST;
	WritePage0(ADDR_HOST_REQ, (PULONG)&hs, sizeof(hs));

	// enable Host to DSP soft interrupt
	m_MemConfig[C6000_HDCR] |= C6000_HDCR_DSPINT;

	return true;
}
#endif // __KC6000_H__
