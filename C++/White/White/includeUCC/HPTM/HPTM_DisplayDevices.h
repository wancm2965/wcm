//========================================================================
//类名：显示设备信息
//功能：获取显示设备信息（数目、分辨率……）
//作者：戴磊
//时间：2008-12-25
//说明：
//--------------------------------------------------------------------------

#pragma once

#include <list>
using namespace std;


typedef struct _HPTM_DISP_DEV
{
	TCHAR	DeviceName[32];
	TCHAR	DeviceString[128];
	TCHAR	DeviceID[128];
	TCHAR	DeviceKey[128];
	bool	IsPrimary;
//	bool	IsAttached;
	POINTL	dmPosition;
	DWORD	dmPelsWidth;
	DWORD	dmPelsHeight;
}HPTM_DISP_DEV, *PHPTM_DISP_DEV;

typedef std::map<int, PHPTM_DISP_DEV>	HPTM_DISP_DEV_MAP;
typedef HPTM_DISP_DEV_MAP::iterator		HPTM_DISP_DEV_MAP_IT;


class HPTM_DisplayDevices
{
public:
	HPTM_DisplayDevices(void);
	virtual~ HPTM_DisplayDevices(void);

	void Initlize();
	int GetDevicesCount();
	HPTM_DISP_DEV_MAP GetDevicesList();

private:
	bool				m_bInitlized;
	HPTM_DISP_DEV_MAP	m_DispDevLst;
};


HPTM_API HPTM_DisplayDevices* HPTM_DisplayDevices_Create();
HPTM_API void HPTM_DisplayDevices_Destroy();
HPTM_API int HPTM_DisplayDevices_GetCount();
HPTM_API HPTM_DISP_DEV_MAP HPTM_DisplayDevices_GetList();

