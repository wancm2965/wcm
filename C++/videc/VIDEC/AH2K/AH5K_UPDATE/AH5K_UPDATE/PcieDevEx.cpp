#include "stdafx.h"
#include "PcieDevEx.h"

#include "stdafx.h"
#include <setupapi.h>
#include <winioctl.h>
#include <sys/types.h>
#include "PcieDevEx.h"
#include <string.h>

#define DEV_EX_INTF_INLINE inline
#define FPGA_CONFIG_BLOCK_SIZE     256

static GUID g_guidAH2K = AH2KEX_DEVEX_CLASS_GUID;

int g_nUpdateCount = 0;


#pragma comment(lib, "setupapi.lib")


/****If have any new wersion, please update this Macro like this formate: 0x13     05     31     00***************/
/*											                                |       |     |      |            ****/
/****                                                                      year   month  date version number  ****/  

#define FPGA_FIRM_LATEST_VERSION    0x14010300



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
				if (strVenID.CompareNoCase(FPGA_VEN_EX_ID)==0)
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
			OutputDebugStringA("CreateFile");
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
PcieDevEx::PcieDevEx(void)
:m_hDevEx(INVALID_HANDLE_VALUE)
,m_hMutex(INVALID_HANDLE_VALUE)
{
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
			pDevInterface = new XDevExInterface(&DevClass, nCount, &err);
			delete pDevInterface;

			if (err == ERROR_SUCCESS)
				nCount++;
			else
				break;
		} 
	}

	char DebugString[1024]= "";
	sprintf(DebugString,"nCount is %d\n",nCount);
	OutputDebugStringA(DebugString);
	return nCount;
}

int PcieDevEx::GetDevType(int nDevID,const GUID*pGUID,DEV_EX_TYPE&type)
{
	DWORD dwError=0;
	return GetDevExType(pGUID,nDevID,&dwError,type);
}



/********************************************************************/
/*			    Write register 32bits								*/
/********************************************************************/
int FpgaRegWrite( HANDLE hHandle, unsigned char bar, unsigned long offset, unsigned long data )
{
	PORT_ACCESS port;
	DWORD       dwBytes;
	int         status = RTN_OK;

	if( (hHandle == NULL) || (hHandle == INVALID_HANDLE_VALUE) || (bar >= MAX_PCI_BAR) )
	{
		return (RTN_PRM_ERR);
	}

	memset(&port,0,sizeof(PORT_ACCESS));
	port.bar	 = bar;
	port.offs	 = offset;
	port.u.ldata = data;

	if(!DeviceIoControl(hHandle, IOCTL_REG_WRITE, 
		&port, sizeof(PORT_ACCESS),
		NULL, 0,
		&dwBytes, NULL))
	{
		status = RTN_ERR;
	}

	return (status);
}

/********************************************************************/
/*			    Read register 32bits								*/
/********************************************************************/
int FpgaRegRead( HANDLE hHandle, unsigned char bar, unsigned long offset, unsigned long *data )
{
	PORT_ACCESS port;
	DWORD       dwBytes;
	int         status = RTN_OK;

	if( (hHandle == NULL) || (hHandle == INVALID_HANDLE_VALUE) || (bar >= MAX_PCI_BAR) )
	{
		return (RTN_PRM_ERR);
	}

	memset(&port,0,sizeof(PORT_ACCESS));
	port.bar	 = bar;
	port.offs	 = offset;

	if(!DeviceIoControl(hHandle, IOCTL_REG_READ, 
		&port, sizeof(PORT_ACCESS),
		&port, sizeof(PORT_ACCESS),
		&dwBytes, NULL))
	{
		*data = 0x1111;
		status = RTN_ERR;
	}
	*data = port.u.ldata;

	return (status);
}


int PcieDevEx::FpgaConfig(int nDevID,const GUID*pGUID,PBYTE pData,DWORD dwLen)
{
	HANDLE hMutex = NULL;

	int SizeCount;
	ULONG ControlValue; 
	int TimeoutCount = 0;
	int FlashAdd = 0;
	int LeftSize = dwLen;
	int RealLeftSize = 0;
	ULONG CurrentVersionData; 
	bool DataFirstFlag = true;

	char DebugString[1024];
	int SendSize = 0;


	//get current fpga firm version
	GetFpgaFirmVersion(nDevID,pGUID, &CurrentVersionData);

	if(CurrentVersionData >= FPGA_FIRM_LATEST_VERSION)
	{
		OutputDebugStringA("Current FPGA_firm is latest version, no need to update!\n");
	}
	else
	{
		sprintf(DebugString,"Fpag config start! firm size: %u\n",dwLen);
		OutputDebugStringA(DebugString);

		SizeCount = dwLen/FPGA_CONFIG_BLOCK_SIZE;
		ULONG *TempBuffer = (ULONG*)malloc(dwLen + 4);
		//CHAR *TempBuffer = (CHAR*)malloc(dwLen + 4);
		ULONG *TempBuffer_Old = TempBuffer;
		memcpy(TempBuffer, pData, dwLen);

		HANDLE hHandle = XOpenDevEx(nDevID,pGUID,&hMutex);
		if (hHandle==INVALID_HANDLE_VALUE)
		{
			return RTN_ERR; 
		}

		for(int i=0; i < SizeCount; i++)
		{
			//wait until the Flash is in idle
			do {
				FpgaRegRead(hHandle, BAR_FPGA_REG, FPGA_FLASH_CTL, &ControlValue);
				TimeoutCount ++;
				if(TimeoutCount >= 18000) { 
					OutputDebugStringA("Fpga config Timeout!\n");
					return RTN_TIME_OUT;
				}
				Sleep(1);
			} while((ControlValue & FLASH_STATUS) != FLASH_IDLE);
			TimeoutCount = 0;

			for(int j = 0; j < FPGA_CONFIG_BLOCK_SIZE/4; j++) 
			{
				FpgaRegWrite(hHandle, BAR_FPGA_REG, FPGA_FLASH_DATA, *TempBuffer++);
				if(DataFirstFlag)
				{
					//Flash need to erase at the first time, please wait
					for(int i = 0; i < 30; i++)
					{
						Sleep(2000);
						OutputDebugStringA("Flash erasing.....\n");
					}
					DataFirstFlag  = false;
				}

			}
			FpgaRegWrite(hHandle, BAR_FPGA_REG, FPGA_FLASH_LEN, FPGA_CONFIG_BLOCK_SIZE);
			FpgaRegWrite(hHandle, BAR_FPGA_REG, FPGA_FLASH_CTL, FLASH_TRIG_BIT);

			SendSize += FPGA_CONFIG_BLOCK_SIZE;
			sprintf(DebugString,"%d Bytes send!\n", SendSize);
			OutputDebugStringA(DebugString);

		}

		//the last block data of FirmWare
		if(dwLen%FPGA_CONFIG_BLOCK_SIZE)
		{
			//wait until the Flash is in idle
			do {
				FpgaRegRead(hHandle, BAR_FPGA_REG, FPGA_FLASH_CTL, &ControlValue);
				TimeoutCount ++;
				if(TimeoutCount >= 18000) { 
					OutputDebugStringA("Fpga config Timeout!\n");
					return RTN_TIME_OUT;
				}
				Sleep(1);
			} while((ControlValue & FLASH_STATUS) != FLASH_IDLE);

			TimeoutCount = 0;
			int RealLeftSize = dwLen - FPGA_CONFIG_BLOCK_SIZE*SizeCount;

			SizeCount = (RealLeftSize%4) ? (RealLeftSize/4 + 1) : (RealLeftSize/4);

			for(int i = 0; i < SizeCount; i++)
			{
				FpgaRegWrite(hHandle, BAR_FPGA_REG, FPGA_FLASH_DATA, *TempBuffer++);
			}
			FpgaRegWrite(hHandle, BAR_FPGA_REG, FPGA_FLASH_LEN, RealLeftSize);
			FpgaRegWrite(hHandle, BAR_FPGA_REG, FPGA_FLASH_CTL, FLASH_LAST_FLAG|FLASH_TRIG_BIT);

			SendSize += RealLeftSize;
			sprintf(DebugString,"%d Bytes send!\n", SendSize);
			OutputDebugStringA(DebugString);
		}
		free(TempBuffer_Old);
		OutputDebugStringA("Fpga Firm updated!\n");
	}

	g_nUpdateCount++;
	return (RTN_OK);

}


int PcieDevEx::GetFpgaFirmVersion(int nDevID,const GUID*pGUID,ULONG*VersionData) 
{
	HANDLE hMutex = NULL;
	int status = RTN_OK;

	HANDLE hDevEx = XOpenDevEx(nDevID,pGUID,&hMutex);
	if (hDevEx==INVALID_HANDLE_VALUE)
	{
		return RTN_ERR; 
	}

	//Read the Version Register
	if(FpgaRegRead(hDevEx, BAR_FPGA_REG, FPGA_FIRM_VERSION, VersionData) != RTN_OK) 
	{
		return RTN_ERR; 
	};

	XCloseDevEx(hDevEx,hMutex);

	return RTN_OK;

}


#if 0  //for debug
int PcieDevEx::Debug_fpga_Reg_Read(int nDevID,const GUID*pGUID, unsigned char bar, unsigned long offset, unsigned long *data )
{
	HANDLE hMutex = NULL;
	int status = RTN_OK;

	HANDLE hDevEx = XOpenDevEx(nDevID,pGUID,&hMutex);
	if (hDevEx==INVALID_HANDLE_VALUE)
	{
		return RTN_ERR; 
	}

	//Read the Version Register
	if(FpgaRegRead(hDevEx, BAR_FPGA_REG, offset, data) != RTN_OK) 
	{  
		return RTN_ERR; 
	};

	XCloseDevEx(hDevEx,hMutex);

	return RTN_OK;
}



int PcieDevEx::Debug_fpga_Reg_Write(int nDevID,const GUID*pGUID, unsigned char bar, unsigned long offset, unsigned long *data )
{
	HANDLE hMutex = NULL;
	int status = RTN_OK;

	HANDLE hDevEx = XOpenDevEx(nDevID,pGUID,&hMutex);
	if (hDevEx==INVALID_HANDLE_VALUE)
	{
		return RTN_ERR; 
	}

	//Read the Version Register
	if(FpgaRegWrite(hDevEx, BAR_FPGA_REG, offset, *data) != RTN_OK) 
	{
		return RTN_ERR; 
	};

	XCloseDevEx(hDevEx,hMutex);

	return RTN_OK;
}
#endif


void PcieDevEx::BootLoad(int nDevID,PBYTE pData,DWORD dwLen,DEV_EX_TYPE nDevType)
{
	if (nDevType == DT_EX_AH5112SE ||
		nDevType == DT_EX_AH5011SE ||
		nDevType == DT_EX_AH5111SE ||
		nDevType == DT_EX_AH5102SE ||
		nDevType == DT_EX_AH5002SE ||
		nDevType == DT_EX_AH5211SE)
	{
		FpgaConfig(nDevID,&g_guidAH2K,pData,dwLen);
	}
}


int GetDevCount()
{
	int nDevCount = PcieDevEx::GetDevExCount(&AH2KEX_DEVEX_CLASS_GUID);
	if (nDevCount <= 0)
	{
		nDevCount = PcieDevEx::GetDevExCount(&AH2K_DEVEX_CLASS_GUID);
		if (nDevCount <= 0)
		{
			return -1;
		}
		else
		{
			g_guidAH2K=AH2K_DEVEX_CLASS_GUID;
		}
	}
	else
	{
		g_guidAH2K=AH2KEX_DEVEX_CLASS_GUID;
	}


	return nDevCount;
}

void GetDevType(int nDevID,DEV_EX_TYPE &device_type)
{
	PcieDevEx::GetDevType(nDevID,&g_guidAH2K,device_type);
}




