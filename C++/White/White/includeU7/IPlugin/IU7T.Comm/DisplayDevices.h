
#ifndef _DISPLAY_DEVICE_H
#define _DISPLAY_DEVICE_H


#include<list>
#include<map>

using namespace std;
typedef struct _DISP_MONITOR
{
	TCHAR	DeviceName[32];
	TCHAR	DeviceString[128];
	TCHAR	DeviceID[128];
	TCHAR	DeviceKey[128];
	bool	IsPrimary;
	//bool	IsAttached;
	POINTL	dmPosition;
	DWORD	dmPelsWidth;
	DWORD	dmPelsHeight;

	
}DISP_MONITOR, *PDISP_MONITOR;


typedef std::map<int, DISP_MONITOR>	DISP_DEV_MAP;
typedef DISP_DEV_MAP::iterator		DISP_DEV_MAP_IT;

typedef std::vector<DISP_MONITOR>  DISP_MON_VEC;
typedef DISP_MON_VEC::iterator   DISP_MON_VEC_IT;

static CString GetPath(void)
{
	static CString s_csExePath;
	if (s_csExePath.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		int len = GetModuleFileName(NULL,path,MAX_PATH);
		path[len] = 0;
		TCHAR* ret = _tcsrchr(path,'\\');
		if (!ret)
			return _T("");
		int idxlen = (int)(ret - path + 1);
		path[idxlen] = 0;
		s_csExePath = CString(path);
	}

	return s_csExePath;
}

static void WriteCfgDisplayInt(CString strFile, CString strSection, CString strKey, int nValue)
{
	FILE *fp = NULL;
	int nError = _tfopen_s(&fp, strFile, _T("r"));
	if (nError != 0 || fp == NULL)
	{
		int nError = _tfopen_s(&fp, strFile, _T("w+b"));
		if (nError != 0 || fp == NULL)
			return;

		TCHAR strUnicode[1];
		strUnicode[0] = TCHAR(0XFEFF);
		fputwc(*strUnicode,fp);
	}
	fclose(fp);

	CString strValue;
	strValue.Format(_T("%d"), nValue);
	WritePrivateProfileString(strSection, strKey, strValue, strFile);
}

static void SetCfgDisplayInt(CString strSection, CString strKey, int nValue)
{
	CString csExePath = GetPath() + _T("U7Displayfig.ini");
	WriteCfgDisplayInt(csExePath, strSection, strKey, nValue);
}

static int ReadCfgDisplayInt(CString strSection, CString strKey, int nDefValue)
{
	CString csExePath = GetPath() + _T("U7Displayfig.ini");
	return GetPrivateProfileInt(strSection, strKey, nDefValue, csExePath);
}

static void SetDisplayRefresh(bool bReFresh)
{
	SetCfgDisplayInt(_T("DISP_MONITOR_REFRESH"), _T("REFRESH"), bReFresh);
}

static DISP_DEV_MAP GetmapDispDev()
{
	DISP_DEV_MAP mapDispDev;
	int nNum = ReadCfgDisplayInt(_T("DISP_MONITOR_NUM"), _T("NUM"), 0);
	if (0 != nNum)
	{
		for(int nMONITOR = 0; nMONITOR < nNum; ++nMONITOR)
		{
			CString csMonitorNum;
			csMonitorNum.Format(_T("DISP_MONITOR_%d"),nMONITOR);
			DISP_MONITOR ddTemp;
			ddTemp.dmPosition.x	= ReadCfgDisplayInt(csMonitorNum, _T("dmPositionX"), 0);
			ddTemp.dmPosition.y	= ReadCfgDisplayInt(csMonitorNum, _T("dmPositiony"), 0);
			ddTemp.dmPelsWidth = ReadCfgDisplayInt(csMonitorNum, _T("dmPelsWidth"), 0);
			ddTemp.dmPelsHeight = ReadCfgDisplayInt(csMonitorNum, _T("dmPelsHeight"), 0);
			int nIsPrimary = ReadCfgDisplayInt(csMonitorNum, _T("IsPrimary"), 0);
			ddTemp.IsPrimary = nIsPrimary == 1 ? true : false;
			mapDispDev[nMONITOR] = ddTemp;
		}
	}
	return mapDispDev;
}

static void SetmapDispDev(DISP_DEV_MAP& mapDispDev)
{
	SetCfgDisplayInt(_T("DISP_MONITOR_NUM"), _T("NUM"), mapDispDev.size());
	int nMONITOR = 0;
	DISP_DEV_MAP_IT it = mapDispDev.begin();
	for(;it != mapDispDev.end();++it)
	{
		CString csMonitorNum;
		csMonitorNum.Format(_T("DISP_MONITOR_%d"),nMONITOR++);
		SetCfgDisplayInt(csMonitorNum, _T("dmPositionX"), it->second.dmPosition.x);
		SetCfgDisplayInt(csMonitorNum, _T("dmPositiony"), it->second.dmPosition.y);
		SetCfgDisplayInt(csMonitorNum, _T("dmPelsWidth"), it->second.dmPelsWidth);
		SetCfgDisplayInt(csMonitorNum, _T("dmPelsHeight"), it->second.dmPelsHeight);
		SetCfgDisplayInt(csMonitorNum, _T("IsPrimary"), it->second.IsPrimary);
	}
}

static bool IsDispDevRefresh()
{
	return ReadCfgDisplayInt(_T("DISP_MONITOR_REFRESH"), _T("REFRESH"), true) == 1 ? true : false;
}

static void sort(DISP_MON_VEC& vcTemp)
{
	for (int i = 0; i < (int)vcTemp.size(); ++i)
	{
		DISP_MONITOR temp;
		for (int j = i+1; j < (int)vcTemp.size(); ++j)
		{
			if (vcTemp[i].dmPosition.x > vcTemp[j].dmPosition.x)
			{
				temp = vcTemp[i];
				vcTemp[i] = vcTemp[j];
				vcTemp[j] = temp;
			}
		}
	}
}

static DISP_DEV_MAP DisplayDevices_GetList()
{
	DISP_DEV_MAP mapDispDev;
	bool s_bRefresh = IsDispDevRefresh();

	if (!s_bRefresh)
	{
		mapDispDev = GetmapDispDev();
		if (!mapDispDev.empty())
			return mapDispDev;
	}

	//使用EnumDisplayDevices的前提
	int index = 0;
	DEVMODE devmode;
	DISPLAY_DEVICE dev;
	ZeroMemory(&dev, sizeof(dev));
	dev.cb = sizeof(dev);
	ZeroMemory(&devmode, sizeof(devmode));
	devmode.dmSize = sizeof(devmode);
	BOOL bNext = TRUE;
	DISP_MON_VEC vcTemp;
	while (true)
	{
		bNext = EnumDisplayDevices(NULL, index, &dev, 0);
		EnumDisplaySettings(dev.DeviceName, ENUM_CURRENT_SETTINGS, &devmode);//枚举显示设备图形模式
		if (bNext == FALSE)
		{
			break;
		}
		DISP_MONITOR DspDev;
		memcpy_s(DspDev.DeviceName,	sizeof(TCHAR)*32,	dev.DeviceName,		sizeof(TCHAR)*32);
		memcpy_s(DspDev.DeviceString,	sizeof(TCHAR)*128,	dev.DeviceString,	sizeof(TCHAR)*128);
		memcpy_s(DspDev.DeviceID,		sizeof(TCHAR)*128,	dev.DeviceID,		sizeof(TCHAR)*128);
		memcpy_s(DspDev.DeviceKey,	sizeof(TCHAR)*128,	dev.DeviceKey,		sizeof(TCHAR)*128);

		DspDev.dmPosition.x	= devmode.dmPosition.x;
		DspDev.dmPosition.y	= devmode.dmPosition.y;
		//获取屏幕的工作区域
		POINT pt;
		pt.x = devmode.dmPosition.x;
		pt.y = devmode.dmPosition.y;
		HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);
		if (GetMonitorInfo(hMonitor, &monitorInfo))
		{
			DspDev.dmPelsWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
			DspDev.dmPelsHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
		}
		else
		{
			DspDev.dmPelsWidth	= devmode.dmPelsWidth;
			DspDev.dmPelsHeight	= devmode.dmPelsHeight;
		}

		//在显示设置中启用选项: "使用该设备作为主显示器"
		if (dev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			DspDev.IsPrimary = true;
		}
		else
		{
			DspDev.IsPrimary = false;
		}
		DWORD devMon = 0;
		DISPLAY_DEVICE ddMon;
		ddMon.cb = sizeof(DISPLAY_DEVICE);
		while (	EnumDisplayDevices(dev.DeviceName, devMon, &ddMon, 0))//得到DeviceString的值，用于判断是否是虚拟显示器
		{
			if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE && ddMon.StateFlags & DISPLAY_DEVICE_ATTACHED) // for ATI, Windows XP
				break;
			++devMon;
		}
		//在显示设置中启用选项: "将 Windows 桌面扩展到该显示器"
 		if (dev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP && NULL != *ddMon.DeviceString)
		{
			vcTemp.push_back(DspDev);
		}

		index++;
	}
	//开始排序
	//sort(vcTemp);
	for (int i = 0; i < (int)vcTemp.size(); ++i)
	{
		mapDispDev[i] = vcTemp[i];
	};

	//如果没有检测到任何显示器，则虚拟一个显示器
	if (mapDispDev.empty())
	{
		SetCfgDisplayInt(_T("REALMONITOR"), _T("REAL"), 0);
	}
	else
	{
		SetCfgDisplayInt(_T("REALMONITOR"), _T("REAL"), 1);
	}
	if (mapDispDev.empty())
	{
		DISP_MONITOR ddTemp;
		ddTemp.dmPosition.x	= 0;
		ddTemp.dmPosition.y	= 0;
		ddTemp.dmPelsWidth = 1024;
		ddTemp.dmPelsHeight = 768;
		ddTemp.IsPrimary = true;
		mapDispDev[0] = ddTemp;
	}
	SetmapDispDev(mapDispDev);
	return mapDispDev;
}

static int DisplayDevices_GetCount()
{
	return DisplayDevices_GetList().size();
}
static bool GetScreenInfo(DISP_MONITOR& dispDev, int nMonitor)
{
	DISP_DEV_MAP emTemp =  DisplayDevices_GetList();
	if (nMonitor < (int)emTemp.size() && nMonitor >= 0)
	{
		dispDev = emTemp[nMonitor];
		return true;
	}
	return false;
}

#endif