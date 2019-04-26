#pragma once

#include "HPDVRChannel.h"

#include <map>
#include <string>
using namespace std;

class IHPDVRCoderCallBack
{
public:
	IHPDVRCoderCallBack(){};
	virtual ~IHPDVRCoderCallBack(){};
public:
	virtual void OnCoderChanged(string& codid, int state) = 0;
};

class IHPDVRCoder
{
public:
	IHPDVRCoder(){};
	virtual ~IHPDVRCoder(){};
public:
	virtual bool Connect() = 0;
	virtual void Release() = 0;
	virtual bool IsConnected() = 0;
	virtual bool RebootCoder() = 0;
	virtual string& GetCoderId() = 0;
	virtual long GetUserId() = 0;
	virtual bool GetConnectParam(string&ip, long&port, string&user, string&pwd) = 0;
	virtual int GetCoderType() = 0;
	virtual bool RemoteKeyboard(int key, int input) = 0;/*Ô¶³Ì°´¼ü*/

	virtual IHPDVRChannel* ConnectChannel(int chnid, HD_CHN_INFO& param, IHPDVRChannelCallBack* callback) = 0;
	virtual int GetChannelTotal() = 0;
	virtual int GetChn2McuTotal() = 0;
	virtual void SetChn2McuTotal(int num) = 0;
};

typedef map<string, IHPDVRCoder*> MAP_CODER;
typedef map<string, IHPDVRCoder*>::iterator MAP_CODER_IT;
