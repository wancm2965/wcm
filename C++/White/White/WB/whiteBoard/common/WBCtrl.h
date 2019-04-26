#pragma once
#include "../ImageEx.h"
#include "WBData.h"
#include "../WBToolTip.h"

#define  BUTTONSIZE 40
#define  FLASHBUTTONSIZE    60
#define  FLASHBUTTONOFFSET  20
#define  BAFFLE_BTN_WIDTH   74    //���尴ť�Ŀ��
#define  BAFFLE_BTN_HIGHT   20    //���尴ť�ĸ߶�
#define  BAFFLE_BTN_SPACE   5      //���尴ť�뵲����ϵľ���
#define  BAFFLE_CLOSEBTN_WIDTH 40   //����رհ�ť�Ŀ��
#define  BAFFLE_CLOSEBTN_HIGHT 40   //����رհ�ť�ĸ߶�

class CWBObject;
//class CWBRectMaqn;
class CSpotlightDlg;

//ͼƬ��ť��
class CWBObject;
class CWBImageButton
{
public:
	CWBImageButton(CWBObject *pWBObject = NULL);
	virtual ~CWBImageButton();

	void Draw(CDC *pDC);	//����ť	

	void ButtonDown(CPoint point);	//��ť���½��еĲ���
	void ButtonMove(CPoint point);	//��ť�ƶ����еĲ���
	void ButtonUp(CPoint point);	//��ť������еĲ���

	BOOL IsInButtonArea(const CPoint& pt); //�ж��Ƿ��а�ť 
	void setOffsetCenter(CPoint ptoffset);
	void ChangeRationCenterOperator();
	void SetObjRationCenterpt(CPoint pt);
	bool InitImageFromFile();	//��ʼ��ÿ����ť��ͼƬ	
	bool InitImageFromResource();   //����Դ��ʼ��
	void ClearImage();         // ɾ��ͼƬ
	void GetObjRationCenterPt();
	CPoint GetMenuPt(); //�õ�����˵���ť�ĵ�
protected:	
	//��ť���ʹ�1��ʼ��ʶ
	enum BTN_TYPE
	{	
		BTN_TYPE_BEGIN = 1,//ֱ����㰴ť
		BTN_TYPE_END,      //ֱ���յ㰴ť

		BTN_TYPE_LOCK,     //������ť
		BTN_TYPE_ROTATION, //��ת��ť
		BTN_TYPE_MENU,     //�˵���ť
		BTN_TYPE_ZOOM,	   //���Ű�ť
		BTN_TYPE_ROTATIONCENTER
	};

	int RotateXForm(double dbAngle); //��ת�Ƕ�
	void ResumeXForm(int nGraphicsMode); //�ָ��Ƕ�
private:
	ImageEx *m_pImageLock; //������ťͼƬ
	ImageEx *m_pImageUnLock;	//������ťͼƬ
	ImageEx *m_pImageRotation;	//��ת��ťͼƬ
	ImageEx *m_pImageMenu;	//�˵���ťͼƬ
	ImageEx *m_pImageZoom;	//���Ż�ֱ����㡢�յ㰴ťͼƬ	
	ImageEx *m_pImageClone;  //���޿�¡ͼƬ
	ImageEx *m_pImageAnnotations; //��ע���ͼƬ
	ImageEx *m_pImageRotationCenter;

	CWBObject *m_pWBObject;	//��ť����Ӧ�Ķ���ָ��
	CDC *m_pDC;	//����DC
	bool m_onlyone;
	int m_nType;
	BOOL m_bRotation; //��ת��ʶ
	BOOL m_bZoom; //���ű�ʶ
	CPoint m_ptRotationPre;// ��ת���
	CPoint m_ptZoomPre;
	CPoint m_ptObjCenter; //�����е�
	CPoint m_ptRotationCenter; //��ת���ĵ�
	CPoint m_ptMenu; //����˵���ťʱ�ĵ�
	XCritSec			m_csImageButtonMoveObjLock;	//��Ϲ�������
	bool m_bIsImageMove;//ͼƬ��ť�ƶ���Ƶ������ͣ��Ƶ����
	BOOL m_bMoveRotionCenter; //�ƶ���ת���ĵ���
	CPoint m_ptMoveRotationCenter; //
	CPoint m_ptOffsetCenter;
	bool m_bCenterMove;
	CPoint m_ptcenterone;
	BOOL m_ptIsRorationType;
};
/*
//ͼƬ�رհ����࣬���ڷŴ󾵹رհ���
class CWBImgCloseButton
{
public:
	CWBImgCloseButton();
	CWBImgCloseButton(CWBObject *pWBObject,UINT uBtnType);//�ƶ�������ָ��Ͱ������ͣ����Ը��ݰ������ͣ�ȷ�������ࡣ
	//CWBImgCloseButton(CWBRectMaqn* WBRectMaqn,int nBtnCount);
	virtual ~CWBImgCloseButton();

	void Draw(CDC *pDC);	//����ť	
	BOOL IsInButtonArea(const CPoint& pt); //�ж��Ƿ��а�ť 
	void ButtonDown(CPoint point);	//��ť���½��еĲ���
	void ButtonMove(CPoint point);	//��ť�ƶ����еĲ���
	void ButtonUp(CPoint point);	//��ť������еĲ���
	//void ClearImage();         // ɾ��ͼƬ
	void SetButtonImage(CString filename);//���ð�ťͼƬ

public:
	CWBObject *m_pWBObject;	//��ť����Ӧ�Ķ���ָ��
	//Bitmap *m_pImg;           //ͼ��ָ��
	ImageEx *m_pImg;
	//int m_nBtnCount; 
	BOOL m_bMoveFlag;         //�Ƿ��ƶ�
	CPoint m_pCurPoint;       //��ǰ���λ��
	BOOL m_IsUpOrDown;        //
	BOOL m_IsLeftOrRight;
	CDC *m_pDC;	//����DC
};
*/

//������ư�ť�ࣨ����Ƶ��FLASH���ĵ��Ŀ��ƣ�
class CWBControlButton
{
public:
	CWBControlButton(CWBObject *pWBObject = NULL);
	virtual ~CWBControlButton();

	void Draw(CDC *pDC);	//����ť	

	void ButtonDown(CPoint point);	//��ť���½��еĲ���
	void ButtonMove(CPoint point);	//��ť�ƶ����еĲ���
	void ButtonUp(CPoint point);	//��ť������еĲ���
	void FlashTip(CPoint point);
	void DocTip(CPoint point);      //�ĵ�������ť��ʾ
	BOOL IsInButtonArea(const CPoint& pt); //�ж��Ƿ��а�ť 
	void MidiTip(CPoint point);
protected:	
	//��ť���ʹ�1��ʼ��ʶ
	enum BTN_TYPE
	{	
		BTN_TYPE_START = 1,//���Ű�ť
		BTN_TYPE_STOP,      //ֹͣ��ť
		BTN_TYPE_CIRCLE,		//ѭ����ť
		BTN_TYPE_CATCH,		//��Ƶ��ͼ��ť

		BTN_TYPE_FIRSTPAGE,    //�ĵ��Ļص���ҳ��ť
		BTN_TYPE_LASTPAGE,     //�ĵ���βҳ��ť
		BTN_TYPE_PREPAGE,      //�ĵ�����һҳ��ť
		BTN_TYPE_NEXTPAGE,	   //�ĵ�����һҳ��ť
		BTN_TYPE_PRINTPAGE,	   //�ĵ��Ĵ�ӡ��ť

		BTN_TYPE_TOP,          //���嶥������
		BTN_TYPE_LETE,         //������߰���
		BTN_TYPE_RIGHT,        //�����ұ߰���
		BTN_TYPE_BOTTOM,        //����ײ�����
		BTN_TYPE_CLOSEPATETTE,   //����رհ�ť

		BTN_TYPE_MAQNCLOSE,  //�Ŵ����رհ�ť

		BTN_TYPE_DXF_ZOOMOUT,// dxf�ļ���С
		BTN_TYPE_DXF_ZOOMIN, //dxf�ļ���
		BTN_TYPE_DXF_MOVE, //dxf�ļ��ƶ�

		BTN_TYPE_MIDI_UP,
		BTN_TYPE_MIDI_DOWN,
		BTN_TYPE_MIDI_PRE,
		BTN_TYPE_MIDI_NEXT,
		BTN_TYPE_MIDI_FIRST,
		BTN_TYPE_MIDI_LAST
	};

	void InitImageFromFile();	//���ļ���ʼ��ͼƬ
	void InitImageFromResource();	//����Դ��ʼ��ͼƬ

	int RotateXForm(double dbAngle); //��ת�Ƕ�
	void ResumeXForm(int nGraphicsMode); //�ָ��Ƕ�
	 
private:
	ImageEx *m_pImageBackground;	//��Ƶ����
	ImageEx *m_pImageStart;       //��ʼ��ťͼƬ
	ImageEx *m_pImagePushStart;	  //���¿�ʼ��ť��ͼƬ
	ImageEx *m_pImagePause;	      //��ͣ��ťͼƬ
	ImageEx *m_pImagePushPause;	  //������ͣ��ťͼƬ
	ImageEx *m_pImageStop;	      //ֹͣ��ťͼƬ
	ImageEx *m_pImagePushStop;	  //������ͣ��ťͼƬ
	ImageEx *m_pImageIsStop;	  //��Ƶ�Ѿ�ֹͣʱ��ʾ��ͼƬ	
	ImageEx *m_pImageCircle;	  //ѭ����ťͼƬ
	ImageEx *m_pImagePushCircle;	//����ѭ����ťʱ��ͼƬ
	ImageEx *m_pImageIsCircle;	//��Ƶ�Ѿ�����ѭ��״̬ʱ��ͼƬ
	ImageEx *m_pImageCatch;		//��ȡ��Ƶ���水ť��ͼƬ

	ImageEx *m_pImageFirstPage;	//�ĵ���ҳ��ťͼƬ
	ImageEx *m_pImageLastPage;	//�ĵ�βҳ��ťͼƬ
	ImageEx *m_pImagePrePage;	//�ĵ���һҳ��ťͼƬ
	ImageEx *m_pImageNextPage;	//�ĵ���һҳ��ťͼƬ
	ImageEx *m_pImagePrintCur;	//��ӡ�ĵ���ǰҳ��ͼƬ

	ImageEx *m_pImageDxfZoomIn; //�Ŵ�Dxf�ļ��İ�ť
	ImageEx *m_pImageDxfZoomOut; //��СDXf�ļ��İ�ť
	ImageEx *m_pImageDxfMove; //Dxf�ļ����ƶ���ť

	ImageEx *m_pImageTopHandle;      //���嶥������ͼƬ
	ImageEx *m_pImageLeftHandle;     //������߰���ͼƬ
	ImageEx *m_pImageRightHandle;    //�����ұ߰���ͼƬ
	ImageEx *m_pImageBottomHandle;   //����ײ�����ͼƬ
	ImageEx *m_pImageBaffleClose;   //����رհ�ť

	ImageEx *m_pImageMaqnClose; //�رշŴ󾵵İ�ť

	ImageEx* m_pImageMIDIUp;
	ImageEx* m_pImageMIDIDown;
	ImageEx* m_pImageMidiPre;
	ImageEx* m_pImageMideNext;
	ImageEx* m_pImageMidiFirst;
	ImageEx* m_pImageMidiLast;
	ImageEx *m_pImageMidiPrint;
	CWBObject *m_pWBObject;	//��ť����Ӧ�Ķ���ָ��
	CDC *m_pDC;	//����DC
	int m_nType;
	BOOL m_bIsButtonDown;	//��¼����Ƿ���
	//���尴ť������ȥ�ĵ㣬���϶�����Ŀ�ʼ��
	CPoint m_baffleBeginPt;
	int m_bShowDocTip;
};
