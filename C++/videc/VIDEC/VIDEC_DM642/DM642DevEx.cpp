// DM642DevEx.cpp: implementation of the DM642DevEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <setupapi.h>
#include <winioctl.h>
#include <sys/types.h>
#include "DM642DevEx.h"
#include "coff.h"
#include "XAH2K.h"
#include "XUtil.h"

#define DEV_EX_INTF_INLINE inline
#define AH2K_MAX_WIDTH		1920
#define AH2K_MAX_HEIGHT		1080
#pragma comment(lib, "setupapi.lib")

//////////////////////////////////////////////////////////////////////////////
// class XDevExInterfaceClass
//
class XDevExInterfaceClass
{
public:
	XDevExInterfaceClass(const GUID* pClassGuid, PDWORD status);
	~XDevExInterfaceClass(void);
	GUID* GetGuid(void)      { return &m_Guid; }
	HDEVINFO GetHandle(void) { return m_hInfo; }

protected:
	HDEVINFO		m_hInfo;
	GUID			m_Guid;
};

//////////////////////////////////////////////////////////////////////////////
// class XDevExInterface
//
class XDevExInterface
{
public:
	DEV_EX_INTF_INLINE XDevExInterface( XDevExInterfaceClass* pClassObject, DWORD Index, PDWORD Error );
	DEV_EX_INTF_INLINE ~XDevExInterface(void);
	DEV_EX_INTF_INLINE TCHAR* DevicePath(void);

protected:
	XDevExInterfaceClass*				m_Class;
	SP_DEVICE_INTERFACE_DATA			m_Data;
	PSP_INTERFACE_DEVICE_DETAIL_DATA	m_Detail;
};


//////////////////////////////////////////////////////////////////////////////
// XDevExInterfaceClass constructor
//
DEV_EX_INTF_INLINE XDevExInterfaceClass::XDevExInterfaceClass(
	const GUID* pClassGuid,
	PDWORD status
	)
{
	DWORD flags = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT ;
	m_hInfo = INVALID_HANDLE_VALUE;
	ZeroMemory(&m_Guid,sizeof(GUID));

	try
	{
		*status = ERROR_INVALID_PARAMETER;
		m_Guid = *pClassGuid;
		m_hInfo = SetupDiGetClassDevs(pClassGuid, NULL, NULL, flags);

		if ( m_hInfo == INVALID_HANDLE_VALUE )
			*status = GetLastError();
		else
			*status = ERROR_SUCCESS;

	}
	catch (...)
	{
		m_hInfo = INVALID_HANDLE_VALUE;
	}
}

//////////////////////////////////////////////////////////////////////////////
// XDevExInterfaceClass destructor
//
DEV_EX_INTF_INLINE XDevExInterfaceClass::~XDevExInterfaceClass(void)
{
	if ( m_hInfo != INVALID_HANDLE_VALUE )
		SetupDiDestroyDeviceInfoList(m_hInfo);

	m_hInfo = INVALID_HANDLE_VALUE;
}

//////////////////////////////////////////////////////////////////////////////
// XDevExInterface constructor
//
DEV_EX_INTF_INLINE XDevExInterface::XDevExInterface(
	XDevExInterfaceClass* pClassObject,
	DWORD Index,
	PDWORD Error
	)
{
	m_Class = pClassObject;

	BOOL status;
	DWORD ReqLen;

	m_Detail = NULL;
	m_Data.cbSize = sizeof SP_DEVICE_INTERFACE_DATA;

	try
	{
		*Error = ERROR_INVALID_PARAMETER;

		status = SetupDiEnumDeviceInterfaces(
			m_Class->GetHandle(),
			NULL,
			m_Class->GetGuid(),
			Index,
			&m_Data
			);

		if ( !status )
		{
			*Error = GetLastError();
			return;
		}

		SetupDiGetInterfaceDeviceDetail (
			m_Class->GetHandle(),
			&m_Data,
			NULL,
			0,
			&ReqLen,
			NULL
			);

		*Error = GetLastError();

		if ( *Error != ERROR_INSUFFICIENT_BUFFER )
			return;

		m_Detail = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[ReqLen]);

		if ( !m_Detail )
		{
			*Error = ERROR_NOT_ENOUGH_MEMORY;
			return;
		}

		m_Detail->cbSize = sizeof SP_INTERFACE_DEVICE_DETAIL_DATA;

		status = SetupDiGetInterfaceDeviceDetail (
			m_Class->GetHandle(),
			&m_Data,
			m_Detail,
			ReqLen,
			&ReqLen,
			NULL
			);

		if ( !status )
		{
			*Error = GetLastError();
			delete [] m_Detail;
			m_Detail = NULL;
			return;
		}

		*Error = ERROR_SUCCESS;
	}
	catch (...)
	{
	}
}

//////////////////////////////////////////////////////////////////////////////
// XDevExInterface destructor
//
DEV_EX_INTF_INLINE XDevExInterface::~XDevExInterface(void)
{
	if (m_Detail)
	{
		delete [] m_Detail;
		m_Detail = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
// XDevExInterface::DevicePath
//
DEV_EX_INTF_INLINE TCHAR* XDevExInterface::DevicePath(void)
{
	try
	{
		if ( m_Detail)
			return m_Detail->DevicePath;
		else
			return NULL;
	}
	catch (...)
	{
		return NULL;
	}

}

int GetDevExType(const GUID*pClassGuid,DWORD instance,PDWORD pError,DEV_EX_TYPE&type)
{
	XDevExInterfaceClass DevClass((GUID*)pClassGuid, pError);
	if ( *pError == ERROR_SUCCESS )
	{
		XDevExInterface DevInterface(&DevClass, instance, pError);
		CString strPath=DevInterface.DevicePath();
		strPath.MakeUpper();
		int nPos=strPath.Find("VEN_");
		if (nPos>=0)
		{
			strPath=strPath.Mid(nPos+4);
			CString strVenID=strPath.Left(4);
			nPos=strPath.Find("DEV_");
			if (nPos>=0)
			{
				strPath=strPath.Mid(nPos+4);
				CString strDevID=strPath.Left(4);
				if (strVenID.CompareNoCase(VEN_EX_ID)==0)
				{
					if (strDevID.CompareNoCase(DEV_EX_ID_AH2K)==0)
					{
						type=DT_EX_AH2K;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH2KA)==0)
					{
						type=DT_EX_AH2KA;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH2100L)==0)
					{
						type=DT_EX_AH2100L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH1100L)==0)
					{
						type=DT_EX_AH1100L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH2200L)==0)
					{
						type=DT_EX_AH2200L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH410)==0)
					{
						type=DT_EX_AH410;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH210)==0)
					{
						type=DT_EX_AH210;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH420L)==0)
					{
						type=DT_EX_AH420L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH2210L)==0)
					{
						type=DT_EX_AH2210L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH2205L)==0)
					{
						type=DT_EX_AH2205L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH4220E)==0)
					{
						type=DT_EX_AH4220E;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3110L)==0)
					{
						type=DT_EX_AH3110L;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3102SE)==0)
					{
						type=DT_EX_AH3102SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3101)==0)
					{
						type=DT_EX_AH3101;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3101A)==0)
					{
						type=DT_EX_AH3101A;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3101EA)==0)
					{
						type=DT_EX_AH3101EA;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3111)==0)
					{
						type=DT_EX_AH3111;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3111A)==0)
					{
						type=DT_EX_AH3111A;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3111E)==0)
					{
						type=DT_EX_AH3111E;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3111EA)==0)
					{
						type=DT_EX_AH3111EA;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3102SEA)==0)
					{
						type=DT_EX_AH3102SEA;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3210)==0)
					{
						type=DT_EX_AH3210;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3210A)==0)
					{
						type=DT_EX_AH3210A;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_QAVP3400)==0)
					{
						type=DT_EX_QAVP3400;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_QAVP3210)==0)
					{
						type=DT_EX_QAVP3210;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3201EA)==0)
					{
						type=DT_EX_AH3201EA;
					}
					else
					{
						type=DT_EX_INVALID;
					}

					if (type!=DT_EX_INVALID)
					{
						return 0;
					}
				}
			}
		}
	}

	return -1;
}

HANDLE DevExOpenByIF(
		const GUID* pClassGuid,		// points to the GUID that identifies the interface class
		DWORD instance,				// specifies which instance of the enumerated devices to open
		PDWORD pError				// address of variable to receive error status
		)
{
	HANDLE hDev = INVALID_HANDLE_VALUE;

	XDevExInterfaceClass DevClass((GUID*)pClassGuid, pError);

	if ( *pError == ERROR_SUCCESS )
	{
		XDevExInterface DevInterface(&DevClass, instance, pError);

		if ( *pError == ERROR_SUCCESS )
		{
			hDev = CreateFile(
				DevInterface.DevicePath(),
				GENERIC_READ | GENERIC_WRITE,
				0,//FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,//FILE_ATTRIBUTE_NORMAL,
				NULL
				);

			if ( hDev == INVALID_HANDLE_VALUE )
				*pError = GetLastError();
		}
	}

	return hDev;
}

HANDLE DevExCreateMutex(const TCHAR*cszMutexName)
{
	DWORD dwErr=::GetLastError();

	HANDLE hMutex = ::CreateMutex(NULL,TRUE,cszMutexName);
	if(::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (hMutex!=INVALID_HANDLE_VALUE)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex=INVALID_HANDLE_VALUE;
		}
	}

	return hMutex;
}

HANDLE XOpenDevEx(int nDevExID,const GUID*pGUID,HANDLE*phMutex)
{
	TCHAR szMutexName[1024];
	_stprintf(szMutexName,_T("%x-%x-%x-%x%x-%x%x%x%x%x%x_%d"),
						pGUID->Data1,
						pGUID->Data2,
						pGUID->Data3,
						pGUID->Data4[0],
						pGUID->Data4[1],
						pGUID->Data4[2],
						pGUID->Data4[3],
						pGUID->Data4[4],
						pGUID->Data4[5],
						pGUID->Data4[6],
						pGUID->Data4[7],
						nDevExID);
	*phMutex=DevExCreateMutex(szMutexName);
	if (*phMutex==INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}


	DWORD dwError=0;
	HANDLE hDevEx = DevExOpenByIF(pGUID, nDevExID, &dwError);
	if (hDevEx==INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(*phMutex);
		CloseHandle(*phMutex);
		*phMutex=INVALID_HANDLE_VALUE;
	}

	return hDevEx;
}

void XCloseDevEx(HANDLE hDevEx,HANDLE hMutex)
{
	CloseHandle(hDevEx);
	if (hMutex!=INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DM642DevEx::DM642DevEx(void)
:m_hDevEx(INVALID_HANDLE_VALUE)
,m_hMutex(INVALID_HANDLE_VALUE)
{
	for (int i = 0; i < 4; i++)
		m_pChan[i] = NULL;

	m_nCurrentIOBufIndex=0;
	m_nReadingIOBufCount=0;

	m_bEveryChannelHasAudio=false;

#ifdef __WWENCODER_H__
	m_hWWDev=NULL;
#endif

	m_nDevID=0;
}

DM642DevEx::~DM642DevEx(void)
{

}

int DM642DevEx::GetDevExCount(const GUID*pGUID)
{
	int nCount=0;
	DWORD err;

	XDevExInterfaceClass DevClass(pGUID, &err);
	XDevExInterface *pDevInterface;
	if ( err == ERROR_SUCCESS )
	{
		while (1)
		{
			pDevInterface = new XDevExInterface(&DevClass, nCount, &err);
			delete pDevInterface;

			if (err == ERROR_SUCCESS)
				nCount++;
			else
				break;
		} 
	}

	return nCount;
}

int DM642DevEx::GetDevType(int nDevID,const GUID*pGUID,DEV_EX_TYPE&type)
{
	DWORD dwError=0;
	return GetDevExType(pGUID,nDevID,&dwError,type);
}

#define BAD_ENTRYPTR	0xffffffff
typedef int (*LOAD_COFF_CALLBACK)(void *fd, unsigned long addr, size_t size, unsigned char *data);
static LOAD_COFF_CALLBACK load_to_dspmem;
enum scntyp {
	SCNTYP_LOAD,
	SCNTYP_NOLOAD,
	SCNTYP_NODATA,
	SCNTYP_ZEROSZ,
	SCNTYP_CINIT_RAM,
};

struct coff {
	unsigned char *src;
	off_t size;
	COFF_AOUTHDR *aouthdr;
	COFF_FILHDR *coffhdr;
	COFF_SCNHDR *scnhdr;
	COFF_SYMENT *symtbl;
	char *strtbl;
};
static struct coff coff;

static char *scnname(COFF_SCNHDR *hdr)
{
        static char nm[9];  

        if (hdr->s_name[0] == 0) { 
                unsigned long idx = COFF_LONG((&hdr->s_name[4]));
                return &coff.strtbl[idx];
        } else {
                strncpy(nm, hdr->s_name, 8);
                nm[8] = '\0'; 
                return nm;
        }
}

COFF_SCNHDR *find_scn_by_name(char *name)
{
	int i;
	unsigned short nscns = COFF_SHORT(coff.coffhdr->f_nscns);

	for (i = 0; i < nscns; i++) {
		if (!strcmp(scnname(&coff.scnhdr[i]), name))
			return &coff.scnhdr[i];
	}
	return NULL;
}

static enum scntyp get_scntyp(COFF_SCNHDR *hdr)
{
	unsigned long ptr;
	unsigned long flags;
	size_t size;

	ptr   = COFF_LONG(hdr->s_scnptr);
	size  = COFF_LONG(hdr->s_size);
	flags = COFF_LONG(hdr->s_flags);

	if (size == 0)
		return SCNTYP_ZEROSZ;

	if (ptr == 0) {
		if (flags & COFF_STYP_BSS)
			return SCNTYP_LOAD;
		else
			return SCNTYP_NODATA;
	}

	if (flags & (COFF_STYP_DSECT | COFF_STYP_NOLOAD | COFF_STYP_PAD))
		return SCNTYP_NOLOAD;

	if (flags & COFF_STYP_COPY) {
		if (!strcmp(scnname(hdr), ".cinit"))
			return SCNTYP_CINIT_RAM;
		else
			return SCNTYP_NOLOAD;
	}

	return SCNTYP_LOAD;
}

static int cinit_bss_init(void *fd, unsigned char *p, size_t size)
{
	unsigned int i = 0;

	while (i < size) {
		COFF_CINIT *cinit = (COFF_CINIT *)&p[i];
		/* size and addr are written in big-endian format */
		unsigned short size = COFF_SHORT_H(cinit->size) * 2; /* size in word */
		unsigned long addr = COFF_24BIT_H(cinit->addr) * 2; /* addr in word */
		unsigned char flags = *cinit->flags;
		int cnt;

		if (size == 0) {
			i+= 2;
			if (i != size) {
				fprintf(stderr,
					"warning: found zero-sized "
					"cinit entry at 0x%x\n", i-2);
			}
			continue;
		}

		if ((flags & 0x01) == COFF_CINIT_FLAG_IO) {
			fprintf(stderr,
				"We can't handle cinit data for I/O space!\n");
			return -1;
		}
		cnt = load_to_dspmem(fd, addr, size, (unsigned char *)cinit->data);
		if (cnt < size)
			return -1;
		i += COFF_CINITSZ + size;
	}

	if (i > size) {
		fprintf(stderr, "cinit data size is inconsistent.\n");
		return -1;
	}

	return 0;
}

static int load_scns(void *fd)
{
	char *msg;
	int ret = 0;
	int i;
	unsigned short nscns = COFF_SHORT(coff.coffhdr->f_nscns);

	for (i = 0; i < nscns; i++) {
		COFF_SCNHDR *hdr = &coff.scnhdr[i];
		unsigned long vaddr, ptr;
		size_t size;

		vaddr = COFF_LONG(hdr->s_vaddr);
		ptr   = COFF_LONG(hdr->s_scnptr);
		size  = COFF_LONG(hdr->s_size);

		switch (get_scntyp(hdr)) {
			case SCNTYP_CINIT_RAM:
				/* .cinit will be treated later */
				continue;
			case SCNTYP_NOLOAD:
				msg = " ... ignored.";
				break;
			case SCNTYP_ZEROSZ:
			case SCNTYP_NODATA:
				msg = "";
				break;
			case SCNTYP_LOAD:
			{
				unsigned char *data;
				int size_loaded;

				if (ptr) {
					data = &coff.src[ptr];
				} else {
					/* .bss */
					data = (unsigned char*)malloc(size);
					memset(data, 0, size);
				}

				size_loaded = load_to_dspmem(fd, vaddr, size, data);
				if (!ptr)
					free(data);
				if (size_loaded == size) {
					msg = " ... initialized.";
				} else {
					msg = " ... initialization failed!!";
					ret = -1;
				}
			}
		}
		printf("%-15s(%06lx) : adr = 0x%08lx, size=%5d %s\n",
		      scnname(hdr), ptr, vaddr, size, msg);
		if (ret < 0)
			goto finish;
	}

	/* RAM Mode .cinit handling */
	for (i = 0; i < nscns; i++) {
		COFF_SCNHDR *hdr = &coff.scnhdr[i];
		unsigned long vaddr, ptr;
		size_t size;

		if (get_scntyp(hdr) != SCNTYP_CINIT_RAM)
			continue;

		vaddr = COFF_LONG(hdr->s_vaddr);
		ptr   = COFF_LONG(hdr->s_scnptr);
		size  = COFF_LONG(hdr->s_size);

		if (cinit_bss_init(fd, &coff.src[ptr], size) < 0) {
			msg = "  ... .bss variables initialization failed!!";
			ret = -1;
		} else {
			msg = "  ... .bss variables are initialized.";
		}
		printf("%-15s : adr = 0x%08lx, size=%5d %s\n",
		       scnname(hdr), vaddr, size, msg);
		if (ret < 0)
			goto finish;
	}

finish:
	return ret;
}
/*
 * load_coff():
 * load the DSP binary into the DSP memory.
 */
unsigned long load_coff_1(void *dev, char *coffbuf, long size, LOAD_COFF_CALLBACK callback)
{
	//struct stat stat;
	unsigned short opthdrsz;
	unsigned long nsyms;
	off_t symptr;

	int ret = BAD_ENTRYPTR;

	load_to_dspmem = callback;
	if (load_to_dspmem == NULL)
		goto out;

	coff.size = size;
	if (coff.size < 0)
		goto out;

	coff.src = (unsigned char*)coffbuf;
	if ((int)coff.src == -1) {
		goto out;
	}

	coff.coffhdr = (COFF_FILHDR*)&coff.src[0];
	printf("version: %04x\n",COFF_SHORT(coff.coffhdr->f_verid));
	printf("magic: %04x\n",COFF_SHORT(coff.coffhdr->f_tgtid));
	opthdrsz = COFF_SHORT(coff.coffhdr->f_opthdr);
	nsyms = COFF_LONG(coff.coffhdr->f_nsyms);
	symptr = COFF_LONG(coff.coffhdr->f_symptr);

	/* a.out header */
	coff.aouthdr = opthdrsz ? (COFF_AOUTHDR*)&coff.src[COFF_FILHSZ] : NULL;
	/* section header */
	coff.scnhdr = (COFF_SCNHDR*)&coff.src[COFF_FILHSZ + opthdrsz];

	/* string table */
	coff.strtbl = (char *)&coff.src[symptr + COFF_SYMESZ * nsyms];

	if (!coff.aouthdr)
		goto out;

	if (load_scns(dev) < 0)
		goto out;

	ret = COFF_LONG(coff.aouthdr->entry);

out:
	return ret;
}

int LoadCallback(void *fd, unsigned long addr, size_t size, unsigned char *data)
{
	HANDLE hDevice;
	OVERLAPPED overlapped;
	ULONG error;
	PCHAR OutBuffer;
	unsigned long OutSize;

	hDevice = (HANDLE)fd;
	OutSize = size;

	if (size > 0)
	{
		OutBuffer = (PCHAR)malloc(sizeof(unsigned long) + size);
		if (OutBuffer == NULL)
			return 0;

		// set the out buffer data
		CopyMemory(OutBuffer + sizeof(unsigned long), data, size);
		*(unsigned long *)OutBuffer = addr;

		overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!DeviceIoControl(hDevice, 
			IOCTL_LOAD, 
			NULL, 
			0, 
			OutBuffer, 
			OutSize+sizeof(unsigned long), 
			NULL,
			&overlapped))
		{
			error = GetLastError();
			if (error == ERROR_IO_PENDING)
			{
				DWORD dwBytes=0;
				GetOverlappedResult(hDevice, &overlapped, &dwBytes, true);
			}
		}

		CloseHandle(overlapped.hEvent);
		free(OutBuffer);
	}

	return OutSize;
}


VOID LoadCoff(HANDLE hDevice, char *buf, ULONG size,bool b64M)
{
	OVERLAPPED	ov;
	ULONG		error = ERROR_SUCCESS;
	static ULONG EMIF_DATA[] = 
	{
		0x00052078,			// GBLCTL;
		0x73a28e01,			// CE1 Flash/FPGA;
		0xffffffd3,			// CE0 SDRAM;
		0x00000000,			// Reserved;
		0x22a28a22,			// CE2 Daughtercard 32-bit async
		0x22a28a42,			// CE3 Daughtercard 32-bit sync
		0x57115000,			// SDRAM contral, 4 banks
		0x0000081b,			// SDRAM timing
		0x001faf4d			// SDRAM extended control
	};

	if (b64M)
	{
		EMIF_DATA[0]=0x00052020;	// for 64M SDRAM
		EMIF_DATA[6]=0x53227000;	// for 64M SDRAM
	}

	do 
	{
		ZeroMemory(&ov, sizeof(OVERLAPPED));

		// Setup the event
		ov.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (ov.hEvent == NULL)
		{
			error = GetLastError();
			break;
		}

		if (!DeviceIoControl(hDevice, 
			IOCTL_WARMRESET, 
			NULL, 
			0, 
			NULL, 
			0, 
			NULL, 
			&ov))
		{
			error = GetLastError();
			if (error == ERROR_IO_PENDING)
			{
				DWORD dwBytes=0;
				GetOverlappedResult(hDevice, &ov, &dwBytes, true);
			}
			else
				break;
		}

		if (!DeviceIoControl(hDevice, 
			IOCTL_INITEMIF,  
			NULL, 
			0, 
			EMIF_DATA, 
			sizeof(EMIF_DATA), 
			NULL, 
			&ov))
		{
			error = GetLastError();
			if (error == ERROR_IO_PENDING)
			{
				DWORD dwBytes=0;
				GetOverlappedResult(hDevice, &ov, &dwBytes, true);
			}
			else
				break;
		}

		load_coff_1(hDevice, buf, size, LoadCallback);

		if (!DeviceIoControl(hDevice, 
			IOCTL_RUN, 
			NULL, 
			0, 
			NULL,
			0,
			NULL,
			&ov))
		{
			error = GetLastError();
			if (error == ERROR_IO_PENDING)
			{
				DWORD dwBytes=0;
				GetOverlappedResult(hDevice, &ov, &dwBytes, true);
			}
			else
				break;
		}

	} while (FALSE);

	// Close our overlapped event handle
	if (ov.hEvent != NULL)
	{
		CloseHandle(ov.hEvent);
	}

}

int DM642DevEx::BootLoad(int nDevID,const GUID*pGUID,PBYTE pData,DWORD dwLen,bool b64M)
{
	HANDLE hMutex=NULL;
	HANDLE hDevEx = XOpenDevEx(nDevID,pGUID,&hMutex);
	if (hDevEx==INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	LoadCoff(hDevEx,(char*)pData,dwLen,b64M);
		
	XCloseDevEx(hDevEx,hMutex);

	return 0;
}

int DM642DevEx::Open(int nDevID,const GUID*pGUID,bool bEveryChannelHasAudio)
{
	if (pGUID==NULL)
		return -1;

	m_guidDev=*pGUID;
	m_bEveryChannelHasAudio=bEveryChannelHasAudio;

	if (m_hDevEx != INVALID_HANDLE_VALUE)
		return 0; 

	m_hDevEx = XOpenDevEx(nDevID,pGUID,&m_hMutex);
	if (m_hDevEx==INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	ZeroMemory(&m_ovPostMsg, sizeof(m_ovPostMsg));
	m_ovPostMsg.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_ovPostMsg.hEvent == NULL)
	{
		return -1;
	}

	for (int i=0;i<MAX_IOBUF_COUNT;i++)
	{
		m_aIOBuf[i].Overlapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	}

	m_nBufLen=AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT;
	m_bRunning=true;


	m_nDevID=nDevID;

	OpenWWDev(NULL);

	StartThread();
	return 0;
}

void DM642DevEx::Close(int idChan)
{
	{
		XAutoLock l(m_csChan);
		m_pChan[idChan] = NULL;
		for (int i = 0; i < 4; i++)
		{
			if (m_pChan[i] != NULL)
				return;
		}
	}

	m_bRunning = false;
	WaitForStop();

	CloseWWDev(NULL);

	if (m_ovPostMsg.hEvent != NULL)
	{
		SetEvent(m_ovPostMsg.hEvent);
		CloseHandle(m_ovPostMsg.hEvent);
		m_ovPostMsg.hEvent=NULL;
	}

	if (m_hDevEx!=INVALID_HANDLE_VALUE)
	{
		XCloseDevEx(m_hDevEx,m_hMutex);
		m_hDevEx=INVALID_HANDLE_VALUE;
	}

	for (int i=0;i<MAX_IOBUF_COUNT;i++)
	{
		if (m_aIOBuf[i].Overlapped.hEvent != NULL)
		{
			CloseHandle(m_aIOBuf[i].Overlapped.hEvent);
			m_aIOBuf[i].Overlapped.hEvent=NULL;
		}

		if (m_aIOBuf[i].pBuf!=NULL)
		{
			free(m_aIOBuf[i].pBuf);
			m_aIOBuf[i].pBuf=NULL;
		}
	}
}

int DM642DevEx::OpenWWDev(XAH2K *pChan)
{
	DEV_EX_TYPE type;
	DM642DevEx::GetDevType(m_nDevID, &AH2K_DEVEX_CLASS_GUID, type);
	if (type==DT_EX_AH4220E)
	{
		int nWWDevID=0;
		for (int i=0;i<m_nDevID;i++)
		{
			DM642DevEx::GetDevType(i, &AH2K_DEVEX_CLASS_GUID, type);
			if (type==DT_EX_AH4220E)
			{
				nWWDevID++;
			}
		}
#ifdef __WWENCODER_H__
		m_hWWDev=WWOpenDev(nWWDevID);
#endif
	}

	{
		XAutoLock l(m_csChan);
		for (int i = 0; i < 4; i++)
		{
			if (m_pChan[i] != NULL && m_pChan[i]!=pChan)
			{
				m_pChan[i]->DoStartVideoStream();
			}
		}
	}

	return 0;
}

void DM642DevEx::CloseWWDev(XAH2K *pChan)
{
	{
		XAutoLock l(m_csChan);
		for (int i = 0; i < 4; i++)
		{
			if (m_pChan[i] != NULL && m_pChan[i]!=pChan)
			{
				m_pChan[i]->DoStopVideoStream();
			}
		}
	}

#ifdef __WWENCODER_H__
	if (m_hWWDev!=NULL)
	{
		WWCloseDev(m_hWWDev);
		m_hWWDev=NULL;
	}
#endif
}


int DM642DevEx::PostMsg(PBYTE pData,DWORD dwLen)
{
	if (m_hDevEx==INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	DWORD dwBytesReturned=0;
	if (!DeviceIoControl(m_hDevEx, IOCTL_POST_MSG, NULL, 0, pData, dwLen, &dwBytesReturned, &m_ovPostMsg) &&
		(GetLastError() != ERROR_IO_PENDING))
	{
		return -1;
	}

	GetOverlappedResult(m_hDevEx, &m_ovPostMsg, &dwBytesReturned, true);

	return dwBytesReturned;
}

#define _USED_SINGLE_EVENT

void DM642DevEx::ThreadProcMain(void)
{
	DWORD nRet = 0;
	int nTimeoutCount=0;
	//unsigned long nLastResetTimestamp=XGetTimestamp();

#ifdef _USED_SINGLE_EVENT
	HANDLE hEvent=NULL;
#else
	HANDLE ahEvent[MAX_IOBUF_COUNT];
#endif

	for (int i = 0; i < MAX_IOBUF_COUNT; i++)
	{
		PrepareRead(&m_aIOBuf[i]);

#ifndef _USED_SINGLE_EVENT
		ahEvent[i]=m_aIOBuf[m_nCurrentIOBufIndex].Overlapped.hEvent;
#endif
	}

	while(m_bRunning || m_nReadingIOBufCount > 0)
	{
#ifdef _USED_SINGLE_EVENT
		hEvent = m_aIOBuf[m_nCurrentIOBufIndex].Overlapped.hEvent;
		nRet = WaitForSingleObject(hEvent, 1000);
#else
		nRet=WaitForMultipleObjects(MAX_IOBUF_COUNT,ahEvent,FALSE,1000);
#endif

		switch (nRet)
		{
		case WAIT_TIMEOUT:
			{
				nTimeoutCount++;
				if (!m_bRunning)
				{
					//OutputDebugString("m_bRunning\n");
					return;
				}
			}
			break;
		case WAIT_FAILED:
			{
				//OutputDebugString("WAIT_FAILED\n");
				return;
			}
			break;
#ifdef _USED_SINGLE_EVENT
		case WAIT_OBJECT_0:
#else
		default:
			m_nCurrentIOBufIndex=nRet-WAIT_OBJECT_0;
#endif
			m_nReadingIOBufCount--;

			nTimeoutCount=0;

			if (m_bRunning)
			{
				DWORD dwBytesRead = 0;
				GetOverlappedResult(m_hDevEx, &m_aIOBuf[m_nCurrentIOBufIndex].Overlapped, &dwBytesRead, true);
				if (dwBytesRead == 0)
				{
					//OutputDebugString("dwBytesRead\n");
					break;
				}

				if (dwBytesRead>=sizeof(PCI_VIDEO_FRAME))
				{
					PPCI_VIDEO_FRAME pVidFrame = (PPCI_VIDEO_FRAME)m_aIOBuf[m_nCurrentIOBufIndex].pBuf;
					if (pVidFrame->base_frame.version == FRAME_VERSION && pVidFrame->base_frame.channel < 4)
					{
						XAutoLock l(m_csChan);
						if (pVidFrame->base_frame.frame_type==FRAME_AUDIO)
						{
							DWORD dwHeaderLen=sizeof(PCI_VIDEO_FRAME);
							int nLen = dwBytesRead-dwHeaderLen;
							PBYTE pData = m_aIOBuf[m_nCurrentIOBufIndex].pBuf + dwHeaderLen;

							if (m_bEveryChannelHasAudio)
							{
								if (m_pChan[pVidFrame->base_frame.channel] != NULL)
								{
									m_pChan[pVidFrame->base_frame.channel]->OnCapturedAudio((short*)pData,nLen>>1,16000);
								}
							}
							else
							{
								for (int nChannelIndex=0;nChannelIndex<4;nChannelIndex++)
								{
									if (m_pChan[nChannelIndex] != NULL)
									{
										m_pChan[nChannelIndex]->OnCapturedAudio((short*)pData,nLen>>1,16000);
									}
								}
							}
						}
						else
						{
							if (m_pChan[pVidFrame->base_frame.channel] != NULL)
							{
								m_pChan[pVidFrame->base_frame.channel]->ReadDataFromDsp(&m_aIOBuf[m_nCurrentIOBufIndex]);
							}
						}
					}
				}
				
				if (!PrepareRead(&m_aIOBuf[m_nCurrentIOBufIndex]))
				{
					//OutputDebugString("PrepareRead\n");
					return ;
				}

#ifdef _USED_SINGLE_EVENT
				m_nCurrentIOBufIndex++;
				if (m_nCurrentIOBufIndex >= MAX_IOBUF_COUNT)
				{
					m_nCurrentIOBufIndex = 0;
				}
#endif
			}
			break;
		}

		if (nTimeoutCount>=2)
		{
			nTimeoutCount=0;

			ResetDev();

			//char szDebug[1024]="";
			//sprintf(szDebug,"Running m_nCurrentIOBufIndex=%d m_nReadingIOBufCount=%d\n",m_nCurrentIOBufIndex,m_nReadingIOBufCount);
			//OutputDebugString(szDebug);
		}

		{
			bool bExpired=false;
			{
				XAutoLock l(m_csChan);
				for (int i = 0; i < 4; i++)
				{
					if (m_pChan[i] != NULL && m_pChan[i]->IsExpired())
					{
						bExpired=true;
					}
				}
			}
	
			//unsigned long nTimestamp=XGetTimestamp();
			//if (nTimestamp-nLastResetTimestamp>=60000)
			//{
			//	bExpired=true;
			//	nLastResetTimestamp=nTimestamp;
			//}

			if (bExpired)
			{
				XAH2K::BootLoad(m_nDevID);

				Sleep(2000);

				ResetDev();
			
				//OutputDebugString("ResetDev\n");
			}
		}
	}

	//{
	//	char szDebug[1024]="";
	//	sprintf(szDebug,"ThreadExit m_nCurrentIOBufIndex=%d m_nReadingIOBufCount=%d m_bRunning=%d\n",m_nCurrentIOBufIndex,m_nReadingIOBufCount,m_bRunning);
	//	OutputDebugString(szDebug);
	//}
}

bool DM642DevEx::PrepareRead(PIOBUF pIOBuf)
{
	if (pIOBuf->pBuf == NULL)
	{
		pIOBuf->pBuf = (PBYTE)malloc(m_nBufLen);
		if (pIOBuf->pBuf == NULL)
			return false;
	}

	if ((!ReadFile(
		m_hDevEx,
		pIOBuf->pBuf,
		m_nBufLen,
		NULL,
		&pIOBuf->Overlapped)) &&
		(GetLastError() != ERROR_IO_PENDING))
	{
		return false;
	}

	m_nReadingIOBufCount++;

	return true;
}

void DM642DevEx::SetChan(DWORD idChan, XAH2K *pChan)
{
	XAutoLock l(m_csChan);
	m_pChan[idChan] = pChan;
}

int DM642DevEx::ResetDev(void)
{
	if (m_ovPostMsg.hEvent != NULL)
	{
		SetEvent(m_ovPostMsg.hEvent);
		CloseHandle(m_ovPostMsg.hEvent);
		m_ovPostMsg.hEvent=NULL;
	}

	if (m_hDevEx!=INVALID_HANDLE_VALUE)
	{
		XCloseDevEx(m_hDevEx,m_hMutex);
		m_hDevEx=INVALID_HANDLE_VALUE;
	}

	for (int i=0;i<MAX_IOBUF_COUNT;i++)
	{
		if (m_aIOBuf[i].Overlapped.hEvent != NULL)
		{
			CloseHandle(m_aIOBuf[i].Overlapped.hEvent);
			m_aIOBuf[i].Overlapped.hEvent=NULL;
		}

		if (m_aIOBuf[i].pBuf!=NULL)
		{
			free(m_aIOBuf[i].pBuf);
			m_aIOBuf[i].pBuf=NULL;
		}
	}

	m_hDevEx = XOpenDevEx(m_nDevID,&m_guidDev,&m_hMutex);
	if (m_hDevEx==INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	ZeroMemory(&m_ovPostMsg, sizeof(m_ovPostMsg));
	m_ovPostMsg.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_ovPostMsg.hEvent == NULL)
	{
		return -1;
	}

	for (int i=0;i<MAX_IOBUF_COUNT;i++)
	{
		m_aIOBuf[i].Overlapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	}

	m_nBufLen=AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT;

	m_nCurrentIOBufIndex=0;
	m_nReadingIOBufCount=0;

	for (int i = 0; i < MAX_IOBUF_COUNT; i++)
	{
		PrepareRead(&m_aIOBuf[i]);

#ifndef _USED_SINGLE_EVENT
		ahEvent[i]=m_aIOBuf[m_nCurrentIOBufIndex].Overlapped.hEvent;
#endif
	}

	{
		XAutoLock l(m_csChan);
		for (int i = 0; i < 4; i++)
		{
			if (m_pChan[i] != NULL)
			{
				m_pChan[i]->Reset();
			}
		}
	}

	return 0;
}
