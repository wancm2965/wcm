// WBMIDI.h : WBMIDI DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CWBMIDIApp
// 有关此类实现的信息，请参阅 WBMIDI.cpp
//
#include "WhiteNote.h"
#define  WBMIDI_API __declspec(dllexport)
extern "C"
{
	int  WBMIDI_API  ShowMidiFileInWB(CString strfilepath/*,int&height = 0*/);
};
// extern "C"
// {
// 	void WBMIDI_API SetZoomSize(float nvalue,CDC* hdc);
// };

extern "C"
{
	void WBMIDI_API DrawMidiFiveStaff(CDC* hdc);
};


extern "C"
{
	void WBMIDI_API CloseConvert();
};

class CWBMIDIApp : public CWinApp
{
public:
	CWBMIDIApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance(); 
	DECLARE_MESSAGE_MAP()
};
