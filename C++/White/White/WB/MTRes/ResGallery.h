#pragma once

#include <list>
#include <map>
#include "MTFolder.h"
#include "Thumbnail.h"
#include "D2DDriver.h"
#include "ResData.h"
#include "ClickTarget.h"
#include "XAutoLock.h"
#include "ResManage.h"
#include "ShellDef.h"
#include "LanguageStr.h"
#define BOTTOM_PHOTOSTRIP_DIRECTORY     TEXT("D:\\WBTemp_\\")
#define RESGALLERY_CLASS_NAME TEXT("ThumbGalleryWindowClass")

#define INTERNAL_Y_MARGIN_DOWN_PERCENTAGE	(0.2f) //��ֱ������±߾�
#define INTERNAL_Y_MARGIN_UP_PERCENTAGE		(0.2f) //��ֱ������ϱ߾�
#define INTERNAL_Y_MARGIN_RATIO (0.05f) // 
#define SIDE_MARGIN_PERCENTAGE (0.3f) //���Բ���ʱ��ƽ�Ƶ���ͷ�ı߽��趨

#define WB_TRG_WINDOW_HEIGHT	(180)//������Դ��Ĵ���߶�(TRG==TOUCH_RESOURCE_GALLERY)
#define WB_TRG_THUMB_INTERNAL_MARGIN	(30)//������Դ��������ͼ��ļ������һ�ţ���ˮƽ�����

#define WB_SINGLETIP_WIDTH 150
#define WB_SINGLETIP_HEIGHT 100
#define WB_CHANGE_LIST_WIDTH 150
#define WB_CHANGE_LIST_HEIGHT 85

#define  PATH_RESOURCE_EXCHANGE_PICTURE TEXT("Skin\\WhiteBoard\\Image\\ResourceMenu.png")//��ԴͼƬ�����·��

#define MOUSEEVENTF_FROMTOUCH 0xFF515700

#define	LEFT_SLIDE	    0x0001
#define UP_DRAG		    0x0002
#define RIGHT_SLIDE     0x0004
#define CHANGE_LIST     0x0008
#define UP_DEL			0x0010
#define ADD_TIP         0x0020

enum PANEL {
	RESSHOW = 0,//��ʾһ����Դ������
	RESLIST = 1,//���ļ��е���ʽ��ʾ��Դ�б�
	RESLISTEDIT = 2,//�༭�ļ�����ʽ����Դ�б�
	RESCLASSLIST = 3//����Դ������ʾ��Դ�б�
};

//��������������˵������������ȴ�С����Ч��Χ��
#define  CLICK_ZONE_WIDTH	(60)
#define  CLICK_ZONE_HEIGHT	(CLICK_ZONE_WIDTH)

typedef int (__cdecl *ResTypeCallbackFun)(const std::string &_strExt);
typedef std::list<CWBThumbnail*> LWBThumbnail, *PLWBThumbnail;
class CResChangeWnd;
class CWBResGallery : public AVCON_SDK_MTTouch::CMTObject
{
public:
	//�������ڼ�������
	static HRESULT CreateThumbGallery(HINSTANCE hInstance, HWND hParent,
		int x, int y, int nWidth, int nHeight, CWBResGallery **ppStrip,
		ResTypeCallbackFun pResCallback, BOOL bAuthorize = FALSE);

	//��ȡ���ھ��
	const HWND GetHWND()const;
	HWND m_hWnd;
	//�װ崰�ھ��
	HWND m_hWBViewHwnd;
	//��ȡ��ʾ���ھ��
	HWND GetHWndToolTip(){return m_hWndToolTip;};

	virtual void CleanUp(){delete this;};

	//��ȡ��ǰ��ԴĿ¼
	LPCTSTR GetCurResDirectory();
	//��ȡ��ǰ��Դ��Ŀ����
	unsigned int GetCurResIndex() const;
	//���
	CD2DDriver *GetD2DDriver(){return m_d2dDriver;}
	//������������ͼ��ˮƽƫ������
	void ResetXPosition();
	//������������ͼ����ֱƫ������
	void ResetYPosition();
	//������Դ����λ��
	void UpdateResSize(unsigned int nNewWidth, unsigned int nNewHieght);
	//��ȡ��Դ����λ��
	void GetResSize(unsigned int &nNewWidth, unsigned int &nNewHieght);
	//���жϵ�ǿ�Ƹ��£��磺����Դ��������ɾ���˵�����Դ����Ҫ����Ƿ�ɾ���˵�ǰ������Դ���еĵ�ǰ��Դ�����ǣ�����գ�
	void UpdateResource();
	//����Ŀ¼���������ã���ʼ�����л��ȣ�
	BOOL LoadResFromDirectory(LPCTSTR lpszDirectory);
	//���ô�����Ȩ
	BOOL SetMTAuthorize(BOOL);
	//����ѡ���ļ������Ϣ
	void ConfigFileSelectPlug(MTFOLDERCONFIG &pconfig,wchar_t *,wchar_t *,wchar_t*);
	//��ʼ����������·��
	void InitLanPathNode(tstring);
	DWORD GetDrawTipFlag()const;
	void ConfigDrawTipFlag(DWORD nflag);
	void DarwTipBackground();
	
	PANEL AdjustState();

	void SetWBResHWnd(HWND hParent){m_hWBViewHwnd = hParent;};

	//����Ƶ����ͼƬɾ��ʱ�������ʾ������ͼ��
	void DrawRubbishIcon();
	void SetIsDrawRubbishIcon(BOOL bdraw){m_bisDrawRubbishIcon=bdraw;};
	BOOL GetIsDrawRubbishIcon(){return m_bisDrawRubbishIcon;};
	void SetResChange(BOOL bchange){m_bResChange = bchange;};
	void SwitchVideoPictureDirectory(LPCTSTR strPath);   //�л���Ƶ����·��
	void InitSpecialResPath();    //��ʼ��������Դ·��
private:
//	LRESULT OnTouch(WPARAM wParam, LPARAM lParam);
	//������wm_touch��Ϣ
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;
	//��ʾ��Ϣ
	LanguageStr m_lStr;
	tstring m_lanPath;
	void RenderTip();
	HINSTANCE m_ResHinstance;
	//������Ȩ״̬
	BOOL m_bMTAuthorize;
	//��һ�δ���ʱ��ʼ����Դ����
	void InitialResData();

	//��ȡ��������ָ��
	LWBThumbnail *GetLstAddress(){return m_lThumbnail;}

	//���������������
	void CreateSpecialClickObject();
	//��������ʾTip��Ϣ
	TOOLINFO m_ToolTip;
	HWND m_hWndToolTip;
	void CreateTipWnd(HWND hWnd,TOOLINFO toolTip);
	

private:
	//operation  for cursor updating message
	virtual void addMTCursor(MTCursor *tcur);
	virtual void removeMTCursor(MTCursor *tcur);
	virtual void updateMTCursor(MTCursor *tcur);

	///�ڲ�����
private:
	bool AddCurorMap(const unsigned long &dID, CMTObject *pObject);
	bool RemoveCurorMap(const unsigned long &dID);

	XCritSec m_csMapChildObject;
	XCritSec m_csListChildThumbnail;
	//��ǰ���ڲ���������б�
	std::map<unsigned long, CMTObject*> *m_pMapChildObject;

private:
	virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);
	virtual bool BInRegion(const POINT &pt)const;

private:
	//����
	void Render(BOOL bRenderBackgroundOnly = FALSE);

private:
	//���������Ĵ��ڲ��д�������û����
    //BOOL InRegion(const float &x, const float &y)const;
	//�����������ͼ��ת��ΪD2Dͼ����̺߳���
	static UINT CWBResGallery::AfxThreadProc( LPVOID pParam );

private:
	//˽�еĹ������������
    CWBResGallery(HWND hWnd, int nWidth, int nHeight, int nXPos, 
		int nYPos, ResTypeCallbackFun pResCallback);
    ~CWBResGallery();
	//���
    void FreePhotos();
	//��Ҫ��һЩcom����ĳ�ʼ��
    HRESULT Initialize();
	//��ȡͼƬ
	HRESULT LoadPhoto(LPCTSTR const uri);
    
	//�л�ѡ���ǩ
	void ChangeResTab(unsigned nResItemIndex);
 
	//��׼�Ĵ�����Ϣ������
    static LRESULT CALLBACK S_WndProc(HWND, UINT, WPARAM, LPARAM);

	ResTypeCallbackFun BSupportedResType;

	HDEVNOTIFY m_hDevNotify;//����������ע����

private:
	//��ȣ��߶ȣ�ˮƽ����ֱ�����ƫ����
    int m_nWidth, m_nHeight, m_nXPos, m_nYPos;
    float m_fpXOffset;
	CD2DDriver *m_d2dDriver;
	//TouchDispatch *m_pDispatch;
    std::list<CWBThumbnail*> *m_lThumbnail;
	//��Դģʽ��Ŀ¼ģʽ
	PANEL m_panel;
	BOOL m_bResChange;

	//��������˵��Ĵ�������
	CWBTouchClick *m_mtSpecialClick;
	//��ǰ��Դ��·��
	tstring m_strCurDir;
	//��Դ���ݣ�ָ���������Դ�������
	PResMapArray m_pLstResData;
	//��ǰ��ʾ����Դ���кţ�0��ʶΪ�գ�1��ʾ��һ��, 2, ...��
	int m_nCurResIndex;
	//��ǰ��Դ��
	tstring m_strCurName;

	//�����㣬���down�Ƿ���
	BOOL m_fIsMouseDown;

	//�л���Դ��bitmap
	ID2D1Bitmap *m_pResChangeBitmap;
	Bitmap* m_pResChangeGDIBmp;

	HRESULT TranslateXP(int xOffset = 0 , int yOffset = 0);

	static int GetResType(const std::string &_strExt);

	unsigned int m_nDrawTipFalg;     //����ʾ��Ϣ��״̬
	unsigned int m_nDrawIntermediateFalg;     //����ʾ��Ϣ���м����״̬

	BOOL m_bisDrawRubbishIcon;   //�Ƿ���Ҫ��ɾ������ͼ��
	tstring m_strVideoPicturePath;   //������Դ·��
	BOOL m_bisSpecialResource;   //�Ƿ���������Դ
	float m_fResTotalLen;        //Ŀ¼��Դ���ܳ���;

///��ק���մ���
public:
	bool RegisterReceivedWnd(const HWND &hWnd);
	bool UnRegisterReceivedWnd(const HWND &hWnd);
	bool BInReceiveDragWndList(const HWND &hWnd);
	HMODULE m_hFolder;
///��ʾ�Ի���
public:
	void SetThumbnail(CWBThumbnail *thumbnail){m_pThumbnail = thumbnail;};
	HMODULE m_hDialog;
	CWBThumbnail *m_pThumbnail;
	BOOL m_bDrapFlag;    //��ק��״̬��־
private:
	std::list<HWND> m_lstReceiveDragWnd;
};

void DisableTheTabFeedback(HWND hWnd);
