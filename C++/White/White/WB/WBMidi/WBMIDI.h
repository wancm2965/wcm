// WBMIDI.h : WBMIDI DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWBMIDIApp
// �йش���ʵ�ֵ���Ϣ������� WBMIDI.cpp
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

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance(); 
	DECLARE_MESSAGE_MAP()
};
