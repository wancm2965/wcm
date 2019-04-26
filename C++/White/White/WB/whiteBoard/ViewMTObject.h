#ifndef _AVCON_WHITEBOARD_VIEWMTOBJECT_H_
#define _AVCON_WHITEBOARD_VIEWMTOBJECT_H_
#include "WBMTObject.h"
#include <MTProtocolType.h>


/*------------------------------------------------------------------------
������;��רһ������߿�Ĵ�������Ķ�̬ƽ�Ʋ���
------------------------------------------------------------------------*/
typedef struct SObjectCursor
{
	CWBMTObject *pObject;
	AVCON_SDK_MTTouch::MTCursor *pCursor;
	struct SObjectCursor(CWBMTObject *_pObject, 
		AVCON_SDK_MTTouch::MTCursor *_pCursor)
		: pObject(_pObject), pCursor(_pCursor){}
	struct SObjectCursor(struct SObjectCursor &cpy){
		pObject = cpy.pObject; pCursor = cpy.pCursor;}
}SObjectCursor;


class CWBMTObjectFramePan : public CWBMTObject
{
public:
	CWBMTObjectFramePan(HWND hWnd);
	~CWBMTObjectFramePan();
public:
	void PreAddMTCursor(MTCursor *tcur, CWBMTObject *pObject);
	void removeMTCursor(MTCursor *tcur);
	void updateMTCursor(MTCursor *tcur);
	//
	void ClearAllMTDrawMap();
	bool ClearMTDrawMap(const DWORD &dwTouchID);
private:
	//��������
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	//�����ƶ�
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);

private:
	//�����Ӷ����ϵĵ㣬���뵽����������С�
	void PopChildCursorAndPush2Frame();
private:
	///��תview����moveʱ��SendMoveSelectedObjOffset
	void SendMoveObjOffset(CPoint cpoffset,unsigned long ulEventId);
private:
	//������Ĵ��ھ��
	HWND m_hViewWnd;
	//ID����������ĵ�ӳ��
	std::map<unsigned long, SObjectCursor> *m_pMapSelectObj;
	//�Ƿ��д���
	bool m_bPanTogether;
	CPoint m_ptOffset;		//�ƶ�ʱ��¼ƫ����
};
/*------------------------------------------------------------------------
������;��רһ����װ������ͼƬ,���ƺͲ�������ť���¼�
------------------------------------------------------------------------*/
class CWBMTObjectButtonManager : public CWBMTObject
{
public:
	CWBMTObjectButtonManager(HWND hWnd);
	~CWBMTObjectButtonManager();

public:
	void SetAssociatedObject(CMTObject *pObject, const int &nButtonState);

private:
	//��������
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//�����ƶ�
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//��������
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);

private:
	DWORD m_dwTimeGetTime; //��¼��һ������ƶ�ʱ����ͬ����Ϣ��ʱ��
	//������Ĵ�������
	CMTObject * m_pAssociatedObject;
	//������Ĵ��ھ��
	HWND m_hViewWnd;
	///�����а�ť����(ImageButton, CtrlButton or IMediaCtrl),BUTTON_TYPE_IMAGEBUTTONΪImageButton,
	///BUTTON_TYPE_CTRLBUTTONΪCtrlButton, BUTTON_TYPE_INMEDIACTRLΪMediaCtrlButton,
	///BUTTON_TYPE_NONEΪ�Ȳ���ImageButtonҲ����CtrlButton
	int m_nButtonState;
};
/*------------------------------------------------------------------------
������;�����аװ�Ĵ������ݵķַ�����
------------------------------------------------------------------------*/
class CWBMTObjectManager : public CWBMTObject
{
protected:
	CWBMTObjectManager(HWND hViewWnd, CWBMTObject * pViewMTObject);
	~CWBMTObjectManager();
	//////////////////////////////////////////////////////////////////////////
	///����ģʽ
public:
	static CWBMTObjectManager *GetIns_();
	//��ʼ��
	static HRESULT Init(HWND hViewWnd, CWBMTObject * pViewMTObject);
	//����
	void Destroy();

public:
	//����ͬ��[�װ�ɾ�������ͬʱ����Ҫ�Ƴ��б��еĶ���,������ڵĻ�]
	bool RemoveMTObject(const CMTObject *pObject);
	bool RemoveMTObject(const DWORD &dwObjectIndex);
	//ͬ�ϣ���ҳ����ʱ��ɾ��ҳ���л�ҳ�ȡ�
	void RemoveAllMTObject();

private:
	///Ψһ����
	static CWBMTObjectManager *m_instance;
	//��ȡ�������ݵ�Э������
	static EProtocolType  m_gType;
	//////////////////////////////////////////////////////////////////////////
	///�̳���AVCON_SDK_MTTouch::CMTObject�ӿ�

public:
	//operation  for cursor updating message
	//��������ͨ���˺�������Ӧ������Ϣ���Ӷ�����CWhiteBoardView�е�touchDownHandler����
	virtual void addMTCursor(MTCursor *tcur);
	//��������ͨ���˺�������Ӧ������Ϣ���Ӷ�����CWhiteBoardView�е�touchUpHandler����
	virtual void removeMTCursor(MTCursor *tcur);
	//��������ͨ���˺�������Ӧ�ƶ���Ϣ���Ӷ�����CWhiteBoardView�е�touchMoveHandler����
	virtual void updateMTCursor(MTCursor *tcur);
	//���ô�����Ȩ
	BOOL SetMTAuthorize(BOOL);
	BOOL GetMTAuthorizeState() {return m_bMTAuthorize;}//��ȡ��ǰ��������Ȩ״̬
private:
	DWORD m_nFirstID;
	DWORD m_nEditTriangleTouchID;
	BOOL m_bMTUserCursor;	
	BOOL m_bIsTouchDustbin;
	//������Ȩ״̬
	BOOL m_bMTAuthorize;
	//��������
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//�����ƶ�
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//��������
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);

	virtual bool  BInRegion(const POINT &pt)const;
	/*---------------------------------------------------------------------------------------------
	����һ�����ֽڱ�ʾ���ƵĴ���[ETouchGestureType]�����ֽڱ�ʾ���Ƶ�С��[�綯̬�����е�]
	������: ���ֽڱ�ʾx�����꣬���ֽڱ�ʾy���������. ��λΪ���أ�����Ϊ��Ļ����
	----------------------------------------------------------------------------------------------*/
	virtual void OnCallbackGesture(const unsigned int &nMessage, 
		const unsigned int &nAppendMessage = 0);

	///�ڲ�����
private:
	bool AddObjectMap(const unsigned long &dID, CWBMTObject *pObject);
	bool RemoveObjectMap(const unsigned long &dID);

private:
	//
	XCritSec m_csMTObjectOperation;
	//��ǰ���ڲ���������б�
	std::map<unsigned long, CWBMTObject*> *m_pObjectMap;

	//��ʵ��������view��ָ��
	#ifdef SUPPORTMTGES
	CWBMTObject*m_pViewMTObject;
#else
	CMTObject * m_pViewMTObject;
#endif
	
	//��ʵ��������view�Ĵ��ھ��
	HWND m_hViewWnd;

	//����������봰�ڲ����йصĴ�������
	CWBMTObjectFramePan *m_pFrameMoveObject;
	CWBMTObjectButtonManager *m_pClickObject;

	///��ѡ�ľ���
	CRect m_rectSelect;
	///�����䱻�ƶ�ǰ�������
	CPoint m_ptPreMoveDustbin;
};

#endif
