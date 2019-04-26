#ifndef __CoUI_API_H__
#define __CoUI_API_H__
#include "Utils.h"
#include <CoUI/Core/UIBase.h>

namespace CoUI {
	/////////////////////////////////////////////////////////////////////////////////////


#define COUI_MSGBOX_AUTODESTROY			(0x0001)//ʧȥ��������
#define COUI_MSGBOX_TIMEOUTDESTROY		(0x0002)//����ʱ����
#define COUI_MSGBOX_CUSTOMEPOSITION		(0x0004)//�Զ���λ��
#define COUI_MSGBOX_CONTENT_LINELIMIT	(0x0008)//�����������
#define COUI_MSGBOX_CONTENT_CENTER		(0x0010)//���ݾ��ж���


#define COUI_MIN_TIMEOUT			(10)//��С��ʱʱ��

	typedef struct MsgboxAppendData
	{
		UINT nActionStyle;
		UINT nSecTimeout;//��ʱ����,����timeout��ʶʱ��Ч
		int nX;	//�������Ͻǵ�xλ�ã������Զ����ʶʱ��Ч����ͬ
		int nY;
		UINT nLineLimit;//���ݵ�������������������������ʱ��Ч
		struct MsgboxAppendData()
		{
			nActionStyle = 0;
			nSecTimeout = COUI_MIN_TIMEOUT;
			nX = 0;
			nY = 0;
			nLineLimit = 0;
		}

	}MsgboxAppendData, *PMsgboxAppendData;

	enum CoUI_Lang
	{
		CoUI_Lang_ZH_CN,//����
		CoUI_Lang_EN_US,//Ӣ��
	};

	//����messagebox����xml�ļ�������·��,xml�ļ���������Messagebox.xml
	//����ֵ������˵������ʧ�ܣ�Ʃ��·�������ڣ���������ʾ��Դ���.
	int UILIB_API CoUI_AddMsgBoxSkin(LPCTSTR strResourcePath, CoUI_Lang lan = CoUI_Lang_ZH_CN);

	//************************************
	// ˵��:    �ڴ����е�����Ϣ����
	// ����:	hOwner   	��Ϣ�������ڵĴ��ھ��
	// ����:	strTitle    ��Ϣ��ı���
	// ����:	strContent	��Ϣ����Ҫ��ʾ������
	// ����:    nStyle		����Ҫ�����Ĵ������ͺ�ͼ������
		                        //�������Ͱ���MB_OK��MB_OKCANCEL��MB_ABORTRETRYIGNORE��MB_YESNOCANCEL��MB_YESNOMB_RETRYCANCEL������Ҫ��
		                        //ͼ�����Ͱ���MB_ICONINFORMATION��MB_ICONERROR��MB_ICONQUESTION��Ĭ��ΪMB_ICONINFORMATION
								//��������˵���� MB_REMEMBER ��ʶʹ�á�����ѯ�ʡ�ѡ���ѡ�˸������Ӧ�ķ���ֵΪ��IDYESREM����IDOKREM��
	// ����:	nResID ��ֵ���ʾʹ���ڲ���Դ������ʹ��ָ����Դ���ο�CoUI_AddMsgBoxSkin�ķ���ֵ˵����
	// ����ֵ:  int			������Ϣ����
	//************************************
	int UILIB_API CoUI_MessageBox(HWND hOwner, 
		LPCTSTR strTitle,
		LPCTSTR strContent,
		UINT nWndStye,
		const MsgboxAppendData &pAppendData,
		int nResID = 0);

	int UILIB_API CoUI_AfxMessageBox(
		LPCTSTR strContent, int nResID = 0);
	
	/* �������е������� */
	void UILIB_API CoUI_CloseAllMessageBox();
	
	/**************************************************************************************/
	//���ܣ�CoUI_CFileDialog��Ϊ�ڴ����е����򿪻򱣴��ļ��Ի�����														
	//		����˵����CoUI_CFileDialog::Init�����Ĳ����б�
	//			������bOpenSave					TRUE --���ļ�, FALSE --�����ļ�						
	//			������ lpszDefExt					ָ��Ĭ�ϵ��ļ���չ��							
	//			������ lpszFilter					ָ���ɹ�ѡ����ļ����ͺ���Ӧ����չ������ΪNULLʱΪ���ļ��жԻ���
	//			������ strMemberID				���ڱ�����Ա���ļ���¼,Ĭ���Ǳ�����Local�ļ�����
	//			������ hParent						�����ھ��																
	//			������ strFilePath					���ʱ���贫���豣���ļ���ԭ·��							
	/**************************************************************************************/
	class /*UILIB_API*/ CoUI_CFileDialog//��ʱ�رմ˹��ܣ��Ƚ��ȶ��󿪷�
	{
	private:
		CoUI_CFileDialog( );
	public:
		void Init(BOOL bOpenSave, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR strMemberID, HWND hParent, LPCTSTR strFilePath);
		 UINT DoModal();
		 const CStdString GetPathName();
		 const CStdString GetFileName();
		 static CoUI_CFileDialog* GetInstance();
		 void DestroyOpenSave(void);

	private:
		static CoUI_CFileDialog* m_pInstance;
		CWindowWnd*	m_pDlgOpenSave;
		CStdString		m_strFilePathName;
		BOOL bOpenSave; 
		LPCTSTR lpszDefExt; 
		LPCTSTR lpszFilter; 
		LPCTSTR strMemberID; 
		HWND hParent; 
		LPCTSTR strFilePath;
		class CGarbo
		{
		public:
			~CGarbo();
		};
		static CGarbo Garbo;
	};

	//************************************
	// ˵��:    �ڴ����е��������߽������ʾ��
	// ����:	strTip   	��ʾ����
	// ����:	strTitle    ����ʱ��
	// ����ʾ������Ļ������ʾ���������趨��ʱ����Զ�����
	// ֻ����ʾ��400�������֣�Ӧ���㹻ʹ����
	//************************************
	void UILIB_API CoUI_Tip( LPCTSTR strTip, int nSeconds = 3 );
} // namespace CoUI
#endif // __CoUI_API_H__