#pragma once
#include <windows.h>
#include "string.h"
#include "stdio.h" 
//�ⲿȫ�ֱ���
extern HHOOK g_Mousehook;
extern HWND g_hWndMsg;
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam, LPARAM lParam );
void InstallMouseHook(HWND hWnd);
void EndHook(void);
// LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam, LPARAM lParam )
// {
// 	BOOL fEatKeystroke = FALSE;
// 	LPMOUSEHOOKSTRUCT pMouseHook = (MOUSEHOOKSTRUCT FAR *)lParam;  
// 	if (nCode == HC_ACTION && WM_LBUTTONUP == wParam)//����������ʱ
// 	{
// 		if (pMouseHook && IsWindow(g_hWndMsg) && pMouseHook->hwnd != g_hWndMsg)
// 		{
// 			POINT ptScreen = pMouseHook->pt;
// 			ScreenToClient(g_hWndMsg,&ptScreen);
// 			::SendMessage(g_hWndMsg,WM_LBUTTONUP,NULL,MAKELPARAM(ptScreen.x,ptScreen.y));  
// 		}
// 	}
// 	return CallNextHookEx(g_Mousehook,nCode,wParam,lParam);
// }
// 
// void EndHook(void)
// {
// 	if(g_Mousehook)
// 	{
// 			UnhookWindowsHookEx(g_Mousehook);
// 			g_Mousehook = 0;
// 			g_hWndMsg = NULL;
// 	}
// }
// 
// void InstallMouseHook(HWND hWnd)
// {
// 	EndHook();
// 	g_hWndMsg = hWnd;
// 	HINSTANCE hInst = GetModuleHandle(NULL);//��ȡ��ǰ���ظ�dll��ģ��ʵ��
// 	g_Mousehook = SetWindowsHookEx(WH_MOUSE_LL,(HOOKPROC)MouseProc,hInst,0);
// }