#pragma once
#include "stdafx.h"
#include "atltypes.h"
#include <vector>
#include <map>
#include <list>
#include <queue>
#include "common\WBCtrl.h"
#include "common\WBData.h"
#include "WBResourceDlg.h"
#include "WBResFunc.h"
#include "WBFunctionDlg.h"
#include "WBToolbarCtrlDlg.h"
#include "WBToolbarDlg.h"
#include "WBPromptFontSetDlg.h"
#include "WBRichEdit.h"
#include "UndoRedo.h"
#include "WBEagelEye.h"
#include "WBAboutDlg.h"
#include "WBOptionDlg.h"
//#include "WBResourceManager.h"
#include "DlgPrintLoadingMsg.h"
#include "Transparent_Dlg.h"
#include "WaterDlg.h"
#include "WBMTDrawObjectTally.h"
#include "LimitLocalObjOperate.h"
#include "MyDropTarget.h"
#include "WBRollCallDialog.h"
#include "WBRollCallShowDlg.h"
#include "ButtonCloseDlg.h"
#include "WBSavePictureDlg.h"
#include "GeometricRecognizer.h"
#include "PointRetain.h"
class CMedioVideoScreenDlg;

extern bool g_isLoacalWB;



int ReadMediaBitrate();//��ȡ��Ƶ����
void WriteMediaBitrate(int iValue);
int ReadMediaControlBitrate();//��ȡ��Ƶ����
void WriteMediaControlBitrate(int iValue);

#define MOUSEEVENTF_FROMTOUCH (0xFF515700)

#define UWM_TRACKPOPUPMENU					(WM_USER+250)	//�Զ���ĵ����˵���Ϣ
#define	UWM_CREATE_RESOURCE_DLG				(WM_USER+2786)	//���ʹ�����Դ�������Ϣ

int ReadBroadCastStatus(); //��ȡ�㲥���״̬��
void WriteBroadCastStatus(int nStatus); //д�������ļ�

enum DICOM_ERROR_CODE
{
	DICOM_TOBMP_SUCCEES =1,      //ת���ɹ�
	DICOM_ERROR_OPENFAILED = -1 ,//�ļ���ʧ��
	DICOM_FILEWRITEFAIL = -2  ,    //�ļ�ת��ʧ��
	FREEDISK_NOENOUGH = -3 ,  //�����ļ�ʱʣ����̿ռ䲻��
	NO_DICOM_FIEL = -4       //�򿪵Ĳ���dicom�ļ�
};
//12-25��� jin�ڿ�ݼ���ʹ��
#define  IsCTRLPressed() ((GetKeyState(VK_CONTROL) & (1<<(sizeof(SHORT)*8-1))) != 0)
//denghong
#define REFRESH_INTERVAL_TIME 40

class CTransparent_Dlg;
typedef std::map<string,CTransparent_Dlg*>MapCursorDlgPointer;  //�û��㲥�Ĺ���͸���Ի���ָ��
typedef MapCursorDlgPointer::iterator IT_MapCursorDlgPointer;

class CWaterDlg;
typedef std::map<string,CWaterDlg*>MapWaterDlgPointer;  //�û��㲥�Ĺ���͸���Ի���ָ��
typedef MapWaterDlgPointer::iterator IT_MapWaterDlgPointer;
// CWhiteBoardView ��ͼ


typedef std::map<DWORD,HPMC_DROP*>MapDropCamInfo;
typedef MapDropCamInfo::iterator IT_MapDropCamInfo;//����ͷ��ק��Ϣ�洢����

typedef std::vector<DWORD> VECTORSPECPIC; //����ͼƬ����ID
typedef VECTORSPECPIC::iterator IT_VectorSpepic;
class CDrawCtl;
class CWBSetSquare;
class CWBGlobalMediaCtrl;
class CWhiteBoardView : public CScrollView , public CWBMTObject
{
	DECLARE_DYNCREATE(CWhiteBoardView)
protected:
	CWhiteBoardView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWhiteBoardView();

public:
	HWND GetHWND();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	///
	afx_msg LRESULT OnTrackPopupMenu(WPARAM wParam, LPARAM lParam);
	//�˵���Ӧ����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void ViewWindowFinalTreat();//��������������֮ǰ�Ĵ��������ڰװ�ĵ���˳�����ȵ��ô˺����ٵ���OnDestroy�����������~CWhiteBoardView()

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	DECLARE_MESSAGE_MAP()


	/*-----------------------------------------------------------------------------------
	//�˵��ϵ�ͼƬ����
	------------------------------------------------------------------------------------*/
public:
	void LoadPopupMenuBitmap();
	//ɾ����ǰѡ��Ķ���
	DWORD DeleteSelectedObjects();
	void CreateObjMenu(DWORD dwLayerIndex);	//�����˵�
	BOOL TranslateBitmapSize(  
		IN HDC hCurScreemDC,   //��ǰ��ĻDC����ͨ��GetDC���  
		IN HBITMAP hBmpSrc,    //ԭBITMAP���  
		IN int nSrcWidth,      //ԭBITMAP�Ŀ��  
		IN int nSrcHeight,     //ԭBITMAP�ĸ߶�  
		OUT HBITMAP &hBmpDst,  //�ı��С���BITMAP�ľ��  
		IN int nDstWidth,      //�ı��С���BITMAP�Ŀ��  
		IN int nDstHeight      //�ı��С���BITMAP�ĸ߶�  
		);
	bool TranslateBitmapSizeToFitXP(CBitmap *pBitmap);	//�ı�CBitmap�Ĵ�С������ӦXP,��С�˵�ͼ���Ա���ͼ��ȫ����ʾ
private:
	CBitmap m_CheckedBmp;  //ѡ��״̬��
	CBitmap m_CutBmp;  //����
	CBitmap m_InfiniteCloneBmp;    //���޿�¡
	CBitmap m_UnLockBmp;    //����
	CBitmap m_LockBmp;    //����
	//��Ƶ
	CBitmap m_MoviePlayBmp;    //����
	CBitmap m_MoviePauseBmp;    //��ͣ
	CBitmap m_MovieStopBmp;    //ֹͣ
	
	CBitmap m_MediaCatchFrameBmp;   //��ͼ
	CBitmap m_MediaSetmuteBmp;      //����
	CBitmap m_MediaNoSetmuteBmp;      //����
	//�ĵ�
	CBitmap m_DocPrePageBmp;    //��һҳ
	CBitmap m_DocFirstPageBmp;   //��ҳ
	CBitmap m_DocNextPageBmp;      //��һҳ
	CBitmap m_DocLastPageBmp;      //ĩҳ
	CBitmap m_DocPrintCurBmp;		//��ӡ��ǰҳ
	CBitmap m_DocPrintAllBmp;		//��ӡ����ҳ

	//Dxf
	CBitmap m_DxfZoomInBmp;  //�Ŵ�
	CBitmap m_DxfZoomOutBmp; //��С
	CBitmap m_DxfDragBmp; //��ק

	CBitmap m_SelectAllBmp;  //ȫѡ
	CBitmap m_UnselectBmp;      //ȡ��ѡ��
	CBitmap m_SckdBmp;           //�����̶�
	CBitmap m_EqualScaleZoomBmp;    //�ȱ�����
	CBitmap m_DeleteBackgroundBmp;    //ɾ������ͼƬ
	CBitmap m_SetAsBackgroundBmp;    //��Ϊ����
	CBitmap m_EditBmp;    //�༭
	CBitmap m_CopyBmp;  //����
	CBitmap m_DeleteBmp;      //ɾ��
	CBitmap m_DeleteAllBmp;  //ɾ��ȫ��
	CBitmap m_PasteBmp;    //ճ��
	CBitmap m_CloneBmp;    //��¡
	CBitmap m_CombineBmp;    //���
	CBitmap m_UnCombineBmp;    //ȡ�����
	CBitmap m_TopLayerBmp;      //����
	CBitmap m_PreLayerBmp;    //��һ��
	CBitmap m_NextLayerBmp;    //��һ��
	CBitmap m_BottomLayerBmp;    //�ײ�
	CBitmap m_AttributeBmp;    //����

	CBitmap m_Deasil45Bmp;        //˳ʱ��45��
	CBitmap m_Deasil90Bmp;        //˳ʱ��90��
	CBitmap m_Widdershins45Bmp;   //��ʱ��45��
	CBitmap m_Widdershins90Bmp;   //��ʱ��90��
	CBitmap m_Rotation180Bmp;     //��ת180��
	CBitmap m_HirizontalBmp;      //ˮƽ����
	CBitmap m_VerticalBmp;        //��ֱ����

	CWBDocument *pDoc;

	/*-----------------------------------------------------------------------------------
	//���� ������
	------------------------------------------------------------------------------------*/
public:

	///�̳���AVCON_SDK_MTTouch::CMTObject
	//��������
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//�����ƶ�
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//��������
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	///�����ж�
	virtual bool BInRegion(const POINT &pt)const;
	/*---------------------------------------------------------------------------------------------
	����һ�����ֽڱ�ʾ���ƵĴ���[ETouchGestureType]�����ֽڱ�ʾ���Ƶ�С��[�綯̬�����е�]
	������: ���ֽڱ�ʾx�����꣬���ֽڱ�ʾy���������. ��λΪ���أ�����Ϊ��Ļ����
	----------------------------------------------------------------------------------------------*/
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage);
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const GestureCombinePar& gesPar);

	//����ϵͳ�ģ������κδ���
	afx_msg LRESULT OnGesture(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);

	///add by aolingwen 2012.11.16
	///���ڴ�����ѡ
	void SetRectSelect(CRect rectSelect);
	///���ڴ���ʹ��������
	CWBDustbin* GetDustbin(){return m_pWBDustbin;}
	void MoveObjToDustbin(CPoint point,int id =0 );
	void SetObjectoutDustbin();
	void SetDustbinTouchID();
	///���ڴ���ʹ�÷Ŵ�
	CWBMagnifier* GetWBMagnifier(){return m_pWBRectMaqn;}
	void SetTouchEventId(unsigned long);
	//���ڴ������޿�¡
	BOOL MTInfiniteClone();

	//�װ�����ģ����ק��U7��
	void SendDropNotify(HPMC_DROP *pBuffer);
	//�ⲿ��ק��Ƶ������ͷ���װ�
	void SendDropNotifyToU7(HPMC_DROP *pBuffer,bool bflag = true);

	void SendDropNotifyU7Message(HPMC_DROP *pBuffer,bool bflag = true);

	bool OpenCamaraFromU7(char *pbuff,int nLen);//����ͷ��u7��ק����
	bool OpenCamaraFromU7(MediaCamaraU7Data *pMediaCamaraU7Data);//����ͷ��u7��ק����


	bool OpenMediaFromU7(const char *pbuff);//��Ƶ
	CWBCamera *GetCamaraObjU7();
	void SetCamaraObjU7(CWBCamera *carama);

	void SetUserCameraName(string st) {m_strUserCurCameraName = st;}
	unsigned long GetchannelID() {return m_DropChanelID;}

private:
	CWBCamera *m_pCamaraObj;
	std::string m_strUserCurCameraName;
	unsigned long m_channelID; 

	unsigned long m_DropChanelID;
	//������wm_touch��Ϣ
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;


	unsigned long m_ulTouchEventId;

private:
	CPoint m_pDownPoint;
	CPoint m_pMovePoint;
	///����ɾ������ͷ
	int m_nCameraObjId;	
	///end
	//���ڴ�����ͼ��������add by huanglongquan 2012��12��18�� 
	CWBMTDrawObjectTally m_MTDrawObjectTally;

public:
	/*-----------------------------------------------------------------------------------
	//��Դ��
	------------------------------------------------------------------------------------*/
	//afx_msg HRESULT OnReceiveDragResouce(WPARAM wPar, LPARAM lPar);
	afx_msg HRESULT OnReceiveTouchResouce(WPARAM wPar, LPARAM lPar);
	CString GetResFilePath(){return m_sLastDragFileURL;}
	void	SetResFilePath();


	/*-----------------------------------------------------------------------------------
	//�������
	---------------------------------------------------------------------------------*/
public:
	// ��ù��ܶԻ���
	CWBFunctionDlg *GetWBFunctionDlg();
	//�ж��Ƿ���Ԥ�������
	BOOL IsOpenPreviewPanel();
	//�ж��Ƿ�򿪹������
	BOOL IsOpenFunctionDlg();
	//������ѡ�����͸����
	void UpdateObjAlpha();
	//������ѡ�������ɫ
	void UpdateObjColor();
	//������ѡ����Ŀ��
	void UpdateObjLineWidth();
	//������ѡ��ѡ�����ͼ��style
	void UpdateObjIcoStyle();
	//������ѡ���������
	void UpdateObjLineType();
	//������ѡ�����������
	void UpdateObjSolidCheck();
	//������ѡ����������ɫ
	void UpdateObjSolidColor();
	//������ѡ����ļ�ͷ
	void UpdateObjLineArrow();
	//���¶������������
	void UpdateObjFontFace();
	//���¶���������С
	void UpdateObjFontSize();
	//���¶��������ֶ�
	void UpdateObjFontBold();
	//���¶����������б
	void UpdateObjFontItalic();
	//������ѡ������»���
	void UpdateObjUnderline();
	//������ѡ���������Ű�
	void UpdateObjVertical();
	//���¶�����ı�����
	void UpdateObjTextStr();
	//�������Ʋ������ԣ��ĵ�����Ƶ��ͼ��
	void UpdateMTOperate();
	//�����������
	void OpenAttributeDlg();
	//ɾ���ı��༭��
	void DeleteEdit();
	void DestroyRichWindow();
	void SetRichEditRect(CRect &rc);
	CRect GetRichEditRect();

	//�ϲ������ĵ�
	bool CombineFile(CWBFile* dst, CWBFile* src);
private:
	CRect m_RichEditRect;
	bool  m_bIsconfernce;
private:
	//�������Ի���
	CWBFunctionDlg	*m_pWBFunctionDlg;
	/*-----------------------------------------------------------------------------------
	//ͬ������
	------------------------------------------------------------------------------------*/
public:
	void OnNETEC_NodeReceivedFromRouter(const char* pData,int nLen);   //�����յ���ͬ�����ݴ��뻺�������
	//ʹ�ö�ʱ��������յ���ͬ������
	static void CALLBACK  OnCallbackData(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	void _OnCallbackData();


	//ͬ��
	/************************************************************************/
	//һ�㷢�͵���������Ч���ݣ��������½�����ͼ�����Ա��浽¼�� ������undoredo��

	void SendPacketData(char *pData, int nLen, bool bToNet = true); //����ͬ������

	//�������ݵ��װ�dll�ⲿ�����������װ��ڲ�undoredo��¼���ļ� 
	void SendPacketInfoOnlyToNet(char* pData,int nLen,bool bToNet = true); //���ݲ����浽
	//���� bToNet ��false��ʾ�ⲿ���ðװ�ػ������  true �����ݷ��͵����ϣ�����������ȥʵ��ͬ������
	/************************************************************************/

	void ProcessPacketData(const char *pData, int nLen); //������յ���ͬ������
	void ProcessObj(XMemIni xmlObj, int nCmdType = CMD_ADD_ATTRIBUTE); //������յ��ö���
	void ProcessRemoveObj(DWORD dwLayerIndex, unsigned long ulEventID); //����ɾ������
	void ProcessObjLayer(XMemIni xmlObj); //�������ı�ͼ��
	void ProcessMediaOperation(XMemIni xmlObj);	//�����ý��Ĳ���
	void GetAllObj(); //�������������ȫ������
	void GetUserColor();//�����û���ɫ
	void SendAllObj(); //��������������ж���
	void RemoveAllObj(); //�����������ɾ�����ж���
	void RemovePageAllObj(int nDelPageId);//�����������ɾ��ҳȫ����Ϣ
	void Broadcast_Data(XMemIni xmlobj); //�㲥��ʱ����
	void ProcBroadcast_Data(const char*pData,int nlen); //���������㲥����������,����ϵͳ�Ļ����
	void OnBroadcast_Data(XMemIni xmlObj); //����㲥��ʱ����
	void AddObj(XMemIni xmlObj); //��Ӷ���
	void RemoveObj(DWORD dwObjLayerIndex); //ɾ������
	void RemoveDelCombineObj(DWORD dwObjLayerIndex);//ɾ����϶���
	void RemoveMediaObj(DWORD dwObjLayerIndex); //ɾ��������Ƶ����
	void RemoveDelComCurve(DWORD dwObjLayerIndex);//ɾ���ںϵ��ĵ���ͼƬ�е�����
	void ChangeObj(XMemIni xmlObj, bool bchange = false); //�޸Ķ���
	void ChangeComObj(CWBObject *pObj); //�޸���϶���
	void ChangeObjLayer(XMemIni xmlObj); //�޸Ķ���ͼ��
	void MediaOperation(XMemIni xmlObj);	//��ý��Ĳ������ĵ�����Ƶ��FLASH�ȣ�
	//void ProcessGesture(CUIntArray &aLayerBox);	//���������޸ĵĶ���
	void Undo();//���ͳ����������������������ʵ�ֳ�������
	void Redo();//���������������������������ʵ����������
	void ClearUndo(); //������ճ����������ʹ������Ч
	void MoveObj_Realtime_Synchronizatio(XMemIni xmlobj); //����ʵʱͬ�����ú������ܶ����ƶ�ʱ��Ĺ㲥������ƫ�����������ƶ����صĶ���

	void SendLocalMessage(eCommend_Type eComType,BOOL bFullScreen = TRUE); //����/�˳�ȫ����������Ϣ��
	void ChangeFullScreen(BOOL bFullScreen = TRUE); //����ȫ��������Ϣ
	void ProcessAttiObj(XMemIni xmlObj); //����IDΪ0�Ķ�����Ϣ

	void ProcessBroadcastAttiObj(XMemIni xmlObj); //�㲥ͬ����Ϣ

	void SendChangePage(); //���ͷ�ҳͬ��
	void SendScrollDraw(); //���͹���ͬ��
	void SendDeletePage(XMemIni xmlobj);//����ɾ��ҳͬ��
	void SendInsertPage(XMemIni xmlPage); //���Ͳ���ҳ
	void SendExchangePage(XMemIni xmlpage);

	void ProcessChangePage(int nPageIndex);  //����ҳͬ����Ϣ
	void ProcessDeletePage(XMemIni xmlObj, int nCmdType = CMD_DEL_PAGE);
	void ProcessInsertPage(XMemIni xmlObj, int nCmdType = CMD_INSERT_PAGE);
	void ProcessExchangPage(XMemIni xmlObj, int nCmdType = CMD_CHANGE_PAGE);


public:
	void processWBDTSData(AVDataInPacket& in); //���粻��ͨ��ʱ���������صķ�����������������

	void BroadcastUserCursor(CPoint pt,string str_username); //�㲥�û���������λ��
	void RejectBroadcastUserCursor(string str_name);//ȡ���㲥�û����
	void OnRejectBroadcastUserCursor(string str_name); //����ȡ���㲥�û����
	void ReJectAllBroadcastUserCursor(); //�������еĹ㲥���
	afx_msg LRESULT ProcessMTUserCursor(WPARAM wParam,LPARAM lParam);

	void SetulIcoCurEvent(unsigned long ul) {m_ulIcoCurEvent = ul;}
	void SetdwIcoCurId(DWORD id) {m_dwIcoCurId = id;}
	DWORD GetdwIcoCurId() {return m_dwIcoCurId;}

	CTransparent_Dlg* GetTransparentDlg(){return m_pTransparent_dlg;}
	BOOL GetBroadcastStatus(){return m_bIsneedjionBroadcast;}
	void SetBroadcastUserCursor(bool IsBroad); //���ù㲥
	void HideUserCursor();
	std::string GetUserAddID() {return m_strUserID;}
	bool GetCameraUNDO() {return m_bIsCameraUNDO;}
	void ShowOrHideMediaCarema(UINT Index);
	void CloseBroadcastUserCursor(); //Ȩ�޿��ƹر�ˮ���ƺ͹��
private:
	bool m_bIsCameraUNDO;//����ͷ��������
	std::string m_strUserID;//�������ͷ����ԱID
	CWBDataModel m_WBDataModel;
	bool		  m_bBroadcastUserCursor; //�㲥�û�����ж�
	CTransparent_Dlg*  m_pTransparent_dlg;//���ĶԻ���
	MapCursorDlgPointer m_MapCursorDlgPointer;
	BOOL         m_bIsneedjionBroadcast;
	BOOL                m_bEraseLineUp;//�����������
	unsigned long	 m_ulIcoCurEvent;//ͼ�����ߴ��ļ������¼�
	DWORD m_dwIcoCurId;//ͼ�����ߴ��ļ����ض���id
private:
	XBufferPool m_tXBufferPool; //�������ݻ������
	UINT_PTR	m_nTimerID;//��������ͬ�����ݻ�����ж�ʱ������ID


	/*-----------------------------------------------------------------------------------
	//�Ҽ��˵������
	------------------------------------------------------------------------------------*/
public:
	std::vector<string> m_pCaremaPicture;
	//���õ�ǰ���弰�ı���ɫ
	void SetTextFontColor(const LOGFONT &lfFont, COLORREF clrText);
	//�õ���ǰ����
	LOGFONT GetCurFont();
	//�õ���ǰ������ɫ
	COLORREF GetFontColor();
	//�õ���ǰ�༭��
	CWBRichEdit* GetCurEdit();
	void SetCurEdit(CWBRichEdit *pEdit = NULL);
	//���ñ༭����������ʽ luowei
	void SetEditCharFormat(CWBRichEdit *pEdit);

	//�õ���ʱ�������öԻ���
	CWBPromptFontSetDlg *GetPromptFontSetDlg();

	CWBOptionDlg *GetWBOptionDlg();
	void DeleteFuncPad();
	CRect GetEditRect(); //�õ��༭�����
	void SetEditRect(CRect rectText);//���ñ༭�����
	void SetEditTopLeft(CPoint ptTopLeft); //���ñ༭�����Ͻǵĵ�
	BOOL GetIsEditing();
	CWBObject *GetEditObj();
	//���õ�ǰ����
	void SetCurTextFont();
	//���õ�ǰ�ı���ɫ
	void SetCurTextClr();
	HBITMAP GetSrcBit(HDC hDC,DWORD BitWidth, DWORD BitHeight);
	//ͼƬ���ĵ�λͼ
	HBITMAP GetPictureBit(HDC hDC, DWORD BitTop, DWORD BitBottom, DWORD BitLeft, DWORD BitRight);

	void AllUnLock(); //���ȫ������

	CRect GetEditTextRect(); //�õ��༭�ı���Ĵ�С
	BOOL Copy(); //���Ƽ���ʱ�Ŀ���
	void CopyObject(); //���ƶ���
	void CutObject(); //���ж���
	BOOL PasteObject(); //ճ������
	BOOL CloneObject(); //��¡����
	void InfiniteCloneObj();//���޿�¡
	CPoint LeftTop(); //���� ���ƻ���еĶ���ָ���б� ���������ϵĵ�
	BOOL GetIsCopy(); //�õ��Ƿ�����
	BOOL GetIsCut(); //�õ��Ƿ������

	void SetCurAnnotationsLayerIndex(DWORD dwLayerIndex); //���õ�ǰѡ��ı�ע�����ͼ���
	void SetCurTrapeziumLayerIndex(DWORD dwLayerIndex); //���õ�ǰѡ������ζ����ͼ���
	//��ת����Ƕȣ�˳ʱ��Ϊ������ʱ��Ϊ��
	void RotateByAngle(int angle);	
	//ˮƽ����ֱ��ת��ˮƽΪ0�ȣ���ֱΪ90��
	void  RotateByHV(int HVAngle);

	void SetCurrentCursorPos();//����view������ͷ������
	CPoint GetCurrentCursorPos();//�õ�view������ͷ������

	//ͼ�����
	void UnderLayer();
	void SuperLayer();
	void TopLayer();
	void BottomLayer();

	//�ж��Ƿ�Ϊ�ĵ����ע��
	BOOL AddComponentToObj(CWBObject *pAddObj, unsigned long ulEventID);
	//�ж��Ƿ�Ϊ�ĵ����ע�ͣ�ѡ����б��еĶ���
	void AddComListToObj();

	//���ڶԻ���
	void OpenAboutDlg();
	void AddResToWBView(DRAW_STATE nDrawStatus,CString strResPath,RES_INPUT_METHOD nAddResMethod);//view�����ѡ�е���Դ
	void UpdateViewBackGround(CWBObject *pObj);//����view����
	void SetCloneMedia(BOOL CloneMedia) {m_bIsCloneMedia = CloneMedia;}
	BOOL GetCloneMedia() {return m_bIsCloneMedia;}

	bool AllowDeleteObj(DWORD dwObjId,unsigned long EventID); //�Ƿ�ɾ�������� ���ڲ����Ķ���
	void BroadCostReleastLimitObjs();//�������ͷ��������ڲ����Ķ��󣬣����ʵʱͬ����ʱ�����ڲ����Ķ��������Ƶģ�����ʱ�򻥶���һֱû�н��ܵ��ͷŵ���Ϣ���ö���Ͳ��ܲ���)
	void OnReleaseLimitObjs();//�����ͷ�������Ϣ
	void SetPastePt(CPoint pt);
	void SetColorPaletteDlg(bool ColorPaletteDlg) {m_bIsColorPaletteDlg = ColorPaletteDlg;}
	bool GetColorPaletteDlg() {return m_bIsColorPaletteDlg;}
	
private:
	bool m_bIsColorPaletteDlg;//���ñ���ʱ������ɫ����ȦȦ��ʾ����
	BOOL m_bIsCloneMedia;//��¡ʱ����Ƶ������λ�ò�ƫ��
	BOOL m_bIsEditing; //�Ƿ��ڱ༭�ı�
	CWBObject *m_pEditObj; //��¼���ڱ༭���ı���ָ��

	DWORD m_dwCurObjLayerIndex; //��ǰ�����ͼ���
	DWORD m_dwCurSelectObjLayerIndex; //��ǰѡ������ͼ���
	DWORD m_dwCurAnnotationsLayerIndex; //��ǰѡ��ı�ע�����ͼ���
	DWORD m_dwCurTrapeziumLayerIndex; //��ǰѡ������ζ����ͼ���
	CRect m_rectSelect; //ѡ���
	DWORD m_dwCurOperationObjId; //��ǰ�����Ķ���Ψһ��ʶid  huanglongquan add by 2012��12��14�� ���ڻ�����ɾ��ȫ�������ʱ�򣨻�������ɾ�����������Ķ��� ���ز�ɾ���ö���
	//denghong
	CPoint m_moveFirstPoint;
	std::vector<CWBObject *> m_pObjCopyList; //���ƻ���еĶ���ָ���б�
	BOOL m_bIsCopy; //�Ƿ�����
	BOOL m_bIsCut; //�Ƿ������
	CPoint m_ptPaste; //ճ��ʱ�����λ��

	//��ӦUP��Ϣʱ���Ƿ��Ǵ���״̬��TRUE�Ǵ���״̬��FALSE�����״̬
	BOOL m_bIsTouch;

	//����������ر���
	//	int m_toolbarPosition; //��ʼֵ��-1��ͬʱĬ��������ߣ����Ϊ1�򹤾�������view���ұ�
	BOOL m_bSDIsTransparent; //��Ļ��ͼ�Ƿ�����
	//	BOOL m_bSDIsFullScreen; //��Ļ��ͼ�Ƿ�ȫ��
	int m_nSDAlpha; //��Ļ��ͼ͸����

	BOOL m_bIsAnnotations; //�Ǳ�ע�����ı�����עTURE���ı�FALSE

	CWBOptionDlg *m_pWBOptionDlg;

	//���ڼ�¼�༭��ľ���
	CRect m_rectText;
	CPoint m_topLeft;

	/*-----------------------------------------------------------------------------------
	//���������
	------------------------------------------------------------------------------------*/
public:
	//����ڵ���
	BOOL addBaffle();
	//��ӹ������
	BOOL addFunctionPanel();
	//����TOOLBAR�Ĵ��ھ��
	void SetToolbarHWND(HWND hTOOLBARWnd);
	//�۹��
	void AddSpotlight();
	HWND GetSpotLightHWND();
	BOOL CreateSpotlight(CRect screenRect = NULL);
	void CloseSpotlight();
	void SendSpotlight(); //���;۹��ͬ��
	//ӥ�۹���
	void EagleEyeFunction();
	BOOL CreateEagleEye();
	void EagleEyeSetposAndShow(CRect rect);
	//���ӥ�۶Ի���
	CWBEagelEye *GetWBEagleEyedlg();

	//���ηŴ�
	void AddMagnifier();  //�Ŵ�
	void CloseRectMagn();   //�رշŴ�
	void SendMagn();  //���ͷŴ�ͬ��
	BOOL MouseDownMagn(const CPoint& point);
	BOOL MouseMoveMagn(const CPoint& point);
	BOOL MouseUpMagn(const CPoint& point);	
	//void SetMaqnState(BOOL bMaqnState); //���÷Ŵ�״̬
	void AddRECScreen();
	BOOL DrawOnScreen(const CRect *rect = NULL);      //��Ļ��ͼ
	BOOL FullScreenShift(bool isFullScreen);    //ȫ�����л�
	BOOL DestroyScreenDraw(void);               //������Ļ��ͼ
	void OpenCtrlDlg();
	BOOL ShiftGuise(BOOL isTransparent);  
	void SetSDAlpha(int nAlpha);

	void SendScreenDraw(); //������Ļ��ͼ��Ϣ
	BOOL SaveSCDraw(char *pBuf, DWORD& dwLen); //������Ļ��ͼ��Ϣ
	BOOL LoadSCDraw(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //��ȡ��Ļ��ͼ��Ϣ

	void OpenSCDrawReStore(); //�װ����С���ָ�ʱ�������ע�����Ǵ򿪵���򿪱�ע����
	void SetIsMinSize(BOOL bIsMinSize); //���ðװ崰��ʹ�Ƿ���С��

	void SetFlashCurPage(BOOL FlashCurPage) {m_bIsFlashCurPage = FlashCurPage;}
	BOOL GetFlashCurPage() const {return m_bIsFlashCurPage;}

	//����Ƶ����¼�ƣ�¼����Ƶ����wangxiaobin
	void MediaAudioID(const char *pData,int nLen);
	void MediaConfVideoID(const char *pData,int nLen);

	//	std::map<int,DWORD> m_MapMediaIDPageID;
	void SetIsMedialubo(BOOL bl) {m_bIsMedialubo = bl;}
	BOOL GetIsMedialubo() {return m_bIsMedialubo;}
	//	std::vector<int> m_MediaPageID;
	void SetRECScreenIsShow(BOOL bRECScreenIsShow) {m_RECScreenIsShow = bRECScreenIsShow;}
	BOOL GetEraseLineMoveMsg() {return m_EraseLineMoveMsg;}

	void SetWBfunction(CWBFunctionDlg * fun);
private:
	BOOL  m_RECScreenIsShow;//¼�ƶԻ����Ƿ񵯳���
	BOOL   m_bIsMedialubo;//�Ƿ��¼���ļ�
	DWORD  m_MediaID;
	BOOL m_bIsFlashCurPage;//FLASH�Ƿ��ڵ�ǰҳ����ҳʱ���õ�
	BOOL m_bIsMinSize; //�װ崰���Ƿ���С����
	BOOL m_bIsOpenSCDraw; //�װ���С��ʱ���Ƿ�ͬ�����˱�ע����
	BOOL m_bIsSDTransparent; //�Ƿ���͸��Ч��

	DWORD m_dwTimeGetTime; //��¼��һ������ƶ�ʱ����ͬ����Ϣ��ʱ��
	BOOL  m_bMoveIsNeedSendData;//�ж���MouseMoveʱ�Ƿ���Ҫ����ͬ����Ϣ

	//���ηŴ�
	CWBMagnifier*   m_pWBRectMaqn;
	CPoint m_CurrentCursorPos;//view������ͷ������

	//������
	CPoint m_ptPreDustbinPoint;
	CPoint m_DustbinPoint;
	CWBDustbin *m_pWBDustbin;
	BOOL m_IsMoveDustbin;
	
	int  m_PreMousePos;//add by xiewenxiu 20111201����ʶ�����һ��λ���Ƿ�������Ͱ��Χ��
	DRAW_STATE    m_nDrawStatus;
	//�۹��
	CWBSpotlight *m_WBSpotlight;
	BOOL m_bIsCloseScreenDlg;
	BOOL m_bIsTranspartentOrNot; //�Ƿ�͸��Ч��

	//��Ļ¼��
	CMedioVideoScreenDlg *m_MedioVideoScreenDlg;
	BOOL m_isFullScreen;
	//eagle eyeӥ��
	CWBEagelEye* m_pEagleEyeDlg;	

	//	CPoint m_MouseMovePrePoint;//mousemove�������û�仯����mousemove
	//	BOOL  m_bMouseMovePrePoint;
	BOOL m_EraseLineMoveMsg;//�ж��Ƿ�㲥������Ϣ

	int m_PrintX,m_PrintY;
	/*-----------------------------------------------------------------------------------
	//�������ϵĲ���
	------------------------------------------------------------------------------------*/
public:
	//���ͼƬ
	BOOL AddPicture();
	BOOL AddPicture(CString strFilePath,CRect rect = CRect(0,0,0,0));
	BOOL AddDicom();
	BOOL AddDicom(CString strFilePath,CRect rect = CRect(0,0,0,0));
	//���������
	void AddDustbin();
	//�����Ƶ
	BOOL AddMovie();
	BOOL AddMedia();//jiabangping 20110804
	BOOL AddMedia(CString strFilePath);
	BOOL AddCamera();//jiabangping 20110915
	UINT ClearObjectOfOffLineUser(CString sUCCUserID);//��������û���һЩ����(����ͷ)
	//���flash
	BOOL AddFlash();
	BOOL AddFlash(CString strFilePath,CRect rect = CRect(0,0,0,0));
	//����ĵ�
	BOOL AddDoc(CRect rect);
	BOOL AddDoc(CString strFilePath, CRect rect);
	//���dxf
	BOOL AddDxf(CString strFilePath,CRect rect = CRect(0,0,0,0));
	BOOL AddDxf();
	//�ĵ�ת��ʱ�Ĵ���
	BOOL DocConvertTip();
	void CreateFullScreen(CWBDocument *pWBDocument);
	void SetFullScreenPos();
	void HideFullScreen();
	void DestroyFullScreen();

	BOOL ScreenShoot(CString filepath);
	//����Դ��
	BOOL OpenResource();

private:
	CWBFullScreen* m_pFullScreen; //�ĵ�ת��ʱ�����Ĵ���
	CbuttonCloseDlg* m_pDocTip; //�ĵ�ת����ʾ
	bool m_pEstimate;

public:
	friend class CScreenDrawDlg;

	/*-----------------------------------------------------------------------------------
	//�װ���ͼ�йز���������ˢ�¡�λ�øı䡢�������ı䡢ϵͳ������
	------------------------------------------------------------------------------------*/
public:
	BOOL GetPageIsNeedDraw();
	void SetPageIsNeedDraw(BOOL PageIsNeedDraw);//���õ�ǰ��view��page�Ƿ���Ҫ����draw()�ػ�����
	void SetViewIsNeedDraw(BOOL ViewIsNeedDraw);//���õ�ǰ��view�Ƿ���Ҫ��view�ػ�
	//Ϊview���ù�����
	void setViewScroll();
	UINT getXnPos();
	UINT getYnPos();
	//��ðװ���ͼ��rect  2011/4/8
	CRect getWBViewRECT();    
	//���ù�������λ��
	void setCurrentViewScroll(POINT pt);
	BOOL DeleteDirectory(CString DirName) ; //ɾ��Ŀ¼

	void SetComObj(CWBObject *pObj = NULL, DWORD dwLayerIndex = 0);
	DWORD GetComLayerIndex();

	UINT GetPrePage();
	void SetPrePage(int nPage);
	void ReductionPrimaryWB(); //��ԭ�װ������״̬
	void HideResBar();//�ر���ԴĿ¼

	//�Ƿ���DOCת��
	void SetDocConvert(BOOL bIsDocConvert);
	BOOL GetDocConvert();

private:
	CPoint m_ptPointLineCurve;//������갴��ʱ��ֱ��q���ߵĵ�����
	CRect m_rect,m_clientRect;                        // ��ͼ����
	CBitmap m_bmpBuf;//view����ʱλͼ
	CDC m_dcMem;
	BOOL m_pageIsNeedDraw;//�ж��Ƿ���Ҫ��PAGE��draw()����
	BOOL m_viewIsNeedDraw;//�ж��Ƿ���Ҫ��view�ػ�
	//�������ƶ�ƫ����
	UINT m_XnPos;
	UINT m_YnPos;
	//�װ���ʾ�Ĵ�С���ܴ�С�ı���
	float m_fWbRatioX;
	float m_fWbRatioY;
	CRect m_RectWbSize; //�װ���ʾ�Ĵ�С

	//��ʼ������view�Ĵ�С
	int m_scrollX;
	int m_scrollY;
	XCritSec			m_csOnWbDeleteObjLock;
	XCritSec			m_csOnWbChangeObjLock;
	XCritSec			m_csAddObj;
	XCritSec			m_csConferenceRecord;
	XCritSec m_csDraw;
	BOOL m_bIsDocConvert; //�Ƿ���DOCת����ʾ
private:
	//	WBLineItem m_wbCurLine;
	//	PWBLineItem m_pSelLine;
	//	BOOL m_bSelectLine;
	CPoint m_ptSelect;
	//	int m_nStatus;
	//	int m_nRoundType;


	CPoint m_ptPrePoint;
	CPoint m_ptMovePoint;  //��һ��mousemove���λ�á���avcon6��onmousemove������겻��Ҳ��� �����ж� 
	long m_lmovecounts;

	CPoint m_ptDownPartRefresh;   //�ֲ�ˢ����������¼Down�µĵ�
	CPoint m_ptMovePartRefresh;  //�ֲ�ˢ����������¼Move�ĵ� 
	CRect  m_rectRefresh;
	CRect  m_PreRectRefresh;     //��¼��һ��ˢ���������
	bool  m_bflag;
public:
	void ChangeLeftTop_RightBottomPoint (CPoint &ptLeft, CPoint &ptRight); //����������
	bool GetIsRectRefresh();
	void SetIsRectRefresh( bool bl);

	void SetRefMoreSelectFrameRange( CRect rect );

	/*-----------------------------------------------------------------------------------
	//�ļ�����
	------------------------------------------------------------------------------------*/

public:
	CDlgPrintLoadingMsg*		m_pDlgPrintLoadingMsg; //���ش��ļ�ʱ��ʾ�ĶԻ���
	bool m_bLocalFileOperate; //�ж��Ƿ�����ļ�   һ��Ļ��ļ������Ļ���Ҫ��һЩ�������ˢ��Ԥ���ȵ� 
	bool m_IsGetAllObj;//�Ƿ����������ж���
	bool m_IsInsertPageOver;//�Ƿ�ɹ�����ҳ
public:
	//��ӡ����
	void PrintPage(/*CDC *pDC,BOOL m_IsPrint,int m_ScreenX,int m_ScreenY,CWBPage * pWBPrintPage*/);
	int SaveBitmapToFile(HBITMAP hBitmap, CString lpFileName);
	void PrintBmp(int count);
	void PrintBmp(HBITMAP hbitmap);

	//����ҳΪͼƬ
	CWBSavePictureDlg *GetWBSavePictureDlg();

	//�ļ�����
	void DeleteAll();	//ȫ��ɾ��
	BOOL LoadLines(CString strFilePath, bool IsCombin=false);	
	BOOL SaveLines(CString strFilePath);
	void CloseLines();    //ɾ�����ж��󣬵��ļ������Ǳ��ز����ǣ��ͷ���ɾ�����ж����ͬ����Ϣ
	bool CreateLoadingPictureDlg();
	bool DestoryLoadingPictureDlg();
	void SetPrintLoadingMsgPercent(float fpercent);
	HWND GetDlgPrintLoadingMsgHWND();
	//ҳ����
	void AddNewPage();
	void PrePage();
	void NextPage();
	void FirstPage();
	void LastPage();
	void GotoPage(UINT  nPage);
	void InsertNewPage();
	void InsertPrePage(UINT nPageIndex);
	void DeletePage();
	void DeletePrePage(UINT nPageIndex);
	void InterchangePage(UINT nFormerPage, UINT nTargetPage);
	void MovePage(UINT nSrcPageIndex, UINT nDesPageIndex); //�ƶ�ҳ
private:
	CWBSavePictureDlg *m_pWBSavePictureDlg;//�ļ�����ΪͼƬ
	//----------------------------------------------------------------------
	//������
	//----------------------------------------------------------------------
public:
	std::vector<DWORD *> m_vecLineLayerIndexBeforeErase;  //�������֮ǰ��ǰҳ����ֱ�߶����ͼ���

	void RecieveMsg(char* buff, int nRead);


	BOOL MoveMsg();
	BOOL MoveMsg(CPoint moveOffset);
	BOOL MoveMsg(CPoint moveOffset,unsigned long ulEvent);

	//BOOL MovInitBroadCastEventID(); //�ƶ���ѡ����ʼ���¼�Id    by  huanglongquan

	BOOL MovBroadCastMsg();	//�㲥��ѡ����ƶ�ƫ����       by   huanglongquan

	BOOL EraseLineMsg();

	BOOL EraseCurveMsg();

	BOOL EraseDocMsg();

	BOOL DeleteObjMsg();
	void	setFrameListAndObjLayerIndex();

	void SetIsLocalWhiteboard(bool isLocalWhiteboard);//���ðװ��еı��ذװ����
	bool GetIsLocalWhiteboard() {return g_isLoacalWB;}

	void SetTouchDustbinDelete(bool bflag) {m_bIsTouchUp = bflag;}
	bool m_bIsCombin;//��ϴ��ļ�
private:
	bool m_bIsTouchUp;//����������ɾ��������
	bool m_isLocalWhiteboard;//�����ж��Ƿ�Ϊ�������õ��Ӱװ壬��ʵ�ֱ��ذװ��redo�� undo
	int m_XOffSetPicture;//avcon����ͼƬλ��
	int m_YOffSetPicture;
	unsigned long m_ulComEventID; //���ͬ�����¼�ID
	int m_nComPageIndex; //���ͬ����ҳ��
	unsigned long m_ulComSenderID; //���ͬ���Ŀͻ���ID
	CWBObject *m_pComObj; //ͬ��ʱ�������Ķ���
	DWORD m_dwComLayerIndex; //ͬ��ʱ�����Ķ����ID
	int m_nComCount; //�����Ķ�����������ͬ��ˢ��
	//	bool m_bChangeObjectList;//�жϸı�����б��ʱ�򣬲���ˢ�£����ڸı��ɾ���������ʱֻ�����һ����������ˢ��
	//	bool m_bDrawView;

	UINT m_nPrePage; //�ı�ҳʱ��¼֮ǰ��ҳ��

	unsigned long m_ulMoveCounts;//�ƶ�����ʱ�ܵý������


	//��Ƥ������
	CRect m_rectBrush; //������
	DWORD m_dwCurBrushTouchID; //��ǰ����ʱ�Ĵ�����ID
	CPoint m_ptErasePre; //����ʱǰһ������������ĵ�

	//����ɾ������
	CWBCurve *m_pRgnDeleteWBCurve;
	std::map<int, CWBCurve*> m_pRgnDeleteWBCurveMap;

	//�˵����������ͼ���
	DWORD m_dwMenuLayerIndex;

	//�ֲ�ˢ��ʹ�õĵ�
	//	CPoint m_ptValidatePre;


	//��չ��ť
	//	CButton * m_viewButton;

	//�������Ĵ��ھ��
	HWND m_hTOOLBARWnd;

	CDrawCtl *m_ScreenDrawCtrl;

	unsigned long m_ulRecEventID; //����ͬ���¼�ID
	unsigned long m_ulSendEventID; //�����¼�ID��ɾ������������ʱʹ��
	unsigned long m_ulRecOldEventID; //��һ���¼�ID
	BOOL  m_bRefurbishPreview; //�Ƿ�ˢ��Ԥ����
	bool m_EraseRectDowm;//��С��������ʱ��

public:
	static int m_nViewCount; //��ͼ����
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg LRESULT OnWbDeleteobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbMaqnobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbSpotlightobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbCreateRichEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbEditRichText(WPARAM wParam, LPARAM lParam);//�ٴα༭�ı�
	afx_msg LRESULT OnWbDestroyRichText(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

protected:
	afx_msg LRESULT OnProcessScreenDraw(WPARAM wParam, LPARAM lParam); //������Ļ��ͼ��Ϣ
	afx_msg LRESULT OnWbProcessobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessAddFlash(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessAddMedia(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessAddCamera(WPARAM wParam, LPARAM lParam);
	//��������ͷ�������ӵ�ͬ����Ϣ��Ҫ��
	afx_msg LRESULT OnSendCameraConnectSynchronous(WPARAM wPar, LPARAM lPar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//����Ŵ󾵵���رհ�ť�ر�wangxiaobin
	afx_msg LRESULT OnProcessRectMagnClose(WPARAM wParam, LPARAM lParam);

public:

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);



	//touch
private:


	//�������Ʋ����ṹ
	typedef struct touchDrawingParameter
	{
	private:
		DWORD dwID;//�����ݿ��Բ���
		int nDrawType;
		CWBObject *pObject;
		//���ƶ�������á�
		DWORD dwTime;
		CPoint ptValid;
		CPoint pt;
		void *pData;
	public:
		touchDrawingParameter(DWORD _dwID, int _nDrawType, CWBObject*_pObject, CPoint _ptValid=CPoint(0,0), DWORD _dwTime=0, CPoint _pt=CPoint(0,0))
			: dwID(_dwID), pObject(_pObject), dwTime(_dwTime), ptValid(_ptValid), nDrawType(_nDrawType), pt(_pt){}
		void UpdateValidPt(CPoint _ptValid){ptValid=_ptValid;};
		void UpdatePt(CPoint _pt){pt=_pt;};
		void UpdateTime(DWORD _dwtime){dwTime=_dwtime;};
		void SetAppendData(void *_pData){pData=_pData;};
		int GetType(){return nDrawType;};
		CWBObject *GetWBObject(){return pObject;};
		DWORD GetTime(){return dwTime;};
		CPoint GetPt(){return pt;};
		CPoint GetValidPt(){return ptValid;};
		void *GetAppendData(){return pData;};
	}MTDrawPar, *PMTDrawPar;
	std::map<DWORD, MTDrawPar*> m_mapMTDrawPar;
	//���һ�δ��������������Դ����
	CString m_sLastDragFileURL;

	//��ȡ��������ӳ���е�ָ���������͵Ķ��������2012-3-16 jiabangping��
	unsigned int GetMapSizeOfSpecialDrawTypeObj(int nDrawType);

public:
	void ClearAllMTDrawMap();
	void ClearMTDrawMap(const DWORD &dwTouchID);

	unsigned long GetEventID() { return m_ulRecEventID;}

public:
	//����Ƶ���ƶԻ������
	//CWBGlobalMediaCtrl* GetMediaCtrl();
	//void SetMediaCtrl(CWBGlobalMediaCtrl* pMediaCtrl) { m_pMediaCtrl = pMediaCtrl ;}

	void SetEraseCurveLine(BOOL bErase) {m_EraseCurveLine = bErase;}
	BOOL GetEraseCurveLine() {return m_EraseCurveLine;}

private:
	BOOL m_EraseCurveLine;
	CWBGlobalMediaCtrl* m_pMediaCtrl;
	//	CDlgCamera* m_pCameraCtrl;
	DWORD m_MediaIDIsActive;//������

	bool m_bIsRButtonDown;//��Ļ¼������¼���Ҽ�����

public:
	void SetMediaVideoRButtonDown(bool bl) {m_bIsRButtonDown = bl;}
	//�洢�����������Ƶ��ָ��
	std::vector<CWBMediaWindow*> m_MediaList;
	void RemoveAllMediaListDate();
	void DeleteMediaListDate( CWBObject* pObj );

	void DeleteIntnetMediaToSin();//ɾ����������������������������Ƶ
	std::map<DWORD,CString> m_MediaStringPath;//�������������Ƶ�ļ�·��
public:
	//һ�����Ӱװ�ͻ��˽�ֻ����һ���װ�����ͷ����
	CWBCamera *m_pUniqueWBCamera;
	CWBPage *m_pPage;

protected:
	afx_msg LRESULT OnProcessFileOperate(WPARAM wParam, LPARAM lParam);
	//	afx_msg LRESULT OnWbDelAllAttribute(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNoticePraviewRefresh(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDelallobject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPraviewRefresh(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnClosewbfunctiondlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbDelPageAllObj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreatePrintLoadingMsg(WPARAM wParam, LPARAM lParam);	

public:
	//ȥ����Ӧ����м������Ϣ
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	void SendDataToConferenceRecord(XMemIni xmlobj ,int iCmdType = CMD_ADD_ATTRIBUTE ,bool bkeyFrame = true);
	bool SendFileToConferenceRecord(string str_path);
	bool	SendLocalDataToConferenceRecord(char* pData,int nLen,bool bKeyFrame = true);

	void SenMediadDataToConferenceRecord(unsigned char*pData,int nLen,int nKeyFrame,DWORD dwLayer = -1 );//�����ŵ���Ƶ�ص�
	void SenAudioDataToConferenceRecord(unsigned char*pData,int nLen,bool&bLoop ,DWORD dwLayer = -1);//�����ŵ���Ƶ�ص�
	void SenPlayMediadDataToConferenceRecord(unsigned char*pData,int nLen,int nKeyFrame,DWORD dwLayer = -1 );//�����ŵ���Ƶ�ص�
	void SenPlayAudioDataToConferenceRecord(unsigned char*pData,int nLen,bool&bLoop,DWORD dwLayer = -1 );//�����ŵ���Ƶ�ص�

private:
	long  m_nMoveObj_Realtime_Synchronizatio_Counts; //���ܵ��ƶ�����ʱ�����ˢ��
public:
	void SetMoveIsNeedSendData(BOOL MoveIsNeedSendData);   //������move��ʱ����Ҫˢ��
	BOOL GetMoveIsNeedSendData();                          //��ȡ��move��ʱ����Ҫˢ�µ��ж�

	void SendMoveSelectedObjOffset(CPoint cpoffset,unsigned long ulEventId); //����ƫ����ͬ��
protected:
	afx_msg LRESULT OnProcess_CurorTranDlg(WPARAM wParam, LPARAM lParam);

public:
	void CWhiteBoardView::BroadcastUserWater( CPoint pt,string str_username );

	CRect GetCamaraMediaRect() {return m_rcCamaraMediaRect;}
	//void  SetCamaraMediaRect(CRect rc) {m_rcCamaraMediaRect = rc;}
	bool GetCamaraMediaU7() {return m_bCamaraMedia;}
	DWORD GetCamaraMediaLayerID() {return m_CamaraMediaID;}
private:
	//unsigned long m_ulUserCompure;
	DWORD m_StartGetLayerId;
	DWORD m_CamaraMediaID;
	bool m_bCamaraMedia;//�㲥���շ�
	CRect m_rcCamaraMediaRect;
	CRect m_rcCamaraMediaRectOpen;
	CWaterDlg *m_waterdlg;
	MapWaterDlgPointer m_MapWaterDlgPointer;
	std::string m_ReceiveWaterUserID;
	CPoint m_ReceiveWaterPoint;
public:
	void RejectBroadcastUserWater(string str_name);
	void OnRejectBroadcasstUserWater(string str_name);
	void ReJectAllBroadcastUserWater();
	void DeleteWaterCursorForAvcon6();
protected:
	afx_msg LRESULT OnSendPageDatatoConferenceRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcess_WaterDlg(WPARAM wParam, LPARAM lParam); //����ˮ���ƹ㲥��Ϣ
	afx_msg LRESULT OnCreateWaterDlg(WPARAM wParam,LPARAM lParam);

public:
	CWBProtractor *m_WBProtractor;
	//BOOL MouseMoveProtarctor(const CPoint& point);
	//BOOL MouseDownProtarctor(const CPoint& point);
	//BOOL MouseUpProtarctor(const CPoint& point);
	void AddProtractor();

private:

	//private:
	//	CPoint m_nProtractorDownpt;
	//	CPoint m__nProtractorUppt;
public:
	CWBAngle *m_WBAngle;
	BOOL MouseMoveAngle(const CPoint& point);
	BOOL MouseDownAngle(const CPoint& point);
	BOOL MouseUpAngle(const CPoint& point);
	void AddAngle();


public:
	CWBRuler *m_WBRuler;
	//BOOL m_bRulerIsneedDraw;
	void CreateAnnRule(CPoint &pt,int number);
	void AddRuler();

	CWBSetSquare * m_WBSetSqaure; //����һ�����ǰ������
	BOOL AddSetSquare();
	//���Բ��
	CWBCompass * m_WBCompass; //����һ��Բ�����
	BOOL AddCompass();
	int GetCompassDrawShape();
	void SetCompassDrawShape(int num);
	//void CloseSetSquare();
	//�ж�Բ��˵��ǹ�ѡ���λ���Բ��
	BOOL GetCompassOptMenu(){return m_bCompassOptMenu;}
	void SetCompassOptMenu(BOOL bl){m_bCompassOptMenu = bl;}
private:
	BOOL m_bCompassOptMenu;//Բ��˵���ѡ
	int m_nCompassDrawShape; //Բ�滭ʲôͼ��ѡ��
	CButton m_btn;
	bool m_bIsCreated;

private:
	//GDI+
	ULONG_PTR	m_gdiplusToken;
		double m_dReduceRate;//����
	DWORD m_dwCheckItem;//��¼�����˶���
private:
	XMemIni m_xmlBroadCastAttiObj;
protected:
	afx_msg LRESULT OnProcessBroadcastObj(WPARAM wParam, LPARAM lParam);
public:
	void StartProReliefTimer(); //������ʱ��
	void KillProReliefTimer();//�رն�ʱ��
	static void CALLBACK  OnProRelief(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime); //��ʱ���ص�������ÿ���ж��Ƿ������ƶ���û���ͷ� huanglongquan
	void _OnProRelief();		//�ж��Ƿ������ƶ���û���ͷ� huanglongquan add
	void AddLimitObjId(unsigned long ulObjId );
	void AddLimitObjId(void* pWBObj);
	void RemoveLimitObjId(unsigned long ulObjId );
	void RemoveLimitObjId(void* pWBObj);
	void SetCheckItem(DWORD dItem);
	DWORD GetCheckItem();
private:
	//�����ͷŲ�����������ʵ��
	CLimitLocalObjOperate  m_LimitLocalObj;
	UINT_PTR m_nTimerLimitObjPro;
public:
	void ChangViewSize(CRect rect,double dReduceRate);//�ı�view�Ĵ�С
	void ChangeViewCoordinate(CDC *pDC);//�ı�view������ϵ
	void ChangeSize(CRect rect,double dReduceRate  );
afx_msg void OnReduce(WPARAM wParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
protected:
	afx_msg LRESULT OnPopmenu(WPARAM wParam, LPARAM lParam);
private:
	CString m_strPrompt;
	bool m_bCamaraMediaClk;

private:
	MapDropCamInfo m_MapDropCamInfo;
	CMyDropTarget m_myDropTarget;
	CPoint m_nLDBKPoint;
	BOOL m_bIsdrop;
	string m_strChannelID;
public:
	afx_msg	LRESULT OnDropNotify(WPARAM wParam,LPARAM lParam);
	void ProcessCaminfo(WPARAM wParam,CRect rc);  //U7��ק
	void ReJectAllProcessCamInfo();   
	void DeleteCaminfo(DWORD nWBObjID);    //ɾ��MAP
	BOOL GetIsdrop(){return m_bIsdrop;}
	void SetIsdrop(BOOL bIsTrue){m_bIsdrop = bIsTrue;}  //�ж��Ƿ�����ק���ɵ�ͼƬ
	BOOL bIsInCamInfoMap(CWBObject* nDWBIndex); //����MAP���ΰ�ť
	bool ProcessGetCamInfo(DWORD nObjID,CString str,string strname,unsigned long channelID); //����������ק��ϢͼƬ

	//��������
public:
	void SendRollCallBroadCast(int UserStyle); //���͵Ĺ㲥������Ϊ���ͣ�
	void ProcessRollCallBroadCast();           //�������������
	void ProcessRollCallLRSultBroadCast(string str);    //���շ�����Ϣ

private:
	unsigned long m_nSendRollCallBroadCastID; //�㲥������
	CWBRollCallDialog *m_wbrcprodlg;
	CWBRollCallShowDlg *m_wbrcshowdlg;
	int m_nRollCallTimer;
	std::map<std::string ,DWORD>*m_pUserNameID;//��ק�û���ID
	
public:
	std::vector<string> m_RollCallNameList;

public:
	void SendCurvePicFromFile(const char* strPath);
    std::map<DWORD, std::string> m_pRecvFileWBIcoCurveMap;
	BOOL m_bFirstLoginConference; //�ж��Ƿ��һ��������û�

private:
	CRect m_RcDrawText;
	bool  m_bUseToControlDraw;
	bool  m_bLimitCursorArea;
	int   m_ntouchID;
private:
	int m_nTriangleType;
	
public:
	void SetDrawTriangleType(int ntype){m_nTriangleType = ntype;}
	int GetDrawTriangleType(){return m_nTriangleType;}
	void ProcessCloseColorDlg();

public:

	void SetIsKeyDel(BOOL bIsTrue){m_ptIsKeyDel = bIsTrue;}
	BOOL GetIsKeyDel(){return m_ptIsKeyDel;}


	void SetIsRationButton(BOOL bIsTrue){m_ptIsRationButton = bIsTrue;}
	BOOL GetIsRationButton(){return m_ptIsRationButton;}
	void ExitRoomDeleteStreamObj();//�˳�����ʱɾ���������յ�������
	int GetDocCurPage(){return m_nDocCurPageIndex;}
	void SetDocCurPage(int Index){m_nDocCurPageIndex = Index;}//����ĵ��ĵ�ǰҳ��
private:
	BOOL m_ptIsKeyDel;
	BOOL m_ptIsTouchCom;
	BOOL m_ptIsRationButton;
	int m_nDocCurPageIndex;
	BOOL m_bIsAllowDropCamera;   //U7�¿����Ƿ�������ק����ͷ���װ�
	CString m_strLocalNotDropCameraToWB; //���ز�֧����ק����ͷ���װ��ַ���
	
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);//ȡ�����������Ҽ�������ť
public:
	void ProcessMTMoveRationalCenter(CPoint ptOffset); //������ʱ�ƶ���ת���ĵ�
	CFile m_cFile;
	CWBFile *m_pLoadWBFile;
private:
	bool m_bStartRecognition;//����ʶ����
	bool m_bRecogniMouseDown;
	bool m_bRecogniTouchDown;
	PointPath path2D;
	std::vector<CPoint> m_RecShowpoint;
	GeometricRecognizer *m_pGeoRecognizer;

public:
	void ChangeUserCurorTranDlgPosition(CPoint ptoffset); //����U7����ק�װ崰�ں�ı����λ��

public:
	void SetNeedSetPageIndexForScreenDraw(bool bneed){m_bNeedSetPageIndexForScreenDraw = bneed;}//���б�ע��ʱ��Ϊ�ļ��˳�������ҳ�ű��Ϊ1����Ϊ�������Ļط����⣩
private:
	bool m_bNeedSetPageIndexForScreenDraw;
public:
	bool ProcessMediaPhotoEffect(CPoint ptRes,bool bRight);
	void SetWBPictureEffect(CWBPicture *pWBObj);
	void DeleteSpePicInMap(DWORD dwlayer); //�װ���ɾ������Ƿ��м�¼
	bool DeleteSpePicFromResource(CString strPath);//��Դ��ɾ������Ƿ��м�¼���о�ɾ��
private:
	CWBPicture* m_pWBPicture;	
	std::map<CString ,VECTORSPECPIC>*m_pResourcePic;//��ק�û���ID
	std::vector<CWBPicture*> m_pDropPicObj;//��Դ���ϳ���ͼƬ
	bool m_bIsDBClick; //˫����
	DWORD m_dDBClickobjlayer;//˫���Ķ���
	CString m_strResourceUpdataPath;//������Դ����ͼƬ��·��
	CString m_strDBClickSavePath;//˫������༭ͼƬ��·��
	int m_bSelectStatueChange;//����ѡ�ж���״̬�任
public:
	afx_msg HRESULT ProcessResourceMessageBox(WPARAM wParam,LPARAM lParam);//������Դ��ɾ���ļ�������Ϣ
	void CheckDeleteObjInVector(CWBPicture* pWBPic); //���ɾ����ͼƬ�����Ƿ����vector�С�
	void CheckVectorBeforCloseWB();//�װ��˳�ʱ����Ƿ�����Ҫ��������ڱ༭��ͼƬ
	void AddPicToVector(CWBPicture* pWbPic);
	bool CheckDiskPlaceForPic();//���·�����̴�С




	void AddMidi(CString strPath);
	DWORD GetdwMenuLayerIndex();
	void SetdwMenuLayerIndex(DWORD dwMenuLayerIndex);
	int GetObjCopyListCount();
	bool SingleObjIsLock();//�жϵ��������Ƿ�Ϊ����״̬
	int GetWBObjCount();//��ȡ�������
	void RefreshWB();	//ˢ�°װ�
	void PostMessageToWhiteBoard(UINT msg, WPARAM wparam, LPARAM lparam);//���첽��Ϣ���װ�
};

typedef int (*pDicomToBmpDll)(CString,CString);//�����붯̬���ӿ��еĺ���ԭ����ͬ��ԭ��
typedef BOOL (*GetBmpPath)(char *pszPath, int nLen);//�����붯̬���ӿ��еĺ���ԭ����ͬ��ԭ��
