#pragma once

#include <string>
using namespace std;

class IAVMSPlayerEvent
{
public:
	virtual void OnAVMSPlayerEvent_RecordFileFound(string&file, string&desc, string&begin, string&end, unsigned long filesize)=0;/*��ѯ��һ��¼���ļ�*/
	virtual void OnAVMSPlayerEvent_RecordFileFoundEnd(int nFound)=0;/*��ѯ�ѵ���β*/
};

class IAVMSPlayer
{
public:
	virtual ~IAVMSPlayer(void) {};
	virtual HWND GetHwnd(void)=0;/*�㲥����*/
	virtual bool StartFindRecordFiles(string&devinfo, string&chnid, int rectype, string&begin, string&end)=0;/*����¼���ļ������������IAVMSPlayerEvent*/
	virtual bool PlayRecordFile(string&file)=0;/*ָ���ļ�����*/
};

AFX_EXT_CLASS IAVMSPlayer*CreateAVMSPlayer(IAVMSPlayerEvent*pEvent, HWND hParent);/*����һ���㲥ʵ��*/
AFX_EXT_CLASS void DestroyAVMSPlayer(IAVMSPlayer*pVODPlayer);/*����һ���㲥ʵ��*/
