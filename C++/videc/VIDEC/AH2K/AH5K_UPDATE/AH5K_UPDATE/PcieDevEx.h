#ifndef __PCIE_DEV_EX_H
#define __PCIE_DEV_EX_H

#include <Windows.h>

/* ERROR */
#define RTN_OK	                                                           	 	0	/* normal OK */
#define	RTN_ERR			-1	/* driver access error etc */
#define	RTN_PRM_ERR		-2	/* parameter error */
#define	RTN_MEM_ERR		-3	/* memory allocate error etc */
#define	RTN_TIME_OUT	-4	/* time out error */

#define MAX_PCI_BAR    	6
#define BAR_FPGA_REG	0

/* FPGA registers definition */
#define FPGA_FIRM_VERSION		0x000
#define FPGA_SOFT_RESET      	0x004

#define FPGA_FLASH_DATA		0xA08
#define FPGA_FLASH_LEN		0xA0C
#define FPGA_FLASH_CTL		0xA10


/*Register bit definition */
#define FLASH_TRIG_BIT       0x00000001
#define FLASH_LAST_FLAG      0X00000010
#define FLASH_STATUS         0x00000100

//#define FLASH_BUSY    0x00000100
#define FLASH_IDLE    0x00000000

/* statues definition */
#define FLASH_WRITE_MODE    0x0
#define FLASH_READ_MODE     0x1
#define FLASH_BUSY          0x0


/* Used 32768-65535 */
#define FILE_DEVICE_DM64  0x8000
#define CODE_BASE_DM64    0x800

#define FILE_DEVICE_FPGA  0x8000
#define CODE_BASE_FPGA    CODE_BASE_DM64 + 0x100


// Define Interface reference/dereference routines for
// Interfaces exported by IRP_MN_QUERY_INTERFACE


//Add for FPGA board
#define IOCTL_REG_READ \
	CTL_CODE(FILE_DEVICE_FPGA, CODE_BASE_FPGA + 1,  METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_REG_WRITE \
	CTL_CODE(FILE_DEVICE_FPGA, CODE_BASE_FPGA + 2,  METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_POST_MSG_FPGA \
	CTL_CODE(FILE_DEVICE_FPGA, CODE_BASE_FPGA + 3,  METHOD_BUFFERED, FILE_WRITE_ACCESS)


//  {24131E5D-D439-4BC0-9574-8DF65A983FF9}
static const GUID AH2K_DEVEX_CLASS_GUID = 
{ 0x24131E5D, 0xD439, 0x4BC0, 0x95, 0x74, 0x8D, 0xF6, 0x5A, 0x98, 0x3F, 0xF9 };

static const GUID AH2KEX_DEVEX_CLASS_GUID = 
{ 0x2bb29a53, 0xcb41, 0x4400, 0xbf, 0xa5, 0x26, 0x04, 0xa5, 0xc4, 0xd5, 0x6e };


/* For Fpga board Access */
typedef struct _tagPortARG
{
	unsigned char bar;         /* Pci BaseAddress number */
	unsigned long offs;        /* offset */
	union 
	{
		unsigned long  ldata;  /* send/recv data buffer */
		unsigned short sdata;  /* send/recv data buffer */
		unsigned char  cdata;  /* send/recv data buffer */
	}u;
} PORT_ACCESS ,*PPORT_ACCESS;


typedef enum {
	DT_EX_INVALID=0,
	DT_EX_AH5112SE,
	DT_EX_AH5011SE,
	DT_EX_AH5111SE,
	DT_EX_AH5102SE,
	DT_EX_AH5002SE,
	DT_EX_AH5211SE,
	DT_EX_COUNT,
}DEV_EX_TYPE;

#define FPGA_VEN_EX_ID			_T("10EE")

#define DEV_EX_ID_AH5112SE		_T("0007")		
#define DEV_EX_ID_AH5011SE		_T("0008")
#define DEV_EX_ID_AH5111SE		_T("0009")
#define DEV_EX_ID_AH5102SE		_T("000A")
#define DEV_EX_ID_AH5002SE		_T("000B")
#define DEV_EX_ID_AH5211SE		_T("000C")

class PcieDevEx 
{
public:
	PcieDevEx(void);
	virtual ~PcieDevEx(void);
public:
	static int GetDevExCount(const GUID*pGUID);
	static int GetDevType(int nDevID,const GUID*pGUID,DEV_EX_TYPE&type);
	static int FpgaConfig(int nDevID,const GUID*pGUID,PBYTE pData,DWORD dwLen);
	static int GetFpgaFirmVersion(int nDevID,const GUID*pGUID,ULONG*VersionData);
	static void BootLoad(int nDevID,PBYTE pData,DWORD dwLen,DEV_EX_TYPE nDevType);

	//add for fpga card
	DEV_EX_TYPE device_type;

protected:
	HANDLE		m_hMutex;
	GUID		m_guidDev;
public:
	HANDLE		m_hDevEx;
};


int GetDevCount();

void GetDevType(int nDevID,DEV_EX_TYPE &device_type);

extern int g_nUpdateCount;

#endif