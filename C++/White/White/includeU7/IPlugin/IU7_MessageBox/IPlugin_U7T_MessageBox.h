#pragma once
#include "ISystem/ISystem.h"///���÷�������ġ�

namespace AVCONPlugin
{
	class IPlugin_U7T_MessageBox : public IPlugin
	{
	public:
		///����һ�����ģ̬����
		// ����bAutoDestroy: Ϊtrue����ʧȥ����ʱ���٣�������������
		virtual int MessageBox(HWND hOwner,CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy = false) = 0;
		///�ر����е���ʽ�Ĵ���
		virtual void CloseAllMessageBox() = 0;
	};
}