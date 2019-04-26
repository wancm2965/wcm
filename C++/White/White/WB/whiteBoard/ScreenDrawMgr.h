#ifndef WHITEBOARD_SCREENDRAWMGR
#define WHITEBOARD_SCREENDRAWMGR

#include "ScreenDrawDlg.h"
#include "MouseDlg.h"
#include "AlphaDlg.h"

///������Ļ���ƹ�����
class CScreenDrawMgr : public CWBObject
{
public:
	friend class CScreenDrawDlg;
	friend class CMouseDlg;
	friend class CAlphaDlg;

	CScreenDrawMgr();
	~CScreenDrawMgr();

	///ʹ�÷���,����Ҫ��Ļ����ʱ�ȵ���CreateIns�ڵ���Init������Ҫ�ر���Ļ����ʱ����DestroyIns()���ɣ�GetIns������ø�������ָ��

	static CScreenDrawMgr*	GetIns();											///���ʵ��
	static bool				CreateIns();										///����ʵ��
	static void				DestroyIns();										///����ʵ��
	void					Init(CRect& rect);							///�����������㴰��

	///��Ϊ��CWBMTObjectManager��������İ��£����£�����ʱҪ�õ�CWBMTObject������ָ�룬������д����������

	virtual void			addMTCursor(MTCursor *tcur);						///�ý���������ת��ScreenDrawDlg����
	virtual void			removeMTCursor(MTCursor *tcur);						///�ý���������ת��ScreenDrawDlg����
	virtual void			updateMTCursor(MTCursor *tcur);						///�ý���������ת��ScreenDrawDlg����


	bool					PtInScreenDrawDlg(const POINT &pt)const;			///�����жϣ���CWBMTObjectManager::BinRegin����
	void					Invalidate();										///��Ļ���ƶԻ����ػ�,����ԭ����g_pScreenDrawDlg->Invalidate();
	void					UpdateWindow();										///������Ļ���ƶԻ���,����ԭ����g_pScreenDrawDlg->UpdateWindow();
	bool					IsTransparent();									///�Ƿ�͸��,����ԭ����g_pMouseDlg->m_isTransparentOrNot;
	bool					IsFullScreen();										///�Ƿ�ȫ��,����ԭ����g_pMouseDlg->m_isFullscreen;
	void					SetIsMinSize(BOOL bIsMinSize);						///��С��
	void					SetRichEditParent(CWnd **parent);					///�����ı���ĸ�����
	void					SetMouseDlgWindowPos();								///��MouseDlg�ŵ�ScreenDrawDlg������
	void					ClearTouchDrawMap(DWORD dwLayerIndex);				///���ScreenDrawDlg�е�DrawMap,����ԭ����g_pScreenDrawDlg->SendMessage(WM_SCREENCLEARMAP);
	void					GetMouseDlgScreenPosition(CPoint& pt);				///����MouseDlg�ϵĵ�ͻ�����ת��Ϊ��Ļ����,�൱��g_pMouseDlg->ClientToScreen(&pt);
	void					GetScreenDrawDlgClientPosition(CPoint& pt);			///����ScreenDrawDlg�ϵĵ����Ļ����ת���ɿͻ�����,�൱��g_pScreenDrawDlg->ScreenToClient(&pt)

	///����ȫΪ��ת����
	void					GuidesShift(BOOL isTransparent);					///Ч���л�
	void					DestroyScreenWindow();								///���ٴ���
	void					FullScreenShift(bool isFullScreen);					///ȫ��
	void					AlphaChange(const int alpha);						///�ı�͸����
	void					SetMoveIsNeedSendData( BOOL MoveIsNeedSendData );	///����ԭ����g_pScreenDrawDlg->SetMoveIsNeedSendData(MoveIsNeedSendData);
	BOOL					GetMoveIsNeedSendData();							///����ԭ����g_pScreenDrawDlg->GetMoveIsNeedSendData();
		
	CScreenDrawDlg* getScreenDrawDlg() {return m_pScreenDrawDlg;}
	void                    SetInvalidateway(bool buse){m_bChangeInvalidateway = buse;}
private:
	bool                   m_bChangeInvalidateway;                              ///����ˢ�µĲ�ͬ��ʽ
	static CScreenDrawMgr	*s_pScreenDrawMgrIns;								///����ָ��
	CMouseDlg				*m_pMouseDlg;										///MouseDlgָ��,���ڷ���ScreenDrawDlg���������������Ϣ,��Ϊ������ȫ͸���Ĳ����޷����������Ϣ
	CAlphaDlg				*m_pAlphaDlg;										///AlphaDlgָ��,���ڵ��ڴ���͸����
	CScreenDrawDlg			*m_pScreenDrawDlg;									///ScreenDrawDlgָ��,͸�����ڣ�������ʾ�ڴ����ϻ��Ƶ�ͼ��
};

#endif