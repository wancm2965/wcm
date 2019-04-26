/***********************************************************
*	
*	Copyright (C) 2004 Intelligent Motion Co. Ltd(IMDSP)
*                All Rights Reserved
* ==============================================
*	File Name: 			Kc6000reg.h
*	Project Name:		IMDSP PCI LIB	
*	Description:		C6000 register definitions
*	Create Time:		2004.6.10
*	Last Modify:		2004.12.13
*
***********************************************************/

#ifndef __KC6000REG_H__
#define __KC6000REG_H__

/////////////////////////////////////////////////////////////////////////////////////
// PCI CONFIGURATION REGISTERS
//

// PCI Configuration ID
#define C6000_PCIIDR					ULONG(0x0000)
#define C6000_PCIIDR_VID(_r_)			USHORT((_r_) & 0xFFFF)
#define C6000_PCIIDR_DID(_r_)			USHORT(((_r_) >> 16) & 0xFFFF)

// PCI Command
#define C6000_PCICR					ULONG(0x0004)
#define C6000_PCICR_IOS				USHORT(1 << 0)
#define C6000_PCICR_MS				USHORT(1 << 1)
#define C6000_PCICR_ME				USHORT(1 << 2)
#define C6000_PCICR_SC				USHORT(1 << 3)
#define C6000_PCICR_MWIE			USHORT(1 << 4)
#define C6000_PCICR_VGAPS			USHORT(1 << 5)
#define C6000_PCICR_PER				USHORT(1 << 6)
#define C6000_PCICR_WCC				USHORT(1 << 7)
#define C6000_PCICR_SERRE			USHORT(1 << 8)
#define C6000_PCICR_FBTBE			USHORT(1 << 9)

// PCI Status
#define C6000_PCISR					ULONG(0x0006)
#define C6000_PCISR_NCFS			USHORT(1 << 4)
#define C6000_PCISR_66MHZC			USHORT(1 << 5)
#define C6000_PCISR_UDF				USHORT(1 << 6)
#define C6000_PCISR_FBTBC			USHORT(1 << 7)
#define C6000_PCISR_MDPED			USHORT(1 << 8)
#define C6000_PCISR_DEVSELT			USHORT(3 << 9)
#define C6000_PCISR_TA				USHORT(1 << 11)
#define C6000_PCISR_RTA				USHORT(1 << 12)
#define C6000_PCISR_RMA				USHORT(1 << 13)
#define C6000_PCISR_SSE				USHORT(1 << 14)
#define C6000_PCISR_DPE				USHORT(1 << 15)

// PCI Revision ID
#define C6000_PCIREV				ULONG(0x0008)

// PCI Class Code
#define C6000_PCICCR					ULONG(0x0009)
#define C6000_PCICCR_RLPI(_r_)	    	UCHAR((_r_) & 0xFF)
#define C6000_PCICCR_SC(_r_)			UCHAR(((_r_) >> 8) & 0xFF)
#define C6000_PCICCR_BCC(_r_)			UCHAR(((_r_) >> 16) & 0xFF)

// PCI Cache Line Size
#define C6000_PCICLSR					ULONG(0x000C)

// PCI Bus Latency Timer
#define C6000_PCILTR					ULONG(0x000D)

// PCI Base Address Register for Memory Accesses to 4M prefetchable
#define C6000_PCIBAR0					ULONG(0x0010)
#define C6000_PCIBAR0_MSI				ULONG(1 << 0)
#define C6000_PCIBAR0_RL				ULONG(3 << 1)
#define C6000_PCIBAR0_P				    ULONG(1 << 3)
#define C6000_PCIBAR0_MBA				ULONG(0xFFFFFFF0)

// PCI Base Address Register for Memory Accesses to 8M non-prefetchable
#define C6000_PCIBAR1					ULONG(0x0014)
#define C6000_PCIBAR1_MSI				ULONG(1 << 0)
#define C6000_PCIBAR1_IOBA		    	ULONG(0xFFFFFFFC)

// PCI Base Address Register for I/O Accesses to pci register
#define C6000_PCIBAR2					ULONG(0x0018)
#define C6000_PCIBAR2_MSI				ULONG(1 << 0)
#define C6000_PCIBAR2_RL				ULONG(3 << 1)
#define C6000_PCIBAR2_P				    ULONG(1 << 3)
#define C6000_PCIBAR2_MBA				ULONG(0xFFFFFFF0)

// PCI Subsystem Vendor ID
#define C6000_PCISVID					ULONG(0x002C)

// PCI Subsystem ID		
#define C6000_PCISID					ULONG(0x002E)

// New Capability Pointer
#define C6000_CAP_PTR					ULONG(0x0034)
#define C6000_CAP_PTR_NCP				ULONG(0xFC)

// PCI Interrupt Line
#define C6000_PCIILR					ULONG(0x003C)

// PCI Interrupt Pin
#define C6000_PCIIPR					ULONG(0x003D)

// PCI Min_Gnt
#define C6000_PCIMGR					ULONG(0x003E)

// PCI Max_Lat
#define C6000_PCIMLR					ULONG(0x003F)

// Power Management Capability ID
#define C6000_PMCAPID					ULONG(0x0040)

// Power Management Next Capability Pointer
#define C6000_PMNEXT					ULONG(0x0041)
#define C6000_PMNEXT_NCP				UCHAR(0xFC)

// Power Management Capabilities
#define C6000_PMC					ULONG(0x0042)
#define C6000_PMC_V					USHORT(3 << 0)
#define C6000_PMC_CRPMES			USHORT(1 << 3)
#define C6000_PMC_DSI				USHORT(1 << 5)
#define C6000_PMC_AUXC				USHORT(7 << 6)
#define C6000_PMC_D1S			    USHORT(1 << 9)
#define C6000_PMC_D2S				USHORT(1 << 10)
#define C6000_PMC_PMES				USHORT(0xF800)

// Power Management Control/Status
#define C6000_PMCSR					ULONG(0x0044)
#define C6000_PMCSR_PS				USHORT(3 << 0)
#define C6000_PMCSR_PMEEN			USHORT(1 << 8)
#define C6000_PMCSR_DSEL			USHORT(0xF << 9)
#define C6000_PMCSR_DSC				USHORT(3 << 13)
#define C6000_PMCSR_PMES			USHORT(1 << 15)

// Power Management Data
#define C6000_PMDATA					ULONG(0x0047)

///////////////////////////////////////////////////////////////////////
//pci interrupt enable register(PCIIEN)
#define C6000_PCIIEN					ULONG(0x40000C)  //C6415
//#define C6000_PCIIEN					ULONG(0x24000C)  //C6205
#define C6000_PCIIEN_PWRMGMT			ULONG(1 << 0)
#define C6000_PCIIEN_PCITARGET			ULONG(1 << 1)
#define C6000_PCIIEN_PCIMASTER			ULONG(1 << 2)
#define C6000_PCIIEN_HOSTSW 			ULONG(1 << 3)
#define C6000_PCIIEN_PWRLH  			ULONG(1 << 4)
#define C6000_PCIIEN_PWRHL  			ULONG(1 << 5)
#define C6000_PCIIEN_MASTEROK			ULONG(1 << 6)
#define C6000_PCIIEN_CFGDONE			ULONG(1 << 7)
#define C6000_PCIIEN_CFGERR			    ULONG(1 << 8)
#define C6000_PCIIEN_EERDY			    ULONG(1 << 9)
#define C6000_PCIIEN_PRST			    ULONG(1 << 10)
#define C6000_PCIIEN_DMAHALTED			ULONG(1 << 11)

//pci interrupt Source register(PCIIS)
#define C6000_PCIIS 					ULONG(0x400008)   //C6415
//#define C6000_PCIIS 					ULONG(0x240008)   //C6205
#define C6000_PCIIS_PWRMGMT 			ULONG(1 << 0)
#define C6000_PCIIS_PCITARGET			ULONG(1 << 1)
#define C6000_PCIIS_PCIMASTER			ULONG(1 << 2)
#define C6000_PCIIS_HOSTSW  			ULONG(1 << 3)
#define C6000_PCIIS_PWRLH   			ULONG(1 << 4)
#define C6000_PCIIS_PWRHL   			ULONG(1 << 5)
#define C6000_PCIIS_MASTEROK			ULONG(1 << 6)
#define C6000_PCIIS_CFGDONE 	 		ULONG(1 << 7)
#define C6000_PCIIS_CFGERR			    ULONG(1 << 8)
#define C6000_PCIIS_EERDY			    ULONG(1 << 9)
#define C6000_PCIIS_PRST			    ULONG(1 << 11)
//#define C6000_PCIIS_DMAHALTED			ULONG(1 << 12) //ONLY C6215

//Power Management DSP Control/Status Register (PMDCSR)
#define C6000_PMDCSR 					ULONG(0x400004)   //C6415
//#define C6000_PMDCSR 					ULONG(0x240004)   //C6205
#define C6000_PMDCSR_REQSTATE0 			ULONG(1 << 0)
#define C6000_PMDCSR_REQSTATE1			ULONG(1 << 1)
#define C6000_PMDCSR_CURSTATE0			ULONG(1 << 2)
#define C6000_PMDCSR_CURSTATE1  	    ULONG(1 << 3)
#define C6000_PMDCSR_AUXDETECT  		ULONG(1 << 4)
#define C6000_PMDCSR_PMEDRVN   			ULONG(1 << 5)
#define C6000_PMDCSR_PMESTAT			ULONG(1 << 6)
#define C6000_PMDCSR_PWRWKP 	 		ULONG(1 << 7)
#define C6000_PMDCSR_PMEEN			    ULONG(1 << 8)
#define C6000_PMDCSR_D2WARMONWKP	    ULONG(1 << 9)
#define C6000_PMDCSR_D3WARMONWKP	    ULONG(1 << 10)
#define C6000_PMDCSR_HWPMECTL0			ULONG(1 << 11)
#define C6000_PMDCSR_HWPMECTL1			ULONG(1 << 12)
#define C6000_PMDCSR_HWPMECTL2			ULONG(1 << 13)
#define C6000_PMDCSR_HWPMECTL3			ULONG(1 << 14)
#define C6000_PMDCSR_HWPMECTL4			ULONG(1 << 15)
#define C6000_PMDCSR_HWPMECTL5			ULONG(1 << 16)
#define C6000_PMDCSR_HWPMECTL6			ULONG(1 << 17)
#define C6000_PMDCSR_HWPMECTL7			ULONG(1 << 18)

//DSP Reset Source/Status Register (RSTSRC)
#define C6000_RSTSRC 					ULONG(0x400000)   //C6415
//#define C6000_RSTSRC 					ULONG(0x240000)   //C6205
#define C6000_RSTSRC_RST    			ULONG(1 << 0)
#define C6000_RSTSRC_PRST   			ULONG(1 << 1)
#define C6000_RSTSRC_WARMRST			ULONG(1 << 2)
#define C6000_RSTSRC_INTREQ     	    ULONG(1 << 3)
#define C6000_RSTSRC_INTRST     		ULONG(1 << 4)
#define C6000_RSTSRC_CFGDONE   			ULONG(1 << 5)
#define C6000_RSTSRC_CFGERR 			ULONG(1 << 6)


//DSP Master Address Register (DSPMA)
#define C6000_DSPMA 					ULONG(0x400010)   //C6415
//#define C6000_DSPMA 					ULONG(0x240010)   //C6205

//PCI Master Address Register (PCIMA)
#define C6000_PCIMA 					ULONG(0x400014)   //C6415
//#define C6000_PCIMA 					ULONG(0x240014)   //C6205

//PCI Master Control Register (PCIMC)
#define C6000_PCIMC 					ULONG(0x400018)   //C6415
//#define C6000_PCIMC 					ULONG(0x240018)   //C6205

//Current DSP Address (CDSPA)
#define C6000_CDSPA 					ULONG(0x40001C)   //C6415
//#define C6000_CDSPA 					ULONG(0x24001C)   //C6205

//Current PCI Address Register (CPCIA)
#define C6000_CPCIA 					ULONG(0x400020)   //C6415
//#define C6000_CPCIA 					ULONG(0x240020)   //C6205

//Current Byte Count Register (CCNT)
#define C6000_CCNT 	    				ULONG(0x400024)   //C6415
//#define C6000_CCNT  					ULONG(0x240024)   //C6205

//PCI Transfer Halt Register (HALT)
//#define C6000_HALT  					ULONG(0x240028)   //ONLY C6205

//Host Status Register (HSR)
#define C6000_HSR 					ULONG(0x41FFF0)   //C6415
//#define C6000_HSR 				ULONG(0x27FFF0)   //C6205
#define C6000_HSR_INTSRC    		ULONG(1 << 0)
#define C6000_HSR_INTAVAL   		ULONG(1 << 1)
#define C6000_HSR_INTAM 			ULONG(1 << 2)
#define C6000_HSR_CFGERR    	    ULONG(1 << 3)
#define C6000_HSR_EEREAD     		ULONG(1 << 4)

//Host-to-DSP Control Register (HDCR)
#define C6000_HDCR 					ULONG(0x41FFF4)   //C6415
//#define C6000_HDCR 				ULONG(0x27FFF4)   //C6205
#define C6000_HDCR_WARMRESET    	ULONG(1 << 0)
#define C6000_HDCR_DSPINT   		ULONG(1 << 1)
#define C6000_HDCR_PCIBOOT 			ULONG(1 << 2)

//DSP Page Register (DSPP)
#define C6000_DSPP 					ULONG(0x41FFF8)   //C6415
//#define C6000_DSPP 				ULONG(0x27FFF8)   //C6205

#include <pshpack4.h>
typedef struct _C6000_DESCRIPTOR
{
	ULONG		PciAddrLow;
	ULONG		LocalAddr;
	ULONG		Size;
	ULONG		NextDescPtr;
	ULONG		PciAddrHigh;
} C6000_DESCRIPTOR, *PC6000_DESCRIPTOR;
#include <poppack.h>

#endif // __KC6000REG_H__

