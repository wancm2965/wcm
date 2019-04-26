#pragma once

#include "HPDVRCoder.h"

#include <map>
#include <string>
using namespace std;

#ifdef	HPDVR_EXPORTS
#define HPDVR_API extern "C" __declspec(dllexport)
#else
#define HPDVR_API extern "C" __declspec(dllimport)
#endif

#define MAX_NVS_TYPE	4
enum E_NVS_TYPE /*设备类型*/
{
	NVS_HPD,/*华平*/
	NVS_HIK,/*海康*/
	NVS_DHD,/*大华*/
	NVS_NZI,/*南自*/
};

static const char* NVSTYPE[] =
{
	"HPD",/*华平*/
	"HIK",/*海康*/
	"DHD",/*大华*/
	"NZI",/*南自*/
};

enum E_DEVICE_STATE/*设备状态*/
{
	DEVICE_OFFLINE = 0,/*离线*/
	DEVICE_ONLINE  = 1,/*在线*/
};

typedef struct tagNVS		//NVS
{	
	UINT size;
	CHAR id[32];	
	CHAR file[64];			//文件
	CHAR version[32];		//版本
	UINT type;				//类型
	LONG state;				//状态
}HD_NVS, *LPHD_NVS;

class IHPNVSCallBack
{
public:
	virtual void OnNVSCommand(WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnNVSCallback(LONG lCommand, char* sDVRIP, char* pBuf, DWORD dwBufLen) = 0;
};

class IHPNVS
{
public:
	virtual bool Connect() = 0;
	virtual void Release() = 0;
	virtual bool IsConnected() = 0;

	virtual bool GetNVSInfo(HD_NVS& data) = 0;
	virtual void SetNVSInfo(HD_NVS& data) = 0;

	virtual IHPDVRCoder* ConnectCoder(HD_COD_INFO& param, IHPDVRCoderCallBack* callback) = 0;
	virtual void ReleaseCoder(long usrid) = 0;
};

HPDVR_API IHPNVS* CreateHPNVS();
HPDVR_API BOOL InitHPNVS();
HPDVR_API void ExitHPNVS();


typedef IHPNVS* (*CREATEHPNVS) ();
typedef BOOL (*INITHPNVS) ();
typedef void (*EXITHPNVS) ();

class CHPNVS
{
public:
	CHPNVS() 
	{
		m_hDLL = NULL;
		m_pNVS = NULL;
	};
	virtual ~CHPNVS() {};

public: 
	BOOL Create(CHAR*pFile)
	{
		m_hDLL = LoadLibraryA(pFile);
		if (!m_hDLL) return FALSE;

		CREATEHPNVS fCreateHPNVS = (CREATEHPNVS) GetProcAddress(m_hDLL, "CreateHPNVS");
		if (!fCreateHPNVS) return FALSE;

		m_pNVS = (IHPNVS*)fCreateHPNVS();
		if (!m_pNVS) return FALSE;

		INITHPNVS fInitHPNVS = (INITHPNVS) GetProcAddress(m_hDLL, "InitHPNVS");
		if (!fInitHPNVS) return FALSE;

		return fInitHPNVS();
	}

	void Destroy()
	{
		EXITHPNVS fExitHPNVS = (EXITHPNVS) GetProcAddress(m_hDLL, "ExitHPNVS");
		if (fExitHPNVS) fExitHPNVS();
		FreeLibrary(m_hDLL);
		m_hDLL = NULL;
		m_pNVS = NULL;
	}	

public:
	bool Connect() { if (!m_pNVS) return false; return m_pNVS->Connect(); }
	void Release() { if (m_pNVS) m_pNVS->Release(); }
	bool IsConnected() { if (!m_pNVS) return false; return m_pNVS->IsConnected(); }
	bool GetNVSInfo(HD_NVS& data) { if (!m_pNVS) return false; return  m_pNVS->GetNVSInfo(data); }
	void SetNVSInfo(HD_NVS& data) { if (m_pNVS) m_pNVS->SetNVSInfo(data); }
	IHPDVRCoder* ConnectCoder(HD_COD_INFO& param, IHPDVRCoderCallBack* callback) { if (!m_pNVS) return NULL; return m_pNVS->ConnectCoder(param, callback); }
	void ReleaseCoder(long usrid) { if (m_pNVS) m_pNVS->ReleaseCoder(usrid); }

protected:
	HINSTANCE m_hDLL;
	IHPNVS* m_pNVS;
};

typedef map<string, HD_NVS>	MAP_HDNVS;
typedef map<string, HD_NVS>::iterator MAP_HDNVS_IT;
typedef map<string, CHPNVS*> MAP_NVS;
typedef map<string, CHPNVS*>::iterator	MAP_NVS_IT;

