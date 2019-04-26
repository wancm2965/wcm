#pragma once

#include "../../ISystem/IPlugin.h"

#define PLUGIN_NAME_U7T_OPENSAVE			"Plugin_U7T_OpenSave"

namespace AVCONPlugin
{
	// ��ͨ���OpenSave
	class IPlugin_U7T_OpenSave : public IPlugin
	{
	public:
		virtual ~IPlugin_U7T_OpenSave(){};

	public:
		virtual HWND CreateOpenSave(BOOL bOpenSave,		//TRUE --OpenDlg, FALSE ----SaveDlg
			LPCTSTR lpszDefExt = NULL,					//ָ��Ĭ�ϵ��ļ���չ�� 
			LPCTSTR lpszFilter = NULL,					//ָ���ɹ�ѡ����ļ����ͺ���Ӧ����չ��
			LPCTSTR strMemberID=_T("Local"),			//���ڱ�����Ա���ļ���¼,Ĭ���Ǳ�����Local�ļ�����
			HWND hParent=NULL,
			LPCTSTR strFilePath =_T("")				//���ʱ���贫���豣���ļ���ԭ·��);
			)=0;
		virtual void ShowDlg(bool bShow) = 0;
		virtual const char* GetOpenFile(void) = 0;
		virtual void DestroyOpenSave() = 0;
	};
}
