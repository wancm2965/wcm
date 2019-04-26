//========================================================================
//������ģ��
//���ܣ���Ӧ������Ҫ��ģ��
//���ߣ�����
//ʱ�䣺2008-12-8
//˵����
//--------------------------------------------------------------------------

#pragma once
#include "HPDLG_Base.h"

typedef enum
{
	TM_TYPE_NONE	= -1,	//������
	TM_TYPE_4X3		= 0,	//4��3��ͨ��
	TM_TYPE_16X9			//16��9����
}HPTM_TM_TYPE;

typedef enum
{
	TM_SUBTYPE_0_NONE		= -1,
	TM_SUBTYPE_1_1X1		= 0,
	TM_SUBTYPE_2_1X2_SPACE,
	TM_SUBTYPE_2_1X2_FULL,
	TM_SUBTYPE_3_L1_R2,
	TM_SUBTYPE_4_2X2,
	TM_SUBTYPE_4_L1_R3,
	TM_SUBTYPE_6_LT1_RD5,
	TM_SUBTYPE_8_LT1_RD7,
	TM_SUBTYPE_9_3X3,
	TM_SUBTYPE_10_T2_B2X4,
	TM_SUBTYPE_13_IN1_RD12,
	TM_SUBTYPE_16_4X4,
	TM_SUBTYPE_2_PIC_IN_PIC,

	TM_SUBTYPE_4_B1_T3,
	TM_SUBTYPE_5_L1_R4,
	TM_SUBTYPE_5_B1_T4,
	TM_SUBTYPE_6_T2_B4,
	TM_SUBTYPE_7_L2X2_R3,
	TM_SUBTYPE_10_LT1_RD9,
	TM_SUBTYPE_12_3X4,
	TM_SUBTYPE_24_LT1_RD5_X4,
	TM_SUBTYPE_25_5X5,
	TM_SUBTYPE_36_6X6,

	TM_SUBTYPE_9_LT1_RD8,
//	TM_SUBTYPE_10_LT1_RD9,
	TM_SUBTYPE_11_MD1_RD10,
	TM_SUBTYPE_12_LT1_RD11,
	TM_SUBTYPE_14_LT1_RD13,

	TVWALL_TYPE_TEST		= 100,
}HPTM_TM_SUBTYPE;



#define HPTM_MAX_WIN_COUNT	36
#define WM_CHILE_ADDED	(WM_USER + 4321)

typedef struct _HPTM_DROP
{
	int				Type;		//0������1����ͼ��2������Ƶ���Ŵ���
	char			MemberID[255];
	unsigned long	ChannelID;
}HPTM_DROP;


#define HPTM_EXPORT
#ifdef HPTM_EXPORT
#define HPTM_API _declspec(dllexport)
#elif HPTM_DLL
#define HPTM_API _declspec(dllimport)
#else
#define HPTM_API
#endif


class HPTM_API HPTM_TempletNotify
{
public:
	HPTM_TempletNotify(){};
	~HPTM_TempletNotify(){};
public:
	/*	�������ܣ��Ϸ��¼�
		��    ����	wParam��		�ϷŲ���
					lParam��		�ϷŲ���
					iTMIndex��		Ŀ��ģ������
					iParent��		Ŀ��ģ���ϸ���������
					ulSessionID��	���
		�� �� ֵ��*/
	virtual void HPTM_TempletDropNotify(WPARAM wParam, LPARAM lParam, int iTMIndex, int iParent, unsigned long ulSessionID) = 0;

	/*	�������ܣ�ѡ��ģ���¼�
		��    ����	iTMIndex��		��ģ������
					iTMSubType��	��ģ���ģ����
					ulSessionID��	���
		�� �� ֵ��*/
	virtual void HPTM_TempletSelectedNotify(int iTMIndex, int iTMSubType, unsigned long ulSessionID) = 0;

	/*	�������ܣ��������ƶ��Ӵ����¼�
		��    ����	iSrcTM��		Դģ������
					iSrcParent��	Դ�����ڵ����
					iTagTM��		Ŀ��ģ������
					iTagParent��	Ŀ�길���ڵ����
					ulSessionID��	���
		�� �� ֵ��*/
	virtual void HPTM_TempletSwapNotify(int iSrcTM, int iSrcParent, int iTagTM, int iTagParent, unsigned long ulSessionID) = 0;

	/*	�������ܣ������¼�
		��    ����	iTMIndex��		ģ������
					iParentIndex��	�����ڵ����
					ulSessionID��	���
		�� �� ֵ��*/
	virtual void HPTM_TempletLButtonDownNotify(int iTMIndex, int iParentIndex, unsigned long ulSessionID) = 0;

	/*	�������ܣ�˫���¼�
		��    ����	iTMIndex��		ģ������
					iParentIndex��	�����ڵ����
					iResult��		˫�����״̬��0��ԭʼ״̬��1����󻯵�ģ�壬2����󻯵�ȫ����
					ulSessionID��	���
		�� �� ֵ��*/
	virtual void HPTM_TempletldbClickNotify(int iTMIndex, int iParentIndex, int iResult, unsigned long ulSessionID) = 0;

	/*	�������ܣ�ģ�����ͻ������͸ı�
		��    ����	iTMIndex��		ģ������
					type��			ģ�������
					subtype��		ģ���������
					ulSessionID��	���
		�� �� ֵ��*/
	virtual void HPTM_TempletlTypeChangedNotify(int iTMIndex, HPTM_TM_TYPE type, HPTM_TM_SUBTYPE subtype, unsigned long ulSessionID, BOOL bAppAll=FALSE) = 0;
};

class HPTM_API HPTM_Templet
{
public:
	HPTM_Templet(void){};
	virtual~ HPTM_Templet(void){};

public:
	//�������
	virtual HWND GetSafeHwnd() const = 0;
	virtual BOOL ShowWindow(int nCmdShow) = 0;
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;
	virtual void GetWindowRect(LPRECT lpRect) const = 0;
	virtual BOOL IsWindowVisible(void) = 0;
	virtual CWnd* SetParent(CWnd* pWndNewParent) = 0;
	virtual CWnd* GetParent() = 0;

public:
	//ģ�����
	/*	�������ܣ���ģ����ָ����������Ӵ���
		��    ����	iParent��	ģ���ϸ���������
					pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWindow(int iParent, CWnd* pWindow) = 0;

	/*	�������ܣ���ģ���ϵ�һ�����еĸ�������Ӵ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWindow(CWnd* pWindow) = 0;

	/*	�������ܣ���ģ���ϱ�ѡ�еĸ�������Ӵ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWin2Select(CWnd* pWindow) = 0;

	/*	�������ܣ�ɾ��ģ���ϵĴ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual void DelWindow(CWnd* pWindow) = 0;

	/*	�������ܣ�ɾ��ģ���ϵ����д���
		��    ����
		�� �� ֵ��*/
	virtual void DelWindow() = 0;

	/*	�������ܣ��鿴ָ���Ĵ������ĸ���������
		��    ����pWindow��	ָ���Ĵ��ھ��
		�� �� ֵ�������ڵ���ţ�δ�ҵ�����-1��*/
	virtual int FindWindow(CWnd* pWindow) = 0;

	/*	�������ܣ���ȡģ���ϸ�����ĸ���
		��    ����
		�� �� ֵ��ģ���ϸ�����ĸ���*/
	virtual int GetParentCount() = 0;

	/*	�������ܣ���ȡģ������ʹ�õĸ��������
		��    ����
		�� �� ֵ��ģ������ʹ�õĸ��������*/
	virtual int GetUsedTMCount() = 0;

	/*	�������ܣ���ȡ�����ģ�常�����ϴ��ڵ�ָ��
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��ģ���ϸ���������*/
	virtual int GetParentIndex(CWnd* pWindow) = 0;

	/*	�������ܣ���ȡ�����ģ�常�����ϴ��ڵ�ָ��
		��    ����	iParentIndex��	ģ���ϸ���������
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual CWnd* GetChildWindow(int iParentIndex) = 0;

	/*	�������ܣ�����ģ������
		��    ����tmType��	ģ�����ͣ�TM_TYPE_4X3,TM_TYPE_16X9��
		�� �� ֵ��*/
	virtual void SetTMType(HPTM_TM_TYPE tmType) = 0;

	/*	�������ܣ�����ģ������
		��    ����
		�� �� ֵ��ģ�����ͣ�TM_TYPE_4X3,TM_TYPE_16X9��*/
	virtual HPTM_TM_TYPE GetTMType() = 0;

	/*	�������ܣ�����ģ��������
		��    ����tmSubType��	ģ��������
		�� �� ֵ��*/
	virtual void SetTMSubType(HPTM_TM_SUBTYPE tmSubType) = 0;

	/*	�������ܣ�����ģ��������
		��    ����
		�� �� ֵ��ģ��������*/
	virtual HPTM_TM_SUBTYPE GetTMSubType() = 0;

	/*	�������ܣ�����ͬһ��ģ���µ�����������
		��    ����	iTMIndex��	ģ������
					iSrcParent��Դģ���ϸ�����ĵ����
					iTagParent��Ŀ��ģ���ϸ�����ĵ����
		�� �� ֵ��*/
	virtual void SwapChild(int iSrcParent, int iTagParent) = 0;

	/*	�������ܣ�ʹ������ȫ����ģ������
		��    ����isFullTemplet��	�Ƿ�ȫ��
		�� �� ֵ��*/
	virtual void SetParentFullTemplet(int nParentIndex, bool isFullTemplet) = 0;

	/*	�������ܣ�ʹģ������ȫ����ȡ��ȫ��
		��    ����isFullScreen��	�Ƿ�ȫ��
		�� �� ֵ��*/
	virtual void SetTempletFullScreen(bool isFullScreen) = 0;

	/*	�������ܣ�ʹģ������ȫ����ȡ��ȫ��
		��    ����iParent��ģ���ϸ�����ĵ����
		�� �� ֵ��*/
	virtual bool IsBigTemplet(int iParent) = 0;

	/*	�������ܣ����ñ���
		��    ����	iParent��ģ���ϸ�����ĵ����
					bIsAlarm���Ƿ񱨾���true���������߿���˸��false��ȡ��������
		�� �� ֵ��*/
	virtual void SetAlarm(int iParent, bool bIsAlarm) = 0;

	/*	�������ܣ����õ�ǰģ�������
		��    ����
		�� �� ֵ��*/
	virtual void SetTMName(std::string strName) = 0;

	/*	�������ܣ���ȡ��ǰģ�������
		��    ����
		�� �� ֵ��*/
	virtual std::string GetTMName() = 0;

	/*	�������ܣ���ȡ��ǰģ������
		��    ����
		�� �� ֵ��*/
	virtual int GetTMIndex() = 0;

	/*	�������ܣ���ȡȫ��ģ������
		��    ����
		�� �� ֵ��*/
	virtual int GetFullTMIndex() = 0;

	/*	�������ܣ�����ģ����ɫ
		��    ����
		�� �� ֵ��*/
	virtual void SetColors(COLORREF BackColor, COLORREF FrameColor, COLORREF TextColor, COLORREF SelectedColor) = 0;

	/*	�������ܣ�����ģ�������С
	��    ����
	�� �� ֵ��*/
	virtual void SetFontSize(int nFontSize = 250) = 0;

	/*	�������ܣ�����ģ�帡������λ��
		��    ����
		�� �� ֵ��*/
	virtual void SetFloatWndPosition(int nX, int nY, int nCX, int nCY) = 0;

	/*  ��������: ����ʱʹ�� x.t
		��    ��: bChangeScreen �Ƿ�����
		�� �� ֵ: 
	*/
	virtual void SetChangeScreenStatus(bool bChangeScreen) = 0;
};


class HPTM_API HPTM_Manager
{
public:
	/*	�������ܣ����캯��
		��    ����
		�� �� ֵ��*/
	HPTM_Manager(void){};
	virtual~ HPTM_Manager(void){};

	/*	�������ܣ���ȡָ�������͵ĸ�����ĸ���
		��    ����
		�� �� ֵ��������ĸ���*/
	virtual int GetSubTypeWinCount(HPTM_TM_SUBTYPE nSubType) = 0;

	/*	�������ܣ����ģ��
		��    ����	rect��		ģ����ʾ����
					pParent��	ģ�常����ָ�루Ĭ��Ϊ�գ�
					tmType��	ģ�����ͣ�HPTM_TM_TYPE�ж����ֵ��Ĭ��ΪTM_TYPE_4X3��
		�� �� ֵ��ģ������*/
	virtual int AddTemplet(CRect &rect, CWnd* pParent = NULL, bool bIsPopWindow = false, 
		HPTM_TM_TYPE tmType = TM_TYPE_4X3, std::string flag = "CF_HPMC") = 0;

	/*	�������ܣ���ȡģ����
		��    ����
		�� �� ֵ��*/
	virtual HPTM_Templet* GetTemplet(int iTMIndex) = 0;

	/*	�������ܣ�ɾ������ģ��
		��    ����
		�� �� ֵ��*/
	virtual void DelTemplet() = 0;

	/*	�������ܣ�ɾ��ָ��ģ��
		��    ����
		�� �� ֵ��*/
	virtual void DelTemplet(int iTMIndex) = 0;

	/*	�������ܣ���ȡģ��ĸ���
		��    ����
		�� �� ֵ��ģ��ĸ���*/
	virtual int GetTMCount() = 0;

	/*	�������ܣ�ɾ��ģ���ϵĴ���
		��    ����pWindow����ӽ�ģ��Ĵ���ָ��
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual void DelWindow(CWnd* pWindow) = 0;

	/*	�������ܣ�ɾ������ģ���ϵ����д���
		��    ����
		�� �� ֵ��*/
	virtual void DelWindow() = 0;

	/*	�������ܣ���ȡָ���������ڵ�ģ�����
		��    ����pWindow����ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��ģ������*/
	virtual int GetTMIndex(CWnd* pWindow) = 0;

	/*	�������ܣ����õ�ǰ�����ģ��
		��    ����iTMIndex�����õ�ǰ�����ģ�����
		�� �� ֵ��*/
	virtual void SetActiveTM(int iTMIndex) = 0;

	/*	�������ܣ�������ͬͬ��ģ���µ�������ģ��
		��    ����	iSrcTM��	Դģ������
					iSrcParent��Դģ���ϸ�����ĵ����
					iTagTM��	Ŀ��ģ������
					iTagParent��Ŀ��ģ���ϸ�����ĵ����
		�� �� ֵ��*/
	virtual void SwapChild(int iSrcTM, int iSrcParent, int iTagTM, int iTagParent) = 0;

	/*	�������ܣ���ʾģ��ѡ��Ի���
		��    ����
		�� �� ֵ��*/
	virtual void ShowSelTMDlg() = 0;

	/*	�������ܣ���ȡģ���Map���е�һ��ģ��
		��    ����pTM��ģ��ָ��
		�� �� ֵ��Map�����Ƿ��е�һ��*/
	virtual bool GetFirstTM(HPTM_Templet** pTM) = 0;

	/*	�������ܣ���ȡģ���Map������һ��ģ��
		��    ����pTM��ģ��ָ��
		�� �� ֵ��Map�����Ƿ�����һ��*/
	virtual bool GetNextTM(HPTM_Templet** pTM) = 0;

	/*	�������ܣ����޸�ģ�����ͶԻ����޸�ģ����Ƿ���������ģ��
		��    ����bImmediately���Ƿ���������ģ��
		�� �� ֵ��*/
	virtual void ChangeTMImmediately(bool bImmediately = true) = 0;

	/*	�������ܣ���ȡ��ǰѡ�е�ģ���
		��    ����	nTMIndex��		ģ�����
					nParentIndex��	�������
		�� �� ֵ��*/
	virtual void GetSelTMIndex(int& nTMIndex, int& nParentIndex) = 0;
};

HPTM_API HPTM_Manager* CreateHPTM_Manager(HPTM_TempletNotify& rNotify, unsigned long ulSessionID = 0);
HPTM_API void DestoryHPTM_Manager(void);
HPTM_API HPTM_Manager* GetHPTM_Manager(void);
HPTM_API void SetTMStyeXY(const int &nX, const int &nY);


class HPTM_API HPTVWall_Templet
{
public:
	HPTVWall_Templet(void){};
	virtual~ HPTVWall_Templet(void){};

public:
	virtual HWND GetSafeHwnd() = 0;
	virtual BOOL ShowWindow(int nCmdShow) = 0;
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;
	virtual void GetWindowRect(LPRECT lpRect) = 0;
	virtual BOOL IsWindowVisible(void) = 0;
	virtual CWnd* SetParent(CWnd* pWndNewParent) = 0;
	virtual BOOL Destroy() = 0;
	virtual BOOL EnableWindow(BOOL bEnable) = 0;

public:
	/*	�������ܣ���ģ����ָ����������Ӵ���
		��    ����	iParent��	ģ���ϸ���������
					pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWindow(int iParent, CWnd* pWindow) = 0;

	/*	�������ܣ���ģ���ϵ�һ�����еĸ�������Ӵ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWindow(CWnd* pWindow) = 0;

	/*	�������ܣ���ģ���ϱ�ѡ�еĸ�������Ӵ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWin2Select(CWnd* pWindow) = 0;

	/*	�������ܣ�ɾ��ģ���ϵĴ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual void DelWindow(CWnd* pWindow) = 0;

	/*	�������ܣ�ɾ��ģ���ϵ����д���
		��    ����
		�� �� ֵ��*/
	virtual void DelWindow() = 0;

	/*	�������ܣ�����ͬһ��ģ���µ�����������
		��    ����	iTMIndex��	ģ������
					iSrcParent��Դģ���ϸ�����ĵ����
					iTagParent��Ŀ��ģ���ϸ�����ĵ����
		�� �� ֵ��*/
	virtual void SwapChild(int iSrcParent, int iTagParent) = 0;

	/*	�������ܣ���ȡģ���ϸ�����ĸ���
		��    ����
		�� �� ֵ��ģ���ϸ�����ĸ���*/
	virtual int GetParentCount() = 0;

	/*	�������ܣ���ȡģ������ʹ�õĸ��������
		��    ����
		�� �� ֵ��ģ������ʹ�õĸ��������*/
	virtual int GetUsedTMCount() = 0;

	/*	�������ܣ���ȡ�����ģ�常�����ϴ��ڵ�ָ��
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��ģ���ϸ���������*/
	virtual int GetParentIndex(CWnd* pWindow) = 0;

	/*	�������ܣ���ȡ�����ģ�常�����ϴ��ڵ�ָ��
		��    ����iParentIndex��	ģ���ϸ���������
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual CWnd* GetChildWindow(int iParentIndex) = 0;

	/*	�������ܣ�����ģ��������
		��    ����tmSubType��	ģ��������
		�� �� ֵ��*/
	virtual void SetTMSubType(HPTM_TM_SUBTYPE TVWallType) = 0;

	/*	�������ܣ�����ģ��������
		��    ����
		�� �� ֵ��ģ��������*/
	virtual HPTM_TM_SUBTYPE GetTMSubType() = 0;

	/*	�������ܣ�����ģ�������ɫ
		��    ����
		�� �� ֵ��*/
	virtual void SetTMColor(COLORREF backColor, COLORREF frameColor, COLORREF textColor, COLORREF selectedColor,int nFontSize=250) = 0;
};

HPTM_API HPTVWall_Templet* CreateHPTVWall_Templet(HPTM_TempletNotify& rNotify, unsigned long ulSessionID = 0, CWnd* pParent = NULL, std::string flag = "CF_HPMC");


class HPTM_API HPTMCtrl_Manager
{
public:
	HPTMCtrl_Manager(void){};
	virtual~ HPTMCtrl_Manager(void){};

public:
	/* ��ȡָ�������͵ĸ�����ĸ��� */
	virtual int GetSubTypeWinCount(HPTM_TM_SUBTYPE nSubType) = 0;

	/*	�������ܣ����ģ��
		��    ����	rect��		ģ����ʾ����
					pParent��	ģ�常����ָ�루Ĭ��Ϊ�գ�
					tmType��	ģ�����ͣ�HPTM_TM_TYPE�ж����ֵ��Ĭ��ΪTM_TYPE_4X3��
		�� �� ֵ��ģ������*/
	virtual int AddTemplet(CRect &rect, CWnd* pParent = NULL, bool bIsPopWindow = false, 
		HPTM_TM_TYPE tmType = TM_TYPE_4X3, std::string flag = "CF_HPMC") = 0;

	/* ��ȡģ���� */
	virtual HPTM_Templet* GetTemplet(int iTMIndex) = 0;

	/* ɾ������ģ�� */
	virtual void DelTemplet(void) = 0;

	/* ɾ��ָ��ģ�� */
	virtual void DelTemplet(int iTMIndex) = 0;

	/* ��ȡģ��ĸ��� */
	virtual int GetTMCount(void) = 0;

	/* ɾ��ģ���ϵĴ��� */
	virtual void DelWindow(CWnd* pWindow) = 0;

	/* ɾ������ģ���ϵ����д��� */
	virtual void DelWindow(void) = 0;

	/* ��ȡָ���������ڵ�ģ����� */
	virtual int GetTMIndex(CWnd* pWindow) = 0;

	/*	�������ܣ�������ͬͬ��ģ���µ�������ģ��
		��    ����	iSrcTM��	Դģ������
					iSrcParent��Դģ���ϸ�����ĵ����
					iTagTM��	Ŀ��ģ������
					iTagParent��Ŀ��ģ���ϸ�����ĵ����
		�� �� ֵ��*/
	virtual void SwapChild(int iSrcTM, int iSrcParent, int iTagTM, int iTagParent) = 0;

	/* ��ȡ��ǰѡ�е�ģ��� */
	virtual void GetSelTMIndex(int& nTMIndex, int& nParentIndex) = 0;

	/* ѡ�е�ǰ���ڵ��ϲര�� */
	virtual void MoveUp(void) = 0;

	/* ѡ�е�ǰ���ڵ��²ര�� */
	virtual void MoveDown(void) = 0;

	/* ѡ�е�ǰ���ڵ���ര�� */
	virtual void MoveLeft(void) = 0;

	/* ѡ�е�ǰ���ڵ��Ҳര�� */
	virtual void MoveRight(void) = 0;
};

HPTM_API HPTMCtrl_Manager* CreateHPTMCtrl_Manager(HPTM_TempletNotify& rNotify, unsigned long ulSessionID = 0);
HPTM_API void DestoryHPTMCtrl_Manager(void);
HPTM_API HPTMCtrl_Manager* GetHPTMCtrl_Manager(void);
