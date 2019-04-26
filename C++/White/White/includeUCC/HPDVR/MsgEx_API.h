
// MsgEx_API.h

#pragma once

#ifdef MsgEx_API
#define DllExport_wt extern "C" __declspec(dllexport)  
#else
#define DllExport_wt extern "C" __declspec(dllimport) 
#endif

// ��ģ�����ⷢ�͵���Ϣ�����ͷ�ʽΪSendMessage()�� ֪ͨ�ⲿ ��ģ�鷢����һ����ϢWPARAM����ΪMsgEx_API_Data�ṹ��
// ��ʱָ�� ��ʾ��Ϣ����
#define WM_MsgEx_API_Data (WM_USER+0x1999)

// �ⲿ��ģ�鷢�͵���Ϣ��ע�⣺���ͷ�ʽӦ��ΪPostMessage()�������п������������
// �ⲿ֪ͨ��ģ��һ��MsgEx_API_Data�ṹ�Ѿ����� WPARAM����ΪMsgEx_API_Data�ṹ����ʱָ��
#define WM_MsgEx_API_Data_Pop WM_USER+1001

struct MsgEx_API_Data 
{
	DWORD data; // Comboboxѡ������

	CString text; // �ı�����
	CString fontface; // �ı���������
	CString fontsize; // �ı�����ߴ�
	CString fontcolor; // �ı�������ɫ
	CString fontEffects; // �ı�����Ч��

	CString textTime; // ʱ����ı�
	CString fontfaceTime; // ʱ����������
	CString fontsizeTime; // ʱ������ߴ�
	CString fontcolorTime; // ʱ��������ɫ
	CString fontEffectsTime; // ʱ������Ч��
};

// �������� ����һ��ģ��ʵ�� �ڴ���֮ǰ ����������������������
// ������
//      hWnd�������ھ��
//      hWndMsg���ⲿ��Ϣ����ģ����������ò���ΪNULL����ģ��ֻ���ı���ʾ����
DllExport_wt BOOL CreateMsgWindow(HWND hWnd, HWND hWndMsg);

// ���ģ���HWND���
DllExport_wt HWND GetMsgWindowHWND();

// ���ģ����CComboBox�ؼ���HWND���
// �˿ؼ�Ϊ��Ա�б�
// Ĭ����һ������������ˡ� ItemDataΪ0
// ���������ⲿ��ӣ� ע��ItemData������Ϊ0
DllExport_wt HWND GetComboHWND();

// �����������
DllExport_wt void ReleaseMsgWindow();


// ������ʾ�����һ���ı�
// ������MsgEx_API_Data�ṹ��Ӧ
DllExport_wt void AppendMsg(LPCTSTR text,LPCTSTR fontface,LPCTSTR fontsize,LPCTSTR fontcolor,LPCTSTR fontEffects);

DllExport_wt void AppendMsgEx(MsgEx_API_Data* pData);