// MTAuthorize.cpp : ���� DLL Ӧ�ó���ĵ���������
//


#include "stdafx.h"
#include "MTAuthorize.h"
#include <WinReg.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cfgmgr32.h>
#include <SetupAPI.h>
#include <Shlwapi.h>
#include <string.h>
#include <tchar.h>
#include <atlstr.h>


BOOL RegistedGUIDExist()
{
	HKEY hAppKey;
	LPCTSTR WINDS_SERVICE_REGISTRY_KEY= _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{745A17A0-74D3-11D0-B6FE-00A0C90F57DA}");
	BOOL bExist = FALSE;
	if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, WINDS_SERVICE_REGISTRY_KEY, 0, KEY_READ, &hAppKey))
	{
		bExist = TRUE;
	}
	return bExist;
}


CString GetDeviceName(DEVNODE DevNode, HMACHINE hMachine)
{
	//CString	strType;
	CString	strValue;
	CString DisplayName;
	LPTSTR	Buffer;

	int  BufferSize = MAX_PATH + MAX_DEVICE_ID_LEN;
	ULONG  BufferLen = BufferSize * sizeof(TCHAR);

	Buffer  = strValue.GetBuffer(BufferSize);
	if (CR_SUCCESS == CM_Get_DevNode_Registry_Property_Ex(DevNode,
		CM_DRP_FRIENDLYNAME, NULL,
		Buffer, &BufferLen, 0, hMachine))
	{
		DisplayName = Buffer;
	}
	else
	{
		BufferLen = BufferSize * sizeof(TCHAR);

		if (CR_SUCCESS == CM_Get_DevNode_Registry_Property_Ex(DevNode,
			CM_DRP_DEVICEDESC, NULL,
			Buffer, &BufferLen, 0, hMachine))
		{
			DisplayName = Buffer;
		}
		else
		{
			DisplayName=_T("Unknown Device");
		}
	}
	strValue.ReleaseBuffer();
	return DisplayName;
}

BOOL RetrieveSubNodes(DEVINST parent, DEVINST sibling, DEVNODE dn, HMACHINE hMachine)
{
	DEVNODE dnSibling, dnChild;
	BOOL bFound = FALSE;
	do
	{
		CONFIGRET cr = CM_Get_Sibling_Ex(&dnSibling, dn, 0, hMachine);

		if (cr != CR_SUCCESS)
			dnSibling = NULL;

		TCHAR GuidString[MAX_GUID_STRING_LEN];
		ULONG Size = sizeof(GuidString);

		cr = CM_Get_DevNode_Registry_Property_Ex(dn, CM_DRP_CLASSGUID, NULL,
			GuidString, &Size, 0,  hMachine);
		if (cr == CR_SUCCESS)
		{
			if (StrStrI(GetDeviceName(dn, hMachine),_T("USB TOUCHSCREEN")))
			{
				bFound = TRUE;
				break;
			}
		}

		cr = CM_Get_Child_Ex(&dnChild, dn, 0, hMachine);
		if (cr == CR_SUCCESS)
		{
			bFound = RetrieveSubNodes(dn, NULL, dnChild, hMachine);
		}

		dn = dnSibling;

	} while (dn != NULL && !bFound);

	return bFound;
}
/*--------------------------------------------------------------------------------
 ****************************�Ƿ������Ű��Ĵ�����**********************************
 --------------------------------------------------------------------------------*/
BOOL IsDeviceByCIST(void)
{
	//����Ҫ�Ƿ������ͻ��Ƕ�ȥ����ȥ��������Return TRUE�������·��ļ����Ҫͬ���޸ġ�
	//return TRUE;

	BOOL bOK = FALSE;
	bOK = RegistedGUIDExist();
	if (bOK)
	{
		HMACHINE hMachine;
		TCHAR LocalComputer[MAX_PATH];
		DWORD Size = MAX_PATH - 2;

		GetComputerName(LocalComputer + 2, &Size);
		LocalComputer[0] = '\\';
		LocalComputer[1] = '\\';
		CONFIGRET cr;
		cr = CM_Connect_Machine(LocalComputer, &hMachine);

		DEVNODE dnRoot;
		CM_Locate_DevNode_Ex(&dnRoot, NULL, 0, hMachine);

		DEVNODE dnFirst;
		CM_Get_Child_Ex(&dnFirst, dnRoot, 0, hMachine);

		BOOL bFound = FALSE;

		bOK = RetrieveSubNodes(dnRoot, NULL, dnFirst, hMachine);

		CM_Disconnect_Machine(hMachine);
	}
	return bOK;
}

/*--------------------------------------------------------------------------------
 ****************************�ж��Ƿ�����֧�ֵĴ�����******************************
  ****************************����Ϊ�豸ID�Ͳ�ƷID******************************
 --------------------------------------------------------------------------------*/
BOOL IsSupportedDeviceByPIDVID(const CHAR VID[], const CHAR PID[])
{
	BOOL bReturnValue = FALSE;

	// ��ȡ��ǰϵͳ����ʹ�õ��豸
	DWORD dwFlag = (DIGCF_ALLCLASSES | DIGCF_PRESENT);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, dwFlag);
	if( INVALID_HANDLE_VALUE == hDevInfo )
	{
		OutputDebugString( _T("��ȡϵͳ�豸�б�ʧ��") );
		return FALSE;
	}

	// ׼�����������豸����USB
	SP_DEVINFO_DATA sDevInfoData;
	sDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	TCHAR szDIS[MAX_PATH]; // Device Identification Strings, 
	DWORD nSize = 0 ;

	for(int i = 0; SetupDiEnumDeviceInfo(hDevInfo,i,&sDevInfoData); i++ )
	{
		nSize = 0;
		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &sDevInfoData, szDIS, sizeof(szDIS), &nSize) )
		{
			OutputDebugString( _T("��ȡ�豸ʶ���ַ���ʧ��\n") );
			break;
		}

		// �豸ʶ�𴮵�ǰ�����ַ��Ƿ���"USB", ģ�壺 USB\VID_XXXX&PID_XXXX\00000xxxxxxx
		CString strDIS( szDIS );
		strDIS.MakeUpper();
		if( strDIS.Left( 3 ) == _T("USB") )
		{
			int nVID_Pos = strDIS.Find(ToDRAWStr(VID).c_str());
			int nPID_Pos = strDIS.Find(ToDRAWStr(PID).c_str());

			if(nVID_Pos == 8 && nPID_Pos == 17)
			{
				bReturnValue = TRUE;
				break;
			}
		}
	}

	// �ͷ��豸
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return bReturnValue;
}

//�����豸��GUID�����豸��Ϣ
BOOL GetDevFromGUID(GUID  &guid)
{
	BOOL bReturn = FALSE;
	HDEVINFO hDevInfo   =   SetupDiGetClassDevs(&guid,  
		NULL, NULL, DIGCF_PRESENT|DIGCF_PROFILE);
	if( INVALID_HANDLE_VALUE != hDevInfo)
	{
		bReturn = TRUE;
	}
	return bReturn;
}

BOOL IsSupportedDeviceByTLink()
{
	BOOL bReturn = FALSE;
	GUID classguid = {0x378de44c,0x56ef, 0x11d1, 0xbc,0x8c, 0x00,0xa0, 0xc9, 0x14, 0x05, 0xdd};
	HDEVINFO hDevInfo   =   SetupDiGetClassDevs(NULL,  
		NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if( INVALID_HANDLE_VALUE == hDevInfo)
	{
		return bReturn;
	}

	// ׼�����������豸����USB
	SP_DEVINFO_DATA sDevInfoData;
	sDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	TCHAR szDIS[MAX_PATH]; // Device Identification Strings, 
	DWORD nSize = 0 ;

	for(int i = 0; SetupDiEnumDeviceInfo(hDevInfo,i,&sDevInfoData); i++ )
	{
		nSize = 0;
		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &sDevInfoData, szDIS, sizeof(szDIS), &nSize) )
		{
			OutputDebugString( _T("��ȡ�豸ʶ���ַ���ʧ��\n") );
			break;
		}

		CString strDIS( szDIS );
		strDIS.MakeUpper();
		if( strDIS.Find(_T("TLINKMTOUCH")) > -1 )
		{
			bReturn = TRUE;
			break;
		}
	}

	// �ͷ��豸
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bReturn;
}

/*--------------------------------------------------------------------------------
*****************************��������֮һ****************************************
 ****************************������֤����֤��ȫ�ԣ��۲��ã�************************
 --------------------------------------------------------------------------------*/
long AuthorizeToBeMyData(unsigned int nData1, unsigned int nData2)
{
	long ldata1 = nData1 % 323 + 1;
	long ldata2 = nData2 % 323 + 1;
	long nDataTemp = ldata2*ldata2 + ldata1*ldata1 - 2*(long)(sqrtl(nData1*nData2)) ;
	return  nDataTemp;
}


/*--------------------------------------------------------------------------------
*****************************��������֮һ****************************************
 ****************************�Ƿ���������֧�ֵĴ�����************************
 --------------------------------------------------------------------------------*/
BOOL MTAUTHORIZE::IsSupportedDevice()
{
	BOOL bReturnValue = FALSE;

	//�ж��Ƿ������Ű��Ĵ���Ӳ��
	if (!bReturnValue)
	{
		bReturnValue = IsDeviceByCIST();
	}

	//�ж��Ƿ���˫��Ĵ�����Ӳ��
	if (!bReturnValue)
	{
		//Ψ��������VIDΪ0x202E��PIDΪ0x0006
		//CString VID = _T("202E");
		//CString PID = _T("0006");
		std::string VID = "202E";
		std::string PID = "0006";
		bReturnValue = IsSupportedDeviceByPIDVID(VID.c_str(), PID.c_str());
	}
	//Timelink
	if (!bReturnValue)
	{
		bReturnValue = IsSupportedDeviceByTLink();
	}

	//�����Ƿ�֧�ֱ�ʶ
	//����ʱֱ�ӷ���TRUE ,��������򷵻�bReturnValue
	return bReturnValue;
	return TRUE;
}