//========================================================================
//������ģ��
//���ܣ���Ӧ������Ҫ��ģ��
//���ߣ�����
//ʱ�䣺2008-12-8
//˵����
//--------------------------------------------------------------------------

#pragma once
/*#include "HPDLG_Base.h"*/

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

	TM_SUBTYPE_3_T1_B2		= 50,
	TM_SUBTYPE_7_L1_R6,
	TVWALL_TYPE_TEST		= 100,
}HPTM_TM_SUBTYPE;

#define HPTM_MAX_WIN_COUNT	36
#define WM_CHILE_ADDED	(WM_USER + 4321)

typedef enum {
	CFSRC_TREE	= 0,
	CFSRC_MAP	= 1,
	CFSRC_AV	= 2,
	CFSRC_MEM	= 3,
	CFSRC_TM	= 99,
	CFSRC_TVTREE  = 100,
	SFSRC_TVSCREEN =101,
}CFSRC;

typedef struct _HPMC_DROP
{
	int						Type;	 			//0���������1����ͼ��2������Ƶ���Ŵ��� 3:������Ա��
	HWND				hWnd;    		//�϶�Դ�ľ�����������ϵĽڵ�HTREEITEM ���ߵ�ͼ�ľ������AV����ľ��
	int						AreaID;	 		//������ID�� ����������(0:����1���豸��2��ͨ��)
	char					MemberID[255];		//����������
	unsigned long	ChannelID;				//����������
}HPMC_DROP;


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
	virtual void HPTM_TempletLButtonUpNotify(int iTMIndex, int iParentIndex, unsigned long ulSessionID) = 0;
	virtual void HPTM_TempletRButtonDowmNotify(int iTMIndex, int iParentIndex, unsigned long ulSessionID) = 0; 
	virtual void HPTM_TempletAddWindowNotify(int iTMIndwx, int IParentIndex) = 0;
	virtual void HPTM_TempletDelWindowNotify(HWND hWnd, int nTMIndex, int nParentIndex) = 0;
	virtual void HPTM_TempletOnMouseHover(int iTMIndex, int iParentIndex, unsigned long ulSessionID){};
	virtual void HPTM_TempletOnMouseLeave(int iTMIndex, int iParentIndex, unsigned long ulSessionID){};
	virtual void HPTM_TempletOnEsc(){};
	/*	�������ܣ�ģ�崰�ڿ�ʼѡ�������¼�
	��    ����	iTMIndex��		ģ������
	iParentIndex��	�����ڵ����
	point:	��굱ǰλ��(�ͻ�������)
	ulSessionID��	���
	�� �� ֵ��*/
	virtual void HPTM_TempletBeginSelectNotify(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
	/*	�������ܣ�ģ�崰��ѡ�������¼�
	��    ����	iTMIndex��		ģ������
	iParentIndex��	�����ڵ����
	point:	��굱ǰλ��(�ͻ�������)
	ulSessionID��	���
	�� �� ֵ��*/
	virtual void HPTM_TempletOnSelectNotify(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
	/*	�������ܣ�ģ�崰�ڽ���ѡ�������¼�
	��    ����	iTMIndex��		ģ������
	iParentIndex��	�����ڵ����
	point:	��굱ǰλ��(�ͻ�������)
	ulSessionID��	���
	�� �� ֵ��*/
	virtual void HPTM_TempletEndSelectNotify(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
	/*	�������ܣ�ģ�崰��ѡ������ʱ�¼�(ÿ��100ms)
	��    ����	iTMIndex��		ģ������
	iParentIndex��	�����ڵ����
	point:	��굱ǰλ��(�ͻ�������)
	ulSessionID��	���
	�� �� ֵ��*/
	virtual void HPTM_TempletSelectTimerNotify(int iTMIndex, int iParentIndex,unsigned long ulSessionID){};
	virtual void HPTM_TempletOnMouseMove(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
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
	virtual BOOL SetWindowPos(const HWND pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;
	virtual void GetWindowRect(LPRECT lpRect) const = 0;
	virtual BOOL IsWindowVisible(void) = 0;
	virtual HWND SetParent(HWND pWndNewParent) = 0;
	virtual HWND GetParent() = 0;

public:
	//ģ�����
	/*	�������ܣ���ģ����ָ����������Ӵ���
		��    ����	iParent��	ģ���ϸ���������
					pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWindow(int iParent, HWND pWindow) = 0;

	/*	�������ܣ���ģ���ϵ�һ�����еĸ�������Ӵ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWindow(HWND pWindow) = 0;

	/*	�������ܣ���ģ���ϱ�ѡ�еĸ�������Ӵ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��*/
	virtual bool AddWin2Select(HWND pWindow) = 0;

	/*	�������ܣ�ɾ��ģ���ϵĴ���
		��    ����pWindow��	��ӽ�ģ��Ĵ���ָ��
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual void DelWindow(HWND pWindow) = 0;

	/*	�������ܣ�ɾ��ģ���ϵ����д���
		��    ����
		�� �� ֵ��*/
	virtual void DelWindow() = 0;

	/*	�������ܣ��鿴ָ���Ĵ������ĸ���������
		��    ����pWindow��	ָ���Ĵ��ھ��
		�� �� ֵ�������ڵ���ţ�δ�ҵ�����-1��*/
	virtual int FindWindow(HWND pWindow) = 0;

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
	virtual int GetParentIndex(HWND pWindow) = 0;

	/*	�������ܣ���ȡ�����ģ�常�����ϴ��ڵ�ָ��
		��    ����	iParentIndex��	ģ���ϸ���������
		�� �� ֵ����ӽ�ģ��Ĵ���ָ��*/
	virtual HWND GetChildWindow(int iParentIndex) = 0;

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
	virtual bool IsTempletFullScreen() = 0;

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

/*  ��������: ����ʱʹ�� x.t
		��    ��: bChangeScreen �Ƿ�����
		�� �� ֵ: 
	*/
	virtual void SetWndText(int nWndID, LPCTSTR strWndText) = 0;

	/*	�������ܣ���ȡģ����ָ�������ڵĴ�С
	��    ����	iParent��	ģ���ϸ���������
	�� �� ֵ��*/
	virtual void GetWndRect(int iParent, int &x, int &y, int &cx, int &cy) = 0;
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
	virtual int AddTemplet(RECT &rect, HWND hParent = NULL, bool bIsPopWindow = false, 
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
	virtual void DelWindow(HWND pWindow) = 0;

	/*	�������ܣ�ɾ������ģ���ϵ����д���
		��    ����
		�� �� ֵ��*/
	virtual void DelWindow() = 0;

	/*	�������ܣ�����������ʾѡ�б߿�
		��    ����bAwaysShow��Ĭ��Ϊfalse�������д���ʱ����ʾѡ�У���Ϊtrue��������ʾ
		�� �� ֵ��*/
	virtual void AwaysShowSelectedFrame(bool bAwaysShow) = 0;

	/*	�������ܣ���ȡָ���������ڵ�ģ�����
		��    ����pWindow����ӽ�ģ��Ĵ���ָ��
		�� �� ֵ��ģ������*/
	virtual int GetTMIndex(HWND pWindow) = 0;

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

	//�������в���
	virtual void SetEnable(bool bEnable = true) = 0;

	/*	�������ܣ�ѡ��ģ�崰��
	��    ����	nTMID��		ģ�����
				nParentID��	�������
	�� �� ֵ��*/
	virtual void SelectWnd(int nTMID, int nParentID) = 0;

	/*	�������ܣ�����ģ�崰���Ƿ�����϶�
	��    ����	nTMID��		ģ�����
	nParentID��	�������
	bDrag		trueΪ���϶�,falseΪ�����϶�
	�� �� ֵ��*/
	virtual void SetWndDrag(int nTMID, int nParentID, bool bDrag) = 0;

	/*	�������ܣ����ģ�崰���Ƿ�����϶�
	��    ����	nTMID��		ģ�����
	nParentID��	�������
	�� �� ֵ��*/
	virtual bool GetWndDrag(int nTMID, int nParentID) const = 0 ;

	/*	�������ܣ�����ģ�崰���Ƿ�Ҫ������������
	��    ����	nTMID��		ģ�����
	nParentID��	�������
	bZoom		TRUEΪҪ��������,FALSEΪֹͣ����
	�� �� ֵ��*/
	virtual void SetWndZoom(int nTMID, int nParentID, BOOL bZoom) = 0;

	/*	�������ܣ����ģ�崰���Ƿ���������
	��    ����	nTMID��		ģ�����
	nParentID��	�������
	�� �� ֵ��*/
	virtual BOOL GetWndZoom(int nTMID, int nParentID) const = 0 ;
};

HPTM_API HPTM_Manager* CreateHPTM_Manager(HPTM_TempletNotify& rNotify, unsigned long ulSessionID);
HPTM_API HPTM_Manager* GetHPTM_Manager(unsigned long ulSessionID);
HPTM_API void DestoryHPTM_Manager(HPTM_Manager* pHPTM_Manager);