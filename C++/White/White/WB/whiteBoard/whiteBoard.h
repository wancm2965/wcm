// whiteBoard.h : whiteBoard DLL ����ͷ�ļ�
//

#pragma once
#pragma comment(lib,"Setupapi.lib")
#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "WhiteBoardDlg.h"
#include "MedioVideoScreenDlg.h"
#include "ScreenDrawDlg.h"
#include <Setupapi.h>



// CwhiteBoardApp
// �йش���ʵ�ֵ���Ϣ������� whiteBoard.cpp
//

class CwhiteBoardApp : public CWinApp
{
public:
	CwhiteBoardApp();
	virtual ~CwhiteBoardApp();   //������������  
	
// ��д
public:
	virtual BOOL InitInstance();
	void KillExe();  //�װ��˳�ʱ����д�ӡ�Ի���Ҳһ���˳�
	DECLARE_MESSAGE_MAP()
	
	virtual int ExitInstance();

	//wangxin 2010-12-28 ɾ��һ���ļ���
	BOOL DeleteDirectory(CString strDir);
	//�Ƿ����˴�����Ȩ���
	BOOL IsDeviceSupported();
private:
	HINSTANCE m_hinstRE41;        //����ʵ������
};

extern CWhiteBoardView *g_pWhiteBoardView; //��ǰ��ͼ����
extern CDialog *g_whiteboardDlg;
extern CWhiteBoardView *g_pBackWhiteBoardView;
extern CWBFile *g_pWBFile; //�װ��ļ�ָ��
extern CWBPage *g_pWBPage; //�װ�ҳָ��
extern BOOL g_bIsFullScreen; //�Ƿ�ȫ��״̬
extern IHPNetNotify *g_pHPWBDataNotify; //����ͬ����Ϣָ��
extern IHPWBNotify *g_pConferenceRecordNotify; //¼����Ϣָ��
extern void *g_pParent ; //������
extern CPoint g_ptComRotCenter; //��ϵ���ת���ĵ�
extern CPoint g_ptDocRotCenter; //�ĵ�����ת���ĵ�
extern double g_dbComRotAngle; //��ϵ���ת�Ƕ�
extern double g_dbDocRotAngle; //�ĵ�����ת�Ƕ�
extern CPoint g_ptComZoomCenter; //��ϵ��������ĵ�
extern CPoint g_ptDocZoomCenter; //�ĵ����������ĵ�
extern BOOL g_bRunWater;         //ˮ������Ч
extern BOOL g_bProcessWater;    //���ưװ���С��ʱ��ʾˮ����
extern int  g_nBrushSize;      //���Ӵ�С
//��϶�������ű���
extern double g_dbXScale;
extern double g_dbYScale;
//�ĵ���������ű���
extern double g_dbDocXScale;
extern double g_dbDocYScale;
//�¹��ܿ���
extern bool g_bShowWBNewFun;
extern bool g_bSimplifyWB;//�Ƿ���ý�幦��
//view ��С
extern double g_reduce;
//�װ�view�ĳ��Ϳ�
extern int g_nWidth;
extern int g_nHeight;

extern DWORD g_dwPreZoomLayerIndex; //ǰһ���������ŵĶ���ͼ���
extern BOOL g_bIsDrawScreen;                  //��Ļ��ͼ�Ƿ�����־
extern unsigned long g_ulSessionID; //�ͻ���ID
extern CString g_strAvconUserID; //AVCON���ص�¼���û�ID����AVCONͨ���ӿں�������
extern BOOL g_bYunNanDeviceSupported;//ר�������Ϲ���Ӳ����Ȩ
extern bool g_bAuthorizedByHandWare ; //��Ӳ�������Ƿ��д���������Ȩ
extern bool g_bWBWndIsneedShowToolBtn;//���ݰװ��С���ж��Ƿ���ʾ��ť
extern CString g_strLocalUserName;//���ñ����û�����
//��AVCON�ṩ��һЩ��ʱ·��
extern CString g_strAvconLanguageFile;
extern CString g_strAvconIniPath;  //�����ļ�Ŀ¼
extern CString g_strAvconTempPath;   //��ʱĿ¼
extern HMODULE g_IsMedicalVersion;//�ж��Ƿ���ҽ�ư汾��
extern bool g_bAvconLock;   //AVCON�Ƿ������װ幦�ܣ�������ʱ���װ�򿪺�û�й�����
extern bool g_bIsConferenceRecord; //��AVCON���ã��Ƿ�ʼ����¼��
extern WB_TYPE g_WBType;			//��ǰ�汾����
extern bool g_bIsSupportMTTouch;//�Ƿ�������Ȩʹ�ö�㴥����
extern BOOL g_bNoSupportTouchInWB;//��������û����Ȩʱ����������Ĵ�������������
extern BOOL g_bSupportU7OpenFile ;// �ж��Ƿ�ΪU7�еĴ��ļ���ʽ
extern CString g_bU7FileTempDir ;// ����U7���ļ�Ĭ��·��
extern BOOL g_MediaIntToSin;//��Ƶ���絽����
extern int g_PageCount;//ҳ�ţ���Ƶ���ļ�ʱ��
extern HWND g_hVedioHwnd;
extern BOOL g_bMedioScreenWndEnd;//����¼�ƴ�������ر�
extern bool g_bShowFullScreenBtn; //��ʾȫ����ť��
extern CString g_strMediaPhoto; //��Ƶ˫�����մ��·����
extern HMODULE g_IsMidiVersion; //midi�汾
//����Ƿ�ΪI7��Ŀ
extern bool g_bSupportI7;
extern CPoint g_preScrollPoint ;//I7�й���������һ��λ�ã�
extern bool g_bU7NewStyle;//��u7�аװ�Ĺ��ܽ��������Ż�