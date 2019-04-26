#pragma once

#include <string>
using namespace std;

class IAVMSPlayerEvent
{
public:
	virtual void OnAVMSPlayerEvent_RecordFileFound(string&file, string&desc, string&begin, string&end, unsigned long filesize)=0;/*查询到一个录像文件*/
	virtual void OnAVMSPlayerEvent_RecordFileFoundEnd(int nFound)=0;/*查询已到结尾*/
};

class IAVMSPlayer
{
public:
	virtual ~IAVMSPlayer(void) {};
	virtual HWND GetHwnd(void)=0;/*点播窗口*/
	virtual bool StartFindRecordFiles(string&devinfo, string&chnid, int rectype, string&begin, string&end)=0;/*搜索录像文件，结果反馈到IAVMSPlayerEvent*/
	virtual bool PlayRecordFile(string&file)=0;/*指定文件播放*/
};

AFX_EXT_CLASS IAVMSPlayer*CreateAVMSPlayer(IAVMSPlayerEvent*pEvent, HWND hParent);/*创建一个点播实例*/
AFX_EXT_CLASS void DestroyAVMSPlayer(IAVMSPlayer*pVODPlayer);/*销毁一个点播实例*/
