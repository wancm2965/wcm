#pragma once
/*---------------------------------------------------------------------------------------------------------------
������

----------------------------------------------------------------------------------------------------------------*/

#include <vector>
#include "WBCtrl.h"
#include "../CShockwaveFlash.h"
#include "../ImageEx.h"
#include <atlimage.h>
#include <atlimage.h>
#include <atlbase.h>
//#include <dshow.h>
//#include <qedit.h>

#include "../WBMTObject.h"
#include "./WBMedia/WBWindows.h"//��Ƶ����
#include "./WBMedia/DlgCamera.h"//����ͷ����
#include "../WBImageSliderControl.h"
#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h> 

#include<crtdbg.h>


//#include "..\MemDC.h"
#include "datec/DATEC_PDFConvert.h"
#include "datec/DATEC_PDFFile.h"
//#include "./TouchDriver/TouchObject.h"
/*#include "../WZoomCtrl.h"*/
#include "../WMBtn.h"
#include "../MySliderControl.h"
#include "threadhelper.h"
#include "../WBFullScreen.h"
#include "../tstring.h"
#include "../CadLib.h"


#define WB_DEFINE_FILE_MAKER "AVCON UCC-WB"
#define WB_DEFINE_FILE_VERSION "V1.0"
#define WB_OBJECT_SELECT_SEGMENT   10   //ѡ�����ʱ�Ĵ�������Χ
#define  BTN_RECTMAQ_CLOSE 9   //�Ŵ󾵹رհ�ť
#define  INSIDERECT     50     //��ԷŴ󾵾��ο�λ����
#define  MEDIAMINWIDTH  500
#define  MEDIAMAXHEIGHT 400
#define  CAREMAMINWIDTH 400
#define  CAREMAMINHEIHT 300
//denghong
//Эͬ��������
#define SYSTEM_RESERVE     0X00  //ϵͳ����
#define DB_ADD_OBJ         0X01  //��Ӷ���
#define DB_ERASE_OBJ       0X02  //����
#define DB_DELETE_OBJ      0X03  //ɾ��
#define DB_ROTATE          0X04  //��ת
#define DB_ZOOM            0X05  //����
#define DB_LOCK            0X06  //����
#define DB_CHAGE_ATTRIBUTE 0X07  //��������
#define DB_MOVE            0X08  //�ƶ�
#define DB_COMBINE         0X09  //���
#define DB_LAYERTOTOP      0X0A  //ͼ���ö�
#define DB_LAYERTOBOTTOM   0X0B  //ͼ���õ�
#define DB_FREVIOUSLAYER   0X0C  //ͼ������
#define DB_NEXTLAYER       0X0D  //ͼ������
#define DB_DELETE_ALLOBJ   0X0E	 //ɾ��ȫ������
#define DB_SETBACKGROUND   0X0F  //��Ϊ����
#define DB_ERASEINDEX      0X10  //����ֱ��˳��


//ֱ�߶���
#define MOUSE_ONRULERATTRIBUTE_BTN 120
#define MOUSE_ONRULERROTATION_BTN  121
#define MOUSE_ONRULERZOOM_BTN      122
#define MOUSE_ONOTHERAREA          123
#define MOUSE_ONCIRCLEAREA         124

#define MOUSE_ONTHEROTA_BTN        125
#define MOUSE_ONTHECIRCLE          126
#define MOUSE_ONTHEOTHER           127
#define MOUSE_ONTHEROTATION_BTN    128


//���ǰ嶨��
#define  MOUSE_ONSETSQUAREATTRIBUTE_BIN  130
#define  MOUSE_ONSETSQUAREOTATION_BIN    131
#define  MOUSE_ONSETSQUAREZOOM_BIN       132
#define  MOUSE_ONSETOTHERAREA            133  //���ƶ�����ķ���
#define  MOUSE_ONSETCIRCLEAREA           134
//Բ��������
#define  MOUSE_ONCOMPASS_ATTRIBUTE_BTN    136  //���԰�ť
#define  MOUSE_ONCOMPASS_MOVE_RGN          137 //move�ƶ�����
#define  MOUSE_ONCOMPASS_VERTICAL_RGN      138  //��ֱ��������
#define  MOUSE_ONCOMPASS_HORIZONTAL_RGN    139  //ˮƽ��������
#define  MOUSE_ONCOMPASS_ROTATE_RGN        140  //��ת����
#define  MOUSE_ONCOMPASS_ROTATE_DRAW_RGN   141  //��ת�ʻ�ͼ����
//�Ŵ������λ�ö���
#define MOUSE_OUTSIDE 0x0
#define MOUSE_INSIDE 0x1
#define MOUSE_TOP 0x2
#define MOUSE_LEFT 0x3
#define MOUSE_BOTTOM 0x4
#define MOUSE_RIGHT 0x5
#define MOUSE_NORTHWEST 0x6
#define MOUSE_SOUTHWEST 0x7
#define MOUSE_SOUTHEAST 0x8
#define MOUSE_NORTHEAST 0x9
#define MOUSE_LEFT_SLIDER 0xA
#define MOUSE_RIGHT_SLIDER 0xB
#define MOUSE_ON_SLIDER 0xC
#define MOUSE_MARGIN_SLIDER 0xD
#define MOUSE_ON_CLOSE_BTN 0xE
#define MAQN_NEED_REFRESH 0xF
#define MOUSE_STATE_LOCK 0x10
#define MOUSE_STATE_UNLOCK 0x11
#define MAQNBTNWIDTH       40
#define MAQNBTNHIGHT       165

//�Ŵ����������߿���Ŵ󾵵����λ�ƶ���
#define MAQN_SLIDER_WIDTH 160
#define MAQN_SLIDER_HEIGHT 32
#define MAQN_SLIDER_TOP_INTERVAL 12
#define MAQN_SLIDER_LEFT_INTERVAL 0
//�Ŵ����������Ļ�������߿���
#define MAQN_SLIDER_TOP_PADDING 2
#define MAQN_SLIDER_BOTTOM_PADDING 5
#define MAQN_SLIDER_LEFT_PADDING 6
#define MAQN_SLIDER_RIGHT_PADDING 6
//�Ŵ����رհ�ť������Ŵ�����ඨ��
#define MAQN_BTN_WIDTH 32
#define MAQN_BTN_HEIGHT 32
#define MAQN_BTN_RIGHT_INTERVAL 0
#define MAQN_BTN_TOP_INTERVAL 12

//ͼ���ļ�·��
#define WBICOCURVE_STYLE1  _T("icostyle1.png")
#define WBICOCURVE_STYLE2  _T("icostyle2.png")
#define WBICOCURVE_STYLE3  _T("icostyle3.png")
#define WBICOCURVE_STYLE4  _T("icostyle4.png")
#define WBICOCURVE_STYLE5  _T("icostyle5.png")
#define WBICOCURVE_STYLE6  _T("icostyle6.png")
#define WBICOCURVE_STYLE7  _T("icostyle7.png")

//��������
enum WB_OBJ_TYPE
{
	WB_OBJ_NORMAL = 0,
	WB_OBJ_LINE, //ֱ��
	WB_OBJ_CURVE, //����
	WB_OBJ_POLYGON, //����� 
	WB_OBJ_RECT,    //����
	WB_OBJ_SELECTRECT, //ѡ���
	WB_OBJ_PICTURE,  //ͼƬ
	WB_OBJ_MOVIE,	//��Ƶ
	WB_OBJ_MEDIA,   //����Ƶ
	WB_OBJ_FLASH,	//FLASH
	WB_OBJ_DOCUMENT, //�ĵ�
	WB_OBJ_TEXT,		//����
	WB_OBJ_ELLIPSE,	//��Բ
	WB_OBJ_TRIANGLE, //������
	WB_OBJ_RHOMBUS,	//����
	WB_OBJ_BAFFLE,  //����
	WB_OBJ_COMBINE, //���
	WB_OBJ_MAQN,     //�Ŵ�
	WB_OBJ_SPOTLIGHT, //�۹��
	WB_OBJ_ANNOTATIONS,  //��ע
	WB_OBJ_CAMERA,   //����ͷ
	WB_OBJ_DUSTBIN,	//������
	WB_OBJ_RULER,//ֱ��
	WB_OBJ_PROTRACTOR,//������
	WB_OBJ_ANGLE,    //���ɿ̶�
	WB_OBJ_SETSQUARE,//���ǰ�
	WB_OBJ_ARBITRARYTRIANGLE, //����������
	WB_OBJ_PALLELOGAM,      //ƽ���ı���
	WB_OBJ_PENTAGON,      //�����
	WB_OBJ_HEXAGON,      //������
	WB_OBJ_FIVEPOINTSTART, //�����
	WB_OBJ_TRAPEZIUM,     //����
	WB_OBJ_DXF, //DXF
	WB_OBJ_ICOCURVE,//ͼ������
	WB_OBJ_COMPASS,    //Բ��
	WB_OBJ_ARC,        //Բ��
	WB_OBJ_SECTOR,     //����
	WB_OBJ_MAGICPEN, //ħ����
	WB_OBJ_MIDI
};
enum WB_COMMON_ATTRIBUTE
{
	WB_NO_ATTRIBUTE = 100, //������
	WB_TEXT_ATTRIBUTE,//�ı�����
	WB_ANNO_ATTRIBUTE,//��ע����
	WB_TRANS_ATTRIBUTE,//͸��������
	WB_TRANS_COL,//͸���� ��ɫ
	WB_TRANS_COL_WIDTH,//͸���� ��ɫ �߿�
	WB_TRANS_COL_SOLID//͸���� ��ɫ ���
};
//ͼ������
enum WB_LAYER_TYPE
{
	WB_LAYER_NORMAL = 0, //һ��ͼ��
	WB_LAYER_LABEL		 //��עͼ��
};

//��ѡ�ж���ť�����ʶ
enum WB_OBJ_AREA
{
	WB_PS_NONE,	//û��ѡ��
	WB_PS_OBJ,	//ѡ�ж���
	WB_PS_CTRLBUTTON,	//ѡ�п��ư�ť�����������ϵİ�ť
	WB_PS_SELECTBUTTON, //ѡ�ж���ѡ����ϵİ�ť
	WB_PS_SELECTFRAME,	//ѡ�ж���ѡ���
	WB_PS_SELECTPOINT,   //ѡ�б�ע�ļ�ǵĵ�
	WB_PS_INMEDIACTRL,    //����Ƶ���ƿ���
	WB_PS_TRIAMGLEPOINT,    //���������еĵ�
	WB_PS_TRAPEZIUMPOINT    //ѡ�����εĵ�
};

//��ע��ǵķ���
enum WB_OBJ_DIRECTION
{
	WB_DRC_UP = 0, //�������
	WB_DRC_DOWN,   //�������
	WB_DRC_LEFT,   //�������
	WB_DRC_RIGHT   //�������
};

//����밴ť�Ĺ�ϵ
enum WB_MOUSE_STATE
{
	MOUSE_NORMAL = 0,//��ť��ʼ״̬
	MOUSE_HOVER,	 //��껬����ťʱ
	MOUSE_LEAVE,	 //����뿪��ťʱ
	MOUSE_LCLICK,	 //����������ʱ
	MOUSE_LBTNDOWN,	 //����������ʱ
	MOUSE_LBTNUP	 //����������ʱ
};
//��λת��
// enum UNIT_TRANSLATE
// {
// 	UNIT_PIEXL = 0,//���ǳߵ�����
// 	UNIT_CM,    //���׵�λ
// 	UNIT_MM,    //����
// 	UNIT_INCH   // Ӣ�絥λ
// };
enum UNIT_TRANSLATE
{
	UNIT_PIEXL = 0,
	UNIT_CM,
	UNIT_MM,
	UNIT_INCH,
	UNIT_RADIAN,
	UNIT_ANGLE
};

typedef struct _WBFileHeadItem //�װ��ļ�ͷ
{
	char szMaker[12];
	char szVersion[4];
	char szDate[10];
	char szTime[8];
}WBFileHeadItem, *PWBFileHeadItem;

typedef struct _WBLineItem    //ֱ��
{
	CPoint ptBegin;    //���
	CPoint ptEnd;		//�յ�
	int nWidth;			//�߿�
	int nLineType;      //����
	COLORREF clrLine;	//��ɫ
	//BOOL bHaveArrow;	//�Ƿ����ͷ

	LineCap nStartLineCap; //�����ñ
	LineCap nEndLineCap; //�յ���ñ
}WBLineItem, *PWBLineItem;

//////////////////////////////////////////////////////////////////////////

enum CURVEPOINT_TYPE  //�����е������
{
	CURVE_TYPE_NORMAL = 0,			//��ͨ��
	CURVE_TYPE_BREAKPOINT_BEGIN,	//�ϵ㣨��㣩
	CURVE_TYPE_BREAKPOINT_END		//�ϵ㣨�յ㣩
};

//typedef struct _WBCurvePointItem   //�������ߵ�����
//{
//	CPoint pt;					//��λ��
//	//CURVEPOINT_TYPE nType;		//������
//	BOOL bIsErase;				//���Ƿ񱻲���
//	int nWidth;					//�߿�
//}WBCurvePointItem, *PWBCurvePointItem;

typedef struct _WBCurvePointAppendItem{
	_WBCurvePointAppendItem(int _nWidth = 0, bool _bErase = FALSE)
		: nWidth(_nWidth), bErase(_bErase){}
	BOOL bErase;//���Ƿ񱻲���
	int nWidth;//�߿�,���ֻ����Ϊ������ɲ��ó�ʼ������
}WBCurvAppendItem, PWBCurvAppendItem;

typedef struct _WBCurveItem //��������
{
	//������ͬ���ȵ�ָ�����飨�����������������Ҫ��Ϊ�����ű��ã�
	//û����һ���ṹ����Ҫ��Ϊ�˱��ڻ�ͼ)
public:
	_WBCurveItem();
	_WBCurveItem(struct _WBCurveItem *pItem);//��������
	~_WBCurveItem();

public:
	void ClearData();
	void ResetData(Point *pPtData, WBCurvAppendItem *pPtAppendData, unsigned int nPtNum);
	bool AddPoint(int _x, int _y, int _nLineWidth);
	BOOL AddPoint(int _x, int _y, int _nLineWidth, BOOL _bErase);
	BOOL IsPointErased(unsigned int nIndex);

	Point GetPtData(unsigned int nIndex);
	int GetPointWidth(unsigned int nIndex);

	unsigned int GetSize(){return nCount;}

	int GetWidth(){return nWidth;}
	COLORREF GetColor(){return clrLine;}
	int GetType(){return nLineType;}
	LineCap GetStartLineCap(){return nStartLineCap;}
	LineCap GetEndLineCap(){return nEndLineCap;}
	Point* GetPointsItem(){
		XAutoLock lock(CurveItemLock);
		return pCurvePointItem;}
	WBCurvAppendItem *GetAppendItem(){
		XAutoLock lock(CurveItemLock);
		return pCurveAppendItem;}

	void SetWidth(int _nWidth){
		XAutoLock lock(CurveItemLock);
		nWidth=_nWidth;}
	void SetColor(COLORREF _color){
		XAutoLock lock(CurveItemLock);
		clrLine=_color;}
	void SetType(int _nType){
		XAutoLock lock(CurveItemLock);
		nLineType=_nType;}
	void SetStartLineCap(LineCap _nLineCap){
		XAutoLock lock(CurveItemLock);
		nStartLineCap=_nLineCap;}
	void SetEndLineCap(LineCap _nLineCap){
		XAutoLock lock(CurveItemLock);
		nEndLineCap=_nLineCap;}
	XCritSec CurveItemLock;

private:
	Point* pCurvePointItem;
	unsigned int nCount;
	unsigned int m_PrenCount;
	WBCurvAppendItem *pCurveAppendItem;//��pCurvePointItem�������б�ǲ����ĵ�

	int nWidth;			//�߿�
	COLORREF clrLine;	//��ɫ
	int nLineType;      //����
	LineCap nStartLineCap; //�����ñ
	LineCap nEndLineCap; //�յ���ñ
}WBCurveItem, *PWBCurveItem;

//////////////////////////////////////////////////////////////////////////
//jiangchao 2013.4.19
typedef struct _WBIcoCurvePointAppendItem{
	_WBIcoCurvePointAppendItem(int _nIcoScaleWidth = 24, bool _bErase = FALSE)
		: nIcoScaleWidth(_nIcoScaleWidth), bErase(_bErase){}
	bool bErase; //���Ƿ񱻲���
	int nIcoScaleWidth; //ͼ���������С������ȣ����ֻ����Ϊ������ɲ��ó�ʼ������
}WBIcoCurvAppendItem, *PWBIcoCurvAppendItem;

//����struct _WBIcoCurveItem����
typedef struct _WBIcoCurveItem //: public _WBCurveItem //����ͼ������
{
public:
	_WBIcoCurveItem();
	_WBIcoCurveItem(struct _WBIcoCurveItem *pItem);//��������
	~_WBIcoCurveItem();

	//Attributes
	CString GetIcoFilePath(){return m_IcoFilePath;}
	Point* GetPointsItem(){
		XAutoLock lock(IcoCurveItemLock);
		return pCurvePointItem;}
	WBIcoCurvAppendItem* GetAppendItem(){
		XAutoLock lock(IcoCurveItemLock);
		return pCurveAppendItem;}
	unsigned int GetCount(){return nCount;}
	int GetScaleWidth(){return m_nScaleWdith;}

	//Operation
	void ResetData(Point *pPtData, WBIcoCurvAppendItem *pPtAppendData, unsigned int nPtNum);
	void ClearData();
	void ClearPointData();
	bool AddPoint(int _x, int _y, int _nIcoScaleWidth = 24, bool _nErase = false);
	void SetScaleWidth(int _nWidth){
		m_nScaleWdith =_nWidth;
	    if (m_nScaleWdith == 1) ++m_nScaleWdith;}//����ͼ��������
	void SetIcoFilePath(CString pIcoFilePath){ 
		m_IcoFilePath = pIcoFilePath;//����ico�ļ�path
	}
	Point GetPtData(unsigned int nIndex);
	int GetPtWidth(unsigned int nIndex);
	bool IsPointErased(unsigned int nIndex);

	XCritSec IcoCurveItemLock;

private:
	Point * pCurvePointItem; //����������
	unsigned int nCount; //�����ĸ���
	WBIcoCurvAppendItem * pCurveAppendItem; //��pCurvePointItem�������б�ǲ����ĵ�

	CString m_IcoFilePath; //ͼ���ļ�·��
	int m_nScaleWdith; //ͼ�������

}WBIcoCurveItem, *PWBIcoCurveItem;

//////////////////////////////////////////////////////////////////////////
typedef struct _WBPageItem    //ҳ��
{
	int nWidth;               //ҳ����
	int nHeight;              //ҳ��߶�
}WBPageItem, *PWBPageItem;

typedef struct _WBBafflePlate  //����
{
	CPoint ptLeftTop;          //������ҳ���е����Ͻǵ�
	CPoint ptRightBottom;      //������ҳ���е����½ǵ�
	COLORREF clrBackground;    //���屳����ɫ(��ʱû������)
}WBBafflePlateItem, *PWBBafflePlateItem;

typedef struct _WBMagnifier    //�Ŵ�
{
	CRect rect; //�Ŵ󾵾��ο������
	int nZoom;  //�Ŵ���
}WBMagnifierItem,*PWBMagnifierItem;

typedef struct _WBProtractor     //������
{
	CPoint        ptProtractorLT;      //���������ϵ� 
	CPoint        ptProtractorRB;      //���������µ�
	CPoint        ptProtractorMP;      //������������
	CPoint        ptProtractorCT;      //����������
}WBProtractorItem,*PWBProtractorItem; 

typedef struct _WBAngle    //���ɿ̶�
{
	CPoint        ptAngleLT;     //�̶������ϵ�
	CPoint        ptAngleRB;     //�̶������µ�
	CPoint        ptAngleCT;     //�̶������
}WBAngleItem,*PWBAngleItem; 

typedef struct _WBSpotLight   //�۹��
{
	CRect rect;//�۹���Բ������
}WBSpotLightItem,*PWBSpotLightItem;

typedef struct _WBPolygonItem //�����
{
	std::vector<CPoint *> vecPoints; //���б�
	int nWidth;			//�߿�
	COLORREF clrLine;	//����ɫ
	BOOL bIsFillSolid;		//�Ƿ����
	COLORREF clrFillSolid;	//�����ɫ
}WBPolygonItem, *PWBPolygonItem;

typedef struct _WBSelectRectangle  //ѡ���
{
	CPoint ptLeftTop;          //ѡ������Ͻǵĵ�
	CPoint ptRightTop;         //ѡ������Ͻǵĵ�
	CPoint ptRightBottom;      //ѡ������½ǵĵ�
	CPoint ptLeftBottom;       //ѡ������½ǵĵ�
}WBSelectRectangleItem, *PWBSelectRectangleItem;

typedef struct _WBPictureItem //ͼƬ
{
	CString strFilePath; //�ļ�·��
	CRect rect;  //λ��	
	//int nAngle;  //�Ƕ�
}WBPictureItem, *PWBPictureItem;

typedef struct _WBFlashItem //ͼƬ
{
	CString strFilePath; //�ļ�·��
	CRect rect;  //λ��
	unsigned long curtime;//��ǰ����ʱ��
	//int nAngle;  //�Ƕ�
}WBFlashItem, *PWBFlashItem;

typedef struct _WBDocumentItem //ͼƬ
{
	CString strFilePath; //�ļ�·��
	CRect rect;  //λ��
	//DOCUMENT_TYPE nType; //�ĵ�����
}WBDocumentItem, *PWBDocumentItem;

enum DOCUMENT_TYPE  //�ĵ�����
{
	DOCUMENT_TYPE_DOC = 0,			
	DOCUMENT_TYPE_PDF,	
	DOCUMENT_TYPE_XLS,		
	DOCUMENT_TYPE_PPT
};

typedef struct _WBMovieItem //��Ƶ
{
	CString strFilePath; //�ļ�·��
	CRect rect;  //λ��
	LONGLONG llCurTime;	//������Ƶ��ǰ����ʱ��
}WBMovieItem, *PWBMovieItem;

typedef struct _WBMediaItem //����Ƶ
{
	CString strFilePath; //�ļ�·��
	CRect rect;  //λ��
	ECtrlState showState; //�Ƿ���ʾ���ƶԻ���
	EMediaPlayState playState;
	EVoiceState voiceState;
	long llCurTime;           //��ǰ����ʱ��
	BOOL bIsMediaLoop;        //�Ƿ�ѭ������
	std::string UCCUerID;
}WBMediaItem, *PWBMediaItem;

typedef struct _WBCameraItem //����ͷ����
{
	CRect rect;  //λ��
	//���շ�������Ҫ���������ݶ���
	SCameraReceiveData dataReceiver;
	//�������ӷ����������ݶ���
	SCameraServerData dataServer;
	//���ش򿪵����ݶ���
	CameraSet dataLocalCamera;
}WBCameraItem, *PWBCameraItem;

typedef struct _WBTextItem //�ı�     
{
	CString strText; //�ı����� 
	LOGFONT  fontItem; //����	
	CPoint ptTopLeft;	//��¼����������Ͻǵĵ�
	COLORREF clrText;  //�ı���ɫ
	CRect rectText; //�ı���ʾ�ľ��Σ�ͨ��DrawText��������
}WBTextItem, *PWBTextItem;

typedef struct _WBEllipseItem //��Բ
{
	CPoint ptTopLeft;	//���ڶ�λ��Բ�����Ͻǵĵ�
	CPoint ptBottomRight;	//��λ��Բ�����½ǵĵ�	
	int nLineType;		//�ߵ�����
	int nWidth;			//�߿�
	COLORREF clrLine;	//����ɫ
	BOOL bIsFillSolid;		//�Ƿ����
	COLORREF clrFillSolid;	//�����ɫ
}WBEllipseItem, *PWBEllipseItem;

typedef struct _WBSetSquare    //���ǰ�
{
	CPoint        ptSetSquareLT;      //���ǰ����ϵ� 
	CPoint        ptSetSquareInLT;      //���ǰ������ڵ�
	CPoint        ptSetSquareInRB;      //���ǰ������ڵ�
	CPoint        ptSetSquareRB;      //���ǰ����µ�
	//CPoint        ptSetSquareInCT;      //��������ֱ�ǵ�
	CPoint        ptSetSquareCT;      //���ǰ����ĵ�
}WBSetSquareItem,*PWBSetSquareItem; 

typedef struct _WBCompass     //Բ��
{
	double  dRadian;             //֧����뾶�нǻ���
	double  dAngle;				 //֧����뾶�нǽǶ�
	double  dRadius;			 //�뾶
	double  dLegLength;			 //֧�ȳ�

	CPoint  ptCompassCT;		 //��ת���ĵ�
	CPoint  ptCompassMid;        //Բ���м��
	//CPoint  ptCompassMidUp;      //Բ���м��Up��
	CPoint  ptCompassMidLT;   
	CPoint  ptCompassMidRT;
	CPoint  ptCompassLT;		 //���ϵ�
	CPoint  ptCompassLTUp;       //����Up��
	CPoint  ptCompassLB;         //���µ�
	CPoint  ptCompassLBUp;       //���µ�Up��
	CPoint  ptCompassRB;		 //���µ�
	CPoint  ptCompassRBUp;		 //���µ�Up��

	CPoint  ptCompassHPLB;		 //����1�ŵ�-����̶��������µ�
	CPoint  ptCompassHPLT;		 //����1�ŵ�-����̶����ʵ����ϵ�
	CPoint  ptCompassHPRT;		//����1�ŵ�-����̶��������ϵ�
	CPoint  ptCompassHPRB;		//����1�ŵ�-����̶��������µ�
	
	CPoint  ptCompassPenLT;    //���ʵ����ϵ�
	CPoint  ptCompassPenLB;		//���ʵ����µ�
	CPoint  ptCompassPenRT;		//���ʵ����ϵ�
	CPoint  ptCompassPenRB;		//���ʵ����µ�

	CPoint  ptCompassRefillLT;  //��о�����ϵ�
	CPoint  ptCompassRefillRT;  //��о�����ϵ�

}WBCompassItem, *PWBCompassItem;

typedef struct _WBArc  //Բ��
{
	double dArcStartAngle; //��ʼ�Ƕ�
	double dArcSweepAngle;  //Բ����Ӧɨ���ĽǶ�
	CPoint ptArcCT;     //Բ�����ĵ�
	double dRadius;    //�뾶
	RectF rectf;  //Բ�������о���
	CPoint ptSelectLT; //Բ��ѡ����LT
	CPoint ptSelectRB;   //Բ��ѡ����RB
	CPoint ptDrawStart;  //Բ�����
	CPoint ptDrawEnd;    //Բ���յ�
	int nWidth;			//�߿�
	COLORREF clrLine;	//����ɫ
	int nLineType;      //����
	LineCap nStartLineCap; //�����ñ
	LineCap nEndLineCap; //�յ���ñ

}WBArcItem, *PWBArcItem;

typedef struct _WBSector   //����
{
	double dSectorStartAngle; //������ʼ�Ƕ�
	double dSectorSweepAngle; //���ζ�Ӧ�ĽǶ�
	CPoint ptSectorCT;        //���ζ�Ӧ���ĵ�
	CPoint ptDrawStart; //�������
	CPoint ptDrawEnd;   //�����յ�
	CPoint ptSelectLT;     //����ѡ����LT
	CPoint ptSelectRB;      //����ѡ����RB
	RectF rectf;  //�������о���
	double dRadius;    //�뾶
	int nWidth;			//�߿�
	int nLineType;      //����
	COLORREF clrLine;	//����ɫ
	BOOL bIsFillSolid;		//�Ƿ����
	COLORREF clrFillSolid;	//�����ɫ

}WBSectorItem, *PWBSectorItem;
//////////////////////////////////////////////////////////////////////////
typedef std::map<std::string,std::string> MapFilePath;			 //huanglongquan add
typedef MapFilePath::iterator IT_MapFilePath;

class CWBObject;
//jiabangping
typedef CTypedPtrList<CObList, CWBObject*>  CWBList;
//�߿���ɫ�����͡�ͼ���ļ�·��
extern CString g_nWBCurObjIcoFilePath;//����ͼ���ļ�·��
extern int g_nWBCurObjWidth; //�߿�
extern COLORREF g_clrWBCurObjColor;//��ɫ
extern COLORREF g_clrWBPageColor;//������ɫ
extern int g_nWBLineType;//����
extern BOOL g_bIsSolid; //�Ƿ����
extern COLORREF g_clrSolid; //�����ɫ
extern LineCap g_nStartLineCap; //�����ñ
extern LineCap g_nEndLineCap; //�յ���ñ
extern CWBObject *g_pObj;//ѡ���б���ֻ��һ��ʱ�Ķ���
//extern BOOL g_IsSelectObj;//�Ƿ���ѡ�еĶ���
extern float g_nTranspar;//��ѡ����͸����
extern BOOL g_bDrawSelectButton; //��ѡ���ť��ʶ
extern CString g_nWBCurObjFontFaceName;//��������
extern int g_nWBCurObjFontSize;//�����С
extern BOOL g_isBold;//�Ƿ����
extern BOOL g_isItalic;//�Ƿ�б��
extern BOOL g_isUnderline;//�Ƿ��»���
extern BOOL g_bIsVertical;//�Ƿ�����
extern CString g_cTextstr;//�ı�����
extern BOOL g_bIsMTRotate; //�ĵ�����Ƶ��ͼ���ƴ���ʱ�Ƿ������ת������TRUE����ת��FLASE�����ţ�
extern BOOL g_bAttriDlginitialize;//��������Ƿ��ʼ��(�������пؼ�����ʾΪ��ʼ��)
extern CString g_strSelectedResPath;//��Դ��·��
extern RES_INPUT_METHOD g_nMethodOfResToView;//��Դ�б��е���Դ��view�еķ�ʽ
//extern BOOL g_isFunctionDlgHide;//��������Ƿ�����
extern int g_nWBTriangleAngle; //���������ε��ڽǶ���
extern CPoint g_ptAddPoint;  //���ڴ洢��ͬ�ֱ��ʾ۹�Ƶ�ƫ����
extern BOOL g_bIsWBLoadIcoFromFile;//����ͼ���Ƿ���ļ�����

//////////////////////////////////////////////////////////////////////////
class CWBFile;
class CWBPage;
class CWBObject;
class CWBLine;
class CWBCurve;
class CWBIcoCurve;
class CWBPolygon;
class CWBRectangle;
class CWBSelectRectangle;
class CWBMovie;
class CWBText;
class CWBFlash;
class CWBImageButton;
class CWBControlButton;
class CWBCombine;
//class CWBRectMaqn;
class CWBMagnifier;
class CWBImageSliderControl;
class CWBAnnotations;
class CWBGlobalMediaCtrl;
class CWBComponent;
class CWBMidi;
//////////////////////////////////////////////////////////////////////////



class CWBFile : public CObject
{
public:
	CWBFile();
	virtual ~CWBFile();//����

	BOOL Save(CString strFilePath); //����
	BOOL Load(CString strFilePath);//����
	BOOL FillinFileHead(char *pBuf, DWORD& dwLen);
	BOOL FillOutFileHead(char *pBuf, ULONGLONG& dwLen);
	BOOL FillOutFileHead(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ


	void ClearPageList();  //���ҳ�б��������е����������Ѿ�����
	void  ReductionList(); // ��ԭҳ���е�¼�����//����

	CWBPage * GetCurPage() const;  //�õ���ǰҳ
	CWBPage * GetPage(UINT nPageIndex) const; //�õ�ĳһҳ
	UINT GetCurPageIndex() const; //�õ���ǰҳ��
	UINT GetPageCount() const;  //�õ���ҳ��
	int DeleteLastContinuousEmptyPages();	//ɾ���ļ��дӺ���ǰ�������������Ŀհ�ҳ

	//ҳ����
	CWBPage* AddPage(CWnd *pParentWnd = NULL); //�����ҳ//����
	void AddPage(CWBPage* pAdd);			//�����ҳ//����
	BOOL PrePage(); //��һҳ
	BOOL NextPage(); //��һҳ //����
	BOOL FirstPage(); //��һҳ
	BOOL LastPage();  //���ҳ //����
	BOOL GotoPage(UINT nPage); //����
	UINT InsertPage(CWnd *pParentWnd = NULL); //����һҳ //����
	UINT InsertPage(UINT nPageIndex, CWnd *pParentWnd = NULL); //����һҳ //����
	UINT DeletePage(CWnd *pParentWnd = NULL);//ɾ��һҳ //����
	UINT DeletePage(UINT nPageIndex, CWnd *pParentWnd = NULL);//ɾ��һҳ //����
	BOOL InterchangePage(UINT nFormerPage, UINT nTargetPage);//����ҳ //����
	BOOL MovePage(UINT nSrcPageIndex, UINT nDesPageIndex); //�ƶ�ҳ //����

	bool ParseMapFilePath(const char* str_name,const char* str_path);
	const char* GetmapfilePathByName( const char* str_name );
	void clearmapfilePath();
	CObList* GetWBPageList(){return m_pWBPageList;}
	void WBPageListSetNull(){m_pWBPageList = NULL;}
	PWBFileHeadItem GetFileHeadItem(){return m_pWBFileHeadItem;};
	void PWBFileHeadItemSetNull(){ m_pWBFileHeadItem = NULL;}
private:
	CObList* m_pWBPageList;
	UINT m_nCurPage;
	PWBFileHeadItem m_pWBFileHeadItem;	

	CString m_strTempDirector; //��ʱ�ļ���·��
	MapFilePath m_mapFilePath;
	DWORD m_nMaxRamdonPageID;

	XCritSec			m_csWBFilePageLock;//�԰װ��ļ��е�ҳ��������
};

class CWBPage : public CObject //ҳ�࣬�洢�����б�
{
public:
	CWBPage(CWnd *pParentWnd = NULL);
	virtual ~CWBPage();//����

	void SetParent(CWnd* pParent);	//����ҳ�ĸ�����

	//���棨��ȡ�������ӣ�������
	BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	BOOL Load(char *pBuf, ULONGLONG& lLen);
	BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const; //���浽�ļ�//����
	BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ

	void Draw(CDC *pDC) /*const*/; //������ʾ������//����
	void DrawLabel(CDC *pDC); //����ע����
	void ClearObjList();  //��������б���������ʹ��������������
	void ClearLabelObjList(); //�����עͼ������б�
    void DrawAsBakgroundCanves(CDC *pDC);//ƽ�̱���ͼƬ
	void CaclBigPic(CString *str);//ƴһ�Ŵ�ͼ
	void SetPageDC(CDC *pDC);//��m_dcColor��������ɫ���һ����ʱλͼ
	void SetBKGColor(COLORREF color);//���ñ�����ɫ
	//����¼��
	void	SetbStartConferenceRecord(bool bStartConferenceRecord);
	bool GetbStartConferenceRecord();
	BOOL SendAllPageDataToConferenceRecord();

	//������Ƶ����ͷ�ص�������
	void StartMediaOrCaremaForceEncode(CWBObject *pobj);

	CDC* GetPageDC();
	/////////////////////////////////
	void SetPageID(UINT nPageID);
	UINT GetPageID();

	//operations	
	//�������
	DWORD GetSelectObject(CPoint pt) const; //���ݵ㷵�ر�ѡ�еĶ���id//����

	//��Ӷ���
	DWORD AddWBObject(CWBObject *pObj, int nLayerType = WB_LAYER_NORMAL); //����
	BOOL AddComObject(DWORD dwLayerIndex, CWBObject *pComObj); // ����������ֱ�� //����
	BOOL AddObjIntoCom(CWBObject *pObj); //����

	//���ݶ���id�Ͷ������Ͳ��Ҷ���
	//denghong
	CWBObject *FindByLayerIndex(DWORD dwLayerIndex, CObList *pList) const;
	CWBObject *FindByLayerIndex(DWORD dwLayerIndex, int nObjType = WB_OBJ_NORMAL) const; //����ͼ��ŵõ�����
	CWBObject *FindComObjByLayerIndex(DWORD dwLayerIndex) const; //����ͼ��ŵõ����������Ķ���
	CWBObject *FindByID(DWORD dwID, int nObjType = WB_OBJ_NORMAL) const; //���ݴ�����ID�õ�����//����
	DWORD GetAllObjLayerIndex(std::vector<DWORD *>& vecLayerIndex, int nObjType = WB_OBJ_NORMAL) const; //�õ�ҳ���ж����ͼ���//����
	DWORD GetLastPageAllObjLayerIndex(std::vector<DWORD *>& vecLayerIndex, int nObjType = WB_OBJ_NORMAL) const; //�õ����ҳ���ж����ͼ���//����
	BOOL DeleteEraseListObject(DWORD dwLayerIndex);//����ͼ���ɾ�������������ڲ��������б�ɾ���Ķ���

	BOOL DelWBObj(DWORD dwLayerIndex); //ɾ������ָ��//����
	BOOL DeleteWBObject(DWORD dwLayerIndex); //����ͼ���ɾ��������m_pWBObjList���Ƴ�ͬʱɾ���ڴ��еĶ���//����
	DWORD DeleteSelectedWBObjects(DWORD** delObjArray); //ɾ����ǰ��ѡ��Ķ���,�������ǵ���DeleteWBObject(DWORD dwLayerIndex)
	BOOL DeleteAllObjects(unsigned long eventId = 0); //ɾ��ȫ��������m_pWBObjList���Ƴ�ͬʱɾ���ڴ��еĶ�����m_pWBObjList���Ƴ�������ָ�����m_pWBDeleteObjList//����
	DWORD DeleteWBObjects(const CRect& rectDelete); //������Ƥ��ɾ�����ж���(���)����m_pWBObjList���Ƴ�������ָ�����m_pWBDeleteObjList //����
	DWORD DeleteWBObjects(const CRgn *pRgn); //ɾ�������ڵ����ж��� ����m_pWBObjList���Ƴ�ͬʱɾ���ڴ��еĶ���//����
	BOOL DeleteListForSetquare();       // ɾ��ѡ�����ǰ� ��ǰѡ�еĵĶ����ѡ���
	//denghong
	DWORD DeleteWBObjects(int nObjType, DWORD dwLayerIndex = 0); //����
	/******************************
	*chenchangtao
	*���ڰװ�������Ƴ���Ƶ����������ͷ����ʱ�������ָ��
	*����ָ���Ƴ���Ƶ����ͷ�����б��е�ĳһ����
	*******************************/
	void RemoveMediaCameraObject(const CWBObject* pWBObj);
	//ѡ������////////////////////////////////////////////////////////////////////////
	void ClearSelectFrameList(); //���ѡ����б�
	BOOL AddSelectFrame(DWORD dwLayerIndex, BOOL bAddSelect = TRUE); //��ӣ�ɾ����ѡ���
	BOOL SelectMoreObjAttribute();//ѡ����������
	DWORD AddSelectFrames(CRect& rect); //���ݾ������ѡ��� //����
	BOOL DeleteSelectFrame(DWORD dwLayerIndex); //���ݶ���ͼ���ɾ��ѡ���
	DWORD AddAllSelectFrames(); //ѡ��ȫ������

	int PointInSelectFrame(const CPoint& pt); //�жϵ��Ƿ���ĳ��ѡ�ж����ѡ����ť��//����
	int PointInText(const CPoint& pt); //�жϵ��Ƿ����ı���//����
	//��Ƶɾ������λ��wangxiaobin
	std::map<DWORD ,CRect> * GetMediaRectMap();
	std::map<DWORD ,CRect> * GetCaremaRectMap();

	std::vector<DWORD> GetWBSelectFrameList();
	bool IsCombinInSelectObj();
	CObList *GetWBObjList();
	CObList *GetWBLabelObjList();
	void SetCurObjLayerIndex(DWORD dwLayerIndex);
	CObList*GetMediaCameraList();

	CObList *GetWBDeleteObjList();
	//��ʹ��ɾ�����������ɾ����ʱ�����DeleteWBObjects������ָ�����m_pWBDeleteObjList�У������б����Ƴ�����û��ɾ���ڴ��ж���
	//�������������ʱ����ô˺�����m_pWBDeleteObjList�ж���ɾ��,
	void DeleteWBDeleteObjList();

	CObList *GetEraseCurveList();
	void DeleteEraseCurveList();
	BOOL IsCurveInEraseCurveList(DWORD dwLayerIndex);

	CObList *GetEraseDocList();
	void DeleteEraseDocList();
	BOOL IsCurveInEraseDocList(DWORD dwLayerIndex);

	void SortWBList(std::vector<DWORD> vecLayerIndex, CObList *pList); //����
	BOOL SetWBList(CObList *pList); //����

	int GetSelectLockCount(); //�õ�ѡ����б��в�����ϵĶ�����������ע������Ƶ������ͷ�����������壬�ĵ����ĸ���

	//����ƫ�����ƶ���ǰѡ��Ķ���
	void MoveSelectedObj(const CPoint& ptOffset); 

	//ͼ�����
	void ResetObjLayerIndex(); //���ݶ���˳�������������ж����ͼ��� //����
	BOOL SetObjUnderLayer(DWORD dwLayerIndex); //�������Ƶ���һ�� //����
	BOOL SetObjSuperLayer(DWORD dwLayerIndex); //�������Ƶ���һ��
	BOOL SetObjBottomLayer(DWORD dwLayerIndex); //�������Ƶ���ײ� //����
	BOOL SetObjTopLayer(DWORD dwLayerIndex); //�������Ƶ����ϲ� //����

	/************************���´������ͼ����� by huanglongquan*************************************/
	BOOL ChangeOBJLayer(DWORD dwObjid,int nLayerNum);//ͼ�㽻�� //����
	CWBObject* FindObjLayerByNum(int nLayerNum, CObList *pList); //ͨ������ͼ����뷵�ض���ָ�� //����
	CWBObject* FindRecentlyBaffleObj(CObList *pList)const;
	CWBObject* Findprotractor(CObList *pList)const;
	int GetCurrentPageMaxLayer();//����
	int GetCurrentPageMinLayer();//����

	/**************************************************************/
	//����
	void EraseObjects(const CRect& rectErase); //����

	void EraseOccasionalObjects(const CRect& rectErase);//��ʱ����ֱ�ߣ����� //����

	//�õ���������
	CWBObject *GetBkgObj(); //����

	//ֹͣ�򲥷���Ƶ��FLASH
	void StartOrPauseMovie(unsigned long ulsend = 0);
	void StartOrPauseMovieCur(bool bl);

	//�õ���ǰ���ڱ༭�Ķ���ID��
	DWORD GetCurEditingObjLayerIndex();

	//���õ�ǰ���ڱ�ݵĶ���ID��
	void SetCurEditingObjLayerIndex(DWORD dwCurEditingObjLayerIndex);

	//�õ�ҳ���ڴ�DC
	HDC GetMemHDC() const;	

	//�õ�ҳ��עͼ���ڴ�CDC
	HDC GetLabelMemDC() const;

	//ҳ�й�������ֵ5-11
	void SetPageViewScroll(POINT viewScrollPos);
	POINT GetPageViewScroll();	
	void SetComLayerIndex(CWBObject *pObj); //������ϳ�Ա��ͼ���
	//��ȡѡ�ж���ĸ���
	UINT GetSelectObjNumber();
	//�õ�ѡ�ж����б�
	CWBList *GetSelectObjLst();

	//����m_nWBMaxLayerId��ֵ
	void SetMaxLayerIdIncrease(int &nWBMaxLayerId);

	//����ҳ��Ψһ��ʶ
	void SetPageRamdonId(DWORD nPageRamdonId);
	DWORD GetPageRamdonId();
	void ReleaseLimitObjs();//huanglongquan add ����ȡ�������������
protected:
	//UINT CreateID();//�����¶���ID��

	//����ֱ��
	void EraseLine(CWBLine *pLine, const CRect& rectErase); 

	void OccasionalEraseLine(CWBLine *pLine, const CRect& rectErase);//��ʱ����ֱ��

	DWORD GenerateObjID(); //��������ID 
    XCritSec			m_csWBObjListLock;//��page���ж����б�m_pWBObjList����	

	DWORD          m_PreLayerObjIndex;
public:
	std::vector<CWBObject *> m_vecFailDoc; //ת��ʧ�ܵ��ĵ�
	std::vector<CWBObject *> m_pVecCombineGif; //�洢��϶����е�ͼƬ���Ͷ���
	std::vector<DWORD> m_vecLineObjID;//ֱ�߶���ID
	std::vector<DWORD> GetvecLineObjID();
	void DeleteLine();//��С����������ֱ�����ɶ���ɾ�������������������õ�
	HBITMAP GetCurPageIndexBitmap();
	BOOL m_bIsDelCombine;//�����������ͻ��Ҫʹ�õ�һ���ж�
	BOOL m_bIsDelComCurve;//����ں϶���
	//�����Ƿ��б���ͼ
	void SetIsExistBackGround(BOOL bl){ m_bIsSetBackGround = bl;}
	BOOL GetIsExistBackGround() {return m_bIsSetBackGround;}
	void ChangeMagicAlpha();
	int GetSelectOneObjectLayer();
	CObList* GetWBList(){return m_pWBObjList;}
private:
	CObList *m_pMediaCameraList;//��Ƶ����ͷ�����б����ڹ���ҳ�ȶ���ʱý�崰�ڵ���ʾ������
	int m_nWBMaxLayerId; //��ǰ�װ�����ͼ���id������������������Ԫ�ص�ͼ��ţ�huanglongquan ADD by2012��4��18��
	int m_nWBMinLayerId; //��ǰ�װ���С��ͼ���id
	int m_nWBLabelLayerMinId;//��ע��Сͼ��
	int m_nWBLabelLayerMaxId;//��ע���ͼ��
	CObList *m_pWBObjList; //�����б�
	CObList *m_pWBLabelObjList; //��עͼ������б�
	
	DWORD m_dwCurObjLayerIndex; //��ǰ���¶���Ĳ���	

	//��Ƶɾ������λ��wangxiaobin
	std::map<DWORD ,CRect> *m_pMediaRect;
	std::map<DWORD ,CRect> *m_pCaremaRect;

	DWORD m_dwCurEditingObjLayerIndex; //��ǰ���ڱ༭�Ķ���ͼ���

	POINT m_viewScrollPos; //ÿҳview�Ĺ�����λ��

	CDC m_dcMem;	//ÿ��ҳ�����Լ����ڴ�DC
	CDC m_dcTempPic;//��ʱ�洢��ͼƬ
	CDC m_dcLabelMem; //��עͼ����ڴ�DC
	
	CBitmap m_bmpBuf;

	HBITMAP m_hbitmap;//Ϊ�ӿ��д���ȥһ��λͼ���������ҳ��λͼ

	CBitmap m_bmpBufLabel;	

	CWnd *m_pParentWnd;

	std::vector<DWORD> m_vecSelectFrameList;	//ѡ����б�ֻ��¼��ѡ�ж����ͼ��ţ���m_dwLayerIndex
	//jiabangping
	CWBList m_lstSelectObject;
	//denghong
	CObList *m_pWBDeleteObjList;

	//denghong
	CObList* m_pEraseCurveList;
	//luowei
	CObList* m_pEraseDocList;
	bool		m_bStartConferenceRecord;//�жϸ�ҳ�����ж����Ƿ��Ѿ����͸�����

	DWORD m_nPageRamdonId;
	COLORREF m_BKGColor;//������ɫֵ


	public:
	 long m_lOnTimer;
	 //std::map<DWORD, int>* m_MapGifID;
	
	BOOL m_bIsSetBackGround;//�ж��Ƿ��б���ͼ
	BOOL m_bIsMoreSelect; //�Ƿ��ѡ

	std::vector<CWBObject *> m_pMagicList; //ħ�������б�
};

class CWBObject : public CObject, public CWBMTObject //���������
{
public:
	CWBObject();
	virtual ~CWBObject();

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);
	virtual void DrawAsBakground(CDC *pDC, CWnd *pParentWnd); //��Ϊ������

	///�̳���AVCON_SDK_MTTouch::CMTObject

	/*---------------------------------------------------------------------------------------------
	����һ�����ֽڱ�ʾ���ƵĴ���[ETouchGestureType]�����ֽڱ�ʾ���Ƶ�С��[�綯̬�����е�]
	������: ���ֽڱ�ʾx�����꣬���ֽڱ�ʾy���������. ��λΪ���أ�����Ϊ��Ļ����
	----------------------------------------------------------------------------------------------*/
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage = 0);
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	//�����ڵ�ǰ��������һ����ָ����ִ��ͬ������
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te);
	//�����������Ƿ���Ҫִ��ͬ��
	bool  NeedTouchSynchronize();

	
	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	void ReservePoint(const CPoint& ptOriginal, CPoint& ptReserve) const; //���ݶ�����תӳѡ���������
	void ReserveObjPoint(const CPoint& ptOriginal, CPoint& ptReserve) const; //���ݶ�����ת�����ż��������������
	virtual int IsInSelectArea(const POINT &pt)const;
	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ��
	virtual BOOL IsInSelectArea(const CRect& rect) const; //����ѡ��

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ

	//����ѡ���
	virtual void CreateSelecrtFrame();

	//�������������
	virtual void AdjustObject();

	//������������
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //���ݶ�����ת�����ź󴴽�����

	//���ö��������ĵ���ת�ĽǶ�
	virtual void SetRotarionAngle(double dbAngle);
	virtual double GetRotationAngle() const;
	void GetRotationCenter(CPoint& ptRotationCenter) const;
	void SetRotationCenter(const CPoint& ptRotationCenter);

	//�������ĵ��ֵ
	void GetDrawRotationCenter(CPoint& ptRotationCenter) const;
	void SetDrawRotationCenter(const CPoint& ptDrawRotationCenter);
	//��ȡ�ϴε���ת���ĵ�
	void GetPreRotationCenter(CPoint& ptPreRotationCenter){ptPreRotationCenter = m_ptPreRotationCenter;}

	//�����������
	double GetXZoomScale() const;
	double GetYZoomScale() const;
	void GetZoomCenter(CPoint& ptZoomCenter) const;
	void SetXZoomScale(const double dbXZoomScale);
	void SetYZoomScale(const double dbYZoomScale);
	void SetZoomCenter(const CPoint& ptZoomCenter);

	//���ö���ѡ�����ת�Ƕ� wangxin
	void SetRectSelectAngle(double dbAngle);
	void SetRectSelectRotationCenter(const CPoint& ptRotationCenter);

	//����id����
	void SetLayerIndex(DWORD dwLayerIndex);
	DWORD GetLayerIndex() const;

	//ͼ�����
	void SetPageLayerIndex(int nPageLayerIndex);
	int	GetPageLayerIndex()const;

	//�������Ͳ���
	void SetType(int nType);
	int GetType() const;

	//����ͼ�����Ͳ���
	void SetLayerType(int nLayerType);
	int GetLayerType() const;

	//����͸���Ȳ���
	virtual void SetAlpha(int alpha);
	int GetAlpha() const;
	
	//�����Ķ���͸���Ȳ���
	void SetAlphaInCom(int AlphaInCom);
	int GetAlphaInCom() const;

      //�Ƿ��ػ�ѡ���
      void SetIsCreat(BOOL IsCreat);
	  BOOL GetIsCreat() const;

	virtual void SetPreAlpha(int prealpha);
	int GetPreAlpha() const;


	//ƫ�����ƶ�����
	virtual void MoveOffset(const CPoint& ptOffset); 

	//������ID
	void SetTouchID(DWORD dwID);
	DWORD GetTouchID() const;

	//�õ�����ѡ���
	CWBSelectRectangle * GetObjSelectFrame() const;
	//���ö���ѡ���
	void SetObjSelectFrame(CWBSelectRectangle *pRectSelect);

	//���������ĵ�
	virtual void GetCenterPoint(CPoint& ptCenter);

	//�ж��Ƿ�Ϊ�ĵ��Ĵ����ƶ�
	void SetInTouchMove();
	bool GetInTouchMove();

	//����ת��
	DashStyle GetDashStyle(int nLineStyle) const;

	//�ԽǶ�ƫ����ת����
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//������ĳһ����ת
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת

	//��������
	//virtual void ZoomByLeftTop(const CPoint& ptOffset); //�����ϵ�Ϊ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���

	//��̬ͨ�ú���////////////////////////////////////////////////////////////////////////
	//�жϵ��Ƿ���ֱ����
	static BOOL PointInLine(const CPoint& pt, const CPoint& ptLineA, const CPoint& ptLineB, int nWidth);
	//�����ź������
	static void ZoomPoint(CPoint& point, const CPoint& ptZoomCenter, double dbXScale, double dbYScale, int nIndex, BOOL bIsToZero);
	static BOOL ZoomRect(CRect& rect, const CPoint& ptZoomCenter, double dbXScale, double dbYScale, BOOL bIsToZero);

	int RotateXForm(double dbAngle); //��ת�Ƕ�
	void ResumeXForm(int nGraphicsMode); //�ָ��Ƕ�

	int ZoomXForm(double dbXScale, double dbYScale); //����
	//void ResumeZoom(int nGraphicsMode); //�ָ�����

	//�����ĵ��ֵ
	CPoint m_ptDrawRotationCenter;


	//����
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);

	//���ö�������
	void Lock(bool bIsLock = true);
	//��ö�������״̬
	BOOL IsLock() const;

	//���ƶ������  by huanglongquan	2012��12��13��
	void Limit(bool bIsLimit = true);
	//��ö�������״̬
	bool IsLimit() const;

	//���ö��󱳾�����
	void SetBkg(BOOL bIsBkg = FALSE);
	//��ö��󱳾�����
	BOOL IsBkg() const;

	virtual BOOL IsSolid() const; //�ж϶����Ƿ����

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	//�����Ƿ����Ź�
	BOOL GetIsZoom();
	virtual void SetIsZoom(BOOL bIsZoom);

	//�������ĵ��Ƿ��ƶ�
    virtual void SetIsCenterpoint(bool bIsCenterpoint);

    //�����Ƿ�Ϊ�ĵ�
    bool GetIsDocument();
    virtual void SetIsDocument(bool bIsDocument);

	bool GetIsTouchDocumnet();
	virtual void SetIsTouchDocument(bool bIsTouchDoc);

	//������������
	BOOL IsEqualScaleZoom() const;
	void SetEqualScaleZoom(BOOL bEqualScaleZoom = TRUE);
	double GetEqualScale() const;

	//�������ĵ�
	BOOL IsRorationCenterZoom() const;
	void SetRorationCenterZoom(BOOL bRorationCenterZoom = TRUE);

	//�õ������ӳ�����ĵ�
	void GetMapCenter(CPoint& ptMapCenter) const;
	void SetMapCenter();

	//����ѡ����Ƿ���ʾ
	virtual void SetSelectFrameShow(BOOL bShow);
	//���ѡ�����ʾ״̬
	BOOL GetSelectFrameShow();

	//��ʼ��ͼƬ��ť
	void InitImageButton();

	//�õ�ͼƬ��ť
	CWBImageButton *GetImageButton();
	void DelImageButton();

	void CopyAttribute(CWBObject *pObj); //���������ĳЩ����

	//�õ����ư�ť
	CWBControlButton *GetControlButton();

	//���޿�¡����
	BOOL IsInfiniteClone() const;
	void SetInfiniteClone(BOOL bInfiniteClone = FALSE);

	//��¡����
	BOOL IsClone() const;
	void SetClone(BOOL bIsClone = FALSE);

	//���ö���������ߣ���������
	void SetOwner(unsigned long ulOwner);

	//�Ƿ�����ϳ�Ա
	BOOL IsCombineMember() const;
	void SetCombineMember(BOOL bCombineMember = FALSE);
	void SetRectSelectCombineMember(BOOL bCombineMember = FALSE);

	//���ö������ڵ���ϵ�ͼ���
	void SetComLayerIndex(DWORD dwComLayerIndex);
	DWORD GetComLayerIndex();

	//���ö���ĵ�λ
	virtual int SetUnit(const int);
	CWBSelectRectangle *m_pRectSelect; //����ѡ���
	//�ж��Ƿ�Ϊ�ĵ��ڵĶ���
    bool GetIsDoInside();
    virtual void SetIsDocInside(bool bIsDocInside);
protected:
	
	CDC *m_pDC;

	double m_dComAngle;
	double m_dbAngle; //���������ĵ���ת�ĽǶ�
	//�������ת���ĵ㣬Ĭ���Զ�������ĵ�Ϊ��ת���ĵ㣬���ô����������תʱ����ָ����ת���ĵ�
	CPoint m_ptRotationCenter; 
	CPoint m_ptPreRotationCenter; // �ϴ���ת�����ĵ�
	//�������ű���
	double m_dbXZoomScale;
	double m_dbYZoomScale;
	//�����������ģ�Ĭ���Զ������ϵ�Ϊ�������ĵ㣬���ô���������ʱ����ָ���������ĵ�
	CPoint m_ptZoomCenter;

	//�����Ƿ�����
	BOOL m_bIsLock;

	//�����Ƿ����Ʋ���
	bool m_bIsLimit;

	//�Ƿ��Ǳ���
	BOOL m_bIsBkg;

	bool m_IsCenterpont;

    //�Ƿ�Ϊ�ĵ�
    bool m_bIsDocument;

	bool m_bIsTouchDoc;

	//�Ƿ�Ϊ�ĵ��еĶ����ƶ�
	bool m_bIsDocInside;

	//�Ƿ�Ϊ�ĵ��Ĵ����ƶ�
	bool m_IsTouchMove;

	//����͸����
	int m_byAlpha;
	int m_preAlpha; //֮ǰ��͸����

	int m_nAlphaInCom; //���ʱ��͸���ȣ����ڸı����������͸����ʱ

	//�����Ƿ�ȱ�������
	BOOL m_bEqualScaleZoom;
	double m_dbEauelScale; //���󳤡������
	//���ĵ��Ƿ�����
	BOOL m_bRorationCenterZoom;

	BOOL m_bInfiniteClone; //�Ƿ����޿�¡
	BOOL m_bIsClone; //�Ƿ��¡

	DWORD m_dwComLayerIndex; //����������ϵ�ͼ���
	BOOL m_bIsCombineMember; //�Ƿ�����ϳ�Ա

	CBitmap m_bmpBuf; //�������������ͼ���������ڴ���
	BOOL m_bHaveDrawBmpBuf; //�Ƿ��Ѿ������ڴ�λͼ

	CPoint m_ptMapCenter;	//������ת�����ź����ĵ��ڴ�������ϵ�е�ӳ���
	XFORM m_xform;		//��¼����ϵ�ı任
	BOOL m_bIsZoom;	//�����Ƿ����Ź�

	BOOL m_bSelectFrameIsShow;	// �Ƿ���ʾѡ���
	CWBImageButton *m_pWBImageButton;	//ͼƬ��ť����

	CWBControlButton *m_pControlButton;	//�ĵ�����Ƶ�͵�����ư�ť

	unsigned long m_ulOwner;		//����������ߣ��������ߣ�����ͬ����

	BOOL m_bIsToZero; //Rounding������ľ�̬�����Ƿ����
    BOOL m_bIsCreat; //�Ƿ񴴽�ѡ���

private:
	DWORD m_dwTouchID;  //������ID
	DWORD m_dwLayerIndex;  //���������ÿ��������һ���������Ҳ��ظ�������Ҳ��������Ψһ��ʶ
	int		m_nPageLayerIndex;//������ҳ�е�ͼ���   huanglongquan ADD by 2012��4��18��
	WB_OBJ_TYPE m_nType; //��������	
	WB_LAYER_TYPE m_nLayerType; //����ͼ������
	
protected:
	virtual void  DrawLast(HDC hDC, int nOffsetX=0, int nOffsetY=0){};
	CWBPage *GetFather() const;
	//��ť��ת��ƫ�ƽǶ�   
	double m_dRotateOffAngle;
	CPoint m_ptZoomOffPoint;//��ť�����ƫ��λ��
};

class CWBLine : public CWBObject//ֱ����
{
public:
	CWBLine(const CPoint& ptBegin, const CPoint& ptEnd);
	CWBLine(const PWBLineItem pItem = NULL);
	CWBLine(const CWBLine& other); //�������캯��
	virtual ~CWBLine(); //����

	CWBLine & operator =(const CWBLine& other); //��ֵ����

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	//���ݿ���
	static PWBLineItem CopyItem(PWBLineItem pDesItem, const PWBLineItem pSrcItem);  //�ڵ��õĵط��Ѿ�����

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC); //����

	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ//����
	virtual BOOL IsInSelectArea(const CRect& rect) const; //��ѡ//����

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������//����
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ//����

	//����ѡ���
	virtual void CreateSelecrtFrame(); //����

	//����
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);

	void SetItem(PWBLineItem pItem); //����
	void GetItem(PWBLineItem pItem); //����

	//�ƶ�
	BOOL Move(const CPoint& pt, BOOL bIsBeginPoint = TRUE); //����
	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset); //����
	BOOL MoveOffset(const CPoint& ptOffset, BOOL bIsBeginPoint = TRUE); //���ݶ˵�ƫ�����ƶ� //����

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj); //����
	PWBLineItem m_pItem;
private:
	
	XCritSec m_csWBLineObjLock;//��ֱ�߶���Ļ����������
};

class CWBCurve : public CWBObject //������
{
public:
	CWBCurve(const PWBCurveItem pItem = NULL);
	CWBCurve(const CWBCurve& other); //�������캯��
	virtual ~CWBCurve();  //����

	CWBCurve & operator =(const CWBCurve& other); //��ֵ����

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ//����
	virtual BOOL IsInSelectArea(const CRect& rect) const; //��ѡ

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������//����
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ//����
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te);//������TouchUpAllCursorʲô������
	//����ƫ�����ƶ�  //����
	virtual void MoveOffset(const CPoint& ptOffset);

	//����ѡ���
	virtual void CreateSelecrtFrame();
	//������������
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	//����
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);
	static void ClearCurveItem(PWBCurveItem pItem); //������߶���
	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);//����

	void DrawErase(CDC *pDC);
	void SetItem(const PWBCurveItem pItem);//����
	WBCurveItem *GetItem();  //����
	UINT AddPoint(CPoint pt, int nType = CURVE_TYPE_NORMAL); //������ߵ���//����
	
	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���

	//�ԽǶ�ƫ����ת����
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//������ĳһ����ת
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת
protected:
	BOOL TranslatePointsToString(std::string& strPoints) const; //��д���ַ���//��SetXmlValue(XMemIni& xmlObj) const�����Ѿ�����
	BOOL ParseStringToPoint(std::string strPoints, int nPointCount); //�������ַ���//����
	BOOL AddendumPointToCurve(std::string strPoints,int nPointCount); //�����ߺ���׷�ӵ�//����

public:
	void SetLeftTopRightBottomPt(CPoint &ptLeftTop, CPoint &ptRightBottom); //��ʼ������ʱ�������Ϻ����µ㸳ֵ
	void GetLeftTopRightBottomPt(CPoint &pt);
	CPoint GetLeftTop();
	CPoint GetRightBottom();

protected:
	DWORD AddMiddlePoints(const CPoint& pt); //�����м��//��AddPoint()�е����Ѿ�����

private:
	XCritSec m_csWBCurveObjLock;//�����߶���Ļ����������
	PWBCurveItem m_pItem;

	CPoint m_ptLeftTop;
	CPoint m_ptLastPoint;	//���ߵ����һ����

	COLORREF crTransparent;//͸����
	CDC m_pmemDC;	
	int n_Width;
	int n_Height;
	BOOL m_bIsDraw;

	mutable int m_nIndexPosPoint;//��һ�η��͵ĵ������λ��
	bool m_bResume_broken_transfer;//�Ƿ�Ҫ�ϵ�����

//	bool m_bDrawingCurve;//�������ڻ�ʱ���ò���

	CPoint m_ptLT; //�����߹��������е�����ϵ�
	CPoint m_ptRB; //�����߹��������е�����µ�

public:
	// Draw only last segment of the stroke.
	void DrawLast(HDC hDC,int nOffsetX=0, int nOffsetY=0, UINT nPtNum = 1);
	void SetIsDraw(BOOL IsDraw = TRUE);//�ж������Ƿ��ػ�
	BOOL GetIsDraw() const;
	void SetResume_broken_taransfer(bool bResume_broken_transfer);//�����Ƿ�Ҫ�ϵ�����
};

//jiangchao����������CWBIcoCurve
class CWBIcoCurve : public CWBCurve
{
public:
	CWBIcoCurve(const PWBIcoCurveItem pItem = NULL);
	CWBIcoCurve(const CWBIcoCurve& other); //�������캯��
	virtual ~CWBIcoCurve();

	CWBIcoCurve & operator =(const CWBIcoCurve& other); //��ֵ����

	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ//����
	//virtual BOOL IsInSelectArea(const CRect& rect) const; //��ѡ

	//����ƫ�����ƶ�  //����
	virtual void MoveOffset(const CPoint& ptOffset);
    //����
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);

	//������������
	//virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������//����
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ//����

	//����ѡ���
	virtual void CreateSelecrtFrame();

	//�������
	static void ClearCurveItem(PWBIcoCurveItem pItem);

	//�ļ�����
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	//virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	virtual void Draw(CDC * pDC); //����
	void DrawIcoTrack(CDC * pDC, CPoint& point, int nOffsetX = 0, int nOffsetY = 0); //��ͼ�����߹켣

    void SetItem(const PWBIcoCurveItem pItem); //����
	WBIcoCurveItem * GetItem(); //����
	void SetBeginPoint(const CPoint& point); 
	UINT AddPoint(CPoint point); //����ͼ�����ߵ�

	int GetPreScaleWidth() const {return m_nPreScaleWdith;}
	void SetPreScaleWidth(int nPreScaleWidth) {m_nPreScaleWdith = nPreScaleWidth;}

	//����Imageָ�����(���ļ�����)
	void SetIcoImage(); 

	//�ж��Ƿ���ļ�����
	void SetIsLoadPicFromFile(BOOL IsLoadPicFromFile) {m_IsLoadPicFromFile = IsLoadPicFromFile;}

public:
	void SetLeftTopRightBottomPt(CPoint &ptLeftTop, CPoint &ptRightBottom); //��ʼ������ʱ�������Ϻ����µ㸳ֵ
	void GetLeftTopRightBottomPt(CPoint &pt);
	CPoint GetLeftTop();
	CPoint GetRightBottom();
	BOOL IsPointInObjArea(const CPoint& point, double nScaleWidth) const; //�жϵ��Ƿ����ڶ�����

    void SetResume_broken_taransfer(bool bResume_broken_transfer);//�����Ƿ�Ҫ�ϵ�����
	BOOL GetIsRefush(){return m_bIsRefush;}
	void SetIsRefush(BOOL brefush){m_bIsRefush = brefush;}
protected:
	mutable int m_nIndexPosPoint;//��һ�η��͵ĵ������λ��
	bool m_bResume_broken_transfer;//�Ƿ�Ҫ�ϵ�����
	BOOL TranslatePointsToString(std::string& strPoints) const; //��д���ַ��� ����
	BOOL ParseStringToPoint(std::string strPoints, int nPointCount); //�������ַ��� ����
    BOOL AddendumPointToCurve(std::string strPoints,int nPointCount); //���׷�ӵ�

	Image * m_Image; //Image ͼ��ָ��

private:
	XCritSec m_csWBIcoCurveObjLock;//�����߶���Ļ����������
	PWBIcoCurveItem m_pItem; //����ͼ��ṹ��
	int m_nPreScaleWdith; //�ʼ������ͼ��Ŀ��
	int m_nWidth; //���
	int m_nHeight;//�߶�
	CPoint m_BeginPt; //��갴��ʱ����
	BOOL m_bIsRefush;//�Ƿ��ػ�
	CPoint m_ptLeftTop; 
	CPoint m_ptLT; //��ͼ�����߹��������е�����ϵ�
	CPoint m_ptRB; //��ͼ�����߹��������е�����µ�
	CDC  m_mpmDC;  //�����˾ʹ���DC���棬��Ҫÿ�ζ���
	BOOL m_IsLoadPicFromFile; //�ж��Ƿ���ļ�����ͼƬ
};

class CWBPolygon : public CWBObject //�������
{
public:
	CWBPolygon(const PWBPolygonItem pItem = NULL);
	CWBPolygon(const CWBPolygon& other); //�������캯��
	virtual ~CWBPolygon();  //����                            

	CWBPolygon & operator=(const CWBPolygon& other); //��ֵ����

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);//����

	//����ѡ���
	virtual void CreateSelecrtFrame();

	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	virtual int IsInSelectArea(CPoint& pt)const;//����
	virtual BOOL IsInSelectArea(const CRect& rect) const;//����
	virtual BOOL IsRegionIn(const CRgn *pRgn) const;//����
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ//����

	//���ݿ���
	static PWBPolygonItem CopyItem(PWBPolygonItem pDesItem, const PWBPolygonItem pSrcItem);

	static void ClearItem(PWBPolygonItem pItem); //�������

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual BOOL Load(char *pBuf, DWORD& dwLen);//����
	virtual void Draw(CDC *pDC);//����

	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������//����

	//�ƶ���
	virtual BOOL Move(const CPoint& pt);//����

	void SetItem(const PWBPolygonItem pItem);//����
	void GetItem(PWBPolygonItem pItem);//����
	void AddPoint(CPoint pt);  //��Ӷ���ε�//����
	void SetClosed(BOOL bIsClosed = TRUE); //���÷��
	void SetFillColor(COLORREF clrFill); //���������ɫ//����
	void DeletePoint();		//ɾ�����һ����//����

	//�������rgn////////////////////////////////////////////////////////////////////////
	virtual BOOL CreateRgn(CRgn *pRgn) const; //��������//����
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //���ݶ�����ת�����ź󴴽�����//����
	virtual BOOL CreateReverseFillRgn(CRgn *pRgn) const; //���ݶ�����ת�����ź󴴽��������//����
	BOOL PointIn(const CPoint& pt) const; //�жϵ��Ƿ��ڶ���η�Χ��//����

	virtual BOOL IsSolid() const; //�ж϶����Ƿ����

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	BOOL SetPointsString(std::string& strPoints) const; //��д���ַ���//����
	BOOL GetPointsString(std::string strPoints, int nPointCount); //�������ַ���//����

	BOOL GetInAndOutPoint(); //���ݶ���εĵ��б�õ��ڲ����ⲿ�ĵ��б�//����
	void GetItemInfo(int &dWidth, int &dHeight); //�õ�ͼ�εĳ��͸�//����
	PWBPolygonItem m_pItem;
protected:
	
	std::vector<CPoint *> vecPointsIn; //�ڲ����б�
	std::vector<CPoint *> vecPointsOut; //�ⲿ���б�

	XCritSec m_csPolygonItemLock;

private:
	BOOL m_bIsClosed; //�Ƿ���
	BOOL m_bHaveFillRgn; //�Ƿ����������
};

class CWBRectangle : public CWBPolygon //����
{
public:
	CWBRectangle();
	CWBRectangle(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBRectangle(const CWBRectangle& other);
	virtual ~CWBRectangle();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	//����ѡ��� 
	virtual void CreateSelecrtFrame();

	//�������������
	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//�������Ű�ťƫ�����Ŷ���
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����

	//operations
	virtual void Draw(CDC *pDC);//����

	int Width() const;//����
	int Height() const;//����
private:
	XCritSec m_csRectangleItemLock;
};

class CWBRhombus : public CWBPolygon //����
{
public:
	CWBRhombus();
	CWBRhombus(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBRhombus(const CWBRhombus& other);
	virtual ~CWBRhombus();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	virtual void CreateSelecrtFrame();//����

	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//operations
	virtual void Draw(CDC *pDC);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
private:
	XCritSec m_csRhombusItemLock;
};

class CWBPentagon : public CWBPolygon //�����
{
public:
	CWBPentagon();
	CWBPentagon(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBPentagon(const CWBPentagon& other);
	virtual ~CWBPentagon();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	virtual void CreateSelecrtFrame();//����

	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//operations
	virtual void Draw(CDC *pDC);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
private:
	XCritSec m_csPentagonItemLock;
};

class CWBHexagon : public CWBPolygon //������
{
public:
	CWBHexagon();
	CWBHexagon(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBHexagon(const CWBHexagon& other);
	virtual ~CWBHexagon();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	virtual void CreateSelecrtFrame();//����

	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//operations
	virtual void Draw(CDC *pDC);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
private:
	XCritSec m_csHexagonItemLock;
};

class CWBFivepointstart : public CWBPolygon //�����
{
public:
	CWBFivepointstart();
	CWBFivepointstart(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBFivepointstart(const CWBFivepointstart& other);
	virtual ~CWBFivepointstart();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	virtual void CreateSelecrtFrame();//����

	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//operations
	virtual void Draw(CDC *pDC);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
private:
	XCritSec m_csFivepointstartItemLock;
};

class CWBTrapezium : public CWBPolygon //����
{
public:
	CWBTrapezium();
	CWBTrapezium(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBTrapezium(const CWBTrapezium& other);
	virtual ~CWBTrapezium();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	virtual void CreateSelecrtFrame();//����

	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//operations
	virtual void Draw(CDC *pDC);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����

	BOOL IsInPointArea(const CPoint& pt);
	BOOL MovePoint(CPoint& pt);
private:
	XCritSec m_csTrapeziumItemLock;
	int nPointIndex; //��ǰѡ�еĵ�����
};

class CWBArbitraryTriangle : public CWBPolygon
{
public:
	CWBArbitraryTriangle();
	CWBArbitraryTriangle(const CPoint& ptA,const CPoint& ptB,const CPoint& ptC);
	CWBArbitraryTriangle(const CWBArbitraryTriangle& other);
	virtual ~CWBArbitraryTriangle();
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	virtual void CreateSelecrtFrame();
	virtual BOOL ZoomByButton(const CPoint& ptOffset);
	virtual void Draw(CDC *pDC);
	virtual BOOL Move(const CPoint& ptRightBottom);
	int IsInSelectArea(CPoint& pt);
	BOOL Move(const CPoint& ptA,const CPoint& ptB,const CPoint& ptC);
	bool InPointArea(CPoint& pt);
	void SetIsEdit(bool IsEdit){m_bIsEdit = IsEdit;}
	bool GetIsEdit(){return m_bIsEdit;}
	void EditTrianglePoint(const CPoint& pt,bool isup = false);
	bool JudgeIsTriangle(CPoint ptF,CPoint ptS,CPoint ptT);
	bool CanbeTriangle();
private:
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage = 0);
private:
	XCritSec m_csATriangleItemLock;
	int m_ypoint; //���Ͻǵ�Y��ȡֵ���ĸ���
	bool m_bIsEdit;
	int m_nEditNUM;
	ImageEx *m_pEditpic; // �༭��ͼƬ
};

class CWBTriangle : public CWBPolygon //������
{
public:
	CWBTriangle(int nType = 0);
	CWBTriangle(const CPoint& ptLeftTop, const CPoint& ptRightBottom,int nType = 0);
	CWBTriangle(const CWBTriangle& other,int nType = 0);
	virtual ~CWBTriangle();


	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	//����ѡ��� 
	virtual void CreateSelecrtFrame();

	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
	//void setTriangleAngle(int nAngle){m_nTriangleAngle = nAngle;}
	//operations
	virtual void Draw(CDC *pDC);//����
	CPoint GetLeftTopPT(){return m_ptLeftTop;}
	int GetTriangleType(){return m_nTriangleType;}
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

private:
	XCritSec m_csTriangleItemLock;
	int  m_nTriangleType; /*0��ʾ�������Σ�1��ʾֱ�������Σ�2��ʾ���������Σ�3��ʾ����ֱ��������*/
	/*int m_nTriangleAngle; //���������εĽǶ�*/
	CPoint m_ptLeftTop;//�������ϵ�
};

class CWBEllipse : public CWBObject //��Բ��
{
public:
	CWBEllipse(const CPoint& ptTopLeft, const CPoint& ptBottomRight);
	CWBEllipse(const PWBEllipseItem pItem = NULL);
	CWBEllipse(const CWBEllipse& other); //�������캯��
	virtual ~CWBEllipse();       //����                       

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	CWBEllipse & operator=(const CWBEllipse& other); //��ֵ����//����

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);//����

	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	virtual int IsInSelectArea(CPoint& pt)const ;//����
	virtual BOOL IsInSelectArea(const CRect& rect) const;//����
	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������

	//���ݿ���
	static PWBEllipseItem CopyItem(PWBEllipseItem pDesItem, const PWBEllipseItem pSrcItem);

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);//����

	//����ѡ���
	virtual void CreateSelecrtFrame();//����

	//�������������
	virtual void AdjustObject();//����

	BOOL Move(const CPoint& ptRightBottom);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
	//�������λ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //����

	void SetItem(const PWBEllipseItem pItem);//����
	void GetItem(PWBEllipseItem pItem) const;//����

	void SetFill(BOOL bIsFill = TRUE); //�����Ƿ����
	void SetFillColor(COLORREF clrFill); //���������ɫ//����

	//�������rgn////////////////////////////////////////////////////////////////////////
	BOOL CreateRgn(CRgn *pRgn) const; //��������//����
	BOOL PointIn(const CPoint& pt) const; //�жϵ��Ƿ��ڶ���η�Χ��

	virtual BOOL IsSolid() const; //�ж϶����Ƿ����

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	int GetLineWidth() const; //�õ��߿�
	PWBEllipseItem m_pItem;
private:
	
	BOOL m_bIsFill; //�Ƿ����
	BOOL m_bHaveFillRgn; //�Ƿ����������

	XCritSec m_csEllipseItemLock;
};

class CWBSelectRectangle : public CWBRectangle //ѡ������
{
public:
	CWBSelectRectangle();
	CWBSelectRectangle(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	CWBSelectRectangle(const CWBSelectRectangle& other);//����
	virtual ~CWBSelectRectangle();//����

	virtual void Draw(CDC *pDC);//����
	virtual void GetCenterPoint(CPoint& ptCenter);//����
	BOOL GetSelectInfo(double &dCenterX, double &dCenterY, double &dWidth, double &dHeight);//����

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//����
	virtual BOOL Move(const CPoint& ptRightBottom);//����

	virtual void MoveOffset(const CPoint& ptOffset);//����

	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	virtual BOOL IsInSelectArea(const CRect& rect) const;
	virtual BOOL IsRegionIn(const CRgn *pRgn) const;//����
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ//����

	//�������rgn////////////////////////////////////////////////////////////////////////
	virtual BOOL CreateRgn(CRgn *pRgn) const; //��������//����
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //���ݶ�����ת�����ź󴴽�����//����

	BOOL PointIn(const CPoint& pt) const; //�жϵ��Ƿ��ڶ���η�Χ��

	virtual BOOL ZoomByButton(const CPoint& ptOffset);
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������

	int Width() const;
	int Height() const;

	void SetLeftTop(CPoint ptLeftTop);//����
	void SetRightTop(CPoint ptRightTop);//����
	void SetRightBottom(CPoint ptRightBottom);//����
	void SetLeftBottom(CPoint ptLeftBottom);//����

	CPoint GetLeftTop();
	CPoint GetRightTop();
	CPoint GetRightBottom();
	CPoint GetLeftBottom();

private:
	PWBSelectRectangleItem m_pItem;
	XCritSec m_csSelectFrameItemLock;
};

class CWBPicture : public CWBObject //ͼƬ
{
public:
	CWBPicture(LPCTSTR lpszFilePath = NULL,CRect rct = CRect( 0, 0, 0, 0 ),CString str = _T(""));//����
	CWBPicture(PWBPictureItem pItem);//����
	CWBPicture(const CWBPicture& other); //�������캯��//����
	virtual ~CWBPicture();//����
	
	CWBPicture & operator=(const CWBPicture& other); //��ֵ����//����

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;//����
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);//����
	virtual void DrawAsBakground(CDC *pDC, CWnd *pParentWnd); //��Ϊ������//����

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);

	//����ѡ���
	virtual void CreateSelecrtFrame();//����
	CRect GetPictureRect() ;

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);//����
	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ��//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
	//�������λ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //����

	//�ԽǶ�ƫ����ת����
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//������ĳһ����ת//����
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת//����

	//���ݿ���
	static PWBPictureItem CopyItem(PWBPictureItem pDesItem, const PWBPictureItem pSrcItem);

	void SetItem(const PWBPictureItem pItem);//����
	void GetItem(PWBPictureItem pItem) const;//����

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	//��ʼ����Ϣ
	virtual void init();//����
	//�Ƚ�ԭͼ��С����ͼƬ������ܳ���view�Ŀͻ�����С4-21����
	void SetPictureBigerSzie(CRect viewClientRect );//����

	//�����Ƿ���Ҫ����ͼƬ
	void SetIsPath(BOOL bIsPath = TRUE);
	BOOL GetIsPath() const;
	//�õ�ͼƬ���ļ����е�ʵ�ʴ�С
	int GetWidth();
	int GetHeight();
    ImageEx* GetImage();
	CString GetPicString(){return m_DrawString;}
	void DrawCaramaString();
	void IsStartTimer();
	void SetstrCaramaPicture(string str) {m_strCaramaPicture = str;}
	void SetZoomNeedCreateFram(bool bneed){m_bneedCreateFram = bneed;}
	//void SetCurchannelID(unsigned long ulnum) {m_CurchannelID = ulnum;}
	//unsigned long GetCurchannelID() {return m_CurchannelID;}

	virtual void SetSelectFrameShow(BOOL bShow); //��ʾѡ���//����
	void SetSelComSelectFrameShow(BOOL bShow); //��ʾѡ�е������ѡ���//����
	CWBObject *GetSelectComObj(); //�õ�ѡ�е��������//����
	void SetSelectedObj(DWORD dwLayerIndex); //ʵʱͬ��ʱȷ������ѡ��Ķ���

	//����͸���Ȳ���
	virtual void SetAlpha(int alpha);//����
	//�жϡ�������
	BOOL ObjectIsComponent(CWBObject *obj);//����
	void AddComponent(CWBObject *obj);//����
	BOOL DeleteComponent();//����
	BOOL PtIsComponent(CPoint pt);
	//�������
	void CopyComponents(CWBComponent *otherCom);//����

	//�����ĵ����������ת���� luowei 2012-04-18
	void GetSrcAnnPt(); //�õ���תǰ����б�ע��ǵĵ�//����
	void GetSrcCenterPt(); //�õ���תǰ�����ÿ���������ת���ĵ�//����
	void GetSrcZoomCenterPt(); //�õ�����ǰ�����ÿ�������ѡ������ϽǺ����½ǵĵ�//����
	void GetEachSrcZoomCenterPt(); //�õ�����ǰ�����ÿ�������ѡ������Ͻǵĵ�//����
	void MoveAnn(const CPoint& ptOffset); //�ƶ��ĵ��еı�ע//����
	void ClearSrcAnnPt(); //�����תǰ����б�ע��ǵĵ�

	//�õ�ѡ�е��ĵ��еı�ע luowei 2012-04-23
	CWBObject *GetAnn();//����

	void DrawToDC(); //��ͼƬ�����ݻ���ͼƬ���ڴ�DC��
	CString SavePictureToJpg(); //��ͼƬ����ΪJPG�ļ�

	BOOL GetIsPrintScreen();
	void SetIsPrintScreen(BOOL bIsPrintScreen = TRUE);
	void SetIsEditpic(bool bispic){m_bIsEditPic = bispic;}
	bool GetIsEditPic(){return m_bIsEditPic;}

	//���Ʋ�����ת����
	void SetIsMTRotate(BOOL bIsMTRotate);
	BOOL GetIsMTRotate();

	//���Ʋ�����������
	void SetIsMTZoom(BOOL bIsMTZoom);
	BOOL GetIsMTZoom();

	BOOL IsEdited(); //ͼƬ�Ƿ��б�ע
	void InitCom(); //��ʼ�����
private:
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage = 0);
private:
	//unsigned long m_CurchannelID;//��קͨ�������ж��Ƿ��Ѿ��ڰװ��д���
	mutable bool m_bSaveSamePicture;
	XCritSec m_csPictureItemLock;
	std::string m_strCaramaPicture;//�ж�����ͷͼƬ�Ƿ����
	bool m_bneedCreateFram; //˫����Ƶ����ͼƬ��תʱ��Ҫ�����С
	bool m_bIsEditPic;//�Ƿ�Ϊ�༭ͼƬ(��ת���õ�)
public:
	PWBPictureItem m_pItem;
protected:
	mutable CString m_DrawString;
	
	ImageEx *m_pImage;
	CRect m_viewClientRect;

	CDC m_bufferpicture;
	int m_nWidth,m_nHeight;
	BOOL m_bIsPath;
	int m_nRealWidth, m_nRealHeight; //ͼƬ���ļ����е�ʵ�ʴ�С
	LPTSTR m_pszExtension;   //ͼƬ��׺��

	CWBComponent *m_comList; //�Ӷ����б�
	CDC m_dcMem;	//���ڱ����ļ����ڴ�DC
	BOOL m_bIsPrintScreen; //�Ƿ�����Ƶ��ͼ

	//���Ʋ���ʱ������ת��������
	BOOL m_bIsMTRotate;
	BOOL m_bIsMTZoom;
};

class CWBDustbin:public CWBPicture //������
{
public:
	CWBDustbin();
	virtual ~CWBDustbin();

	//operations
	virtual void MoveOffset(const CPoint& ptOffset);
	virtual void Draw(CDC *pDC);
	void  MoveDustbin(CPoint ptMovePoint);
	void  GetRect(CRect &rect); //�õ����ο�
	void  SetRect(CRect &rect); // ���þ��ο�
	void  MoveRect(CPoint ptMovePoint); //�ƶ����ο�
	int   PointIn(CPoint ptPoint);

private:
	CRect m_rect;	//���ο�
};
class CWBFlash: public CWBObject //flash
{
public:
	CWBFlash(LPCTSTR lpszFilePath = NULL,CRect rect = CRect(0,0,0,0));//����
	CWBFlash(PWBFlashItem pItem);//����
	CWBFlash(const CWBFlash& other); //�������캯��//����
	virtual ~CWBFlash();//����

	CWBFlash & operator=(const CWBFlash& other); //��ֵ����//����

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;//����
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);

	//����ѡ���
	virtual void CreateSelecrtFrame();//����

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
	//�������λ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //����

	//���ݿ���
	static PWBFlashItem CopyItem(PWBFlashItem pDesItem, const PWBFlashItem pSrcItem);

	void SetItem(const PWBFlashItem pItem);//����
	void GetItem(PWBFlashItem pItem) const;//����

	void SetCurRect(CRect rc);//wangxiaobin
	CRect GetCurRect();

	void init();//����
	void Start();
	void Pause();
	void Stop();

	void SetCircle();	//����FLASHѭ������
	void SetSinglePlay();//����FLASH���β���
	BOOL GetCircle();	//�õ���ǰFLASH�Ƿ�ѭ��

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	CString CatchFlash();//����

	bool GetFlashIsPlay() {return m_bFlashIsPlay;}
	void SetFlashIsPlay(bool FlashIsPlay) {m_bFlashIsPlay = FlashIsPlay;}
private:
	bool m_bFlashIsPlay;
	//int m_FlashCout;
	long m_current;
	PWBFlashItem m_pItem;
	CShockwaveFlash *m_swf;
	BOOL m_bFindFile;	//�ļ��Ƿ����
	static int count;    //����flash�ĸ���
	static int num;      //һҳֻ��һ��Flashʱ��
	
	unsigned long m_CurrentNum; //���浱ǰ֡��
	XCritSec m_csFlashOperationLock;

public:
	BOOL m_bIsRunning;	//�Ƿ񲥷�
	BOOL m_bIsPause;	//�Ƿ���ͣ
    BOOL m_bIsStop;     //�Ƿ�ֹͣ
	BOOL m_bIsCircle;	//�Ƿ�ѭ������
	CRect m_rect;
	mutable bool m_bSaveSameFlash;
};

typedef std::map<int, CWBComponent*>::const_iterator ComList_Iterator;
class CWBDocument : public ThreadHelperImpl<CWBDocument>, public CWBObject //�ĵ�
{
public:
	CWBDocument(LPCTSTR lpszFilePath = NULL,CRect rect = CRect(0, 0, 0, 0)); //����
	CWBDocument(PWBDocumentItem pItem);//����
	CWBDocument(const CWBDocument& other); //�������캯��//����
	virtual ~CWBDocument();//����

	CWBDocument & operator=(const CWBDocument& other); //��ֵ����//����

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;//����
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);//����

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	//����ѡ���
	virtual void CreateSelecrtFrame();//����

	//���ݿ���
	static PWBDocumentItem CopyItem(PWBDocumentItem pDesItem, const PWBDocumentItem pSrcItem);

	void SetItem(const PWBDocumentItem pItem);//����
	void GetItem(PWBDocumentItem pItem) const;//����

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);//����

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//����
	BOOL ZoomRect(CRect& rect, const CPoint& ptZoomCenter, double dbXScale, double dbYScale, BOOL bIsToZero);

	//�������λ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //����
	virtual void SetIsZoom(BOOL bIsZoom);//����

	//�ԽǶ�ƫ����ת����
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//������ĳһ����ת//����
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת//����

	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ��//����

	virtual void ButtonClose();

	virtual void SetSelectFrameShow(BOOL bShow); //��ʾѡ���//����
	void SetSelComSelectFrameShow(BOOL bShow); //��ʾѡ�е������ѡ���//����
	CWBObject *GetSelectComObj(); //�õ�ѡ�е��������//����
	void SetSelectedObj(DWORD dwLayerIndex); //ʵʱͬ��ʱȷ������ѡ��Ķ���

	//�ĵ�ҳ����
	int GetPageCount() const; //�õ��ĵ���ҳ��
	int GetCurPageIndex() const; //�õ���ǰҳ
	BOOL GotoPage(int nPageIndex); //����ĳһҳ//����
	BOOL PrePage();
	BOOL NextPage();
	BOOL FirstPage();
	BOOL LastPage();

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	//����͸���Ȳ���
	virtual void SetAlpha(int alpha);//����

	void _Thread(); //�鿴DOCת����Ϣ���߳�//����

	//�жϡ�������
	BOOL ObjectIsComponent(CWBObject *obj);//����
	void AddComponent(CWBObject *obj);//����
	BOOL DeleteComponent();//����
	BOOL PtIsComponent(CPoint pt);
	//�������
	void CopyComponents(const std::map<int, CWBComponent*> &otherCom);//����
	void SetPageIndex(int nPageIndex);

	//�����ĵ����������ת���� luowei 2012-04-18
	void GetSrcAnnPt(); //�õ���תǰ����б�ע��ǵĵ�//����
	void GetSrcCenterPt(); //�õ���תǰ�����ÿ���������ת���ĵ�//����
	void GetSrcZoomCenterPt(); //�õ�����ǰ�����ÿ�������ѡ������ϽǺ����½ǵĵ�//����
	void GetEachSrcZoomCenterPt(); //�õ�����ǰ�����ÿ�������ѡ������Ͻǵĵ�//����
	void MoveAnn(const CPoint& ptOffset); //�ƶ��ĵ��еı�ע//����

	//�õ�ѡ�е��ĵ��еı�ע luowei 2012-04-23
	CWBObject *GetAnn();//����
	//int  OnLButtonDoubleAnn(); //˫���ĵ��еı�ע
	//���Ʋ�����ת���� luowei 2012-05-02
	void SetIsMTRotate(BOOL bIsMTRotate);
	BOOL GetIsMTRotate();

	//���Ʋ����������� luowei 2012-05-02
	void SetIsMTZoom(BOOL bIsMTZoom);
	BOOL GetIsMTZoom();

	BOOL GetIsConvertTip(); //�õ��Ƿ���DOCת����ʾ

	void Erase(const CRect& rectErase);//����

	void IsComRegionIn(const CRgn *pRgn);//����

	void SetRect(CRect rect);

	void DrawToDC(); //���ĵ���ǰҳ�����ݻ����ĵ����ڴ�DC��
	void GetPageBmp(int i = 0); //���ĵ�ҳ����Ϊλͼ�ļ�
	void PrintCurPage(); //��ӡ��ǰҳ
	HBITMAP GetCurPageBmp(); //�õ���ǰҳ��λͼ

protected:
	BOOL OpenDoc(); //���ĵ�//����
	BOOL OpenPDFDoc();//����

	void CALLBACK DocConverTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

private:

	PWBDocumentItem m_pItem;

	DATEC_PDFConvert*		m_pPDFConvert;
	DATEC_PDFFile*			m_pPDFFile;

	int m_nCurPageIndex; //��ǰ�ĵ���ʾҳ
	int m_nPageCount; //�ĵ���ҳ��
	ImageEx *m_curPageBmp; //��ǰPDFҳ��ͼƬ����

	CString m_strTempFilePath;

	BOOL m_bConvertTip; //�Ƿ���DOCת����ʾ
	CWBRectangle *m_convertTipFrameBg;
	CWBRectangle *m_convertTipProgressbar;
	CWBText      *m_convertTipText;

	std::map<int, CWBComponent*> m_comList; //�Ӷ����б�

	CWBText *m_pPageIndexTip; //ҳ��
	int m_nPageIndex; //�����ĵ�ʱ��ҳ��
	BOOL m_bIsOpenPDF; //PDF�ĵ��Ƿ�򿪳ɹ�

	//���Ʋ���ʱ������ת��������
	BOOL m_bIsMTRotate;
	BOOL m_bIsMTZoom;

	mutable bool m_bSaveSameDocument;
	CRect m_rect; //���������ĵ�λ�ô�С
	CDC m_dcMem;	//ÿ���ĵ������Լ����ڴ�DC
	XCritSec m_csDocOperationLock;
};

class CWBText : public CWBObject
{
public:
	CWBText(CPoint &ptTopLeft, LOGFONT &lfLogFont, COLORREF &clrText, LPCTSTR lpszText = NULL);//����
	CWBText(PWBTextItem pItem = NULL);//����
	CWBText(const CWBText& other);//����
	virtual ~CWBText();//����

	CWBText & operator=(const CWBText& other);//����

	///add by aolingwen 2012.12.4
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����
	///end

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//����
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);

	//���ݿ���
	static PWBTextItem CopyItem(PWBTextItem pDesItem, const PWBTextItem pSrcItem);


	//����ѡ���
	virtual void CreateSelecrtFrame();//����

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);//����



	void SetItem(const PWBTextItem pItem);//����
	void GetItem(const PWBTextItem pItem) const; //����

	//�����ı���Ҫ���¼�����ʾ��
	void SetCalRectText(BOOL bCalRectrText = TRUE);

	void StringReplace(std::string &srcStr, const std::string &oldStr, const std::string &newStr) const;

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//����
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//����

	BOOL SetTextXmlValue(XMemIni& xmlObj) const;//����
	BOOL GetTextXmlValue(XMemIni& xmlObj);//����

	BOOL GetIsVertical() const;
	void SetIsVertical(BOOL bIsVertical = FALSE);
	PWBTextItem m_pItem;
private:
	
	BOOL m_bIsVertical; //�����Ƿ�����
	//CRect m_rectText; //�ı���ʾ�ľ��Σ�ͨ��DrawText��������
	BOOL m_bCalRecttext; //�Ƿ�Ҫ���¼�����ʾ��
	XCritSec m_csTextOperationLock;
};
//jin
class CWBBafflePlate : public CWBObject //������
{
public:
	CWBBafflePlate(const CPoint& ptLeftTop, const CPoint& ptRightBottom, COLORREF clrBackground = RGB(0, 0, 0));
	CWBBafflePlate(const PWBBafflePlateItem pItem = NULL, COLORREF clrBackground = RGB(0, 0, 0));
	~CWBBafflePlate();

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);//��������Ի���

	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);
	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	//���úͻ�õ���ṹ����
	void SetItem(PWBBafflePlateItem pItem);
	void SetItem( CPoint ptLeftTop,  CPoint ptRightBottom, COLORREF clrBackground = RGB(0, 0, 0));
	PWBBafflePlateItem GetItem();
	//�϶�������ָı䵲��ṹ����
	void ChangeTopHandle(CPoint ptOffSet);
	void ChangeLeftHandle(CPoint ptOffSet);
	void ChangeBottomHandle(CPoint ptOffSet);
	void ChangeRightHandle(CPoint ptOffSet);
	//����ѡ���
	void CreateSelecrtFrame();
	//�������λ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); 

private:
	PWBBafflePlateItem m_pItem;
//	CBafflePlate *m_pDlg;
	BOOL m_isNewBaffle;  //�½��ĵ�����Ҫ����ʾʱ����Ҫ��ȥ������ƫ����
};

class CWBCombine : public CWBObject //�����
{
public:
	CWBCombine();
	CWBCombine(const CWBCombine& other);
	virtual ~CWBCombine();

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);

	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ��
	virtual BOOL IsInSelectArea(const CRect& rect) const; //����ѡ��

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ

	BOOL AddCombineLayer(); //�õ���ϵĶ���ͼ���
	BOOL AddCombineObj(); //������б�����Ӷ��󣬲�����Щ����ָ��Ӷ����б���ɾ��

	void CancelCombineObj(DWORD dwLayerIndex , DWORD dwSendId); //������б��еĶ������¼�����������У����������б�

	void AddComMebMsg(unsigned long ulEventID, int nPageIndex);
	void DelComMebMsg(); //ɾ�����ǰ����ɾ����ϳ�Ա����Ϣ
	void ChangeObjItem(unsigned long ulEventID, DWORD dwLayerIndex, BOOL bIsCombineMember);

	int GetComCount(); //����ڰ����Ķ�����
	DWORD IsObjInCom(DWORD dwLayerIndex); //�ж϶����Ƿ�����ϳ�Ա
	BOOL AddObjInCom(CWBObject *pComObj, DWORD dwComLayerIndex); //���������Ӷ���

	//����ѡ���
	virtual void CreateSelecrtFrame();

	//������������
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //���ݶ�����ת�����ź󴴽�����

	//ƫ�����ƶ�����
	virtual void MoveOffset(const CPoint& ptOffset); 

	void GetSrcLinePoint(); //�õ���תǰ�����ֱ�ߵ������˵�
	void GetSrcCenterPoint(); //�õ���תǰ�����ÿ���������ת���ĵ�

	//�ԽǶ�ƫ����ת����
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת
	//virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//������ĳһ����ת

	void GetSrcZoomCenter(); //�õ�����ǰ�����ÿ�������ѡ������ϽǺ����½ǵĵ�

	//��������
//	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���

	//����
	void Erase(const CRect& rectErase);
	void EraseLine(CWBLine *pLine, const CRect& rectErase, int nIndex);

	virtual BOOL IsSolid() const; //�ж϶����Ƿ����

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	BOOL SetComLayerXml(XMemIni& xmlObj) const;
	BOOL GetComLayerXml(XMemIni& xmlObj);

	BOOL GetComXml(XMemIni& xmlObj); //ͬ�����ʱ����϶��������ǵ���

	void SetComXmlValue(unsigned long ulEventID);

	BOOL SetCombineString(std::string& strCombine) const;
	BOOL GetCombineString(std::string strCombine, int nObjCount);
	BOOL SetPointsString(std::string& strPoints) const; //��д���ַ���
	BOOL GetPointsString(std::string strPoints, int nPointCount); //�������ַ���

	std::vector<CWBObject *> GetCombineList(); //�õ�����еĶ���ָ��
	std::vector<DWORD> GetCombineLayerIndexList(); //�õ�����еĶ���ID

	void InitFlash(); //��ʼ����е���Ƶ���ʼ������е�Flash

	void GetEachSrcZoomCenter(); //�õ�����ǰ�����ÿ�������ѡ������Ͻǵĵ�
	CWBObject * GetControlObj(); //�õ�����е��п��ư�ť�Ķ���
	CWBObject * SetControlObj(CPoint& pt); //��¼����е��п��ư�ť�Ķ���

	CWBObject *FindObjByLayerIndex(DWORD dwLayerIndex) const; //����ͼ��ŵõ�����
	void GetComLineLayerIndex(); //�õ����������ֱ�ߵ�ͼ���
	DWORD GetComLayerIndexByLine(DWORD dwLayerIndex); //�õ�ֱ��������ϵ�ͼ���
	void DelComLineMsg(); //ɾ������֮ǰ������ֱ�߶���
	BOOL DeleteComObject(DWORD dwLayerIndex); //ɾ�������Ķ���
	void AddComLineMsg(unsigned long ulEventID); //����֮�����������ֱ�߶���
	BOOL AddComObjIntoCom(DWORD dwLayerIndex, CWBObject *pComObj); //���������Ӷ���ֱ�ߣ����ߣ�
	BOOL DelComObj(DWORD dwLayerIndex); //ɾ�������Ķ���ָ��
	//����͸���Ȳ���
	virtual void SetAlpha(int alpha);
	void SetComMebAlpha();

	void SetComMinFlash(); //��������гߴ���С��FLASH��ͼ���

	void SetComLayerNum(); //���ʱ������������϶����ͼ���

	void GetFlashCombinRgn(CRgn &AllFlashRgn);

	std::vector<DWORD> GetLineLayerList() {return m_pLineLayerList;}

	void DeleteCurCombineObjectList();
private:
	XCritSec			m_csCombineLock;	//�������

	std::vector<CWBObject *> m_pCombineList; //����еĶ���ָ���б�
	std::vector<DWORD> m_pCombineLayerList; //����еĶ���ͼ���б�

	std::vector<CPoint> m_ptSrcCenterList; //��¼�����ÿ��������תǰ�����ĵ�����
	std::vector<CPoint> m_ptPreCenterList; //��¼�����ÿ������ǰһ�ε���ת�������
	double m_dbRotateAngle; //���������ĵ���ת�ĽǶ�
	std::vector<CPoint> m_ptBeginList; //ֱ�����
	std::vector<CPoint> m_ptEndList; //ֱ���յ�

	std::vector<CPoint> m_ptSrcLeftTopList; //��¼�����ÿ����������ǰ���Ͻǵĵ�
	std::vector<CPoint> m_ptSrcRightBottomList; //��¼�����ÿ����������ǰ���½ǵĵ�
	std::vector<CPoint> m_ptPreLeftTopList; //��¼�����ÿ������ǰһ�����ź����Ͻǵĵ�
	std::vector<CPoint> m_ptPreRightBottomList; //��¼�����ÿ������ǰһ�����ź����½ǵĵ�

	std::vector<CPoint> m_ptResBeginList; //ֱ���������ϵ�����ϵ�������
	std::vector<CPoint> m_ptResEndList; //ֱ���յ�����ϵ�����ϵ�������

	//�������ű���
	double m_dbXScale;
	double m_dbYScale;

	std::vector<CPoint> m_ptEachSrcLeftTopList; //��¼�����ÿ������ÿ������ǰ���Ͻǵĵ�

	std::vector<CPoint> m_ptResObjSrcLeftTopList;     //��¼�����ÿ����������ǰ ���Ͻ� �ĵ�ӳ���ڶ�������ϵ�������

	std::vector<CPoint> m_ptResObjSrcCenterList; //��¼�����ÿ����������ǰ�������ĵ�ӳ������϶�������ϵ�������

	CWBObject *m_pControlObj; //����е��п��ư�ť�Ķ���
	std::vector<DWORD> m_pLineLayerList; //���������ֱ�ߵ�ͼ���

	DWORD m_dwComMinFlash; //����е�ѡ�����С��Flash��ͼ���
};

//�Ŵ��࣬add by zhuyuyuan
class CWBMagnifier : public CWBObject
{
public:
	CWBMagnifier(const PWBMagnifierItem pItem = NULL);
	CWBMagnifier(const CRect& rect, const int& nZoom = 0);
	~CWBMagnifier();

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj);
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	
	//���úͻ�ȡ���ݶ���ֵ
	void SetItem(const PWBMagnifierItem pItem);
	void SetItem(const CRect& rect, const int& zoom);
	PWBMagnifierItem GetItem();
	void SetRect(const CRect& rect);
	void GetRect(CRect& rect) const;

	//����
	virtual void MoveOffset(const CPoint& ptOffset);
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������

	//����Ŵ󾵵������޸��ˣ�����TRUE�����򣬷���FALSE
	BOOL  ChangeRect(const CPoint& point);
	void  DrawMagnifier(CDC* pDC);
	void  ChangeCursor();

	//�жϵ�λ��
	int   PointIsInSide(CPoint ptPoint);
	int   PointIn(CPoint ptPoint);

	//������������,����ֵΪTRUE������Ҫˢ�£�������Ҫ
	BOOL MouseDown(const CPoint& point);
	BOOL MouseMove(const CPoint& point);
	BOOL MouseUp(const CPoint& point);

	//��갴�½��еĲ���,�������TRUE����ôʹ���߱���ˢ�´���
	BOOL MouseDownSlider(const CPoint& point);	

	//��ť�ƶ����еĲ�������ôʹ���߱���ˢ�´���
	BOOL MouseMoveSlider(const CPoint& point);

	//��ť������еĲ�������ôʹ���߱���ˢ�´���
	BOOL MouseUpSlider(const CPoint& point);

	//��ȡ�������ķŴ���
	int GetSliderPos();

	//���û�����
	void  SetSliderPos(int zoomPos); 

	//��ȡ������������
	void GetSliderRect(CRect& rect);

	//���û�����������,�ǵı���ͼƬ��������
	void SetSliderRect();
private:
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
private:

	CPoint        m_ptPrePoint;
	CPoint        m_ptCurPoint;
	BOOL   m_bMoveMagn; //�ж��Ƿ�����������
	int    m_nMousePos;//��ǰ������ڵ�λ��
	int    m_nMousePos2;//����λ���������״̬�仯
	int    m_nMouseState;//����״̬
	DRAW_STATE    m_nDrawStatus;
	PWBMagnifierItem m_pItem;//���ݽṹ�壬����Ŵ󾵵�����ͷŴ���
	CWBImageSliderControl* m_pSliderControl;//������ָ��
	CWBControlButton* m_pCloseBtn;//�رհ�ťָ��

	XCritSec m_csMagnifierOperationLock;
};

//�۹����һ�������Ĵ��ڣ���������view. modify by zhuyuyuan
class CWBSpotlight : public CWBObject //�۹��  
{
public:
	CWBSpotlight(CRect screenRect = NULL);
	~CWBSpotlight();
	BOOL CreateSpotlight(CRect rect, HWND hwnd);
	HWND GetSafeHwnd() const;


	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	//���úͻ�ȡ���ݶ���ֵ
	void SetItem(const PWBSpotLightItem pItem);
	void SetItem(const CRect& rect);
	PWBSpotLightItem GetItem();
	void SetRect(const CRect& rect);
	void GetRect(CRect& rect) const;

	//����
	void ProcessSpotlight(CRect rect);  //����Ŵ�ͬ��
	void AdjustSize(CRect& newRect,bool bSeedNet);//������С�������Դ���

	CSpotlightDlg *GetSpotlightDlg() {return m_pSpotlight;}
private:
	CSpotlightDlg *m_pSpotlight;
	PWBSpotLightItem m_pItem;

	XCritSec m_csSpotlightOperationLock;
};


class CWBAnnotations : public CWBObject //ע����
{
public:
	CWBAnnotations();
	CWBAnnotations(CPoint &ptTopLeft, LOGFONT &lfLogFont,COLORREF &clrText, LPCTSTR lpszText = NULL);
	CWBAnnotations(CPoint &ptTopLeft);
	CWBAnnotations(const CWBAnnotations &other);
	virtual ~CWBAnnotations();

	///add by aolingwen 2012.12.4
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	///end

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ

	virtual void Draw(CDC *pDC);

	BOOL IsInPointArea(CPoint &pt)const; //�ж���굥�����Ƿ��ڵ�ķ�Χ��
	virtual int IsInSelectArea(CPoint& pt)const; //��ѡ��

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //�ж϶����Ƿ�������������
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //�ж϶����Ƿ��������ཻ

	//����ѡ���
	virtual void CreateSelecrtFrame();

	void CreatePolygonFrame(); //������ע���
	CWBText *GetText();
	CWBPolygon *GetPolygon();
	void SetIsChange(BOOL bIsChange);
	void SetChangePoint(CPoint ptChange);
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te);//��ע��TouchUpAllCursorʲô������
	//��������еı�ע�ļ������
	void SetChangePtInCom(CPoint ptChange);

	CPoint GetChangePoint(); //�õ���ǵĵ������
	void SetDirection(); //���ݼ�ǵ��������ü�Ƿ���
	void SetSrcPoint(); //��¼����ǰ��ǵ���������Ͻǵĵ�ӳ�䵽���������

	//������������
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //���ݶ�����ת�����ź󴴽�����

	//ƫ�����ƶ�����
	virtual void MoveOffset(const CPoint& ptOffset); 

	//�ԽǶ�ƫ����ת����
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת

	//��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���

	virtual BOOL IsSolid() const; //�ж϶����Ƿ����

	//ͬ�������ڶ�ȡXMLֵ

	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);	
	//����͸���Ȳ���
	virtual void SetAlpha(int alpha);
private:
	XCritSec m_csAnnotationsLock;	//��ע����

	CWBText *m_pText;
	CWBPolygon *m_pPolygon;

	BOOL m_bIsChange; //�Ƿ�ı�����
	CPoint m_ptChange; //��ǵĵ������
	int m_nDirection; //��ǵķ���

	CPoint m_ptSrcChange; //����ǰ��ǵ������
	//�������ű���
	double m_dbXScale;
	double m_dbYScale;

	BOOL m_bIsCopy; //�Ƿ��Ǹ��ƵĶ���
};

class CWBMedia : public CWBObject
{
public:
	friend class CWBMediaWindow;
	CWBMedia(CString strFilePath = NULL, CRect rct = CRect( 74, 43, 580, 450 ), BOOL bActiveCreated = TRUE);

	CWBMedia(const CWBMedia &other, BOOL bActiveCreated = TRUE);

	~CWBMedia();
public:
	//����ͬ��
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;

	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	//CWBMedia  operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;

	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;

	virtual BOOL Load(char *pBuf, DWORD& dwLen);

	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ

	virtual void Draw(CDC *pDC);

	CString GetFilePath(){return m_pItem->strFilePath;}

	//���ݿ���
	static PWBMediaItem CopyItem(PWBMediaItem pDesItem, const PWBMediaItem pSrcItem);

	void SetItem(const PWBMediaItem pItem);

	void GetItem(PWBMediaItem pItem) const;

	//����ѡ���
	virtual void CreateSelecrtFrame();
	
	void SetMediaRect(CRect rect);
	CRect GetMediaRect();
	//�õ����ƿ�ָ��
	CWBGlobalMediaCtrl* GetMediaCtrl() const { return m_pMediaCtrl; }

	SCameraReceiveData cameraData;
public:

	void SetMediaCtrlTip(CPoint point);
	//����ѡ���
	void UpdateSelectFrame(CRect rct);

	//�ı���
	void SetZOrder();

	//���ƿ��Ƿ���ʾ
	BOOL m_bIsHide;

	BOOL GetIsHide() const { return m_bIsHide ;}

	void SetHideState(BOOL isShow) { m_bIsHide = isShow; }

	void HideAndPauseMedia();

	void ShowAndPlayMedia();

	//���ƿ��Ƿ���ʾ����ͼ��
	BOOL m_bIsMediaCtrlShow;

	BOOL GetMediaCtrlShow() const { return m_bIsMediaCtrlShow; }

	void SetMediaCtrlShow( BOOL bIsMediaCtrlShow ) { m_bIsMediaCtrlShow = bIsMediaCtrlShow; }

	//ƫ�����ƶ�����
	virtual void MoveOffset(const CPoint& ptOffset); 

	//��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������

	virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���

	//��ѡ��
	virtual int IsInSelectArea(CPoint& pt)const;

	void SetMediaBeCreated(BOOL bCreated) { m_bCreated = bCreated ;}

	//�ж��Ƿ���Ҫ��SET�з���ͨ��ID����
	void SetMediaBitrate(BOOL birate) {m_MediaBitrate = birate;}
	BOOL GetMediaBitrate() const {return m_MediaBitrate;}
	//���汻�����ܵ���Ƶ����
	void SetSaveGetMediaName(CString strName){m_strGetName = strName;}
	//��ȡ�������ܵ���Ƶ����
	CString GetSaveGetMediaName(){return m_strGetName;}
	//��ȡ�û�ID
	CString GetUCCUserID();
private:
    double m_Reduce;
	CImage m_img;
	CString m_strGetName; //�������ܵ���Ƶ����
	BOOL m_MediaBitrate;

	void ProcessMove(int nOffSetX, int nOffSetY);

private:
	//����
	CWBMediaWindow *m_pMediaWnd;

	//���ƿ�
	CWBGlobalMediaCtrl *m_pMediaCtrl;

	//����λ����Ϣ
	CRect m_rctMediaWindow;

	//����������ڼ���
	static unsigned int m_nStcNum;

	//��ý�����ݽṹ
	PWBMediaItem m_pItem;

	//ͨ���ţ���Ƶ������Ψһ��ʶ��
	unsigned int m_nNumChannel;

	//��ý�岥��״̬
	EMediaPlayState m_eMediaPlayState;

	//��ý�廻ҳ֮ǰ��״̬
	EMediaPlayState m_ePrePlayState;

	EVoiceState m_eVoiceState;

	//�Ƿ������������ı�־ TRUE ���������� FALSE ��������
	BOOL m_bActiveCreated;

	//�Ƿ��Ѿ�����������Զ����Ƶ�����Ŀ��ƣ���Ϊ��ʹ����ͷ��Ϣֻ��һ��
	BOOL m_bCreated;

	////UCC�б�ʶ���û�ID������Ƶ���ڸ��û���
	//std::string m_strUCCUerID;

	XCritSec		m_csOpenMedia;
	int             m_SetCurVolice;//��Ƶ���õ�����ֵ
public:
	CRect GetMoveRect();//�ƶ���Ƶʱ����Ƶ���ھ���
	void SetMoveRect(CRect rect);

	void OpenMedia(const char* strFilePath = NULL, BOOL bActiveCreated = TRUE);

	void CreateMedia(const char* strFilePath = NULL);

	/*��ȡ�����ò���״̬*/
	EMediaPlayState GetMediaPlayState() const;

	void SetMediaPlayState(EMediaPlayState mediaPlayState);

	/*��ҳ����ʱ��Ƶ�Ĳ���״̬*/
	EMediaPlayState GetPrePlayState() const;

	void SetPrePlayState(EMediaPlayState prePlayState);

    //��Ƶ����
	void MediaPlay();

    //��Ƶ��ͣ
	void MediaPause();

    //��Ƶֹͣ
	void MediaStop();
    
    //ѭ������
	void MediaLoop();

	//��ȡ�Ƿ�ѭ��������Ϣ
	BOOL GetMediaLoopState() const;
	
	//������Ƶ������ѭ������״̬
	void SetMediaLoopState(BOOL bMediaLoopState);

	//��Ƶ��ͼ
	BOOL CaptureToBMPFile();

	//����
	void MediaSetMute(EVoiceState eVoiceState);
	
	//��������
	void SetVedioVolume(int nVolume);

	int  GetVedioVolume() const { return m_SetCurVolice;}
	void SetCurVolume(int volume) {m_SetCurVolice = volume;}

    //��ȡ��������
	EVoiceState GetVoiceState() const;

	//���þ���
	void SetVoiceState(EVoiceState voiceState);

	//���ȶ�λ
	void SeekVedioPosition(unsigned long ulSeconds);

	//��ȡ��Ƶ���ļ����ȣ�ʱ�����
	unsigned long GetVedioDuration() const;

	//��ȡ��Ƶ��ǰ��λ�ã���ʱ��Ϊ����
	unsigned long GetVedioPosition() const;

	CWBMediaWindow* GetMediaWindow() const; 

	//�ж��Ƿ�������������, TRUE������FALSE
	BOOL IsActiveCreated() { return m_bActiveCreated; }
	BOOL IsMdediaOpen()  {return m_pMediaWnd->IsMediaOpen();}
	CAVCapture *GetCapture() const  {return m_pMediaWnd->GetCapture();}
	//int GetBitrate() {return m_pMediaWnd->GetBitrate();}
    ImageEx* m_bkImg;
};

//ͬ����ʶ
enum WBCameraSynchronousLable{
	Connect_Synchronous,
	Position_Synchronous,
};
class CWBCamera : public CWBObject
{
public:
	friend class CDlgCamera;
	CWBCamera(BOOL bActiveCreator = TRUE, CRect rctPosition = CRect(74, 43, 474, 343),BOOL bLocal = FALSE);
	~CWBCamera();
public:
	//����ͬ��
	void SetSynchronousLable(int nType);
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	//operatiCWBMediaons
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);
	CDlgCamera* GetCameraDlg() { return m_pDlgCamera;}
	//���ݿ���
	static PWBCameraItem CopyItem(PWBCameraItem pDesItem, const PWBCameraItem pSrcItem);

	//����ѡ���
	virtual void CreateSelecrtFrame();

	virtual BOOL ZoomByButton( const CPoint& ptOffset );

	void HideAndPauseCamera();
	void ShowAndPlayCamera();
	
	CRect GetCaremaRect();
	CRect CWBCamera::GetMoveRect();
	void CWBCamera::SetMoveRect( CRect rect );

public:
	void SetIsHide(BOOL hide){ m_bIsHide = hide;}
	BOOL GetIsHide() { return m_bIsHide ;}
	//ƫ�����ƶ�����
	virtual void MoveOffset(const CPoint& ptOffset);

	//��ȡ�û�ID
	CString GetUCCUserID();
	void SetCCUserID(string str) {m_strUCCUerID = str;}

	//��������
	//virtual void ZoomByLeftTop(const CPoint& ptOffset); //�����ϵ�Ϊ��������
	//virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������
	//virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���
	//�Ƿ��Ǵ�����
	BOOL BCreatorOwner(){return m_bActiveCreator;};
private:
	BOOL m_bIsHide;

	CDlgCamera *m_pDlgCamera;
	//WBCameraItem *m_pItem;//m_pCameraItem
	CRect m_rctCameraWindow;
	//����������ͷ(�ܽ�������)�����Ǳ�����������ͷ�����ܽ������ã�
	BOOL m_bActiveCreator;
 	//UCC�б�ʶ���û�ID��������ͷ���ڸ��û���
 	std::string m_strUCCUerID;

	//����ѡ���
	virtual void UpdateSelectFrame( CRect rct );
	//����ͷ����
	//PSWBCameraData m_dataCamera;
	//
	WBCameraSynchronousLable m_SynchronousLableType;
	double m_reduce;
	XCritSec		m_csOpenCamera;
};

typedef struct COMPONENTSCALE
{
	double dbScaleX;
	double dbScaleY;
}ComponentScale;

class CWBComponent : public CWBObject
{
public:
	CWBComponent();
	~CWBComponent();

	virtual void Draw(CDC *pDC);

	//��������и�������� m_ptMapCenter ��ֵ
	void SetComMapCenterPt();

	//����ѡ���
	virtual void CreateSelecrtFrame();

	//����ƫ�����ƶ�
	virtual void MoveOffset(const CPoint& ptOffset);

	//����
	virtual BOOL ZoomByButton(const CPoint& ptOffset);

	//�������λ��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); 

	//�ԽǶ�ƫ����ת����
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//������ĳһ����ת
	virtual void RotateByCenter(double dbAngleOffset); //���������ĵ���ת

	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	int SetSelectedObj(CPoint& pt); //��ѡ��
	void SetSelectedObj(DWORD dwLayerIndex); //ʵʱͬ��ʱȷ������ѡ��Ķ���

	virtual void SetSelectFrameShow(BOOL bShow); //��ʾѡ���

	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ

	//����͸���Ȳ���
	virtual void SetAlpha(int alpha);

	void ZoomForMidi(const CPoint& ptZoomCenter,double dbScale,bool bmag = true);
public:
	void GetSrcAnnPoint(); //�õ���תǰ����б�ע�ļ�ǵĵ�
	void GetSrcCenterPoint(); //�õ���תǰ�����ÿ���������ת���ĵ�
	void GetSrcZoomCenter(); //�õ�����ǰ�����ÿ�������ѡ������ϽǺ����½ǵĵ�
	void GetEachSrcZoomCenter(); //�õ�����ǰ�����ÿ�������ѡ������Ͻǵĵ�
	void MoveAnn(const CPoint& ptOffset); //�ƶ�����еı�ע
	void ClearSrcAnnPt(); //�����תǰ����б�ע�ļ�ǵĵ�

	void Erase(const CRect& rectErase);

	void IsComRegionIn(const CRgn *pRgn);
public:
	void SetParent(CWBObject* pParent);

	void AddObject(CWBObject *obj);
	void DeleteObject(UINT layerIndex);
	void DeleteSelectedObject();
	void DeleteAll();

	BOOL HasSeletedComponent();
	
	void SetSelectCom(CWBObject *pObj);
	CWBObject* GetSelectCom();

	void SetOffsetBound(CRect &rc);

	CWBComponent* Copy(CWBComponent *pNew);
	BOOL IsComponentsEmpty(); //����Ƿ�Ϊ��
private:
	XCritSec m_csComponentLock;	//�������

	CWBObject * m_pParent;
	std::vector<CWBObject*> m_vecComponents; //�Ӷ����б�

	std::vector<ComponentScale*> m_vecComponentScale;

	CWBObject *m_pCurSelectedObj; //��ǰ��ѡ�еĶ���

	CRgn m_rgnOffsetBound;  //�Ӷ����ƶ���Χ

private:
	std::vector<CPoint> m_ptAnnPtList; //��ע�ļ�ǵĵ�
	std::vector<CPoint> m_ptResAnnPtList; //��ע�ļ�ǵĵ������������ϵ�������

	std::vector<CPoint> m_ptSrcCenterList; //��¼�����ÿ��������תǰ�����ĵ�����
	std::vector<CPoint> m_ptPreCenterList; //��¼�����ÿ������ǰһ�ε���ת�������
	double m_dbRotateAngle; //���������ĵ���ת�ĽǶ�

	std::vector<CPoint> m_ptSrcLeftTopList; //��¼�����ÿ����������ǰ���Ͻǵĵ�
	std::vector<CPoint> m_ptSrcRightBottomList; //��¼�����ÿ����������ǰ���½ǵĵ�
	std::vector<CPoint> m_ptPreLeftTopList; //��¼�����ÿ������ǰһ�����ź����Ͻǵĵ�
	std::vector<CPoint> m_ptPreRightBottomList; //��¼�����ÿ������ǰһ�����ź����½ǵĵ�

	//�������ű���
	double m_dbXScale;
	double m_dbYScale;

	std::vector<CPoint> m_ptEachSrcLeftTopList; //��¼�����ÿ������ÿ������ǰ���Ͻǵĵ�

	std::vector<CPoint> m_ptResObjSrcLeftTopList;     //��¼�����ÿ����������ǰ ���Ͻ� �ĵ�ӳ���ڶ�������ϵ�������

	std::vector<CPoint> m_ptResObjSrcCenterList; //��¼�����ÿ����������ǰ�������ĵ�ӳ������϶�������ϵ�������
};


typedef struct _WBRuler    //ֱ��
{
	CPoint        ptRulerLT;      //ֱ�����ϵ� 
// 	CPoint        ptRulerRT;      //ֱ�����ϵ�
// 	CPoint        ptRulerLB;      //ֱ�����µ�
	CPoint        ptRulerRB;      //ֱ�����µ�
	CPoint        ptRulerMP;      //ֱ�߲�����
	CPoint        ptRulerCT;      //ֱ�����ĵ�
}WBRulerItem,*PWBRulerItem; 


class CWBRuler : public CWBObject
{
public:
	CWBRuler(CPoint &ptLTpoint);
	~CWBRuler();

	virtual void MoveOffset(const CPoint& ptOffset);
	virtual void CreateSelecrtFrame();

	virtual BOOL ZoomByButton(const CPoint& ptOffset);
	virtual void Draw(CDC *pDC);
	CRect GetRuler1Rect();
	/*CWBSelectRectangle get*/
	void SetItem(const PWBRulerItem pItem);
	void GetItem(PWBRulerItem pItem)const;
	CPoint GetRulerMoveBtn();
	CPoint GetRulerLT();
	void MoveRulerBtn(CPoint point);
	BOOL RulerPointIn(CPoint point);
    int GetNum();
private:
	PWBRulerItem m_pRuleItem;  //���ݽṹ
	int m_ncountnumber;
};

/*********************������**************************************/

class CWBProtractor : public CWBObject  //��������
{
public:
	CWBProtractor(CPoint &ptLTpoint);
	~CWBProtractor();
public:
	void Draw(CDC *pDC);    //��������
	void setProtractorPoint(const CPoint &ptProtractorLT,const CPoint &ptProtractorRB);  //���õ�����
	PWBProtractorItem GetProtractorItem(); //��ȡ����
	int PointIn(CPoint point);   //�ж�����
	int MouseDown(const CPoint &point);  //����
	int MouseMove(const CPoint &point);  //����
	int MouseUp(const CPoint &point);   //����
	void MoveDrawBtn(const CPoint &point);  //�ƶ�������
	virtual void MoveOffset(const CPoint& ptOffset); //�ƶ�
	int IsInSelectArea(CPoint& pt)const;
	int SetUnit(const int);
	void CreateSelecrtFrame();//����ѡ���
	//��ȡ���ɽǶȵ���ص�����
	CPoint GetCTpoint();
	CPoint GetRBpoint();
	CPoint GetLTpoint();
	double GetRadian();
	void SetInButton();
	bool GetInButton();
	int m_nMousePos;
private:
	PWBProtractorItem m_pProtractorItem;  //���ݽṹ
	ImageEx *m_pImageProtractorBtn;
	ImageEx *m_pImageRotationBtn;
	ImageEx *m_pImageAttributeBtn; //���԰�ť

	int m_nSetUnit;
	CPoint m_AttBTN;
	CPoint m_ptPrePoint;
	CPoint m_ptCurPoint;
	bool m_binprotractor;
	double m_angle;
	double m_RotateAngle;
	CPoint m_CTpoint;//�������ĽǶ����ĵ�
	CPoint m_LTpoint;//�������ĽǶ��ƶ���
	CPoint m_RBpoint;//�������ĽǶ����¹̶���
	double m_radian;
	bool m_binButton;
}; 

//���ɿ̶���
class CWBAngle:public CWBObject
{
public:
	CWBAngle(CPoint &ptLTpoint, CPoint &ptRBpoint,CPoint &ptCTpoint,double &angle);
	~CWBAngle();

public:
	void Draw(CDC *pDC);          //���̶�
	void setAnglePoint(const CPoint &ptAngleLT,const CPoint &ptAngleRB,const CPoint &ptAngleCT);  //���õ�����

	virtual void CreateSelecrtFrame();   //����ѡ���
	virtual void MoveOffset(const CPoint& ptOffset);    //�ƶ�
	CRect GetAngleRect();   //��ÿ̶ȶ����Rect
	void SetItem(const PWBAngleItem pItem);      //��������
	void GetItem(PWBAngleItem pItem)const;       //�������
private:
	PWBAngleItem m_pAngleItem;
	bool m_bchange;
	CPoint m_CTpoint;//�������ĽǶ����ĵ�
	CPoint m_LTpoint;//�������ĽǶ��ƶ���
	CPoint m_RBpoint;//�������ĽǶ����¹̶���
	double m_radian; //���ɿ̶ȵĳ�ʼ�Ƕ�
};

//typedef struct _WBSetSquare    //���ǰ�
//{
//	CPoint        ptSetSquareLT;      //���ǰ����ϵ� 
//	CPoint        ptSetSquareInLT;      //���ǰ������ڵ�
//	CPoint        ptSetSquareInRB;      //���ǰ������ڵ�
//	CPoint        ptSetSquareRB;      //���ǰ����µ�
//	//CPoint        ptSetSquareInCT;      //��������ֱ�ǵ�
//	CPoint        ptSetSquareCT;      //���ǰ����ĵ�
//}WBSetSquareItem,*PWBSetSquareItem; 
class CWBSetSquare : public CWBObject  //���ǰ�
{
public:
	CWBSetSquare(CPoint &ptLTpoint);
	//CWBSetSquare(const PWBSetSquareItem  pItem = NULL);
	~CWBSetSquare();

	void Draw(CDC *pDC);    //�����ǰ�
	void setSetSquarePoint(const CPoint &ptSetSquareLT,const CPoint &ptSetSquareRB);  //���õ�����
	int PointIn(CPoint point);   //�ж�����
	int MouseDown(const CPoint &point);  //����
	int MouseMove(const CPoint &point);  //����
	int MouseUp(const CPoint &point);   //����
	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	void IsInSetSquare(CPoint point);//�ж��Ƿ������ǰ�����
	int IsInSelectArea(CPoint& pt)const;//��ѡ

	virtual void MoveOffset(const CPoint& ptOffset); //�ƶ�
	void MoveDrawBtn(const CPoint &point);  //�ƶ�������
	void MoveLengthBtn(const CPoint &point); //���ų���
	//����ѡ���
	void CreateSelecrtFrame();
	//������ĵ�
	void GetCenterPoint(CPoint& ptcenter);
	CPoint GetSetSquareCT(){return m_ptRotationCenter;}
	//���õ�λ
	int SetUnit(const int);
	BOOL GetIsInBotton();
	void SetIsInBotton();
	CWBSetSquare * m_pSetSquare;
	BOOL m_bInButton;

private:
	PWBSetSquareItem m_pSetSquareItem;  //���ݽṹ
	ImageEx *m_pImageRotationBtn;  //��ת��ť
	ImageEx *m_pImageZoomBtn;   //���Ű�ť
	ImageEx *m_pImageAttributeBtn; //���԰�ť

	//int m_ncountnumber;   //�̶��� 
	int m_nSetSquareUnit;//��λת���жϱ���

	//double m_angle;
	CPoint m_ptPrePoint;  //���µĵ�
	CPoint m_ptCurPoint;  //�ƶ��ĵ�
	bool m_binSetSquare; //�ж��Ƿ������ǰ�������
	int m_nMousePos;    //���λ��
	CPoint m_ptcenter; //���ĵ�
	CPoint m_ptOffCenter;//����ƫ�����ĵ�

};
class CWBCompass : public CWBObject  //Բ��
{
public:
	CWBCompass(CPoint &ptLTpoint);
	~CWBCompass();

	void Draw(CDC *pDC);    //��Բ��
	//void Draw_Arc();   //Բ������
	void setCompassPoint(const CPoint &ptCompassLT,const CPoint &ptCompassRB);  //���õ�����
	int PointIn(CPoint point);   //�ж�����
	int MouseDown(const CPoint &point);  //����
	int MouseMove(const CPoint &point);  //����
	int MouseUp(const CPoint &point);   //����
	//�жϵ��Ƿ��ڶ����ѡ��Χ��
	void IsInCompass(CPoint& point);//�ж��Ƿ���Բ������
	int IsInSelectArea(CPoint& point)const;//��ѡ

	virtual void MoveOffset(const CPoint& ptOffset); //�ƶ�
	void HorizontalZoom(const CPoint& ptOffset);//ˮƽ����
	void VerticalZoom(const CPoint& ptOffset); //��ֱ����
	//void MoveDrawBtn(const CPoint &point);  //�ƶ�������
	//void MoveLengthBtn(const CPoint &point); //���ų���
	////����ѡ���
	void AdjustLT_RB(CPoint &ptLT,CPoint &ptRB); //�������º����ϵ�����
	void CreateSelecrtFrame();
	////������ĵ�
	void GetCenterPoint(CPoint& ptcenter);
	////���õ�λ
	//int SetUnit(const int);
	//BOOL GetIsInBotton();
	//void SetIsInBotton();
	BOOL m_bInButton;

private:
	PWBCompassItem m_pCompassItem;  //���ݽṹ
	ImageEx *m_pImageAttributeBtn; //���԰�ť

	//int m_ncountnumber;   //�̶��� 
	int m_nCompassUnit;//��λת���жϱ���

	//double m_angle;
	CPoint m_ptPrePoint;  //���µĵ�
	CPoint m_ptCurPoint;  //�ƶ��ĵ�
	bool m_bInCompass; //�ж��Ƿ���Բ��������
	int m_nMousePos;    //���λ��
	CPoint m_ptcenter; //���ĵ�
	CPoint m_ptOffCenter;//����ƫ�����ĵ�

	bool m_bIsDrawArc; //�ж��Ƿ�Բ��
	double m_dStartAngle; //��Բ����ʼ�Ƕ�
	double m_dSweepAngle;  //��Բ��ƫ�ƽǶ�
	bool m_bMaxCompass;    //Բ��뾶�ﵽ����´�MOVEֱ�ӷ��أ�֪��UP��
	CPoint m_ptDrawStart;    //��Բ��������ѡ����START
	CPoint m_ptDrawEnd;     //��Բ����������ѡ����END	 
   //���ڴ���Բ�����������
	double  m_dRadian;             //����
	double  m_dAngle;				 //�Ƕ�
	double  m_dRadius;			 //�뾶
	double  m_dLegLength;			 //֧�ȳ�

	CPoint	m_ptCompassCT;		 //��ת���ĵ�
	CPoint  m_ptCompassMid;        //Բ���м��
	CPoint  m_ptCompassMidUp;      //Բ���м��Up��
	CPoint  m_ptCompassMidLT;   
	CPoint  m_ptCompassMidRT;
	CPoint  m_ptCompassLT;		 //���ϵ�
	CPoint  m_ptCompassLTUp;       //����Up��
	CPoint  m_ptCompassLB;         //���µ�
	CPoint  m_ptCompassLBUp;       //���µ�Up��
	CPoint  m_ptCompassRB;		 //���µ�
	CPoint  m_ptCompassRBUp;		 //���µ�Up��

	CPoint  m_ptCompassHPLB;		 //����1�ŵ�-����̶��������µ�
	CPoint  m_ptCompassHPLT;		 //����1�ŵ�-����̶����ʵ����ϵ�
	CPoint  m_ptCompassHPRT;		//����1�ŵ�-����̶��������ϵ�
	CPoint  m_ptCompassHPRB;		//����1�ŵ�-����̶��������µ�
	
	CPoint  m_ptCompassPenLT;    //���ʵ����ϵ�
	CPoint  m_ptCompassPenLB;		//���ʵ����µ�
	CPoint  m_ptCompassPenRT;		//���ʵ����ϵ�
	CPoint  m_ptCompassPenRB;		//���ʵ����µ�

	CPoint	m_ptCompassRefillRT;   //���ʵ����ϵ�
	CPoint  m_ptCompassRefillLT;   //���ʵ����ϵ�
};

class CWBArc: public CWBObject  //Բ����
{
public:
	CWBArc(double dStartAngle, double dSweepAngle, CPoint &pt,double dRidius, CPoint &ptSelectLT, CPoint &ptSelectRB);
	CWBArc(const PWBArcItem pItem = NULL);//����������ɾ���Ķ�������
	CWBArc(const CWBArc& other);//�������캯��
	~CWBArc();
	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	CWBArc & operator=(const CWBArc& other); //��ֵ����//����

	void Draw(CDC *pDC);    //��Բ��
	virtual BOOL Save(char *pBuf, DWORD& dwLen)const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength);//���ļ���ȡ

	//�жϵ��Ƿ��ڶ���ѡ��������
	virtual int IsInSelectArea(const CRect& rect) const; //��ѡ ����IsRegionIn()����
	virtual BOOL IsInSelectArea(CPoint& pt) const;    //��ѡ
	virtual BOOL IsRegionIn(const CRgn *pRgn) const; ////�ж϶���ѡ���������ߵ��е��Ƿ��ڿ�ѡ��������

	void SetItem(const PWBArcItem pItem);      //��������
	void GetItem(PWBArcItem pItem) const;      
	static PWBArcItem CopyItem(PWBArcItem pDesItem, const PWBArcItem pSrcItem);//���ݿ���

	virtual void CreateSelecrtFrame();   //����ѡ���
	virtual void MoveOffset(const CPoint& ptOffset);    //�ƶ�

	BOOL ZoomByButton(const CPoint& ptOffset);//��ť���� ���ø���
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale);//���������ĵ�����

	void AdjustLT_RB( CPoint &ptLT,CPoint &ptRB );

		//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	int GetLineWidth() const;
private:
	PWBArcItem  m_pWBArcItem;  //Բ���ṹ��
	CPoint m_ptRotationCenter;
	XCritSec m_csArcItemLock;

};

class CWBSector: public CWBObject  //������
{
public:
	CWBSector(double dStartAngle, double dSweepAngle, CPoint &pt,double dRidius, CPoint &ptSelectLT, CPoint &ptSelectRB);
	CWBSector(const PWBSectorItem pItem = NULL);//����������ɾ���Ķ�������
	CWBSector(const CWBSector& other);//�������캯��
	~CWBSector();

	/*---------------------------------------------------------------------------------------------
	����Ϊ������������������ƽ��������ת�������,����������ݽṹ
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//����

	CWBSector & operator=(const CWBSector& other); //��ֵ����//����

	//����
	void Draw(CDC *pDC);    //������
	virtual BOOL Save(char *pBuf, DWORD& dwLen)const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength);//���ļ���ȡ

	//�жϵ��Ƿ��ڶ���ѡ��������
	virtual int IsInSelectArea(const CRect& rect)const; //��ѡ ����IsRegionIn()����
	virtual BOOL IsInSelectArea(CPoint& pt)const;    //��ѡ
	virtual BOOL IsRegionIn(const CRgn *pRgn)const; ////�ж϶���ѡ���������ߵ��е��Ƿ��ڿ�ѡ��������

	void SetItem(const PWBSectorItem pItem);      //��������
	void GetItem(PWBSectorItem pItem)const;      
	static PWBSectorItem CopyItem(PWBSectorItem pDesItem, const PWBSectorItem pSrcItem);//���ݿ���

	virtual void MoveOffset(const CPoint& ptOffset);    //�ƶ�
	BOOL ZoomByButton( const CPoint& ptOffset );
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale);//���������ĵ�����
	void AdjustLT_RB( CPoint &ptLT,CPoint &ptRB );
	virtual void CreateSelecrtFrame();   //����ѡ���

	void SetFill(BOOL bIsFill = TRUE);//�����Ƿ����
	void SetFillColor(COLORREF clrFill); //���������ɫ
	
	//�������rgn
	BOOL CreateRgn(CRgn *pRgn)const; //��������   ---δʹ��
	BOOL PointIn(const CPoint& pt)const; //�жϵ��Ƿ��ڶ����(���)��Χ��
	
	virtual BOOL IsSolid()const; //�ж϶����Ƿ����
	
	//ͬ�������ڶ�ȡXMLֵ
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	int GetLineWidth() const;
private:
	PWBSectorItem  m_pWBSectorItem;  //���νṹ��
	//double m_dStartAngle;
	//double m_dSweepAngle; 
	//CPoint m_ptSelectLT; //Բ��ѡ������
	//CPoint m_ptSelectRB;   //Բ��ѡ����յ�
	//CPoint m_ptDrawStart;  //Բ�����
	//CPoint m_ptDrawEnd;    //Բ���յ�
	/*CPoint m_ptCT;*/
	//double m_dRadius;
	CPoint m_ptRotationCenter;
	XCritSec m_csSectorItemLock;
	//RectF m_rectf;  //�������о���
	BOOL m_bIsFill; //�Ƿ����
	BOOL m_bHaveFillRgn; //�Ƿ����������
};

typedef struct _WBDxfItem //DxfItem
{
	CString strFilePath;
	CRect rect; //λ��;
    VIEW  View; 
	COLORREF BKColor;
}WBDxfItem,*PWBDxfItem;

class CWBDxf : public CWBObject //Dxf��
{
public:
	CWBDxf(CString strFilePath = NULL, CRect rct = CRect( 0, 0, 800, 600 ));
	CWBDxf(const CWBDxf&other);
	~CWBDxf();

public:
	//����ͬ��
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //���ļ���ȡ
	virtual void Draw(CDC *pDC);
	CString GetFilePath() {return m_pItem->strFilePath;}

	//ƽ�Ʋ���
	void SetOrgPoint(CPoint _orgPoint);
	void DxfDrag(CPoint _dragPoint);

	//���Ų���
	void CWBDxf::ZoomIn(float scale);
	void CWBDxf::ZoomOut(float scale);

	//���ݿ���
	static PWBDxfItem CopyItem(PWBDxfItem pDesItem, const PWBDxfItem pSrcItem);
	void SetItem(const PWBDxfItem pItem);
	void GetItem(PWBDxfItem pItem) const;

	//����ѡ���
	virtual void CreateSelecrtFrame();

	//�����Ƿ�Ҫ�ػ�DXF
	void SetIsNeedReDraw(BOOL bIsNeedReDraw = TRUE) { m_bIsNeedReDraw = bIsNeedReDraw; };
    BOOL GetIsNeedReDraw() { return m_bIsNeedReDraw; };

public:
	//����ѡ���
	void UpdateSelectFrame(CRect rct);
	void ChangBKColor(COLORREF BKColor);

	//���ƿ��Ƿ���ʾ
	//BOOL m_bIsHide;
	//BOOL GetIsHide() const { return m_bIsHide ;}
	//void SetHideState(BOOL isShow) { m_bIsHide = isShow; }
	virtual void MoveOffset(const CPoint& ptOffset); 

	//��������
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //�������λ��������
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//�������Ű�ťƫ�����Ŷ���
	virtual int IsInSelectArea(CPoint& pt);

private:
	mutable bool m_bSaveSameDxf;

	CDrawing m_Drawing;
	CPoint orgPoint;

	//dxf���ݽṹ
	PWBDxfItem m_pItem;

	CDC memDC;
	BOOL m_bIsNeedReDraw;
	CBitmap *m_oldBitmap;

public:
	void CreateDxf(char* strFilePath = NULL);
	BOOL CaptureToBMPFile();
};

 class CWBPallelogam : public CWBPolygon
{
public:
	CWBPallelogam();
	CWBPallelogam(const CPoint& ptLeft,const CPoint& ptRight);
	CWBPallelogam(const CWBPallelogam &other);
	virtual ~CWBPallelogam();

	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);

	virtual void CreateSelecrtFrame();
	virtual void AdjustObject();

	BOOL Move(const CPoint& ptLeft,const CPoint& ptRight);
	virtual BOOL Move(const CPoint& ptRightBottom);
	virtual BOOL ZoomByButton(const CPoint& ptOffset);

	virtual void Draw(CDC *pDC);
	int Width() const;
	int Height() const;
private:
	XCritSec m_csPallelogamItemLock;
	int  m_nlength;
	bool m_bDrawType;
};

 class CWBMagicPen : public CWBCurve
 {
 public:
	 CWBMagicPen(const PWBCurveItem pItem = NULL);
	 virtual ~CWBMagicPen();


	 
	 int GetDrawCount();
	 void AddDrawCount();
 private:
	 int m_nDrawCount;//���Ĵ���
 };


 typedef struct _WBMidiItem
 {
	 CString strFilePath;//midi�ļ�·��
	 int mMidiWidth;//midi�Ŀ��
	 int mMidiHeight;//midi�ĸ߶�
	 CRect rect;//WBmidi������ʾ��λ��
 }WBMidiItem,*PWBMidiItem;

 class CWBMidi : public CWBObject
 {
 public:
	CWBMidi(CString strPath = NULL);
	~CWBMidi();
	CWBMidi(const CWBMidi& other);

	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	virtual void Draw(CDC* pDC);
	virtual void MoveOffset(const CPoint& ptOffset);
	 
	virtual void CreateSelecrtFrame();
	virtual BOOL ZoomByButton(const CPoint& ptOffset);

	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	virtual int IsInSelectArea(CPoint& pt)const;
	virtual void SetSelectFrameShow(BOOL bShow);
	 
	void init(CString strpath);
	void ChangeShowPosition(const CPoint point);
	void DownChangePosition(const CPoint point);

	BOOL DeleteComponent();
	CWBObject *GetAnn();
	void MoveAnn(const CPoint& ptOffset);
	void SetZoomMagn();
	void SetZoomReduce();
	BOOL PtIsComponent(CPoint pt);
	bool GetShowAutoRunBtn();
	//void SetPositionUp();
	//void SetPositionDown();
	BOOL ObjectIsComponent(CWBObject* obj);
	void AddComponent(CWBObject* obj);

	bool Getshowmagbtn(){return m_bshowmagbtn;}
	bool Getshowredubtn(){return m_bshowredubtn;}
	void SetItem(PWBMidiItem pItem);
	void GetItem(PWBMidiItem pItem);
	static PWBMidiItem CopyItem(PWBMidiItem pDesItem,const PWBMidiItem pSrcItem);
	void SetBneedDraw(BOOL bshow){m_bneedDraw = bshow;}
	void SetBSetZoomM(bool bset){m_bSetZoomMidi = bset;}
	void CopyComponets(const std::map<int, CWBComponent*> &otherCom);
	void Setbneedinit(bool bneed){m_bneedInit = bneed;}
	bool Getbneedinit(){return m_bneedInit;}
	void SetBShowCompont(bool bshow){m_bshowCompont = bshow;}
	CWBObject *GetSelectComObj();
	bool GetShowChangeViewBtn();
	int GetCopyView(){return m_nCopyView;} 
// 	void SetPrePage();
// 	void SetNextPage();
// 	void SetFirstPage();
// 	void SetLastPage();
// 	int GetPageIndex();
// 	int GetPageTotal();
	void SetSelectedObj(DWORD dwLayerIndex);

	void SaveAsJpg();

 public:
	 bool m_bSetZoomMidi;
	 bool m_bshowCompont;
	 BOOL m_bneedDraw;
	 bool m_bAddobj;
	 CWBObject* m_pAddobj;
 private:
	//int m_nPageTotal;//z��ҳ��
	int m_nPageindex;
	PWBMidiItem m_pItem;
	CDC m_dcMidi;//����midi�ļ���ȡ�Ļ���
	HMODULE m_hmd;
	XCritSec m_csMidiItemLock;
	
	CPoint m_ptPreDraw;//�ϴ�ʹ�õ��ĵ�λ��
	int m_nCopyView;//�ӻ����п�����λ��
	CDC dcMidi;
	float m_nScale; 
	
	bool m_bneedInit;
	std::map<int, CWBComponent*> m_comlist;//�ں��������
	bool m_bDrawusedll;
	//CWBText *m_pPageIndexTip;
	bool m_bshowmagbtn;//�Ƿ���ʾ�Ŵ�ť
	bool m_bshowredubtn;//�Ƿ���ʾ��С��ť
	
 };
