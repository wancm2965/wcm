// DM642DevEx.cpp: implementation of the DM642DevEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <setupapi.h>
#include <winioctl.h>
#include <sys/types.h>
#include "PcieDevEx.h"
#include "coff.h"
#include "AH2KDev.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

#define DEV_EX_INTF_INLINE inline
#define AH2K_MAX_WIDTH		1920
#define AH2K_MAX_HEIGHT		1080

#pragma comment(lib, "setupapi.lib")

AH2K_MALLOC AH2K_Malloc = malloc ;
AH2K_FREE	AH2K_Free=free;

void AH2K_SetMemory(AH2K_MALLOC Malloc,AH2K_FREE Free)
{
	AH2K_Malloc=Malloc;
	AH2K_Free=Free;
}


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
		{
			*status = GetLastError();
		}
		else
		{
			*status = ERROR_SUCCESS;
		}

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
	{
		SetupDiDestroyDeviceInfoList(m_hInfo);
	}

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

		NEWRECORD3( char * ,pDetailPsp , new char[ReqLen] );
		m_Detail = PSP_INTERFACE_DEVICE_DETAIL_DATA( pDetailPsp );

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
		{
			return m_Detail->DevicePath;
		}
		else
		{
			return NULL;
		}
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
					else if (strDevID.CompareNoCase(DEV_EX_ID_HAVP3201)==0)
					{
						type=DT_EX_HAVP3201;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_HAVPM3002SM)==0)
					{
						type=DT_EX_HAVP3002SM;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3002SE_T)==0)
					{
						type=DT_EX_AH3002SE_T;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3110ED_T)==0)
					{
						type=DT_EX_AH3110ED_T;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3110D_T)==0)
					{
						type=DT_EX_AH3110D_T;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3002SE)==0)
					{
						type=DT_EX_AH3002SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3210ED)==0)
					{
						type=DT_EX_AH3210ED;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3110ED)==0)
					{
						type=DT_EX_AH3110ED;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3210D)==0)
					{
						type=DT_EX_AH3210D;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH3110D)==0)
					{
						type=DT_EX_AH3110D;
					}
					else
					{
						type=DT_EX_INVALID;
					}
					
				}
				else if (strVenID.CompareNoCase(FPGA_VEN_EX_ID)==0)
				{
					if (strDevID.CompareNoCase(DEV_EX_ID_AH5112SE)==0)
					{
						type=DT_EX_AH5112SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH5011SE)==0)
					{
						type=DT_EX_AH5011SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH5111SE)==0)
					{
						type=DT_EX_AH5111SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH5102SE)==0)
					{
						type=DT_EX_AH5102SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH5002SE)==0)
					{
						type=DT_EX_AH5002SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH5211SE)==0)
					{
						type=DT_EX_AH5211SE;
					}
					else if (strDevID.CompareNoCase(DEV_EX_ID_AH5210SEB)==0)
					{
						type=DT_EX_AH5210SEB;
					}
					else
					{
						type=DT_EX_INVALID;
					}
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
		char msg[256]={0};
		sprintf(msg,"XOpenDevEx DevExOpenByIF   Failed .nDevExID:%d,Error:%d\n",nDevExID , dwError);
		DebugOutput( logcallstack ,msg);
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
PcieDevEx::PcieDevEx(void)
:m_hDevEx(INVALID_HANDLE_VALUE)
,m_hMutex(INVALID_HANDLE_VALUE)
{
	for (int i = 0; i < 4; i++)
		m_pChan[i] = NULL;

	m_nCurrentIOBufIndex=0;
	m_nReadingIOBufCount=0;

	m_bEveryChannelHasAudio=false;

	m_nDevID=0;
	m_IOCP =NULL;
}

PcieDevEx::~PcieDevEx(void)
{

}

int PcieDevEx::GetDevExCount(const GUID*pGUID)
{
	int nCount=0;
	DWORD err;

	XDevExInterfaceClass DevClass(pGUID, &err);
	XDevExInterface *pDevInterface;
	if ( err == ERROR_SUCCESS )
	{
		while (1)
		{
			NEWRECORD( pDevInterface , new XDevExInterface(&DevClass, nCount, &err)  )
			delete pDevInterface;

			if (err == ERROR_SUCCESS)
				nCount++;
			else
				break;
		} 
	}

	return nCount;
}

int PcieDevEx::GetDevType(int nDevID,const GUID*pGUID,DEV_EX_TYPE&type)
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
					data = (unsigned char*)MALLOCMSG(size);
					memset(data, 0, size);
				}

				size_loaded = load_to_dspmem(fd, vaddr, size, data);
				if (!ptr)
					FREEMSG(data);
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
		OutBuffer = (PCHAR)MALLOCMSG(sizeof(unsigned long) + size);
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
		FREEMSG(OutBuffer);
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

int PcieDevEx::BootLoad(int nDevID,const GUID*pGUID,PBYTE pData,DWORD dwLen,bool b64M)
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

int PcieDevEx::Open(int nDevID,const GUID*pGUID,bool bEveryChannelHasAudio)
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

	m_nBufLen=AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT;

	m_nDevID=nDevID;

//add ??
#ifdef USEIOCP

NEWRECORD( m_IOCP , new CIOCPModel(this,m_hDevEx) )
if(m_IOCP)
{
	//Start,Start失败时再Start
	int nCount = MAX_FAIL_COUNT ;
	while(nCount >0)
	{
		if(m_IOCP->Start() )
		{
			DebugOutput( logcallstack ,"IOCP Start OK,Dev Handle:%p \n",m_hDevEx );
			break;
		}
		nCount --;
	}

}

#else 

	for (int i=0;i<MAX_IOBUF_COUNT;i++)
	{
		m_aIOBuf[i].Overlapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	}

	m_bRunning=true;

	StartThread();

#endif

	return 0;
}

void PcieDevEx::Close(int idChan)
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

//add ??
#ifdef USEIOCP 

	if(m_IOCP)
	{
		m_IOCP->StopThread();
	}

#else 
	m_bRunning = false;
	WaitForStop();

#endif


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
			AH2K_Free(m_aIOBuf[i].pBuf);
			m_aIOBuf[i].pBuf=NULL;
		}
	}

//add ??
#ifdef USEIOCP 

	if(m_IOCP)
	{
		m_IOCP->Stop();
		delete 	m_IOCP;
		m_IOCP =NULL;
	}
#endif

}



int PcieDevEx::PostMsg(PBYTE pData,DWORD dwLen)
{
	if (m_hDevEx==INVALID_HANDLE_VALUE)
	{
		DebugOutput( logcoreexcep ,"PostMsg INVALID_HANDLE \n");
		return -1;
	}

	DWORD dwBytesReturned=0;

	if( PcieDevEx::m_deviceType == DT_EX_AH5112SE	||
		PcieDevEx::m_deviceType == DT_EX_AH5011SE	||
		PcieDevEx::m_deviceType == DT_EX_AH5111SE	||
		PcieDevEx::m_deviceType == DT_EX_AH5102SE	||
		PcieDevEx::m_deviceType == DT_EX_AH5002SE	||
		PcieDevEx::m_deviceType == DT_EX_AH5211SE	||
		PcieDevEx::m_deviceType == DT_EX_AH5210SEB) //FPGA Card
	{
		if (!DeviceIoControl(m_hDevEx, IOCTL_POST_MSG_FPGA, pData, dwLen, NULL, 0,  &dwBytesReturned, &m_ovPostMsg) &&
			(GetLastError() != ERROR_IO_PENDING))
		{
			DebugOutput( logcoreexcep ,"PostMsg DeviceIoControl \n");
			return -1;
		}
		GetOverlappedResult(m_hDevEx, &m_ovPostMsg, &dwBytesReturned, true);

	}
	else //DSP Card
	{

		if (!DeviceIoControl(m_hDevEx, IOCTL_POST_MSG, NULL, 0, pData, dwLen, &dwBytesReturned, &m_ovPostMsg) &&
			(GetLastError() != ERROR_IO_PENDING))
		{
			DebugOutput( logcoreexcep ,"PostMsg DeviceIoControl 2 \n");

			return -1;
		}
		GetOverlappedResult(m_hDevEx, &m_ovPostMsg, &dwBytesReturned, true);
	}

	return dwBytesReturned;
}

#define _USED_SINGLE_EVENT

void PcieDevEx::ThreadProcMain(void)
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
				if( m_deviceType ==  DT_EX_AH5112SE ||
					m_deviceType ==  DT_EX_AH5011SE ||
					m_deviceType ==  DT_EX_AH5111SE ||
					m_deviceType ==  DT_EX_AH5102SE ||
					m_deviceType ==  DT_EX_AH5002SE ||
					m_deviceType ==  DT_EX_AH5211SE	||
					m_deviceType ==	 DT_EX_AH5210SEB) 
				{
					DebugOutput( logcoreexcep ,"TIMEOUT PcieDevEx CanceIO\n");
					Sleep(20);
					CancelIo(m_hDevEx);
				}
				
				nTimeoutCount++;
				if (!m_bRunning)
				{
					//DebugOutput("m_bRunning\n");
					return;
				}
			}
			break;
		case WAIT_FAILED:
			{
				DebugOutput( logcoreexcep ,"WAIT_FAILED\n");
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
					DebugOutput( logcoreexcep ,"GetOverlappedResult dwBytesRead ==0\n");
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
					//DebugOutput("PrepareRead\n");
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
			DebugOutput( logcallstack ,"PcieDevEx 0 nTimeoutCount>=2 ResetDev begin\n");
			ResetDev();
			DebugOutput( logcallstack ,"PcieDevEx 0 nTimeoutCount>=2 ResetDev   end\n");

			//char szDebug[1024]="";
			//sprintf(szDebug,"Running m_nCurrentIOBufIndex=%d m_nReadingIOBufCount=%d\n",m_nCurrentIOBufIndex,m_nReadingIOBufCount);
			//DebugOutput(szDebug);
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
			DebugOutput( logcallstack ,"PcieDevEx 0 Expired  ResetDev begin\n");

				if( PcieDevEx::m_deviceType != DT_EX_AH5112SE	&&
					PcieDevEx::m_deviceType != DT_EX_AH5011SE	&&
					PcieDevEx::m_deviceType != DT_EX_AH5111SE	&&
					PcieDevEx::m_deviceType != DT_EX_AH5102SE	&&
					PcieDevEx::m_deviceType != DT_EX_AH5002SE	&&
					PcieDevEx::m_deviceType != DT_EX_AH5211SE	&&
					PcieDevEx::m_deviceType != DT_EX_AH5210SEB)
				{
					AH2KDev::BootLoad(m_nDevID);
				}

				Sleep(1000);

				ResetDev();

			DebugOutput( logcallstack ,"PcieDevEx 0 Expired  ResetDev   end\n");

				//DebugOutput("ResetDev\n");
			}
		}
	}

	//{
	//	char szDebug[1024]="";
	//	sprintf(szDebug,"ThreadExit m_nCurrentIOBufIndex=%d m_nReadingIOBufCount=%d m_bRunning=%d\n",m_nCurrentIOBufIndex,m_nReadingIOBufCount,m_bRunning);
	//	DebugOutput(szDebug);
	//}
}



bool PcieDevEx::PrepareRead(PIOBUF pIOBuf)
{
	if (pIOBuf->pBuf == NULL)
	{
		//pIOBuf->pBuf = (PBYTE)AH2K_Malloc(m_nBufLen);
		//if (pIOBuf->pBuf == NULL)
		//{
		//	DebugOutput("PcieDevEx 0 PrepareRead  Malloc   Failed return.\n");
		//	//return false;
		//	return true; //20161010 add 
		//}

		//while malloc ,//20161010 add 
		int nCount = MAX_MALLOC_COUNT ;
		while(nCount >0)
		{
			pIOBuf->pBuf  = (PBYTE)AH2K_Malloc(m_nBufLen);
			if (pIOBuf->pBuf == NULL)
			{
				DebugOutput( logcoreerror ,"PcieDevEx 0 PrepareRead  Malloc Failed. \n" );
				Sleep(10);
				nCount --;
			}
			else
			{
				break;
			}
		}

		if(pIOBuf->pBuf  == NULL)
		{
			DebugOutput( logcoreerror ,"PcieDevEx 0 PrepareRead  Malloc MAX_MALLOC_COUNT Failed return. \n" );
			return false;
		}

	}

	//if ((!ReadFile(
	//	m_hDevEx,
	//	pIOBuf->pBuf,
	//	m_nBufLen,
	//	NULL,
	//	&pIOBuf->Overlapped)) &&
	//	(GetLastError() != ERROR_IO_PENDING))
	//{
	//	DebugOutput("PcieDevEx 0 PrepareRead  ReadFile   false return.\n");
	//	return false;
	//}

	DWORD nErr = 0;
	BOOL bRet = ReadFile(
		m_hDevEx,
		pIOBuf->pBuf,
		m_nBufLen,
		NULL,
		&pIOBuf->Overlapped);

	if( !bRet)
	{
		nErr =GetLastError();
		if( nErr != ERROR_IO_PENDING )
		{
			char msg[256]={0};
			sprintf(msg,"PcieDevEx 0 PrepareRead  ReadFile   Failed .buf:%p,Error:%d\n",pIOBuf->pBuf,nErr);
			DebugOutput( logcoreerror ,msg);
			//return false;
			return true; //20161010 add 
		}
	}

	m_nReadingIOBufCount++;

	return true;
}

void PcieDevEx::SetChan(DWORD idChan, AH2KDev *pChan)
{
	{
	XAutoLock l(m_csChan);
	m_pChan[idChan] = pChan;
	}

}

int PcieDevEx::ResetDev(void)
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
			AH2K_Free(m_aIOBuf[i].pBuf);
			m_aIOBuf[i].pBuf=NULL;
		}
	}

	m_hDevEx = XOpenDevEx(m_nDevID,&m_guidDev,&m_hMutex);
	if (m_hDevEx==INVALID_HANDLE_VALUE)
	{
		DebugOutput( logcoreexcep ,"PcieDevEx XOpenDevEx Invalid.\n");
		return -1;
	}

	ZeroMemory(&m_ovPostMsg, sizeof(m_ovPostMsg));
	m_ovPostMsg.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_ovPostMsg.hEvent == NULL)
	{
		DebugOutput( logcoreexcep ,"PcieDevEx CreateEvent Invalid.\n");
		return -1;
	}

	//add ??
#ifdef USEIOCP

	if(m_IOCP)
	{
		//ReStart,ReStart失败时再ReStart
		int nCount = MAX_FAIL_COUNT ;
		while(nCount >0)
		{
			if( m_IOCP->ReStart(m_hDevEx) )
			{
				DebugOutput( logcallstack ,"IOCP ReStart OK,Dev Handle:%p \n",m_hDevEx );
				break;
			}
			nCount --;
		}
	}

#else

	for (int i=0;i<MAX_IOBUF_COUNT;i++)
	{
		m_aIOBuf[i].Overlapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	}
#endif

	m_nBufLen=AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT;

	m_nCurrentIOBufIndex=0;
	m_nReadingIOBufCount=0;

	//add ??
#ifndef USEIOCP 

	for (int i = 0; i < MAX_IOBUF_COUNT; i++)
	{
		PrepareRead(&m_aIOBuf[i]);

#ifndef _USED_SINGLE_EVENT
		ahEvent[i]=m_aIOBuf[m_nCurrentIOBufIndex].Overlapped.hEvent;
#endif
	}

#endif

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

int PcieDevEx::RegisterWriteULong(unsigned char bar,unsigned long offset,unsigned long data)
{
	PORT_ACCESS port;
	DWORD       dwBytes;

	memset(&port,0,sizeof(PORT_ACCESS));
	port.bar	 = bar;
	port.offs	 = offset;
	port.u.ldata = data;

	if(!DeviceIoControl(m_hDevEx, IOCTL_REG_WRITE, 
		&port, sizeof(PORT_ACCESS),
		NULL, 0,
		&dwBytes, NULL))
	{
		OutputDebugStringA("DeviceIoControl return error.\n");
		return -1;
	}

	return 0;
}



bool PcieDevEx::ThreadProcMainIOCP(PBYTE pDataBuf,int dwBytesRead,void* pIoContext)
{
	IOBUF ioBuf;
	ioBuf.pBuf =pDataBuf;
	PBYTE pDataBufBack =pDataBuf;

	if (dwBytesRead >=sizeof(PCI_VIDEO_FRAME))
	{
		PPCI_VIDEO_FRAME pVidFrame = (PPCI_VIDEO_FRAME)pDataBuf /*m_aIOBuf[m_nCurrentIOBufIndex].pBuf*/;
		if (pVidFrame->base_frame.version == FRAME_VERSION && pVidFrame->base_frame.channel < 4)
		{
			XAutoLock l(m_csChan); //?? 
			if (pVidFrame->base_frame.frame_type==FRAME_AUDIO)
			{
				DWORD dwHeaderLen=sizeof(PCI_VIDEO_FRAME);
				int nLen = dwBytesRead-dwHeaderLen;
				PBYTE pData = pDataBuf /*m_aIOBuf[m_nCurrentIOBufIndex].pBuf*/ + dwHeaderLen;

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
					pDataBufBack =pDataBuf;
					m_pChan[pVidFrame->base_frame.channel]->ReadDataFromDsp( &ioBuf /*&m_aIOBuf[m_nCurrentIOBufIndex]*/);
				}
			}
		}
	}

	//投递下次的读请求
	if(m_IOCP->m_bRunning)  return PrepareReadIOCP(pIoContext, &ioBuf  /*&m_aIOBuf[m_nCurrentIOBufIndex]*/,pDataBufBack);
	else return false;

}

bool PcieDevEx::PrepareReadIOCP(void* pContext ,PIOBUF pIOBuf,void * pBufBack)
{
#ifdef NOLISTRECORD  

	if (pIOBuf->pBuf == NULL)
	{
		//pIOBuf->pBuf  = (PBYTE)AH2K_Malloc(MAX_BUFFER_LEN);
		//if (pIOBuf->pBuf == NULL)
		//{
		//	DebugOutput("IOBuf  malloc失败. \n" );
		//	return false;
		//}

		//while malloc
		int nCount = MAX_MALLOC_COUNT ;
		while(nCount >0)
		{
			pIOBuf->pBuf  = (PBYTE)AH2K_Malloc(MAX_BUFFER_LEN);
			if (pIOBuf->pBuf == NULL)
			{
				DebugOutput( logcoreerror ,"IOBuf  malloc Failed. \n" );
				Sleep(10);
				nCount --;
			}
			else
			{
				break;
			}
		}

		if(pIOBuf->pBuf  == NULL)
		{
			DebugOutput( logcoreerror ,"IOBuf  malloc MAX_MALLOC_COUNT Failed. \n" );
			return false;
		}

		m_IOCP->m_pIOBuffer =(char *)pIOBuf->pBuf ;
	}

#else

	//use list record 
	if (pIOBuf->pBuf == NULL)
	{
		if(pBufBack )
		{
			m_IOCP->m_listBuffer.remove(pBufBack);
		}

		//pIOBuf->pBuf = (PBYTE)AH2K_Malloc(m_nBufLen);
		//if (pIOBuf->pBuf == NULL)
		//{
		//	DebugOutput("IOBuf  malloc Failed. \n" );
		//	return false;
		//}

		int nCount = MAX_MALLOC_COUNT ;
		while(nCount >0)
		{
			pIOBuf->pBuf  = (PBYTE)AH2K_Malloc(MAX_BUFFER_LEN);
			if (pIOBuf->pBuf == NULL)
			{
				DebugOutput( logcoreerror ,"IOBuf  malloc Failed. \n" );
				Sleep(10);
				nCount --;
			}
			else
			{
				break;
			}
		}

		if(pIOBuf->pBuf  ==NULL)
		{
			DebugOutput( logcoreerror ,"IOBuf  malloc MAX_MALLOC_COUNT Failed. \n" );
			return false;
		}

		//push list record , use last delete  
		m_IOCP->m_listBuffer.push_back(pIOBuf->pBuf);
		//DebugOutput( logcallstack ,"listBuffer push back size:%d\n",m_IOCP->m_listBuffer.size() );
		if(m_IOCP->m_listBuffer.size() > 100 ) DebugOutput( logcallstack ,"listBuffer push back size >100 \n" );
	}

#endif


	PER_IO_CONTEXT* pIoContext = (PER_IO_CONTEXT*) pContext;
	DWORD dwBytes = 0;
	pIoContext->m_szBuffer = (char *)pIOBuf->pBuf;
	pIoContext->m_OpType     = RECV_POSTED;

	// 初始化完成后,投递ReadFile请求
	BOOL bRet = ReadFile(pIoContext->m_fileHandle, pIoContext->m_szBuffer, pIoContext->m_nBuffLen, &dwBytes, &pIoContext->m_Overlapped ) ;
	if( !bRet)
	{
		int nErr = WSAGetLastError();
		// Overlapped I/O operation is in progress. #define ERROR_IO_PENDING 997L   
		if(WSA_IO_PENDING !=nErr )
		{
			DebugOutput( logcallstack ,"投递ReadFile 失败,Error:%d ！", nErr );
			//return false;
		}
	}

	return true;
}


bool PcieDevEx::PrepareReadIOCPFirst(void* pContext)
{
	bool rt =false;
	IOBUF  ioBuf;
	ioBuf.pBuf =(PBYTE) ( (PER_IO_CONTEXT*)pContext)->m_szBuffer;
	rt = PrepareReadIOCP(pContext , &ioBuf /*&m_aIOBuf[m_nCurrentIOBufIndex]*/ ,NULL);
	return rt;
}


void PcieDevEx::CanceIO(void)
{
	if( m_deviceType == DT_EX_AH5112SE ||
		m_deviceType == DT_EX_AH5011SE ||
		m_deviceType == DT_EX_AH5111SE ||
		m_deviceType == DT_EX_AH5102SE ||
		m_deviceType == DT_EX_AH5002SE ||
		m_deviceType == DT_EX_AH5211SE ||
		m_deviceType == DT_EX_AH5210SEB) 
	{
		CancelIo(m_hDevEx);
	}

	DebugOutput( logcallstack ,"PcieDevEx CanceIO\n");
}


bool PcieDevEx::ExpiredReset(int & nTimeoutCount)
{
	bool bRet =false;
	if (nTimeoutCount >=2 )
	{
		nTimeoutCount =0;

		DebugOutput( logcallstack ,"PcieDevEx Count >=2 ResetDev begin\n");
		ResetDev();
		DebugOutput( logcallstack ,"PcieDevEx Count >=2 ResetDev    end \n");
		bRet = true;
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

		if (bExpired)
		{
			if( PcieDevEx::m_deviceType != DT_EX_AH5112SE	&&
				PcieDevEx::m_deviceType != DT_EX_AH5011SE	&&
				PcieDevEx::m_deviceType != DT_EX_AH5111SE	&&
				PcieDevEx::m_deviceType != DT_EX_AH5102SE	&&
				PcieDevEx::m_deviceType != DT_EX_AH5002SE	&&
				PcieDevEx::m_deviceType != DT_EX_AH5211SE	&&
				PcieDevEx::m_deviceType != DT_EX_AH5210SEB)
			{
				AH2KDev::BootLoad(m_nDevID);
			}

			Sleep(200);
			DebugOutput( logcallstack ,"PcieDevEx Expired ResetDev begin\n");
			ResetDev();
			DebugOutput( logcallstack ,"PcieDevEx Expired ResetDev   end\n");
			bRet = true;
		}

	}

	return bRet;
}








// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define WORKER_THREADS_PER_PROCESSOR 2
   
// 传递给Worker线程的退出信号
#define EXIT_CODE           NULL    

// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}



//工作者线程,为IOCP请求服务的工作者线程 也就是每当完成端口上出现了完成数据包，就将之取出来进行处理的线程  
void CIOCPModel::ThreadProcMain(void)
{
	OVERLAPPED            *pOverlapped = NULL;
	PER_FILE_CONTEXT   *pFileContext = NULL;
	PER_IO_CONTEXT     *pIoContext =NULL;
	DWORD                    dwBytesTransfered = 0;
	BOOL                        bReturn =0;
	DWORD                    dwErr =0;
	int                             count =0;
	int                             nTimeoutCount=0;

	DebugOutput( logcallstack ,"Work Thread Start.IOCP:%p \n",m_hIOCompletionPort);

	while (m_bRunning)
	{
		bReturn = GetQueuedCompletionStatus(
			m_hIOCompletionPort,
			&dwBytesTransfered,
			(PULONG_PTR)&pFileContext,
			&pOverlapped,
			2000);  //INFINITE);

		// 判断是否出现了错误
		if( !bReturn )
		{

			dwErr = GetLastError();
			if(WAIT_TIMEOUT == dwErr) 
			{
				if (m_bRunning)
				{
					m_pMain->CanceIO();
					nTimeoutCount++;
					DebugOutput( logcoreexcep ,"wait timeout ,try ...\n");
				}
				else
				{
					DebugOutput( logcoreexcep ,"IOCP Thread Not Running \n");
					break;
				}
			}
			else if( ERROR_NETNAME_DELETED==dwErr )
			{
				DebugOutput( logcoreexcep ,"An exception was detected.\n");
			}
			else
			{
				DebugOutput( logcoreexcep , "IOCP Get Error ,code: %d \n",dwErr );
				//break;  //notice 此不再设置退出,出现图像不动时,开工具看此日志
			}

		}  
		else //if1 
		{
			if ( EXIT_CODE==pFileContext && EXIT_CODE == pOverlapped )
			{
				//如果收到的是退出标志，则直接退出
				DebugOutput( logcallstack ,"Received IOCP Exit Message.\n");
				break;
			}

			//读取传入的参数
			if(pOverlapped) //if2 
			{
				pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);  
				if( pIoContext) //if3 
				{
					switch( pIoContext->m_OpType )
					{
					//Read
					case RECV_POSTED:
						{
							//用专门的ThreadProcMainIOCP函数进行处理请求及投递下次请求
							if(m_bRunning && m_pMain)
							{
								if (dwBytesTransfered <= 0)  DebugOutput( logcoreexcep ,"GetQueuedCompletionStatus Read Bytes <=0 ,Read Len:%d  \n",dwBytesTransfered);
								if(m_pMain->ThreadProcMainIOCP((PBYTE)pIoContext->m_szBuffer,dwBytesTransfered,pIoContext) ==false)
								{
									DebugOutput( logcoreexcep ,"IOCP Read Work Thread Exit.\n");
									return;
								}
							}
							break;
						}
					default:
						DebugOutput( logcoreexcep ,("Work Thread m_OpType Exception.\n"));
						break;
					} //switch
				}//if3 
			}//if2 
		}//if1 

		//监测超时重置
		if (m_bRunning) m_pMain->ExpiredReset(nTimeoutCount);

	}//while

	DebugOutput( logcoreexcep ,"Work Thread Exit.\n");
	return ;
}


CIOCPModel::CIOCPModel(PcieDevEx * p,HANDLE pFile):
m_nThreads(1),
m_hShutdownEvent(NULL),
m_hIOCompletionPort(NULL),
m_pMain(p),
m_fileReadHandle(pFile),
m_fileReadContext( NULL )
{
	m_pIOBuffer =NULL;
	m_bCallerClose =false;
	if(pFile) m_bCallerClose =true;
}


CIOCPModel::~CIOCPModel(void)
{
	// 确保资源彻底释放
	this->Stop();
}


//	启动服务
bool CIOCPModel::Start()
{
	// 建立系统退出的事件通知
	//m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化IOCP
	if (false == _InitializeIOCP())
	{
		return false;
	}
	else
	{
		DebugOutput( logcallstack ,"IOCP initialization is complete,IOCP:%p \n",m_hIOCompletionPort);
	}

	//启动线程
	m_bRunning=true;
	StartThread();

	// 初始化File Read,绑定至IOCP 
	if( false==_InitializeFileRead() )
	{
		this->_DeInitialize();
		return false;
	}
	else
	{
		DebugOutput( logcallstack ,"Bind the device to IOCP successfully.\n");
	}


	//根据本机中的处理器数量，建立对应的线程数
	//m_nThreads = WORKER_THREADS_PER_PROCESSOR * _GetNoOfProcessors();
	DebugOutput( logcallstack ,("IOCP  starts successfully !\n"));
	return true;
}

//退出线程资源
void CIOCPModel::StopThread()
{
	if( m_hIOCompletionPort && m_bRunning )
	{
		DebugOutput( logcoreexcep ,"Prepare to exit the IOCP thread.\n");

		// 激活关闭消息通知
		//SetEvent(m_hShutdownEvent);

		// 通知所有的完成端口操作退出
		DebugOutput( logcoreexcep ,"Notifies IOCP to exit.\n");
		PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);

		m_bRunning = false;
		WaitForStop();

		// 等待所有的资源退出
		//WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

		DebugOutput( logcoreexcep ,"The IOCP thread resource has been exited.\n");
	}	
}

//退出完成端口
void CIOCPModel::Stop()
{
	if( m_fileReadContext!=NULL && m_fileReadContext->m_fileReadHandle!=NULL )
	{
		// 释放其他资源
		this->_DeInitialize();
		DebugOutput( logcallstack ,"The completion port has been exited.\n");
	}

	ClearBuffer();

	DebugOutput( logcallstack ,"The release of resources is completed.\n");
}

//重新启动IOCP,但线程不停
bool CIOCPModel::ReStart(HANDLE pHandle)
{
	//stop
	Stop();
	Sleep(100);

	//start
	if(pHandle)
	{
		m_fileReadHandle=pHandle;
		m_bCallerClose =true;
	}

	//初始化IOCP
	if (false == _InitializeIOCP())
	{
		return false;
	}
	else
	{
		DebugOutput( logcallstack ,"IOCP initialization is complete,IOCP:%p \n",m_hIOCompletionPort);
	}

	// 初始化File Read,绑定至IOCP 
	if( false==_InitializeFileRead() )
	{
		this->_DeInitialize();
		return false;
	}
	else
	{
		DebugOutput( logcallstack ,"Bind the device to IOCP successfully.\n");
	}

	DebugOutput( logcallstack ,"IOCP  starts successfully !\n");
	return true;
}

// 初始化完成端口
bool CIOCPModel::_InitializeIOCP()
{
	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_nThreads ); //notice , 0-->1
	if ( NULL == m_hIOCompletionPort)
	{
		DebugOutput( logcoreerror ,("Failed to complete port establishment, error code: %d!\n"), WSAGetLastError());
		return false;
	}

	return true;
}

// 初始化 绑定至完成端口中  first post 
bool CIOCPModel::_InitializeFileRead()
{
	NEWRECORD( m_fileReadContext , new PER_FILE_CONTEXT )
	if(m_fileReadContext ==NULL)
	{
		DebugOutput( logcoreerror , ("new fileReadContext Failed.\n") );
		return false;
	}

	NEWRECORD( m_fileReadContext->m_pNewIoContext , new _PER_IO_CONTEXT )
	if(m_fileReadContext->m_pNewIoContext ==NULL)
	{
		DebugOutput( logcoreerror , ("new IoContext  Failed.\n") );
		return false;
	}

	if(m_fileReadHandle) m_fileReadContext->m_fileReadHandle =m_fileReadHandle;
	else
	{
		DebugOutput( logcoreerror , ("Device Handle NULL.\n") );
		return false;
	}

	if(m_fileReadContext->m_fileReadHandle == INVALID_HANDLE_VALUE)  
	{
		DebugOutput( logcoreerror ,"Device Handle invalid.\n");
		//if(!m_bCallerClose) CloseHandle(m_fileReadContext->m_fileReadHandle); //notice,caller close 
		return false;  
	}

	// 绑定至完成端口中
	if( NULL== CreateIoCompletionPort( (HANDLE)m_fileReadContext->m_fileReadHandle, m_hIOCompletionPort,(DWORD)m_fileReadContext, m_nThreads ))  
	{  
		DebugOutput( logcoreerror ,"Failed to bind device to completion port, error code: %d \n", WSAGetLastError());  
		if(!m_bCallerClose)  RELEASE_HANDLE( m_fileReadContext->m_fileReadHandle ); //notice,caller close 
		return false;
	}
	else
	{
		DebugOutput( logcallstack ,"bind device to completion port successfully,Dev Handle:%p, IOCP:%p \n",m_fileReadHandle,m_hIOCompletionPort);
	}

	//first post,投递失败时再投
	int nCount = MAX_FAIL_COUNT ;
	while(nCount >0)
	{
		if( _PostRead( m_fileReadContext->m_pNewIoContext) ) break;
		nCount --;
	}

	return true;
}

//	最后释放掉所有资源
void CIOCPModel::_DeInitialize()
{
	// 关闭系统退出事件句柄
	//RELEASE_HANDLE(m_hShutdownEvent);

	if(!m_bCallerClose) RELEASE_HANDLE(m_fileReadContext->m_fileReadHandle); //notice,caller close 
	RELEASE(m_fileReadContext->m_pNewIoContext);
	RELEASE(m_fileReadContext);
	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);

}

//初始化后第一次投递数据请求 
bool CIOCPModel::_PostRead( PER_IO_CONTEXT* pIoContext )
{
	if(m_pMain) 
	{
			pIoContext->m_fileHandle = m_fileReadContext->m_fileReadHandle;
			pIoContext->m_OpType    = RECV_POSTED;

#ifdef NOLISTRECORD  

			if(m_pIOBuffer ==NULL)
			{
				m_pIOBuffer = (char *)AH2K_Malloc(MAX_BUFFER_LEN);
			}

			if (m_pIOBuffer == NULL)
			{
				DebugOutput( logcoreerror ,"pIOBuffer  malloc Failed.\n" );
				return false;
			}

			pIoContext->m_szBuffer    = m_pIOBuffer;
			return m_pMain->PrepareReadIOCPFirst(pIoContext );

#else

			pIoContext->m_szBuffer    = NULL;
			for(int i=0;i<MAX_POST_RECV;i++)
			{
				return m_pMain->PrepareReadIOCPFirst(pIoContext );
			}

#endif

	}
	return true;
}

// 将句柄绑定到完成端口中
bool CIOCPModel::_AssociateWithIOCP( PER_FILE_CONTEXT *pContext )
{
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pContext->m_fileReadHandle, m_hIOCompletionPort, (DWORD)pContext, m_nThreads);
	if (NULL == hTemp)
	{
		DebugOutput( logcoreerror ,("Execute CreateIoCompletionPort with an error: %d\n"),GetLastError());
		return false;
	}
	return true;
}

// 获得本机中处理器的数量
int CIOCPModel::_GetNoOfProcessors()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}


void  CIOCPModel::ClearBuffer(void)
{
	//XAutoLock lock(m_csListBuffer);
	while (m_listBuffer.size())
	{
		PBYTE pBuf =(PBYTE)m_listBuffer.front();
		m_listBuffer.pop_front();
		if(pBuf)
		{
			AH2K_Free(pBuf);
			pBuf=NULL;
		}
	}

	if(m_pIOBuffer)
	{
		AH2K_Free(m_pIOBuffer);
		m_pIOBuffer =NULL;
	}

}
