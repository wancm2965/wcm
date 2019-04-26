#pragma once
/*---------------------------------------------------------------------------------------------------------------
数据类

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
#include "./WBMedia/WBWindows.h"//视频窗体
#include "./WBMedia/DlgCamera.h"//摄像头窗体
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
#define WB_OBJECT_SELECT_SEGMENT   10   //选择对象时的触发区域范围
#define  BTN_RECTMAQ_CLOSE 9   //放大镜关闭按钮
#define  INSIDERECT     50     //相对放大镜矩形框位移量
#define  MEDIAMINWIDTH  500
#define  MEDIAMAXHEIGHT 400
#define  CAREMAMINWIDTH 400
#define  CAREMAMINHEIHT 300
//denghong
//协同操作定义
#define SYSTEM_RESERVE     0X00  //系统保留
#define DB_ADD_OBJ         0X01  //添加对象
#define DB_ERASE_OBJ       0X02  //擦除
#define DB_DELETE_OBJ      0X03  //删除
#define DB_ROTATE          0X04  //旋转
#define DB_ZOOM            0X05  //缩放
#define DB_LOCK            0X06  //锁定
#define DB_CHAGE_ATTRIBUTE 0X07  //更改属性
#define DB_MOVE            0X08  //移动
#define DB_COMBINE         0X09  //组合
#define DB_LAYERTOTOP      0X0A  //图层置顶
#define DB_LAYERTOBOTTOM   0X0B  //图层置底
#define DB_FREVIOUSLAYER   0X0C  //图层上移
#define DB_NEXTLAYER       0X0D  //图层下移
#define DB_DELETE_ALLOBJ   0X0E	 //删除全部对象
#define DB_SETBACKGROUND   0X0F  //设为背景
#define DB_ERASEINDEX      0X10  //擦除直线顺序


//直尺定义
#define MOUSE_ONRULERATTRIBUTE_BTN 120
#define MOUSE_ONRULERROTATION_BTN  121
#define MOUSE_ONRULERZOOM_BTN      122
#define MOUSE_ONOTHERAREA          123
#define MOUSE_ONCIRCLEAREA         124

#define MOUSE_ONTHEROTA_BTN        125
#define MOUSE_ONTHECIRCLE          126
#define MOUSE_ONTHEOTHER           127
#define MOUSE_ONTHEROTATION_BTN    128


//三角板定义
#define  MOUSE_ONSETSQUAREATTRIBUTE_BIN  130
#define  MOUSE_ONSETSQUAREOTATION_BIN    131
#define  MOUSE_ONSETSQUAREZOOM_BIN       132
#define  MOUSE_ONSETOTHERAREA            133  //可移动区域的返回
#define  MOUSE_ONSETCIRCLEAREA           134
//圆规区域定义
#define  MOUSE_ONCOMPASS_ATTRIBUTE_BTN    136  //属性按钮
#define  MOUSE_ONCOMPASS_MOVE_RGN          137 //move移动区域
#define  MOUSE_ONCOMPASS_VERTICAL_RGN      138  //垂直拉伸区域
#define  MOUSE_ONCOMPASS_HORIZONTAL_RGN    139  //水平拉伸区域
#define  MOUSE_ONCOMPASS_ROTATE_RGN        140  //旋转区域
#define  MOUSE_ONCOMPASS_ROTATE_DRAW_RGN   141  //旋转笔绘图区域
//放大器鼠标位置定义
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

//放大器滑动条高宽及与放大镜的相对位移定义
#define MAQN_SLIDER_WIDTH 160
#define MAQN_SLIDER_HEIGHT 32
#define MAQN_SLIDER_TOP_INTERVAL 12
#define MAQN_SLIDER_LEFT_INTERVAL 0
//放大器滑动条的滑动块与边框间距
#define MAQN_SLIDER_TOP_PADDING 2
#define MAQN_SLIDER_BOTTOM_PADDING 5
#define MAQN_SLIDER_LEFT_PADDING 6
#define MAQN_SLIDER_RIGHT_PADDING 6
//放大器关闭按钮长宽及与放大器间距定义
#define MAQN_BTN_WIDTH 32
#define MAQN_BTN_HEIGHT 32
#define MAQN_BTN_RIGHT_INTERVAL 0
#define MAQN_BTN_TOP_INTERVAL 12

//图标文件路径
#define WBICOCURVE_STYLE1  _T("icostyle1.png")
#define WBICOCURVE_STYLE2  _T("icostyle2.png")
#define WBICOCURVE_STYLE3  _T("icostyle3.png")
#define WBICOCURVE_STYLE4  _T("icostyle4.png")
#define WBICOCURVE_STYLE5  _T("icostyle5.png")
#define WBICOCURVE_STYLE6  _T("icostyle6.png")
#define WBICOCURVE_STYLE7  _T("icostyle7.png")

//对象类型
enum WB_OBJ_TYPE
{
	WB_OBJ_NORMAL = 0,
	WB_OBJ_LINE, //直线
	WB_OBJ_CURVE, //曲线
	WB_OBJ_POLYGON, //多边形 
	WB_OBJ_RECT,    //矩形
	WB_OBJ_SELECTRECT, //选择框
	WB_OBJ_PICTURE,  //图片
	WB_OBJ_MOVIE,	//视频
	WB_OBJ_MEDIA,   //新视频
	WB_OBJ_FLASH,	//FLASH
	WB_OBJ_DOCUMENT, //文档
	WB_OBJ_TEXT,		//文字
	WB_OBJ_ELLIPSE,	//椭圆
	WB_OBJ_TRIANGLE, //三角形
	WB_OBJ_RHOMBUS,	//菱形
	WB_OBJ_BAFFLE,  //挡板
	WB_OBJ_COMBINE, //组合
	WB_OBJ_MAQN,     //放大镜
	WB_OBJ_SPOTLIGHT, //聚光灯
	WB_OBJ_ANNOTATIONS,  //标注
	WB_OBJ_CAMERA,   //摄像头
	WB_OBJ_DUSTBIN,	//垃圾箱
	WB_OBJ_RULER,//直尺
	WB_OBJ_PROTRACTOR,//量角器
	WB_OBJ_ANGLE,    //生成刻度
	WB_OBJ_SETSQUARE,//三角板
	WB_OBJ_ARBITRARYTRIANGLE, //任意三角形
	WB_OBJ_PALLELOGAM,      //平行四边形
	WB_OBJ_PENTAGON,      //五边形
	WB_OBJ_HEXAGON,      //六边形
	WB_OBJ_FIVEPOINTSTART, //五角星
	WB_OBJ_TRAPEZIUM,     //梯形
	WB_OBJ_DXF, //DXF
	WB_OBJ_ICOCURVE,//图标曲线
	WB_OBJ_COMPASS,    //圆规
	WB_OBJ_ARC,        //圆弧
	WB_OBJ_SECTOR,     //扇形
	WB_OBJ_MAGICPEN, //魔术笔
	WB_OBJ_MIDI
};
enum WB_COMMON_ATTRIBUTE
{
	WB_NO_ATTRIBUTE = 100, //无属性
	WB_TEXT_ATTRIBUTE,//文本属性
	WB_ANNO_ATTRIBUTE,//备注属性
	WB_TRANS_ATTRIBUTE,//透明度属性
	WB_TRANS_COL,//透明度 颜色
	WB_TRANS_COL_WIDTH,//透明度 颜色 线宽
	WB_TRANS_COL_SOLID//透明度 颜色 填充
};
//图层类型
enum WB_LAYER_TYPE
{
	WB_LAYER_NORMAL = 0, //一般图层
	WB_LAYER_LABEL		 //标注图层
};

//点选中对象按钮区域标识
enum WB_OBJ_AREA
{
	WB_PS_NONE,	//没有选中
	WB_PS_OBJ,	//选中对象
	WB_PS_CTRLBUTTON,	//选中控制按钮，即播放器上的按钮
	WB_PS_SELECTBUTTON, //选中对象选择框上的按钮
	WB_PS_SELECTFRAME,	//选中对象选择框
	WB_PS_SELECTPOINT,   //选中标注的尖角的点
	WB_PS_INMEDIACTRL,    //在视频控制框内
	WB_PS_TRIAMGLEPOINT,    //在三角形中的点
	WB_PS_TRAPEZIUMPOINT    //选中梯形的点
};

//标注尖角的方向
enum WB_OBJ_DIRECTION
{
	WB_DRC_UP = 0, //尖角向上
	WB_DRC_DOWN,   //尖角向下
	WB_DRC_LEFT,   //尖角向左
	WB_DRC_RIGHT   //尖角向右
};

//鼠标与按钮的关系
enum WB_MOUSE_STATE
{
	MOUSE_NORMAL = 0,//按钮初始状态
	MOUSE_HOVER,	 //鼠标滑过按钮时
	MOUSE_LEAVE,	 //鼠标离开按钮时
	MOUSE_LCLICK,	 //鼠标左键单击时
	MOUSE_LBTNDOWN,	 //鼠标左键按下时
	MOUSE_LBTNUP	 //鼠标左键弹起时
};
//单位转换
// enum UNIT_TRANSLATE
// {
// 	UNIT_PIEXL = 0,//三角尺的像素
// 	UNIT_CM,    //厘米单位
// 	UNIT_MM,    //毫米
// 	UNIT_INCH   // 英寸单位
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

typedef struct _WBFileHeadItem //白板文件头
{
	char szMaker[12];
	char szVersion[4];
	char szDate[10];
	char szTime[8];
}WBFileHeadItem, *PWBFileHeadItem;

typedef struct _WBLineItem    //直线
{
	CPoint ptBegin;    //起点
	CPoint ptEnd;		//终点
	int nWidth;			//线宽
	int nLineType;      //线型
	COLORREF clrLine;	//颜色
	//BOOL bHaveArrow;	//是否带箭头

	LineCap nStartLineCap; //起点线帽
	LineCap nEndLineCap; //终点线帽
}WBLineItem, *PWBLineItem;

//////////////////////////////////////////////////////////////////////////

enum CURVEPOINT_TYPE  //曲线中点的类型
{
	CURVE_TYPE_NORMAL = 0,			//普通电
	CURVE_TYPE_BREAKPOINT_BEGIN,	//断点（起点）
	CURVE_TYPE_BREAKPOINT_END		//断点（终点）
};

//typedef struct _WBCurvePointItem   //任意曲线点属性
//{
//	CPoint pt;					//点位置
//	//CURVEPOINT_TYPE nType;		//点类型
//	BOOL bIsErase;				//点是否被擦除
//	int nWidth;					//线宽
//}WBCurvePointItem, *PWBCurvePointItem;

typedef struct _WBCurvePointAppendItem{
	_WBCurvePointAppendItem(int _nWidth = 0, bool _bErase = FALSE)
		: nWidth(_nWidth), bErase(_bErase){}
	BOOL bErase;//点是否被擦除
	int nWidth;//线宽,如果只是作为保留项，可不用初始化数据
}WBCurvAppendItem, PWBCurvAppendItem;

typedef struct _WBCurveItem //任意曲线
{
	//两个相同长度的指针数组（保留除点外的数据主要是为了留着备用，
	//没有用一个结构体主要是为了便于画图)
public:
	_WBCurveItem();
	_WBCurveItem(struct _WBCurveItem *pItem);//拷贝构造
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
	WBCurvAppendItem *pCurveAppendItem;//在pCurvePointItem点数组中标记擦除的点

	int nWidth;			//线宽
	COLORREF clrLine;	//颜色
	int nLineType;      //线型
	LineCap nStartLineCap; //起点线帽
	LineCap nEndLineCap; //终点线帽
}WBCurveItem, *PWBCurveItem;

//////////////////////////////////////////////////////////////////////////
//jiangchao 2013.4.19
typedef struct _WBIcoCurvePointAppendItem{
	_WBIcoCurvePointAppendItem(int _nIcoScaleWidth = 24, bool _bErase = FALSE)
		: nIcoScaleWidth(_nIcoScaleWidth), bErase(_bErase){}
	bool bErase; //点是否被擦除
	int nIcoScaleWidth; //图标的缩大缩小比例宽度，如果只是作为保留项，可不用初始化数据
}WBIcoCurvAppendItem, *PWBIcoCurvAppendItem;

//新增struct _WBIcoCurveItem代码
typedef struct _WBIcoCurveItem //: public _WBCurveItem //任意图标曲线
{
public:
	_WBIcoCurveItem();
	_WBIcoCurveItem(struct _WBIcoCurveItem *pItem);//拷贝构造
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
	    if (m_nScaleWdith == 1) ++m_nScaleWdith;}//设置图标比例宽度
	void SetIcoFilePath(CString pIcoFilePath){ 
		m_IcoFilePath = pIcoFilePath;//设置ico文件path
	}
	Point GetPtData(unsigned int nIndex);
	int GetPtWidth(unsigned int nIndex);
	bool IsPointErased(unsigned int nIndex);

	XCritSec IcoCurveItemLock;

private:
	Point * pCurvePointItem; //保存点的数组
	unsigned int nCount; //保存点的个数
	WBIcoCurvAppendItem * pCurveAppendItem; //在pCurvePointItem点数组中标记擦除的点

	CString m_IcoFilePath; //图标文件路径
	int m_nScaleWdith; //图标比例宽

}WBIcoCurveItem, *PWBIcoCurveItem;

//////////////////////////////////////////////////////////////////////////
typedef struct _WBPageItem    //页面
{
	int nWidth;               //页面宽度
	int nHeight;              //页面高度
}WBPageItem, *PWBPageItem;

typedef struct _WBBafflePlate  //挡板
{
	CPoint ptLeftTop;          //挡板在页面中的左上角点
	CPoint ptRightBottom;      //挡板在页面中的右下角点
	COLORREF clrBackground;    //挡板背景颜色(暂时没有设置)
}WBBafflePlateItem, *PWBBafflePlateItem;

typedef struct _WBMagnifier    //放大镜
{
	CRect rect; //放大镜矩形框的坐标
	int nZoom;  //放大倍数
}WBMagnifierItem,*PWBMagnifierItem;

typedef struct _WBProtractor     //量角器
{
	CPoint        ptProtractorLT;      //量角器左上点 
	CPoint        ptProtractorRB;      //量角器右下点
	CPoint        ptProtractorMP;      //量角器测量点
	CPoint        ptProtractorCT;      //量角器基点
}WBProtractorItem,*PWBProtractorItem; 

typedef struct _WBAngle    //生成刻度
{
	CPoint        ptAngleLT;     //刻度类左上点
	CPoint        ptAngleRB;     //刻度类右下点
	CPoint        ptAngleCT;     //刻度类基点
}WBAngleItem,*PWBAngleItem; 

typedef struct _WBSpotLight   //聚光灯
{
	CRect rect;//聚光椭圆的坐标
}WBSpotLightItem,*PWBSpotLightItem;

typedef struct _WBPolygonItem //多边形
{
	std::vector<CPoint *> vecPoints; //点列表
	int nWidth;			//线宽
	COLORREF clrLine;	//线颜色
	BOOL bIsFillSolid;		//是否填充
	COLORREF clrFillSolid;	//填充颜色
}WBPolygonItem, *PWBPolygonItem;

typedef struct _WBSelectRectangle  //选择框
{
	CPoint ptLeftTop;          //选择框左上角的点
	CPoint ptRightTop;         //选择框右上角的点
	CPoint ptRightBottom;      //选择框右下角的点
	CPoint ptLeftBottom;       //选择框左下角的点
}WBSelectRectangleItem, *PWBSelectRectangleItem;

typedef struct _WBPictureItem //图片
{
	CString strFilePath; //文件路径
	CRect rect;  //位置	
	//int nAngle;  //角度
}WBPictureItem, *PWBPictureItem;

typedef struct _WBFlashItem //图片
{
	CString strFilePath; //文件路径
	CRect rect;  //位置
	unsigned long curtime;//当前播放时间
	//int nAngle;  //角度
}WBFlashItem, *PWBFlashItem;

typedef struct _WBDocumentItem //图片
{
	CString strFilePath; //文件路径
	CRect rect;  //位置
	//DOCUMENT_TYPE nType; //文档类型
}WBDocumentItem, *PWBDocumentItem;

enum DOCUMENT_TYPE  //文档类型
{
	DOCUMENT_TYPE_DOC = 0,			
	DOCUMENT_TYPE_PDF,	
	DOCUMENT_TYPE_XLS,		
	DOCUMENT_TYPE_PPT
};

typedef struct _WBMovieItem //视频
{
	CString strFilePath; //文件路径
	CRect rect;  //位置
	LONGLONG llCurTime;	//保存视频当前播放时间
}WBMovieItem, *PWBMovieItem;

typedef struct _WBMediaItem //新视频
{
	CString strFilePath; //文件路径
	CRect rect;  //位置
	ECtrlState showState; //是否显示控制对话框
	EMediaPlayState playState;
	EVoiceState voiceState;
	long llCurTime;           //当前播放时间
	BOOL bIsMediaLoop;        //是否循环播放
	std::string UCCUerID;
}WBMediaItem, *PWBMediaItem;

typedef struct _WBCameraItem //摄像头数据
{
	CRect rect;  //位置
	//接收方的所需要的连接数据对象
	SCameraReceiveData dataReceiver;
	//网络连接服务器的数据对象
	SCameraServerData dataServer;
	//本地打开的数据对象
	CameraSet dataLocalCamera;
}WBCameraItem, *PWBCameraItem;

typedef struct _WBTextItem //文本     
{
	CString strText; //文本内容 
	LOGFONT  fontItem; //字体	
	CPoint ptTopLeft;	//记录字体输出左上角的点
	COLORREF clrText;  //文本颜色
	CRect rectText; //文本显示的矩形，通过DrawText函数计算
}WBTextItem, *PWBTextItem;

typedef struct _WBEllipseItem //椭圆
{
	CPoint ptTopLeft;	//用于定位椭圆的左上角的点
	CPoint ptBottomRight;	//定位椭圆的右下角的点	
	int nLineType;		//线的类型
	int nWidth;			//线宽
	COLORREF clrLine;	//线颜色
	BOOL bIsFillSolid;		//是否填充
	COLORREF clrFillSolid;	//填充颜色
}WBEllipseItem, *PWBEllipseItem;

typedef struct _WBSetSquare    //三角板
{
	CPoint        ptSetSquareLT;      //三角板左上点 
	CPoint        ptSetSquareInLT;      //三角板左上内点
	CPoint        ptSetSquareInRB;      //三角板右下内点
	CPoint        ptSetSquareRB;      //三角板右下点
	//CPoint        ptSetSquareInCT;      //三角内中直角点
	CPoint        ptSetSquareCT;      //三角板中心点
}WBSetSquareItem,*PWBSetSquareItem; 

typedef struct _WBCompass     //圆规
{
	double  dRadian;             //支腿与半径夹角弧度
	double  dAngle;				 //支腿与半径夹角角度
	double  dRadius;			 //半径
	double  dLegLength;			 //支腿长

	CPoint  ptCompassCT;		 //旋转中心点
	CPoint  ptCompassMid;        //圆规中间点
	//CPoint  ptCompassMidUp;      //圆规中间点Up点
	CPoint  ptCompassMidLT;   
	CPoint  ptCompassMidRT;
	CPoint  ptCompassLT;		 //左上点
	CPoint  ptCompassLTUp;       //左上Up点
	CPoint  ptCompassLB;         //左下点
	CPoint  ptCompassLBUp;       //左下点Up点
	CPoint  ptCompassRB;		 //右下点
	CPoint  ptCompassRBUp;		 //右下点Up点

	CPoint  ptCompassHPLB;		 //右下1号点-延伸固定画笔左下点
	CPoint  ptCompassHPLT;		 //右下1号点-延伸固定画笔的左上点
	CPoint  ptCompassHPRT;		//右下1号点-延伸固定画笔右上点
	CPoint  ptCompassHPRB;		//右下1号点-延伸固定画笔右下点
	
	CPoint  ptCompassPenLT;    //画笔的左上点
	CPoint  ptCompassPenLB;		//画笔的左下点
	CPoint  ptCompassPenRT;		//画笔的右上点
	CPoint  ptCompassPenRB;		//画笔的右下点

	CPoint  ptCompassRefillLT;  //笔芯的左上点
	CPoint  ptCompassRefillRT;  //笔芯的右上点

}WBCompassItem, *PWBCompassItem;

typedef struct _WBArc  //圆弧
{
	double dArcStartAngle; //起始角度
	double dArcSweepAngle;  //圆弧对应扫过的角度
	CPoint ptArcCT;     //圆弧中心点
	double dRadius;    //半径
	RectF rectf;  //圆弧的外切矩形
	CPoint ptSelectLT; //圆弧选择框的LT
	CPoint ptSelectRB;   //圆弧选择框的RB
	CPoint ptDrawStart;  //圆弧起点
	CPoint ptDrawEnd;    //圆弧终点
	int nWidth;			//线宽
	COLORREF clrLine;	//线颜色
	int nLineType;      //线型
	LineCap nStartLineCap; //起点线帽
	LineCap nEndLineCap; //终点线帽

}WBArcItem, *PWBArcItem;

typedef struct _WBSector   //扇形
{
	double dSectorStartAngle; //扇形起始角度
	double dSectorSweepAngle; //扇形对应的角度
	CPoint ptSectorCT;        //扇形对应中心点
	CPoint ptDrawStart; //扇形起点
	CPoint ptDrawEnd;   //扇形终点
	CPoint ptSelectLT;     //扇形选择框的LT
	CPoint ptSelectRB;      //扇形选择框的RB
	RectF rectf;  //扇形外切矩形
	double dRadius;    //半径
	int nWidth;			//线宽
	int nLineType;      //线形
	COLORREF clrLine;	//线颜色
	BOOL bIsFillSolid;		//是否填充
	COLORREF clrFillSolid;	//填充颜色

}WBSectorItem, *PWBSectorItem;
//////////////////////////////////////////////////////////////////////////
typedef std::map<std::string,std::string> MapFilePath;			 //huanglongquan add
typedef MapFilePath::iterator IT_MapFilePath;

class CWBObject;
//jiabangping
typedef CTypedPtrList<CObList, CWBObject*>  CWBList;
//线宽、颜色、线型、图标文件路径
extern CString g_nWBCurObjIcoFilePath;//曲线图标文件路径
extern int g_nWBCurObjWidth; //线宽
extern COLORREF g_clrWBCurObjColor;//颜色
extern COLORREF g_clrWBPageColor;//背景颜色
extern int g_nWBLineType;//线型
extern BOOL g_bIsSolid; //是否填充
extern COLORREF g_clrSolid; //填充颜色
extern LineCap g_nStartLineCap; //起点线帽
extern LineCap g_nEndLineCap; //终点线帽
extern CWBObject *g_pObj;//选中列表中只有一个时的对象
//extern BOOL g_IsSelectObj;//是否有选中的对象
extern float g_nTranspar;//所选对象透明度
extern BOOL g_bDrawSelectButton; //画选择框按钮标识
extern CString g_nWBCurObjFontFaceName;//字体名称
extern int g_nWBCurObjFontSize;//字体大小
extern BOOL g_isBold;//是否粗体
extern BOOL g_isItalic;//是否斜体
extern BOOL g_isUnderline;//是否下划线
extern BOOL g_bIsVertical;//是否竖排
extern CString g_cTextstr;//文本内容
extern BOOL g_bIsMTRotate; //文档或视频截图手势触摸时是否进行旋转操作（TRUE则旋转，FLASE则缩放）
extern BOOL g_bAttriDlginitialize;//属性面板是否初始化(上面所有控件皆显示为初始化)
extern CString g_strSelectedResPath;//资源的路径
extern RES_INPUT_METHOD g_nMethodOfResToView;//资源列表中的资源入view中的方式
//extern BOOL g_isFunctionDlgHide;//控制面板是否隐藏
extern int g_nWBTriangleAngle; //等腰三角形的内角度数
extern CPoint g_ptAddPoint;  //用于存储不同分辨率聚光灯的偏移量
extern BOOL g_bIsWBLoadIcoFromFile;//曲线图标是否从文件加载

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
	virtual ~CWBFile();//上锁

	BOOL Save(CString strFilePath); //上锁
	BOOL Load(CString strFilePath);//上锁
	BOOL FillinFileHead(char *pBuf, DWORD& dwLen);
	BOOL FillOutFileHead(char *pBuf, ULONGLONG& dwLen);
	BOOL FillOutFileHead(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取


	void ClearPageList();  //清除页列表，在析构中调用且其中已经上锁
	void  ReductionList(); // 还原页当中的录像变量//上锁

	CWBPage * GetCurPage() const;  //得到当前页
	CWBPage * GetPage(UINT nPageIndex) const; //得到某一页
	UINT GetCurPageIndex() const; //得到当前页码
	UINT GetPageCount() const;  //得到总页数
	int DeleteLastContinuousEmptyPages();	//删除文件中从后往前数的所有连续的空白页

	//页操作
	CWBPage* AddPage(CWnd *pParentWnd = NULL); //添加新页//上锁
	void AddPage(CWBPage* pAdd);			//添加新页//上锁
	BOOL PrePage(); //上一页
	BOOL NextPage(); //下一页 //上锁
	BOOL FirstPage(); //第一页
	BOOL LastPage();  //最后页 //上锁
	BOOL GotoPage(UINT nPage); //上锁
	UINT InsertPage(CWnd *pParentWnd = NULL); //插入一页 //上锁
	UINT InsertPage(UINT nPageIndex, CWnd *pParentWnd = NULL); //插入一页 //上锁
	UINT DeletePage(CWnd *pParentWnd = NULL);//删除一页 //上锁
	UINT DeletePage(UINT nPageIndex, CWnd *pParentWnd = NULL);//删除一页 //上锁
	BOOL InterchangePage(UINT nFormerPage, UINT nTargetPage);//互换页 //上锁
	BOOL MovePage(UINT nSrcPageIndex, UINT nDesPageIndex); //移动页 //上锁

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

	CString m_strTempDirector; //临时文件夹路径
	MapFilePath m_mapFilePath;
	DWORD m_nMaxRamdonPageID;

	XCritSec			m_csWBFilePageLock;//对白版文件中的页操作上锁
};

class CWBPage : public CObject //页类，存储对象列表
{
public:
	CWBPage(CWnd *pParentWnd = NULL);
	virtual ~CWBPage();//上锁

	void SetParent(CWnd* pParent);	//设置页的父窗口

	//保存（读取）到（从）缓冲区
	BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	BOOL Load(char *pBuf, ULONGLONG& lLen);
	BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const; //保存到文件//上锁
	BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取

	void Draw(CDC *pDC) /*const*/; //划（显示）对象//上锁
	void DrawLabel(CDC *pDC); //画标注对象
	void ClearObjList();  //清除对象列表，在析构中使用且在其中上锁
	void ClearLabelObjList(); //清除标注图层对象列表
    void DrawAsBakgroundCanves(CDC *pDC);//平铺背景图片
	void CaclBigPic(CString *str);//拼一张大图
	void SetPageDC(CDC *pDC);//在m_dcColor里面用颜色填充一张临时位图
	void SetBKGColor(COLORREF color);//设置背景颜色
	//会议录像
	void	SetbStartConferenceRecord(bool bStartConferenceRecord);
	bool GetbStartConferenceRecord();
	BOOL SendAllPageDataToConferenceRecord();

	//处理视频摄像头回调。开启
	void StartMediaOrCaremaForceEncode(CWBObject *pobj);

	CDC* GetPageDC();
	/////////////////////////////////
	void SetPageID(UINT nPageID);
	UINT GetPageID();

	//operations	
	//对象操作
	DWORD GetSelectObject(CPoint pt) const; //根据点返回被选中的对象id//上锁

	//添加对象
	DWORD AddWBObject(CWBObject *pObj, int nLayerType = WB_LAYER_NORMAL); //上锁
	BOOL AddComObject(DWORD dwLayerIndex, CWBObject *pComObj); // 向组合里添加直线 //上锁
	BOOL AddObjIntoCom(CWBObject *pObj); //上锁

	//根据对象id和对象类型查找对象
	//denghong
	CWBObject *FindByLayerIndex(DWORD dwLayerIndex, CObList *pList) const;
	CWBObject *FindByLayerIndex(DWORD dwLayerIndex, int nObjType = WB_OBJ_NORMAL) const; //根据图层号得到对象
	CWBObject *FindComObjByLayerIndex(DWORD dwLayerIndex) const; //根据图层号得到组合链表里的对象
	CWBObject *FindByID(DWORD dwID, int nObjType = WB_OBJ_NORMAL) const; //根据触摸屏ID得到对象//上锁
	DWORD GetAllObjLayerIndex(std::vector<DWORD *>& vecLayerIndex, int nObjType = WB_OBJ_NORMAL) const; //得到页所有对象的图层号//上锁
	DWORD GetLastPageAllObjLayerIndex(std::vector<DWORD *>& vecLayerIndex, int nObjType = WB_OBJ_NORMAL) const; //得到最后页所有对象的图层号//上锁
	BOOL DeleteEraseListObject(DWORD dwLayerIndex);//根据图层号删除擦除链表中在擦除过程中被删除的对象

	BOOL DelWBObj(DWORD dwLayerIndex); //删除对象指针//上锁
	BOOL DeleteWBObject(DWORD dwLayerIndex); //根据图层号删除对象，在m_pWBObjList中移除同时删除内存中的对象//上锁
	DWORD DeleteSelectedWBObjects(DWORD** delObjArray); //删除当前所选择的对象,本质上是调用DeleteWBObject(DWORD dwLayerIndex)
	BOOL DeleteAllObjects(unsigned long eventId = 0); //删除全部对象，在m_pWBObjList中移除同时删除内存中的对象，在m_pWBObjList中移除将对象指针放入m_pWBDeleteObjList//上锁
	DWORD DeleteWBObjects(const CRect& rectDelete); //根据橡皮擦删除所有对象(清除)，在m_pWBObjList中移除将对象指针放入m_pWBDeleteObjList //上锁
	DWORD DeleteWBObjects(const CRgn *pRgn); //删除区域内的所有对象 ，在m_pWBObjList中移除同时删除内存中的对象//上锁
	BOOL DeleteListForSetquare();       // 删除选中三角板 以前选中的的对象的选择框
	//denghong
	DWORD DeleteWBObjects(int nObjType, DWORD dwLayerIndex = 0); //上锁
	/******************************
	*chenchangtao
	*当在白板对象上移除视频或者是摄像头对象时传入对象指针
	*根据指针移除视频摄像头对象列表中的某一对象
	*******************************/
	void RemoveMediaCameraObject(const CWBObject* pWBObj);
	//选择框操作////////////////////////////////////////////////////////////////////////
	void ClearSelectFrameList(); //清除选择框列表
	BOOL AddSelectFrame(DWORD dwLayerIndex, BOOL bAddSelect = TRUE); //添加（删除）选择框
	BOOL SelectMoreObjAttribute();//选择多对象属性
	DWORD AddSelectFrames(CRect& rect); //根据矩形添加选择框 //上锁
	BOOL DeleteSelectFrame(DWORD dwLayerIndex); //根据对象图层号删除选择框
	DWORD AddAllSelectFrames(); //选择全部对象

	int PointInSelectFrame(const CPoint& pt); //判断点是否在某个选中对象的选择框或按钮内//上锁
	int PointInText(const CPoint& pt); //判断点是否在文本内//上锁
	//视频删除撤销位置wangxiaobin
	std::map<DWORD ,CRect> * GetMediaRectMap();
	std::map<DWORD ,CRect> * GetCaremaRectMap();

	std::vector<DWORD> GetWBSelectFrameList();
	bool IsCombinInSelectObj();
	CObList *GetWBObjList();
	CObList *GetWBLabelObjList();
	void SetCurObjLayerIndex(DWORD dwLayerIndex);
	CObList*GetMediaCameraList();

	CObList *GetWBDeleteObjList();
	//在使用删除对象或曲线删除的时候调用DeleteWBObjects将对象指针放在m_pWBDeleteObjList中，并在列表中移除但并没有删除内存中对象，
	//当鼠标或触摸弹起的时候调用此函数将m_pWBDeleteObjList中对象删除,
	void DeleteWBDeleteObjList();

	CObList *GetEraseCurveList();
	void DeleteEraseCurveList();
	BOOL IsCurveInEraseCurveList(DWORD dwLayerIndex);

	CObList *GetEraseDocList();
	void DeleteEraseDocList();
	BOOL IsCurveInEraseDocList(DWORD dwLayerIndex);

	void SortWBList(std::vector<DWORD> vecLayerIndex, CObList *pList); //上锁
	BOOL SetWBList(CObList *pList); //上锁

	int GetSelectLockCount(); //得到选择框列表中不能组合的对象（锁定，标注，新视频，摄像头，背景，挡板，文档）的个数

	//根据偏移量移动当前选择的对象
	void MoveSelectedObj(const CPoint& ptOffset); 

	//图层操作
	void ResetObjLayerIndex(); //根据对象顺序重新生成所有对象的图层号 //上锁
	BOOL SetObjUnderLayer(DWORD dwLayerIndex); //将对象移到下一层 //上锁
	BOOL SetObjSuperLayer(DWORD dwLayerIndex); //将对象移到上一层
	BOOL SetObjBottomLayer(DWORD dwLayerIndex); //将对象移到最底层 //上锁
	BOOL SetObjTopLayer(DWORD dwLayerIndex); //将对象移到最上层 //上锁

	/************************重新处理添加图层操作 by huanglongquan*************************************/
	BOOL ChangeOBJLayer(DWORD dwObjid,int nLayerNum);//图层交换 //上锁
	CWBObject* FindObjLayerByNum(int nLayerNum, CObList *pList); //通过查找图层号码返回对象指针 //上锁
	CWBObject* FindRecentlyBaffleObj(CObList *pList)const;
	CWBObject* Findprotractor(CObList *pList)const;
	int GetCurrentPageMaxLayer();//上锁
	int GetCurrentPageMinLayer();//上锁

	/**************************************************************/
	//擦除
	void EraseObjects(const CRect& rectErase); //上锁

	void EraseOccasionalObjects(const CRect& rectErase);//临时擦除直线，曲线 //上锁

	//得到背景对象
	CWBObject *GetBkgObj(); //上锁

	//停止或播放视频及FLASH
	void StartOrPauseMovie(unsigned long ulsend = 0);
	void StartOrPauseMovieCur(bool bl);

	//得到当前正在编辑的对象ID号
	DWORD GetCurEditingObjLayerIndex();

	//设置当前正在便捷的对象ID号
	void SetCurEditingObjLayerIndex(DWORD dwCurEditingObjLayerIndex);

	//得到页的内存DC
	HDC GetMemHDC() const;	

	//得到页标注图层内存CDC
	HDC GetLabelMemDC() const;

	//页中滚动条的值5-11
	void SetPageViewScroll(POINT viewScrollPos);
	POINT GetPageViewScroll();	
	void SetComLayerIndex(CWBObject *pObj); //设置组合成员的图层号
	//获取选中对象的个数
	UINT GetSelectObjNumber();
	//得到选中对象列表
	CWBList *GetSelectObjLst();

	//增加m_nWBMaxLayerId的值
	void SetMaxLayerIdIncrease(int &nWBMaxLayerId);

	//设置页的唯一标识
	void SetPageRamdonId(DWORD nPageRamdonId);
	DWORD GetPageRamdonId();
	void ReleaseLimitObjs();//huanglongquan add 处理取消对象操作限制
protected:
	//UINT CreateID();//生成新对象ID号

	//擦除直线
	void EraseLine(CWBLine *pLine, const CRect& rectErase); 

	void OccasionalEraseLine(CWBLine *pLine, const CRect& rectErase);//临时擦除直线

	DWORD GenerateObjID(); //产生对象ID 
    XCritSec			m_csWBObjListLock;//对page类中对象列表m_pWBObjList上锁	

	DWORD          m_PreLayerObjIndex;
public:
	std::vector<CWBObject *> m_vecFailDoc; //转换失败的文档
	std::vector<CWBObject *> m_pVecCombineGif; //存储组合对象中的图片类型对象
	std::vector<DWORD> m_vecLineObjID;//直线对象ID
	std::vector<DWORD> GetvecLineObjID();
	void DeleteLine();//大小擦除过程中直线生成对象删除，最后对象由主动方得到
	HBITMAP GetCurPageIndexBitmap();
	BOOL m_bIsDelCombine;//解决跟撤销冲突，要使用的一个判断
	BOOL m_bIsDelComCurve;//解决融合对象
	//设置是否有背景图
	void SetIsExistBackGround(BOOL bl){ m_bIsSetBackGround = bl;}
	BOOL GetIsExistBackGround() {return m_bIsSetBackGround;}
	void ChangeMagicAlpha();
	int GetSelectOneObjectLayer();
	CObList* GetWBList(){return m_pWBObjList;}
private:
	CObList *m_pMediaCameraList;//视频摄像头对象列表，用于管理翻页等动作时媒体窗口的显示或隐藏
	int m_nWBMaxLayerId; //当前白板最大的图层号id（就是链表中最后面的元素的图层号）huanglongquan ADD by2012年4月18日
	int m_nWBMinLayerId; //当前白板最小的图层号id
	int m_nWBLabelLayerMinId;//标注最小图层
	int m_nWBLabelLayerMaxId;//标注最大图层
	CObList *m_pWBObjList; //对象列表
	CObList *m_pWBLabelObjList; //标注图层对象列表
	
	DWORD m_dwCurObjLayerIndex; //当前最新对象的层数	

	//视频删除撤销位置wangxiaobin
	std::map<DWORD ,CRect> *m_pMediaRect;
	std::map<DWORD ,CRect> *m_pCaremaRect;

	DWORD m_dwCurEditingObjLayerIndex; //当前正在编辑的对象图层号

	POINT m_viewScrollPos; //每页view的滚动条位置

	CDC m_dcMem;	//每个页都有自己的内存DC
	CDC m_dcTempPic;//暂时存储大图片
	CDC m_dcLabelMem; //标注图层的内存DC
	
	CBitmap m_bmpBuf;

	HBITMAP m_hbitmap;//为接口中传出去一张位图给保存具体页的位图

	CBitmap m_bmpBufLabel;	

	CWnd *m_pParentWnd;

	std::vector<DWORD> m_vecSelectFrameList;	//选择框列表，只记录被选中对象的图层号，即m_dwLayerIndex
	//jiabangping
	CWBList m_lstSelectObject;
	//denghong
	CObList *m_pWBDeleteObjList;

	//denghong
	CObList* m_pEraseCurveList;
	//luowei
	CObList* m_pEraseDocList;
	bool		m_bStartConferenceRecord;//判断该页的所有对象是否已经发送给会议

	DWORD m_nPageRamdonId;
	COLORREF m_BKGColor;//背景颜色值


	public:
	 long m_lOnTimer;
	 //std::map<DWORD, int>* m_MapGifID;
	
	BOOL m_bIsSetBackGround;//判断是否有背景图
	BOOL m_bIsMoreSelect; //是否框选

	std::vector<CWBObject *> m_pMagicList; //魔术线条列表
};

class CWBObject : public CObject, public CWBMTObject //数据类基类
{
public:
	CWBObject();
	virtual ~CWBObject();

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);
	virtual void DrawAsBakground(CDC *pDC, CWnd *pParentWnd); //作为背景画

	///继承自AVCON_SDK_MTTouch::CMTObject

	/*---------------------------------------------------------------------------------------------
	参数一：低字节表示手势的大类[ETouchGestureType]，高字节表示手势的小类[如动态手势中的]
	参数二: 低字节表示x的坐标，高字节表示y方向的坐标. 单位为像素；坐标为屏幕坐标
	----------------------------------------------------------------------------------------------*/
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage = 0);
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	//作用于当前对象的最后一个手指弹起，执行同步功能
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te);
	//触摸过程中是否需要执行同步
	bool  NeedTouchSynchronize();

	
	//判断点是否在对象的选择范围内
	void ReservePoint(const CPoint& ptOriginal, CPoint& ptReserve) const; //根据对象旋转映选择射点坐标
	void ReserveObjPoint(const CPoint& ptOriginal, CPoint& ptReserve) const; //根据对象旋转与缩放计算对象点的新坐标
	virtual int IsInSelectArea(const POINT &pt)const;
	virtual int IsInSelectArea(CPoint& pt)const; //点选择
	virtual BOOL IsInSelectArea(const CRect& rect) const; //区域选择

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交

	//创建选择框
	virtual void CreateSelecrtFrame();

	//修正对象坐标点
	virtual void AdjustObject();

	//创建对象区域
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //根据对象旋转与缩放后创建区域

	//设置对象绕中心点旋转的角度
	virtual void SetRotarionAngle(double dbAngle);
	virtual double GetRotationAngle() const;
	void GetRotationCenter(CPoint& ptRotationCenter) const;
	void SetRotationCenter(const CPoint& ptRotationCenter);

	//传画中心点的值
	void GetDrawRotationCenter(CPoint& ptRotationCenter) const;
	void SetDrawRotationCenter(const CPoint& ptDrawRotationCenter);
	//获取上次的旋转中心点
	void GetPreRotationCenter(CPoint& ptPreRotationCenter){ptPreRotationCenter = m_ptPreRotationCenter;}

	//获得缩放属性
	double GetXZoomScale() const;
	double GetYZoomScale() const;
	void GetZoomCenter(CPoint& ptZoomCenter) const;
	void SetXZoomScale(const double dbXZoomScale);
	void SetYZoomScale(const double dbYZoomScale);
	void SetZoomCenter(const CPoint& ptZoomCenter);

	//设置对象选择框旋转角度 wangxin
	void SetRectSelectAngle(double dbAngle);
	void SetRectSelectRotationCenter(const CPoint& ptRotationCenter);

	//对象id操作
	void SetLayerIndex(DWORD dwLayerIndex);
	DWORD GetLayerIndex() const;

	//图层操作
	void SetPageLayerIndex(int nPageLayerIndex);
	int	GetPageLayerIndex()const;

	//对象类型操作
	void SetType(int nType);
	int GetType() const;

	//对象图层类型操作
	void SetLayerType(int nLayerType);
	int GetLayerType() const;

	//对象透明度操作
	virtual void SetAlpha(int alpha);
	int GetAlpha() const;
	
	//组合里的对象透明度操作
	void SetAlphaInCom(int AlphaInCom);
	int GetAlphaInCom() const;

      //是否重画选择框
      void SetIsCreat(BOOL IsCreat);
	  BOOL GetIsCreat() const;

	virtual void SetPreAlpha(int prealpha);
	int GetPreAlpha() const;


	//偏移量移动对象
	virtual void MoveOffset(const CPoint& ptOffset); 

	//触摸屏ID
	void SetTouchID(DWORD dwID);
	DWORD GetTouchID() const;

	//得到对象选择框
	CWBSelectRectangle * GetObjSelectFrame() const;
	//设置对象选择框
	void SetObjSelectFrame(CWBSelectRectangle *pRectSelect);

	//求对象的中心点
	virtual void GetCenterPoint(CPoint& ptCenter);

	//判断是否为文档的触摸移动
	void SetInTouchMove();
	bool GetInTouchMove();

	//线型转换
	DashStyle GetDashStyle(int nLineStyle) const;

	//以角度偏移旋转对象
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//对象绕某一点旋转
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转

	//对象缩放
	//virtual void ZoomByLeftTop(const CPoint& ptOffset); //以左上点为中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象

	//静态通用函数////////////////////////////////////////////////////////////////////////
	//判断点是否在直线上
	static BOOL PointInLine(const CPoint& pt, const CPoint& ptLineA, const CPoint& ptLineB, int nWidth);
	//点缩放后的坐标
	static void ZoomPoint(CPoint& point, const CPoint& ptZoomCenter, double dbXScale, double dbYScale, int nIndex, BOOL bIsToZero);
	static BOOL ZoomRect(CRect& rect, const CPoint& ptZoomCenter, double dbXScale, double dbYScale, BOOL bIsToZero);

	int RotateXForm(double dbAngle); //旋转角度
	void ResumeXForm(int nGraphicsMode); //恢复角度

	int ZoomXForm(double dbXScale, double dbYScale); //缩放
	//void ResumeZoom(int nGraphicsMode); //恢复缩放

	//画中心点的值
	CPoint m_ptDrawRotationCenter;


	//擦除
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);

	//设置对象锁定
	void Lock(bool bIsLock = true);
	//获得对象锁定状态
	BOOL IsLock() const;

	//限制对象操作  by huanglongquan	2012年12月13日
	void Limit(bool bIsLimit = true);
	//获得对象限制状态
	bool IsLimit() const;

	//设置对象背景属性
	void SetBkg(BOOL bIsBkg = FALSE);
	//获得对象背景属性
	BOOL IsBkg() const;

	virtual BOOL IsSolid() const; //判断对象是否填充

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	//对象是否被缩放过
	BOOL GetIsZoom();
	virtual void SetIsZoom(BOOL bIsZoom);

	//任意中心点是否移动
    virtual void SetIsCenterpoint(bool bIsCenterpoint);

    //对象是否为文档
    bool GetIsDocument();
    virtual void SetIsDocument(bool bIsDocument);

	bool GetIsTouchDocumnet();
	virtual void SetIsTouchDocument(bool bIsTouchDoc);

	//对象缩放属性
	BOOL IsEqualScaleZoom() const;
	void SetEqualScaleZoom(BOOL bEqualScaleZoom = TRUE);
	double GetEqualScale() const;

	//隐藏中心点
	BOOL IsRorationCenterZoom() const;
	void SetRorationCenterZoom(BOOL bRorationCenterZoom = TRUE);

	//得到对象的映射中心点
	void GetMapCenter(CPoint& ptMapCenter) const;
	void SetMapCenter();

	//设置选择框是否显示
	virtual void SetSelectFrameShow(BOOL bShow);
	//获得选择框显示状态
	BOOL GetSelectFrameShow();

	//初始化图片按钮
	void InitImageButton();

	//得到图片按钮
	CWBImageButton *GetImageButton();
	void DelImageButton();

	void CopyAttribute(CWBObject *pObj); //拷贝对象的某些属性

	//得到控制按钮
	CWBControlButton *GetControlButton();

	//无限克隆属性
	BOOL IsInfiniteClone() const;
	void SetInfiniteClone(BOOL bInfiniteClone = FALSE);

	//克隆属性
	BOOL IsClone() const;
	void SetClone(BOOL bIsClone = FALSE);

	//设置对象的所有者，即创建者
	void SetOwner(unsigned long ulOwner);

	//是否是组合成员
	BOOL IsCombineMember() const;
	void SetCombineMember(BOOL bCombineMember = FALSE);
	void SetRectSelectCombineMember(BOOL bCombineMember = FALSE);

	//设置对象属于的组合的图层号
	void SetComLayerIndex(DWORD dwComLayerIndex);
	DWORD GetComLayerIndex();

	//设置对象的单位
	virtual int SetUnit(const int);
	CWBSelectRectangle *m_pRectSelect; //对象选择框
	//判断是否为文档内的对象
    bool GetIsDoInside();
    virtual void SetIsDocInside(bool bIsDocInside);
protected:
	
	CDC *m_pDC;

	double m_dComAngle;
	double m_dbAngle; //对象绕中心点旋转的角度
	//对象的旋转中心点，默认以对象的中心点为旋转中心点，当用触摸屏多点旋转时，可指定旋转中心点
	CPoint m_ptRotationCenter; 
	CPoint m_ptPreRotationCenter; // 上次旋转的中心点
	//对象缩放比例
	double m_dbXZoomScale;
	double m_dbYZoomScale;
	//对象缩放中心，默认以对象左上点为缩放中心点，当用触摸屏缩放时，可指定缩放中心点
	CPoint m_ptZoomCenter;

	//对象是否锁定
	BOOL m_bIsLock;

	//对象是否被限制操作
	bool m_bIsLimit;

	//是否是背景
	BOOL m_bIsBkg;

	bool m_IsCenterpont;

    //是否为文档
    bool m_bIsDocument;

	bool m_bIsTouchDoc;

	//是否为文档中的对象移动
	bool m_bIsDocInside;

	//是否为文档的触摸移动
	bool m_IsTouchMove;

	//对象透明度
	int m_byAlpha;
	int m_preAlpha; //之前的透明度

	int m_nAlphaInCom; //组合时的透明度，用于改变组合里对象的透明度时

	//对象是否等比例缩放
	BOOL m_bEqualScaleZoom;
	double m_dbEauelScale; //对象长、宽比例
	//中心点是否隐藏
	BOOL m_bRorationCenterZoom;

	BOOL m_bInfiniteClone; //是否无限克隆
	BOOL m_bIsClone; //是否克隆

	DWORD m_dwComLayerIndex; //对象所属组合的图层号
	BOOL m_bIsCombineMember; //是否是组合成员

	CBitmap m_bmpBuf; //保存对象所做的图，保存在内存中
	BOOL m_bHaveDrawBmpBuf; //是否已经创建内存位图

	CPoint m_ptMapCenter;	//对象旋转或缩放后中心点在窗口坐标系中的映射点
	XFORM m_xform;		//记录坐标系的变换
	BOOL m_bIsZoom;	//对象是否被缩放过

	BOOL m_bSelectFrameIsShow;	// 是否显示选择框
	CWBImageButton *m_pWBImageButton;	//图片按钮对象

	CWBControlButton *m_pControlButton;	//文档、视频和挡板控制按钮

	unsigned long m_ulOwner;		//对象的所有者，即创建者（用于同步）

	BOOL m_bIsToZero; //Rounding函数里的静态变量是否归零
    BOOL m_bIsCreat; //是否创建选择框

private:
	DWORD m_dwTouchID;  //触摸屏ID
	DWORD m_dwLayerIndex;  //对象层数，每个对象都有一个层数，且不重复，所以也可作对象唯一标识
	int		m_nPageLayerIndex;//对象在页中的图层号   huanglongquan ADD by 2012年4月18日
	WB_OBJ_TYPE m_nType; //对象类型	
	WB_LAYER_TYPE m_nLayerType; //对象图层类型
	
protected:
	virtual void  DrawLast(HDC hDC, int nOffsetX=0, int nOffsetY=0){};
	CWBPage *GetFather() const;
	//按钮旋转的偏移角度   
	double m_dRotateOffAngle;
	CPoint m_ptZoomOffPoint;//按钮拉伸的偏移位置
};

class CWBLine : public CWBObject//直线类
{
public:
	CWBLine(const CPoint& ptBegin, const CPoint& ptEnd);
	CWBLine(const PWBLineItem pItem = NULL);
	CWBLine(const CWBLine& other); //拷贝构造函数
	virtual ~CWBLine(); //上锁

	CWBLine & operator =(const CWBLine& other); //赋值函数

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	//数据拷贝
	static PWBLineItem CopyItem(PWBLineItem pDesItem, const PWBLineItem pSrcItem);  //在调用的地方已经上锁

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC); //上锁

	//判断点是否在对象的选择范围内
	virtual int IsInSelectArea(CPoint& pt)const; //点选//上锁
	virtual BOOL IsInSelectArea(const CRect& rect) const; //框选//上锁

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内//上锁
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交//上锁

	//创建选择框
	virtual void CreateSelecrtFrame(); //上锁

	//擦除
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);

	void SetItem(PWBLineItem pItem); //上锁
	void GetItem(PWBLineItem pItem); //上锁

	//移动
	BOOL Move(const CPoint& pt, BOOL bIsBeginPoint = TRUE); //上锁
	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset); //上锁
	BOOL MoveOffset(const CPoint& ptOffset, BOOL bIsBeginPoint = TRUE); //根据端点偏移量移动 //上锁

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj); //上锁
	PWBLineItem m_pItem;
private:
	
	XCritSec m_csWBLineObjLock;//对直线对象的互斥操作上锁
};

class CWBCurve : public CWBObject //曲线类
{
public:
	CWBCurve(const PWBCurveItem pItem = NULL);
	CWBCurve(const CWBCurve& other); //拷贝构造函数
	virtual ~CWBCurve();  //上锁

	CWBCurve & operator =(const CWBCurve& other); //赋值函数

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	//判断点是否在对象的选择范围内
	virtual int IsInSelectArea(CPoint& pt)const; //点选//上锁
	virtual BOOL IsInSelectArea(const CRect& rect) const; //框选

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内//上锁
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交//上锁
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te);//曲线中TouchUpAllCursor什么都不做
	//根据偏移量移动  //上锁
	virtual void MoveOffset(const CPoint& ptOffset);

	//创建选择框
	virtual void CreateSelecrtFrame();
	//创建对象区域
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	//擦除
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);
	static void ClearCurveItem(PWBCurveItem pItem); //清除曲线对象
	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);//上锁

	void DrawErase(CDC *pDC);
	void SetItem(const PWBCurveItem pItem);//上锁
	WBCurveItem *GetItem();  //上锁
	UINT AddPoint(CPoint pt, int nType = CURVE_TYPE_NORMAL); //添加曲线点数//上锁
	
	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放//上锁
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象

	//以角度偏移旋转对象
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//对象绕某一点旋转
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转
protected:
	BOOL TranslatePointsToString(std::string& strPoints) const; //填写点字符串//在SetXmlValue(XMemIni& xmlObj) const调用已经上锁
	BOOL ParseStringToPoint(std::string strPoints, int nPointCount); //解析点字符串//上锁
	BOOL AddendumPointToCurve(std::string strPoints,int nPointCount); //向曲线后面追加点//上锁

public:
	void SetLeftTopRightBottomPt(CPoint &ptLeftTop, CPoint &ptRightBottom); //开始画曲线时，给左上和右下点赋值
	void GetLeftTopRightBottomPt(CPoint &pt);
	CPoint GetLeftTop();
	CPoint GetRightBottom();

protected:
	DWORD AddMiddlePoints(const CPoint& pt); //计算中间点//在AddPoint()中调用已经上锁

private:
	XCritSec m_csWBCurveObjLock;//对曲线对象的互斥操作上锁
	PWBCurveItem m_pItem;

	CPoint m_ptLeftTop;
	CPoint m_ptLastPoint;	//曲线的最后一个点

	COLORREF crTransparent;//透明度
	CDC m_pmemDC;	
	int n_Width;
	int n_Height;
	BOOL m_bIsDraw;

	mutable int m_nIndexPosPoint;//上一次发送的点结束的位置
	bool m_bResume_broken_transfer;//是否要断点续传

//	bool m_bDrawingCurve;//曲线正在画时候不让操作

	CPoint m_ptLT; //画曲线过程中所有点的左上点
	CPoint m_ptRB; //画曲线过程中所有点的右下点

public:
	// Draw only last segment of the stroke.
	void DrawLast(HDC hDC,int nOffsetX=0, int nOffsetY=0, UINT nPtNum = 1);
	void SetIsDraw(BOOL IsDraw = TRUE);//判断曲线是否重画
	BOOL GetIsDraw() const;
	void SetResume_broken_taransfer(bool bResume_broken_transfer);//设置是否要断点续传
};

//jiangchao新增代码类CWBIcoCurve
class CWBIcoCurve : public CWBCurve
{
public:
	CWBIcoCurve(const PWBIcoCurveItem pItem = NULL);
	CWBIcoCurve(const CWBIcoCurve& other); //拷贝构造函数
	virtual ~CWBIcoCurve();

	CWBIcoCurve & operator =(const CWBIcoCurve& other); //赋值函数

	//判断点是否在对象的选择范围内
	virtual int IsInSelectArea(CPoint& pt)const; //点选//上锁
	//virtual BOOL IsInSelectArea(const CRect& rect) const; //框选

	//根据偏移量移动  //上锁
	virtual void MoveOffset(const CPoint& ptOffset);
    //擦除
	virtual BOOL Erase(const CRect& rectErase, BOOL& bDelObj);

	//创建对象区域
	//virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内//上锁
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交//上锁

	//创建选择框
	virtual void CreateSelecrtFrame();

	//清除对象
	static void ClearCurveItem(PWBIcoCurveItem pItem);

	//文件操作
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	//virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	virtual void Draw(CDC * pDC); //上锁
	void DrawIcoTrack(CDC * pDC, CPoint& point, int nOffsetX = 0, int nOffsetY = 0); //画图标曲线轨迹

    void SetItem(const PWBIcoCurveItem pItem); //上锁
	WBIcoCurveItem * GetItem(); //上锁
	void SetBeginPoint(const CPoint& point); 
	UINT AddPoint(CPoint point); //增加图标曲线点

	int GetPreScaleWidth() const {return m_nPreScaleWdith;}
	void SetPreScaleWidth(int nPreScaleWidth) {m_nPreScaleWdith = nPreScaleWidth;}

	//设置Image指针对象(从文件加载)
	void SetIcoImage(); 

	//判断是否从文件加载
	void SetIsLoadPicFromFile(BOOL IsLoadPicFromFile) {m_IsLoadPicFromFile = IsLoadPicFromFile;}

public:
	void SetLeftTopRightBottomPt(CPoint &ptLeftTop, CPoint &ptRightBottom); //开始画曲线时，给左上和右下点赋值
	void GetLeftTopRightBottomPt(CPoint &pt);
	CPoint GetLeftTop();
	CPoint GetRightBottom();
	BOOL IsPointInObjArea(const CPoint& point, double nScaleWidth) const; //判断点是否是在对象上

    void SetResume_broken_taransfer(bool bResume_broken_transfer);//设置是否要断点续传
	BOOL GetIsRefush(){return m_bIsRefush;}
	void SetIsRefush(BOOL brefush){m_bIsRefush = brefush;}
protected:
	mutable int m_nIndexPosPoint;//上一次发送的点结束的位置
	bool m_bResume_broken_transfer;//是否要断点续传
	BOOL TranslatePointsToString(std::string& strPoints) const; //填写点字符串 上锁
	BOOL ParseStringToPoint(std::string strPoints, int nPointCount); //解析点字符串 上锁
    BOOL AddendumPointToCurve(std::string strPoints,int nPointCount); //向后追加点

	Image * m_Image; //Image 图像指针

private:
	XCritSec m_csWBIcoCurveObjLock;//对曲线对象的互斥操作上锁
	PWBIcoCurveItem m_pItem; //曲线图标结构体
	int m_nPreScaleWdith; //最开始画曲线图标的宽度
	int m_nWidth; //宽度
	int m_nHeight;//高度
	CPoint m_BeginPt; //鼠标按下时坐标
	BOOL m_bIsRefush;//是否重绘
	CPoint m_ptLeftTop; 
	CPoint m_ptLT; //画图标曲线过程中所有点的左上点
	CPoint m_ptRB; //画图标曲线过程中所有点的右下点
	CDC  m_mpmDC;  //画好了就存在DC里面，不要每次都画
	BOOL m_IsLoadPicFromFile; //判断是否从文件加载图片
};

class CWBPolygon : public CWBObject //多边形类
{
public:
	CWBPolygon(const PWBPolygonItem pItem = NULL);
	CWBPolygon(const CWBPolygon& other); //拷贝构造函数
	virtual ~CWBPolygon();  //上锁                            

	CWBPolygon & operator=(const CWBPolygon& other); //赋值函数

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);//上锁

	//创建选择框
	virtual void CreateSelecrtFrame();

	//判断点是否在对象的选择范围内
	virtual int IsInSelectArea(CPoint& pt)const;//上锁
	virtual BOOL IsInSelectArea(const CRect& rect) const;//上锁
	virtual BOOL IsRegionIn(const CRgn *pRgn) const;//上锁
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交//上锁

	//数据拷贝
	static PWBPolygonItem CopyItem(PWBPolygonItem pDesItem, const PWBPolygonItem pSrcItem);

	static void ClearItem(PWBPolygonItem pItem); //清除对象

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual BOOL Load(char *pBuf, DWORD& dwLen);//上锁
	virtual void Draw(CDC *pDC);//上锁

	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放//上锁

	//移动线
	virtual BOOL Move(const CPoint& pt);//上锁

	void SetItem(const PWBPolygonItem pItem);//上锁
	void GetItem(PWBPolygonItem pItem);//上锁
	void AddPoint(CPoint pt);  //添加多边形点//上锁
	void SetClosed(BOOL bIsClosed = TRUE); //设置封闭
	void SetFillColor(COLORREF clrFill); //设置填充颜色//上锁
	void DeletePoint();		//删除最后一个点//上锁

	//区域操作rgn////////////////////////////////////////////////////////////////////////
	virtual BOOL CreateRgn(CRgn *pRgn) const; //创建区域//上锁
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //根据对象旋转与缩放后创建区域//上锁
	virtual BOOL CreateReverseFillRgn(CRgn *pRgn) const; //根据对象旋转与缩放后创建填充区域//上锁
	BOOL PointIn(const CPoint& pt) const; //判断点是否在多边形范围内//上锁

	virtual BOOL IsSolid() const; //判断对象是否填充

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	BOOL SetPointsString(std::string& strPoints) const; //填写点字符串//上锁
	BOOL GetPointsString(std::string strPoints, int nPointCount); //解析点字符串//上锁

	BOOL GetInAndOutPoint(); //根据多边形的点列表得到内部和外部的点列表//上锁
	void GetItemInfo(int &dWidth, int &dHeight); //得到图形的长和高//上锁
	PWBPolygonItem m_pItem;
protected:
	
	std::vector<CPoint *> vecPointsIn; //内部点列表
	std::vector<CPoint *> vecPointsOut; //外部点列表

	XCritSec m_csPolygonItemLock;

private:
	BOOL m_bIsClosed; //是否封闭
	BOOL m_bHaveFillRgn; //是否有填充区域
};

class CWBRectangle : public CWBPolygon //矩形
{
public:
	CWBRectangle();
	CWBRectangle(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBRectangle(const CWBRectangle& other);
	virtual ~CWBRectangle();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	//创建选择框 
	virtual void CreateSelecrtFrame();

	//修正对象坐标点
	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//根据缩放按钮偏移缩放对象
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁

	//operations
	virtual void Draw(CDC *pDC);//上锁

	int Width() const;//上锁
	int Height() const;//上锁
private:
	XCritSec m_csRectangleItemLock;
};

class CWBRhombus : public CWBPolygon //菱形
{
public:
	CWBRhombus();
	CWBRhombus(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBRhombus(const CWBRhombus& other);
	virtual ~CWBRhombus();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	virtual void CreateSelecrtFrame();//上锁

	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//operations
	virtual void Draw(CDC *pDC);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
private:
	XCritSec m_csRhombusItemLock;
};

class CWBPentagon : public CWBPolygon //五边形
{
public:
	CWBPentagon();
	CWBPentagon(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBPentagon(const CWBPentagon& other);
	virtual ~CWBPentagon();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	virtual void CreateSelecrtFrame();//上锁

	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//operations
	virtual void Draw(CDC *pDC);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
private:
	XCritSec m_csPentagonItemLock;
};

class CWBHexagon : public CWBPolygon //六边形
{
public:
	CWBHexagon();
	CWBHexagon(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBHexagon(const CWBHexagon& other);
	virtual ~CWBHexagon();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	virtual void CreateSelecrtFrame();//上锁

	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//operations
	virtual void Draw(CDC *pDC);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
private:
	XCritSec m_csHexagonItemLock;
};

class CWBFivepointstart : public CWBPolygon //五角星
{
public:
	CWBFivepointstart();
	CWBFivepointstart(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBFivepointstart(const CWBFivepointstart& other);
	virtual ~CWBFivepointstart();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	virtual void CreateSelecrtFrame();//上锁

	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//operations
	virtual void Draw(CDC *pDC);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
private:
	XCritSec m_csFivepointstartItemLock;
};

class CWBTrapezium : public CWBPolygon //梯形
{
public:
	CWBTrapezium();
	CWBTrapezium(const CPoint& ptLeftTop, const CPoint& ptRightBottom);
	CWBTrapezium(const CWBTrapezium& other);
	virtual ~CWBTrapezium();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	virtual void CreateSelecrtFrame();//上锁

	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//operations
	virtual void Draw(CDC *pDC);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁

	BOOL IsInPointArea(const CPoint& pt);
	BOOL MovePoint(CPoint& pt);
private:
	XCritSec m_csTrapeziumItemLock;
	int nPointIndex; //当前选中的点的序号
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
	int m_ypoint; //左上角点Y的取值于哪个点
	bool m_bIsEdit;
	int m_nEditNUM;
	ImageEx *m_pEditpic; // 编辑点图片
};

class CWBTriangle : public CWBPolygon //三角形
{
public:
	CWBTriangle(int nType = 0);
	CWBTriangle(const CPoint& ptLeftTop, const CPoint& ptRightBottom,int nType = 0);
	CWBTriangle(const CWBTriangle& other,int nType = 0);
	virtual ~CWBTriangle();


	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	//创建选择框 
	virtual void CreateSelecrtFrame();

	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
	//void setTriangleAngle(int nAngle){m_nTriangleAngle = nAngle;}
	//operations
	virtual void Draw(CDC *pDC);//上锁
	CPoint GetLeftTopPT(){return m_ptLeftTop;}
	int GetTriangleType(){return m_nTriangleType;}
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

private:
	XCritSec m_csTriangleItemLock;
	int  m_nTriangleType; /*0表示正三角形，1表示直角三角形，2表示等腰三角形，3表示等腰直角三角形*/
	/*int m_nTriangleAngle; //等腰三角形的角度*/
	CPoint m_ptLeftTop;//保存坐上点
};

class CWBEllipse : public CWBObject //椭圆类
{
public:
	CWBEllipse(const CPoint& ptTopLeft, const CPoint& ptBottomRight);
	CWBEllipse(const PWBEllipseItem pItem = NULL);
	CWBEllipse(const CWBEllipse& other); //拷贝构造函数
	virtual ~CWBEllipse();       //上锁                       

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	CWBEllipse & operator=(const CWBEllipse& other); //赋值函数//上锁

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);//上锁

	//判断点是否在对象的选择范围内
	virtual int IsInSelectArea(CPoint& pt)const ;//上锁
	virtual BOOL IsInSelectArea(const CRect& rect) const;//上锁
	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内

	//数据拷贝
	static PWBEllipseItem CopyItem(PWBEllipseItem pDesItem, const PWBEllipseItem pSrcItem);

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);//上锁

	//创建选择框
	virtual void CreateSelecrtFrame();//上锁

	//修正对象坐标点
	virtual void AdjustObject();//上锁

	BOOL Move(const CPoint& ptRightBottom);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
	//以任意点位中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //上锁

	void SetItem(const PWBEllipseItem pItem);//上锁
	void GetItem(PWBEllipseItem pItem) const;//上锁

	void SetFill(BOOL bIsFill = TRUE); //设置是否填充
	void SetFillColor(COLORREF clrFill); //设置填充颜色//上锁

	//区域操作rgn////////////////////////////////////////////////////////////////////////
	BOOL CreateRgn(CRgn *pRgn) const; //创建区域//上锁
	BOOL PointIn(const CPoint& pt) const; //判断点是否在多边形范围内

	virtual BOOL IsSolid() const; //判断对象是否填充

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	int GetLineWidth() const; //得到线宽
	PWBEllipseItem m_pItem;
private:
	
	BOOL m_bIsFill; //是否填充
	BOOL m_bHaveFillRgn; //是否有填充区域

	XCritSec m_csEllipseItemLock;
};

class CWBSelectRectangle : public CWBRectangle //选择框矩形
{
public:
	CWBSelectRectangle();
	CWBSelectRectangle(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	CWBSelectRectangle(const CWBSelectRectangle& other);//上锁
	virtual ~CWBSelectRectangle();//上锁

	virtual void Draw(CDC *pDC);//上锁
	virtual void GetCenterPoint(CPoint& ptCenter);//上锁
	BOOL GetSelectInfo(double &dCenterX, double &dCenterY, double &dWidth, double &dHeight);//上锁

	BOOL Move(const CPoint& ptLeftTop, const CPoint& ptRightBottom);//上锁
	virtual BOOL Move(const CPoint& ptRightBottom);//上锁

	virtual void MoveOffset(const CPoint& ptOffset);//上锁

	//判断点是否在对象的选择范围内
	virtual BOOL IsInSelectArea(const CRect& rect) const;
	virtual BOOL IsRegionIn(const CRgn *pRgn) const;//上锁
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交//上锁

	//区域操作rgn////////////////////////////////////////////////////////////////////////
	virtual BOOL CreateRgn(CRgn *pRgn) const; //创建区域//上锁
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //根据对象旋转与缩放后创建区域//上锁

	BOOL PointIn(const CPoint& pt) const; //判断点是否在多边形范围内

	virtual BOOL ZoomByButton(const CPoint& ptOffset);
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放

	int Width() const;
	int Height() const;

	void SetLeftTop(CPoint ptLeftTop);//上锁
	void SetRightTop(CPoint ptRightTop);//上锁
	void SetRightBottom(CPoint ptRightBottom);//上锁
	void SetLeftBottom(CPoint ptLeftBottom);//上锁

	CPoint GetLeftTop();
	CPoint GetRightTop();
	CPoint GetRightBottom();
	CPoint GetLeftBottom();

private:
	PWBSelectRectangleItem m_pItem;
	XCritSec m_csSelectFrameItemLock;
};

class CWBPicture : public CWBObject //图片
{
public:
	CWBPicture(LPCTSTR lpszFilePath = NULL,CRect rct = CRect( 0, 0, 0, 0 ),CString str = _T(""));//上锁
	CWBPicture(PWBPictureItem pItem);//上锁
	CWBPicture(const CWBPicture& other); //拷贝构造函数//上锁
	virtual ~CWBPicture();//上锁
	
	CWBPicture & operator=(const CWBPicture& other); //赋值函数//上锁

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;//上锁
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);//上锁
	virtual void DrawAsBakground(CDC *pDC, CWnd *pParentWnd); //作为背景画//上锁

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);

	//创建选择框
	virtual void CreateSelecrtFrame();//上锁
	CRect GetPictureRect() ;

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);//上锁
	virtual int IsInSelectArea(CPoint& pt)const; //点选择//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
	//以任意点位中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //上锁

	//以角度偏移旋转对象
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//对象绕某一点旋转//上锁
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转//上锁

	//数据拷贝
	static PWBPictureItem CopyItem(PWBPictureItem pDesItem, const PWBPictureItem pSrcItem);

	void SetItem(const PWBPictureItem pItem);//上锁
	void GetItem(PWBPictureItem pItem) const;//上锁

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	//初始化信息
	virtual void init();//上锁
	//比较原图大小，让图片的最大不能超出view的客户区大小4-21金鹏
	void SetPictureBigerSzie(CRect viewClientRect );//上锁

	//设置是否需要加载图片
	void SetIsPath(BOOL bIsPath = TRUE);
	BOOL GetIsPath() const;
	//得到图片在文件夹中的实际大小
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

	virtual void SetSelectFrameShow(BOOL bShow); //显示选择框//上锁
	void SetSelComSelectFrameShow(BOOL bShow); //显示选中的组件的选择框//上锁
	CWBObject *GetSelectComObj(); //得到选中的组件对象//上锁
	void SetSelectedObj(DWORD dwLayerIndex); //实时同步时确定当期选择的对象

	//对象透明度操作
	virtual void SetAlpha(int alpha);//上锁
	//判断、添加组件
	BOOL ObjectIsComponent(CWBObject *obj);//上锁
	void AddComponent(CWBObject *obj);//上锁
	BOOL DeleteComponent();//上锁
	BOOL PtIsComponent(CPoint pt);
	//复制组件
	void CopyComponents(CWBComponent *otherCom);//上锁

	//用于文档和组件的旋转缩放 luowei 2012-04-18
	void GetSrcAnnPt(); //得到旋转前组件中备注尖角的点//上锁
	void GetSrcCenterPt(); //得到旋转前组件中每个对象的旋转中心点//上锁
	void GetSrcZoomCenterPt(); //得到缩放前组件中每个对象的选择框左上角和右下角的点//上锁
	void GetEachSrcZoomCenterPt(); //得到缩放前组合中每个对象的选择框左上角的点//上锁
	void MoveAnn(const CPoint& ptOffset); //移动文档中的备注//上锁
	void ClearSrcAnnPt(); //清空旋转前组件中备注尖角的点

	//得到选中的文档中的备注 luowei 2012-04-23
	CWBObject *GetAnn();//上锁

	void DrawToDC(); //把图片的内容画到图片的内存DC上
	CString SavePictureToJpg(); //把图片保存为JPG文件

	BOOL GetIsPrintScreen();
	void SetIsPrintScreen(BOOL bIsPrintScreen = TRUE);
	void SetIsEditpic(bool bispic){m_bIsEditPic = bispic;}
	bool GetIsEditPic(){return m_bIsEditPic;}

	//手势操作旋转属性
	void SetIsMTRotate(BOOL bIsMTRotate);
	BOOL GetIsMTRotate();

	//手势操作缩放属性
	void SetIsMTZoom(BOOL bIsMTZoom);
	BOOL GetIsMTZoom();

	BOOL IsEdited(); //图片是否含有备注
	void InitCom(); //初始化组件
private:
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage = 0);
private:
	//unsigned long m_CurchannelID;//拖拽通道用来判断是否已经在白板中存在
	mutable bool m_bSaveSamePicture;
	XCritSec m_csPictureItemLock;
	std::string m_strCaramaPicture;//判断摄像头图片是否存在
	bool m_bneedCreateFram; //双击视频产生图片旋转时需要重算大小
	bool m_bIsEditPic;//是否为编辑图片(旋转中用的)
public:
	PWBPictureItem m_pItem;
protected:
	mutable CString m_DrawString;
	
	ImageEx *m_pImage;
	CRect m_viewClientRect;

	CDC m_bufferpicture;
	int m_nWidth,m_nHeight;
	BOOL m_bIsPath;
	int m_nRealWidth, m_nRealHeight; //图片在文件夹中的实际大小
	LPTSTR m_pszExtension;   //图片后缀名

	CWBComponent *m_comList; //子对象列表
	CDC m_dcMem;	//用于保存文件的内存DC
	BOOL m_bIsPrintScreen; //是否是视频截图

	//手势操作时，是旋转还是缩放
	BOOL m_bIsMTRotate;
	BOOL m_bIsMTZoom;
};

class CWBDustbin:public CWBPicture //垃圾箱
{
public:
	CWBDustbin();
	virtual ~CWBDustbin();

	//operations
	virtual void MoveOffset(const CPoint& ptOffset);
	virtual void Draw(CDC *pDC);
	void  MoveDustbin(CPoint ptMovePoint);
	void  GetRect(CRect &rect); //得到矩形框
	void  SetRect(CRect &rect); // 设置矩形框
	void  MoveRect(CPoint ptMovePoint); //移动矩形框
	int   PointIn(CPoint ptPoint);

private:
	CRect m_rect;	//矩形框
};
class CWBFlash: public CWBObject //flash
{
public:
	CWBFlash(LPCTSTR lpszFilePath = NULL,CRect rect = CRect(0,0,0,0));//上锁
	CWBFlash(PWBFlashItem pItem);//上锁
	CWBFlash(const CWBFlash& other); //拷贝构造函数//上锁
	virtual ~CWBFlash();//上锁

	CWBFlash & operator=(const CWBFlash& other); //赋值函数//上锁

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;//上锁
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);

	//创建选择框
	virtual void CreateSelecrtFrame();//上锁

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
	//以任意点位中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //上锁

	//数据拷贝
	static PWBFlashItem CopyItem(PWBFlashItem pDesItem, const PWBFlashItem pSrcItem);

	void SetItem(const PWBFlashItem pItem);//上锁
	void GetItem(PWBFlashItem pItem) const;//上锁

	void SetCurRect(CRect rc);//wangxiaobin
	CRect GetCurRect();

	void init();//上锁
	void Start();
	void Pause();
	void Stop();

	void SetCircle();	//设置FLASH循环播放
	void SetSinglePlay();//设置FLASH单次播放
	BOOL GetCircle();	//得到当前FLASH是否循环

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	CString CatchFlash();//上锁

	bool GetFlashIsPlay() {return m_bFlashIsPlay;}
	void SetFlashIsPlay(bool FlashIsPlay) {m_bFlashIsPlay = FlashIsPlay;}
private:
	bool m_bFlashIsPlay;
	//int m_FlashCout;
	long m_current;
	PWBFlashItem m_pItem;
	CShockwaveFlash *m_swf;
	BOOL m_bFindFile;	//文件是否存在
	static int count;    //加载flash的个数
	static int num;      //一页只有一个Flash时候
	
	unsigned long m_CurrentNum; //保存当前帧数
	XCritSec m_csFlashOperationLock;

public:
	BOOL m_bIsRunning;	//是否播放
	BOOL m_bIsPause;	//是否暂停
    BOOL m_bIsStop;     //是否停止
	BOOL m_bIsCircle;	//是否循环播放
	CRect m_rect;
	mutable bool m_bSaveSameFlash;
};

typedef std::map<int, CWBComponent*>::const_iterator ComList_Iterator;
class CWBDocument : public ThreadHelperImpl<CWBDocument>, public CWBObject //文档
{
public:
	CWBDocument(LPCTSTR lpszFilePath = NULL,CRect rect = CRect(0, 0, 0, 0)); //上锁
	CWBDocument(PWBDocumentItem pItem);//上锁
	CWBDocument(const CWBDocument& other); //拷贝构造函数//上锁
	virtual ~CWBDocument();//上锁

	CWBDocument & operator=(const CWBDocument& other); //赋值函数//上锁

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;//上锁
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);//上锁

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	//创建选择框
	virtual void CreateSelecrtFrame();//上锁

	//数据拷贝
	static PWBDocumentItem CopyItem(PWBDocumentItem pDesItem, const PWBDocumentItem pSrcItem);

	void SetItem(const PWBDocumentItem pItem);//上锁
	void GetItem(PWBDocumentItem pItem) const;//上锁

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);//上锁

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//上锁
	BOOL ZoomRect(CRect& rect, const CPoint& ptZoomCenter, double dbXScale, double dbYScale, BOOL bIsToZero);

	//以任意点位中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //上锁
	virtual void SetIsZoom(BOOL bIsZoom);//上锁

	//以角度偏移旋转对象
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//对象绕某一点旋转//上锁
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转//上锁

	virtual int IsInSelectArea(CPoint& pt)const; //点选择//上锁

	virtual void ButtonClose();

	virtual void SetSelectFrameShow(BOOL bShow); //显示选择框//上锁
	void SetSelComSelectFrameShow(BOOL bShow); //显示选中的组件的选择框//上锁
	CWBObject *GetSelectComObj(); //得到选中的组件对象//上锁
	void SetSelectedObj(DWORD dwLayerIndex); //实时同步时确定当期选择的对象

	//文档页操作
	int GetPageCount() const; //得到文档总页数
	int GetCurPageIndex() const; //得到当前页
	BOOL GotoPage(int nPageIndex); //翻到某一页//上锁
	BOOL PrePage();
	BOOL NextPage();
	BOOL FirstPage();
	BOOL LastPage();

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	//对象透明度操作
	virtual void SetAlpha(int alpha);//上锁

	void _Thread(); //查看DOC转换信息的线程//上锁

	//判断、添加组件
	BOOL ObjectIsComponent(CWBObject *obj);//上锁
	void AddComponent(CWBObject *obj);//上锁
	BOOL DeleteComponent();//上锁
	BOOL PtIsComponent(CPoint pt);
	//复制组件
	void CopyComponents(const std::map<int, CWBComponent*> &otherCom);//上锁
	void SetPageIndex(int nPageIndex);

	//用于文档和组件的旋转缩放 luowei 2012-04-18
	void GetSrcAnnPt(); //得到旋转前组件中备注尖角的点//上锁
	void GetSrcCenterPt(); //得到旋转前组件中每个对象的旋转中心点//上锁
	void GetSrcZoomCenterPt(); //得到缩放前组件中每个对象的选择框左上角和右下角的点//上锁
	void GetEachSrcZoomCenterPt(); //得到缩放前组合中每个对象的选择框左上角的点//上锁
	void MoveAnn(const CPoint& ptOffset); //移动文档中的备注//上锁

	//得到选中的文档中的备注 luowei 2012-04-23
	CWBObject *GetAnn();//上锁
	//int  OnLButtonDoubleAnn(); //双击文档中的备注
	//手势操作旋转属性 luowei 2012-05-02
	void SetIsMTRotate(BOOL bIsMTRotate);
	BOOL GetIsMTRotate();

	//手势操作缩放属性 luowei 2012-05-02
	void SetIsMTZoom(BOOL bIsMTZoom);
	BOOL GetIsMTZoom();

	BOOL GetIsConvertTip(); //得到是否开启DOC转换提示

	void Erase(const CRect& rectErase);//上锁

	void IsComRegionIn(const CRgn *pRgn);//上锁

	void SetRect(CRect rect);

	void DrawToDC(); //把文档当前页的内容画到文档的内存DC上
	void GetPageBmp(int i = 0); //把文档页保存为位图文件
	void PrintCurPage(); //打印当前页
	HBITMAP GetCurPageBmp(); //得到当前页的位图

protected:
	BOOL OpenDoc(); //打开文档//上锁
	BOOL OpenPDFDoc();//上锁

	void CALLBACK DocConverTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

private:

	PWBDocumentItem m_pItem;

	DATEC_PDFConvert*		m_pPDFConvert;
	DATEC_PDFFile*			m_pPDFFile;

	int m_nCurPageIndex; //当前文档显示页
	int m_nPageCount; //文档总页数
	ImageEx *m_curPageBmp; //当前PDF页的图片缓存

	CString m_strTempFilePath;

	BOOL m_bConvertTip; //是否开启DOC转换提示
	CWBRectangle *m_convertTipFrameBg;
	CWBRectangle *m_convertTipProgressbar;
	CWBText      *m_convertTipText;

	std::map<int, CWBComponent*> m_comList; //子对象列表

	CWBText *m_pPageIndexTip; //页码
	int m_nPageIndex; //插入文档时的页号
	BOOL m_bIsOpenPDF; //PDF文档是否打开成功

	//手势操作时，是旋转还是缩放
	BOOL m_bIsMTRotate;
	BOOL m_bIsMTZoom;

	mutable bool m_bSaveSameDocument;
	CRect m_rect; //传进来的文档位置大小
	CDC m_dcMem;	//每个文档都有自己的内存DC
	XCritSec m_csDocOperationLock;
};

class CWBText : public CWBObject
{
public:
	CWBText(CPoint &ptTopLeft, LOGFONT &lfLogFont, COLORREF &clrText, LPCTSTR lpszText = NULL);//上锁
	CWBText(PWBTextItem pItem = NULL);//上锁
	CWBText(const CWBText& other);//上锁
	virtual ~CWBText();//上锁

	CWBText & operator=(const CWBText& other);//上锁

	///add by aolingwen 2012.12.4
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁
	///end

	//operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;//上锁
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);

	//数据拷贝
	static PWBTextItem CopyItem(PWBTextItem pDesItem, const PWBTextItem pSrcItem);


	//创建选择框
	virtual void CreateSelecrtFrame();//上锁

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);//上锁



	void SetItem(const PWBTextItem pItem);//上锁
	void GetItem(const PWBTextItem pItem) const; //上锁

	//设置文本需要重新计算显示框
	void SetCalRectText(BOOL bCalRectrText = TRUE);

	void StringReplace(std::string &srcStr, const std::string &oldStr, const std::string &newStr) const;

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;//上锁
	virtual BOOL GetXmlValue(XMemIni& xmlObj);//上锁

	BOOL SetTextXmlValue(XMemIni& xmlObj) const;//上锁
	BOOL GetTextXmlValue(XMemIni& xmlObj);//上锁

	BOOL GetIsVertical() const;
	void SetIsVertical(BOOL bIsVertical = FALSE);
	PWBTextItem m_pItem;
private:
	
	BOOL m_bIsVertical; //文字是否竖排
	//CRect m_rectText; //文本显示的矩形，通过DrawText函数计算
	BOOL m_bCalRecttext; //是否要重新计算显示框
	XCritSec m_csTextOperationLock;
};
//jin
class CWBBafflePlate : public CWBObject //挡板类
{
public:
	CWBBafflePlate(const CPoint& ptLeftTop, const CPoint& ptRightBottom, COLORREF clrBackground = RGB(0, 0, 0));
	CWBBafflePlate(const PWBBafflePlateItem pItem = NULL, COLORREF clrBackground = RGB(0, 0, 0));
	~CWBBafflePlate();

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);//创建挡板对话框

	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);
	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	//设置和获得挡板结构数据
	void SetItem(PWBBafflePlateItem pItem);
	void SetItem( CPoint ptLeftTop,  CPoint ptRightBottom, COLORREF clrBackground = RGB(0, 0, 0));
	PWBBafflePlateItem GetItem();
	//拖动挡板把手改变挡板结构数据
	void ChangeTopHandle(CPoint ptOffSet);
	void ChangeLeftHandle(CPoint ptOffSet);
	void ChangeBottomHandle(CPoint ptOffSet);
	void ChangeRightHandle(CPoint ptOffSet);
	//创建选择框
	void CreateSelecrtFrame();
	//以任意点位中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); 

private:
	PWBBafflePlateItem m_pItem;
//	CBafflePlate *m_pDlg;
	BOOL m_isNewBaffle;  //新建的挡板需要在显示时候需要减去滚动条偏移量
};

class CWBCombine : public CWBObject //组合类
{
public:
	CWBCombine();
	CWBCombine(const CWBCombine& other);
	virtual ~CWBCombine();

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);

	virtual int IsInSelectArea(CPoint& pt)const; //点选择
	virtual BOOL IsInSelectArea(const CRect& rect) const; //区域选择

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交

	BOOL AddCombineLayer(); //得到组合的对象图层号
	BOOL AddCombineObj(); //向组合列表中添加对象，并把这些对象指针从对象列表中删除

	void CancelCombineObj(DWORD dwLayerIndex , DWORD dwSendId); //把组合列表中的对象重新加入对象链表中，并清空组合列表

	void AddComMebMsg(unsigned long ulEventID, int nPageIndex);
	void DelComMebMsg(); //删除组合前发送删除组合成员的消息
	void ChangeObjItem(unsigned long ulEventID, DWORD dwLayerIndex, BOOL bIsCombineMember);

	int GetComCount(); //组合内包含的对象数
	DWORD IsObjInCom(DWORD dwLayerIndex); //判断对象是否是组合成员
	BOOL AddObjInCom(CWBObject *pComObj, DWORD dwComLayerIndex); //向组合里添加对象

	//创建选择框
	virtual void CreateSelecrtFrame();

	//创建对象区域
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //根据对象旋转与缩放后创建区域

	//偏移量移动对象
	virtual void MoveOffset(const CPoint& ptOffset); 

	void GetSrcLinePoint(); //得到旋转前组合中直线的两个端点
	void GetSrcCenterPoint(); //得到旋转前组合中每个对象的旋转中心点

	//以角度偏移旋转对象
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转
	//virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//对象绕某一点旋转

	void GetSrcZoomCenter(); //得到缩放前组合中每个对象的选择框左上角和右下角的点

	//对象缩放
//	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象

	//擦除
	void Erase(const CRect& rectErase);
	void EraseLine(CWBLine *pLine, const CRect& rectErase, int nIndex);

	virtual BOOL IsSolid() const; //判断对象是否填充

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	BOOL SetComLayerXml(XMemIni& xmlObj) const;
	BOOL GetComLayerXml(XMemIni& xmlObj);

	BOOL GetComXml(XMemIni& xmlObj); //同步组合时，组合对象不完整是调用

	void SetComXmlValue(unsigned long ulEventID);

	BOOL SetCombineString(std::string& strCombine) const;
	BOOL GetCombineString(std::string strCombine, int nObjCount);
	BOOL SetPointsString(std::string& strPoints) const; //填写点字符串
	BOOL GetPointsString(std::string strPoints, int nPointCount); //解析点字符串

	std::vector<CWBObject *> GetCombineList(); //得到组合中的对象指针
	std::vector<DWORD> GetCombineLayerIndexList(); //得到组合中的对象ID

	void InitFlash(); //开始组合中的视频或初始化组合中的Flash

	void GetEachSrcZoomCenter(); //得到缩放前组合中每个对象的选择框左上角的点
	CWBObject * GetControlObj(); //得到组合中点中控制按钮的对象
	CWBObject * SetControlObj(CPoint& pt); //记录组合中点中控制按钮的对象

	CWBObject *FindObjByLayerIndex(DWORD dwLayerIndex) const; //根据图层号得到对象
	void GetComLineLayerIndex(); //得到组合里所有直线的图层号
	DWORD GetComLayerIndexByLine(DWORD dwLayerIndex); //得到直线所在组合的图层号
	void DelComLineMsg(); //删除擦除之前组合里的直线对象
	BOOL DeleteComObject(DWORD dwLayerIndex); //删除组合里的对象
	void AddComLineMsg(unsigned long ulEventID); //擦除之后添加组合里的直线对象
	BOOL AddComObjIntoCom(DWORD dwLayerIndex, CWBObject *pComObj); //向组合里添加对象（直线，曲线）
	BOOL DelComObj(DWORD dwLayerIndex); //删除组合里的对象指针
	//对象透明度操作
	virtual void SetAlpha(int alpha);
	void SetComMebAlpha();

	void SetComMinFlash(); //保存组合中尺寸最小的FLASH的图层号

	void SetComLayerNum(); //组合时，重新设置组合对象的图层号

	void GetFlashCombinRgn(CRgn &AllFlashRgn);

	std::vector<DWORD> GetLineLayerList() {return m_pLineLayerList;}

	void DeleteCurCombineObjectList();
private:
	XCritSec			m_csCombineLock;	//组合上锁

	std::vector<CWBObject *> m_pCombineList; //组合中的对象指针列表
	std::vector<DWORD> m_pCombineLayerList; //组合中的对象图层列表

	std::vector<CPoint> m_ptSrcCenterList; //记录组合中每个对象旋转前的中心点坐标
	std::vector<CPoint> m_ptPreCenterList; //记录组合中每个对象前一次的旋转后的坐标
	double m_dbRotateAngle; //对象绕中心点旋转的角度
	std::vector<CPoint> m_ptBeginList; //直线起点
	std::vector<CPoint> m_ptEndList; //直线终点

	std::vector<CPoint> m_ptSrcLeftTopList; //记录组合中每个对象缩放前左上角的点
	std::vector<CPoint> m_ptSrcRightBottomList; //记录组合中每个对象缩放前右下角的点
	std::vector<CPoint> m_ptPreLeftTopList; //记录组合中每个对象前一次缩放后左上角的点
	std::vector<CPoint> m_ptPreRightBottomList; //记录组合中每个对象前一次缩放后右下角的点

	std::vector<CPoint> m_ptResBeginList; //直线起点在组合的坐标系里的坐标
	std::vector<CPoint> m_ptResEndList; //直线终点在组合的坐标系里的坐标

	//对象缩放比例
	double m_dbXScale;
	double m_dbYScale;

	std::vector<CPoint> m_ptEachSrcLeftTopList; //记录组合中每个对象每次缩放前左上角的点

	std::vector<CPoint> m_ptResObjSrcLeftTopList;     //记录组合中每个对象缩放前 左上角 的点映射在对象坐标系里的坐标

	std::vector<CPoint> m_ptResObjSrcCenterList; //记录组合中每个对象缩放前对象中心点映射在组合对象坐标系里的坐标

	CWBObject *m_pControlObj; //组合中点中控制按钮的对象
	std::vector<DWORD> m_pLineLayerList; //组合里所有直线的图层号

	DWORD m_dwComMinFlash; //组合中的选择框最小的Flash的图层号
};

//放大镜类，add by zhuyuyuan
class CWBMagnifier : public CWBObject
{
public:
	CWBMagnifier(const PWBMagnifierItem pItem = NULL);
	CWBMagnifier(const CRect& rect, const int& nZoom = 0);
	~CWBMagnifier();

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj);
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	
	//设置和获取数据对象值
	void SetItem(const PWBMagnifierItem pItem);
	void SetItem(const CRect& rect, const int& zoom);
	PWBMagnifierItem GetItem();
	void SetRect(const CRect& rect);
	void GetRect(CRect& rect) const;

	//操作
	virtual void MoveOffset(const CPoint& ptOffset);
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放

	//如果放大镜的坐标修改了，返回TRUE；否则，返回FALSE
	BOOL  ChangeRect(const CPoint& point);
	void  DrawMagnifier(CDC* pDC);
	void  ChangeCursor();

	//判断点位置
	int   PointIsInSide(CPoint ptPoint);
	int   PointIn(CPoint ptPoint);

	//鼠标操作处理函数,返回值为TRUE，则需要刷新，否则不需要
	BOOL MouseDown(const CPoint& point);
	BOOL MouseMove(const CPoint& point);
	BOOL MouseUp(const CPoint& point);

	//鼠标按下进行的操作,如果返回TRUE，那么使用者必须刷新窗口
	BOOL MouseDownSlider(const CPoint& point);	

	//按钮移动进行的操作，那么使用者必须刷新窗口
	BOOL MouseMoveSlider(const CPoint& point);

	//按钮弹起进行的操作，那么使用者必须刷新窗口
	BOOL MouseUpSlider(const CPoint& point);

	//获取滑动条的放大倍数
	int GetSliderPos();

	//设置滑动块
	void  SetSliderPos(int zoomPos); 

	//获取滑动条的坐标
	void GetSliderRect(CRect& rect);

	//设置滑动条的坐标,是的背景图片可以伸缩
	void SetSliderRect();
private:
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
private:

	CPoint        m_ptPrePoint;
	CPoint        m_ptCurPoint;
	BOOL   m_bMoveMagn; //判断是否鼠标左键按下
	int    m_nMousePos;//当前鼠标所在的位置
	int    m_nMousePos2;//鼠标的位置用于鼠标状态变化
	int    m_nMouseState;//鼠标的状态
	DRAW_STATE    m_nDrawStatus;
	PWBMagnifierItem m_pItem;//数据结构体，保存放大镜的坐标和放大倍数
	CWBImageSliderControl* m_pSliderControl;//滑动条指针
	CWBControlButton* m_pCloseBtn;//关闭按钮指针

	XCritSec m_csMagnifierOperationLock;
};

//聚光灯是一个单独的窗口，父窗口是view. modify by zhuyuyuan
class CWBSpotlight : public CWBObject //聚光灯  
{
public:
	CWBSpotlight(CRect screenRect = NULL);
	~CWBSpotlight();
	BOOL CreateSpotlight(CRect rect, HWND hwnd);
	HWND GetSafeHwnd() const;


	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	//设置和获取数据对象值
	void SetItem(const PWBSpotLightItem pItem);
	void SetItem(const CRect& rect);
	PWBSpotLightItem GetItem();
	void SetRect(const CRect& rect);
	void GetRect(CRect& rect) const;

	//操作
	void ProcessSpotlight(CRect rect);  //处理放大镜同步
	void AdjustSize(CRect& newRect,bool bSeedNet);//调整大小，当属性窗口

	CSpotlightDlg *GetSpotlightDlg() {return m_pSpotlight;}
private:
	CSpotlightDlg *m_pSpotlight;
	PWBSpotLightItem m_pItem;

	XCritSec m_csSpotlightOperationLock;
};


class CWBAnnotations : public CWBObject //注释类
{
public:
	CWBAnnotations();
	CWBAnnotations(CPoint &ptTopLeft, LOGFONT &lfLogFont,COLORREF &clrText, LPCTSTR lpszText = NULL);
	CWBAnnotations(CPoint &ptTopLeft);
	CWBAnnotations(const CWBAnnotations &other);
	virtual ~CWBAnnotations();

	///add by aolingwen 2012.12.4
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);
	///end

	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取

	virtual void Draw(CDC *pDC);

	BOOL IsInPointArea(CPoint &pt)const; //判断鼠标单击点是否在点的范围内
	virtual int IsInSelectArea(CPoint& pt)const; //点选择

	virtual BOOL IsRegionIn(const CRgn *pRgn) const; //判断对象是否整个在区域内
	virtual BOOL IsRegionIntersect(const CRgn *pRgn) const; //判断对象是否与区域相交

	//创建选择框
	virtual void CreateSelecrtFrame();

	void CreatePolygonFrame(); //创建标注外框
	CWBText *GetText();
	CWBPolygon *GetPolygon();
	void SetIsChange(BOOL bIsChange);
	void SetChangePoint(CPoint ptChange);
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te);//标注中TouchUpAllCursor什么都不做
	//设置组件中的备注的尖角坐标
	void SetChangePtInCom(CPoint ptChange);

	CPoint GetChangePoint(); //得到尖角的点的坐标
	void SetDirection(); //根据尖角点坐标设置尖角方向
	void SetSrcPoint(); //记录缩放前尖角点坐标和左上角的点映射到桌面的坐标

	//创建对象区域
	virtual BOOL CreateRgn(CRgn *pRgn) const;
	virtual BOOL CreateReverseRgn(CRgn *pRgn) const; //根据对象旋转与缩放后创建区域

	//偏移量移动对象
	virtual void MoveOffset(const CPoint& ptOffset); 

	//以角度偏移旋转对象
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转

	//对象缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象

	virtual BOOL IsSolid() const; //判断对象是否填充

	//同步设置于读取XML值

	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);	
	//对象透明度操作
	virtual void SetAlpha(int alpha);
private:
	XCritSec m_csAnnotationsLock;	//备注上锁

	CWBText *m_pText;
	CWBPolygon *m_pPolygon;

	BOOL m_bIsChange; //是否改变过尖角
	CPoint m_ptChange; //尖角的点的坐标
	int m_nDirection; //尖角的方向

	CPoint m_ptSrcChange; //缩放前尖角点的坐标
	//对象缩放比例
	double m_dbXScale;
	double m_dbYScale;

	BOOL m_bIsCopy; //是否是复制的对象
};

class CWBMedia : public CWBObject
{
public:
	friend class CWBMediaWindow;
	CWBMedia(CString strFilePath = NULL, CRect rct = CRect( 74, 43, 580, 450 ), BOOL bActiveCreated = TRUE);

	CWBMedia(const CWBMedia &other, BOOL bActiveCreated = TRUE);

	~CWBMedia();
public:
	//设置同步
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;

	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	//CWBMedia  operations
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;

	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;

	virtual BOOL Load(char *pBuf, DWORD& dwLen);

	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取

	virtual void Draw(CDC *pDC);

	CString GetFilePath(){return m_pItem->strFilePath;}

	//数据拷贝
	static PWBMediaItem CopyItem(PWBMediaItem pDesItem, const PWBMediaItem pSrcItem);

	void SetItem(const PWBMediaItem pItem);

	void GetItem(PWBMediaItem pItem) const;

	//创建选择框
	virtual void CreateSelecrtFrame();
	
	void SetMediaRect(CRect rect);
	CRect GetMediaRect();
	//得到控制框指针
	CWBGlobalMediaCtrl* GetMediaCtrl() const { return m_pMediaCtrl; }

	SCameraReceiveData cameraData;
public:

	void SetMediaCtrlTip(CPoint point);
	//更新选择框
	void UpdateSelectFrame(CRect rct);

	//改变层次
	void SetZOrder();

	//控制框是否显示
	BOOL m_bIsHide;

	BOOL GetIsHide() const { return m_bIsHide ;}

	void SetHideState(BOOL isShow) { m_bIsHide = isShow; }

	void HideAndPauseMedia();

	void ShowAndPlayMedia();

	//控制框是否显示（画图）
	BOOL m_bIsMediaCtrlShow;

	BOOL GetMediaCtrlShow() const { return m_bIsMediaCtrlShow; }

	void SetMediaCtrlShow( BOOL bIsMediaCtrlShow ) { m_bIsMediaCtrlShow = bIsMediaCtrlShow; }

	//偏移量移动对象
	virtual void MoveOffset(const CPoint& ptOffset); 

	//对象缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放

	virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象

	//点选择
	virtual int IsInSelectArea(CPoint& pt)const;

	void SetMediaBeCreated(BOOL bCreated) { m_bCreated = bCreated ;}

	//判断是否需要进SET中发送通道ID里面
	void SetMediaBitrate(BOOL birate) {m_MediaBitrate = birate;}
	BOOL GetMediaBitrate() const {return m_MediaBitrate;}
	//保存被动接受的视频名称
	void SetSaveGetMediaName(CString strName){m_strGetName = strName;}
	//获取被动接受的视频名称
	CString GetSaveGetMediaName(){return m_strGetName;}
	//获取用户ID
	CString GetUCCUserID();
private:
    double m_Reduce;
	CImage m_img;
	CString m_strGetName; //被动接受的视频名称
	BOOL m_MediaBitrate;

	void ProcessMove(int nOffSetX, int nOffSetY);

private:
	//窗体
	CWBMediaWindow *m_pMediaWnd;

	//控制框
	CWBGlobalMediaCtrl *m_pMediaCtrl;

	//窗体位置信息
	CRect m_rctMediaWindow;

	//类变量，用于计数
	static unsigned int m_nStcNum;

	//多媒体数据结构
	PWBMediaItem m_pItem;

	//通道号（视频类对象的唯一标识）
	unsigned int m_nNumChannel;

	//多媒体播放状态
	EMediaPlayState m_eMediaPlayState;

	//多媒体换页之前的状态
	EMediaPlayState m_ePrePlayState;

	EVoiceState m_eVoiceState;

	//是否是主动创建的标志 TRUE 主动创建， FALSE 被动创建
	BOOL m_bActiveCreated;

	//是否已经创建，用于远程视频创建的控制，因为在使摄像头信息只传一次
	BOOL m_bCreated;

	////UCC中标识的用户ID（此视频属于该用户）
	//std::string m_strUCCUerID;

	XCritSec		m_csOpenMedia;
	int             m_SetCurVolice;//视频设置的音量值
public:
	CRect GetMoveRect();//移动视频时候视频窗口矩形
	void SetMoveRect(CRect rect);

	void OpenMedia(const char* strFilePath = NULL, BOOL bActiveCreated = TRUE);

	void CreateMedia(const char* strFilePath = NULL);

	/*获取和设置播放状态*/
	EMediaPlayState GetMediaPlayState() const;

	void SetMediaPlayState(EMediaPlayState mediaPlayState);

	/*翻页操作时视频的播放状态*/
	EMediaPlayState GetPrePlayState() const;

	void SetPrePlayState(EMediaPlayState prePlayState);

    //视频播放
	void MediaPlay();

    //视频暂停
	void MediaPause();

    //视频停止
	void MediaStop();
    
    //循环播放
	void MediaLoop();

	//获取是否循环播放信息
	BOOL GetMediaLoopState() const;
	
	//设置视频单步或循环播放状态
	void SetMediaLoopState(BOOL bMediaLoopState);

	//视频截图
	BOOL CaptureToBMPFile();

	//静音
	void MediaSetMute(EVoiceState eVoiceState);
	
	//音量调整
	void SetVedioVolume(int nVolume);

	int  GetVedioVolume() const { return m_SetCurVolice;}
	void SetCurVolume(int volume) {m_SetCurVolice = volume;}

    //获取静音设置
	EVoiceState GetVoiceState() const;

	//设置静音
	void SetVoiceState(EVoiceState voiceState);

	//进度定位
	void SeekVedioPosition(unsigned long ulSeconds);

	//获取视频的文件长度，时间衡量
	unsigned long GetVedioDuration() const;

	//获取视频当前的位置，以时间为参照
	unsigned long GetVedioPosition() const;

	CWBMediaWindow* GetMediaWindow() const; 

	//判断是否是主动创建的, TRUE主动，FALSE
	BOOL IsActiveCreated() { return m_bActiveCreated; }
	BOOL IsMdediaOpen()  {return m_pMediaWnd->IsMediaOpen();}
	CAVCapture *GetCapture() const  {return m_pMediaWnd->GetCapture();}
	//int GetBitrate() {return m_pMediaWnd->GetBitrate();}
    ImageEx* m_bkImg;
};

//同步标识
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
	//设置同步
	void SetSynchronousLable(int nType);
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	//operatiCWBMediaons
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);
	CDlgCamera* GetCameraDlg() { return m_pDlgCamera;}
	//数据拷贝
	static PWBCameraItem CopyItem(PWBCameraItem pDesItem, const PWBCameraItem pSrcItem);

	//创建选择框
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
	//偏移量移动对象
	virtual void MoveOffset(const CPoint& ptOffset);

	//获取用户ID
	CString GetUCCUserID();
	void SetCCUserID(string str) {m_strUCCUerID = str;}

	//对象缩放
	//virtual void ZoomByLeftTop(const CPoint& ptOffset); //以左上点为中心缩放
	//virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放
	//virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象
	//是否是创建者
	BOOL BCreatorOwner(){return m_bActiveCreator;};
private:
	BOOL m_bIsHide;

	CDlgCamera *m_pDlgCamera;
	//WBCameraItem *m_pItem;//m_pCameraItem
	CRect m_rctCameraWindow;
	//主创建摄像头(能进行设置)，还是被动创建摄像头（不能进行设置）
	BOOL m_bActiveCreator;
 	//UCC中标识的用户ID（此摄像头属于该用户）
 	std::string m_strUCCUerID;

	//更新选择框
	virtual void UpdateSelectFrame( CRect rct );
	//摄像头数据
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

	//计算组件中各个对象的 m_ptMapCenter 的值
	void SetComMapCenterPt();

	//创建选择框
	virtual void CreateSelecrtFrame();

	//根据偏移量移动
	virtual void MoveOffset(const CPoint& ptOffset);

	//缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);

	//以任意点位中心缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); 

	//以角度偏移旋转对象
	virtual void RotateByPoint(const CPoint& ptRotationCenter, double dbAngleOffset);//对象绕某一点旋转
	virtual void RotateByCenter(double dbAngleOffset); //对象绕中心点旋转

	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	int SetSelectedObj(CPoint& pt); //点选择
	void SetSelectedObj(DWORD dwLayerIndex); //实时同步时确定当期选择的对象

	virtual void SetSelectFrameShow(BOOL bShow); //显示选择框

	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取

	//对象透明度操作
	virtual void SetAlpha(int alpha);

	void ZoomForMidi(const CPoint& ptZoomCenter,double dbScale,bool bmag = true);
public:
	void GetSrcAnnPoint(); //得到旋转前组件中备注的尖角的点
	void GetSrcCenterPoint(); //得到旋转前组件中每个对象的旋转中心点
	void GetSrcZoomCenter(); //得到缩放前组件中每个对象的选择框左上角和右下角的点
	void GetEachSrcZoomCenter(); //得到缩放前组件中每个对象的选择框左上角的点
	void MoveAnn(const CPoint& ptOffset); //移动组件中的备注
	void ClearSrcAnnPt(); //清空旋转前组件中备注的尖角的点

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
	BOOL IsComponentsEmpty(); //组件是否为空
private:
	XCritSec m_csComponentLock;	//组件上锁

	CWBObject * m_pParent;
	std::vector<CWBObject*> m_vecComponents; //子对象列表

	std::vector<ComponentScale*> m_vecComponentScale;

	CWBObject *m_pCurSelectedObj; //当前被选中的对象

	CRgn m_rgnOffsetBound;  //子对象移动范围

private:
	std::vector<CPoint> m_ptAnnPtList; //备注的尖角的点
	std::vector<CPoint> m_ptResAnnPtList; //备注的尖角的点在组件的坐标系里的坐标

	std::vector<CPoint> m_ptSrcCenterList; //记录组件中每个对象旋转前的中心点坐标
	std::vector<CPoint> m_ptPreCenterList; //记录组件中每个对象前一次的旋转后的坐标
	double m_dbRotateAngle; //对象绕中心点旋转的角度

	std::vector<CPoint> m_ptSrcLeftTopList; //记录组件中每个对象缩放前左上角的点
	std::vector<CPoint> m_ptSrcRightBottomList; //记录组件中每个对象缩放前右下角的点
	std::vector<CPoint> m_ptPreLeftTopList; //记录组件中每个对象前一次缩放后左上角的点
	std::vector<CPoint> m_ptPreRightBottomList; //记录组件中每个对象前一次缩放后右下角的点

	//对象缩放比例
	double m_dbXScale;
	double m_dbYScale;

	std::vector<CPoint> m_ptEachSrcLeftTopList; //记录组件中每个对象每次缩放前左上角的点

	std::vector<CPoint> m_ptResObjSrcLeftTopList;     //记录组件中每个对象缩放前 左上角 的点映射在对象坐标系里的坐标

	std::vector<CPoint> m_ptResObjSrcCenterList; //记录组件中每个对象缩放前对象中心点映射在组合对象坐标系里的坐标
};


typedef struct _WBRuler    //直尺
{
	CPoint        ptRulerLT;      //直尺左上点 
// 	CPoint        ptRulerRT;      //直尺右上点
// 	CPoint        ptRulerLB;      //直尺左下点
	CPoint        ptRulerRB;      //直尺右下点
	CPoint        ptRulerMP;      //直尺测量点
	CPoint        ptRulerCT;      //直尺中心点
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
	PWBRulerItem m_pRuleItem;  //数据结构
	int m_ncountnumber;
};

/*********************量角器**************************************/

class CWBProtractor : public CWBObject  //量角器类
{
public:
	CWBProtractor(CPoint &ptLTpoint);
	~CWBProtractor();
public:
	void Draw(CDC *pDC);    //画量角器
	void setProtractorPoint(const CPoint &ptProtractorLT,const CPoint &ptProtractorRB);  //设置点坐标
	PWBProtractorItem GetProtractorItem(); //获取数据
	int PointIn(CPoint point);   //判断区域
	int MouseDown(const CPoint &point);  //处理
	int MouseMove(const CPoint &point);  //处理
	int MouseUp(const CPoint &point);   //处理
	void MoveDrawBtn(const CPoint &point);  //移动测量点
	virtual void MoveOffset(const CPoint& ptOffset); //移动
	int IsInSelectArea(CPoint& pt)const;
	int SetUnit(const int);
	void CreateSelecrtFrame();//创建选择框
	//获取生成角度的相关点数据
	CPoint GetCTpoint();
	CPoint GetRBpoint();
	CPoint GetLTpoint();
	double GetRadian();
	void SetInButton();
	bool GetInButton();
	int m_nMousePos;
private:
	PWBProtractorItem m_pProtractorItem;  //数据结构
	ImageEx *m_pImageProtractorBtn;
	ImageEx *m_pImageRotationBtn;
	ImageEx *m_pImageAttributeBtn; //属性按钮

	int m_nSetUnit;
	CPoint m_AttBTN;
	CPoint m_ptPrePoint;
	CPoint m_ptCurPoint;
	bool m_binprotractor;
	double m_angle;
	double m_RotateAngle;
	CPoint m_CTpoint;//量角器的角度中心点
	CPoint m_LTpoint;//量角器的角度移动点
	CPoint m_RBpoint;//量角器的角度右下固定点
	double m_radian;
	bool m_binButton;
}; 

//生成刻度类
class CWBAngle:public CWBObject
{
public:
	CWBAngle(CPoint &ptLTpoint, CPoint &ptRBpoint,CPoint &ptCTpoint,double &angle);
	~CWBAngle();

public:
	void Draw(CDC *pDC);          //画刻度
	void setAnglePoint(const CPoint &ptAngleLT,const CPoint &ptAngleRB,const CPoint &ptAngleCT);  //设置点坐标

	virtual void CreateSelecrtFrame();   //创建选择框
	virtual void MoveOffset(const CPoint& ptOffset);    //移动
	CRect GetAngleRect();   //获得刻度对象的Rect
	void SetItem(const PWBAngleItem pItem);      //设置属性
	void GetItem(PWBAngleItem pItem)const;       //获得属性
private:
	PWBAngleItem m_pAngleItem;
	bool m_bchange;
	CPoint m_CTpoint;//量角器的角度中心点
	CPoint m_LTpoint;//量角器的角度移动点
	CPoint m_RBpoint;//量角器的角度右下固定点
	double m_radian; //生成刻度的初始角度
};

//typedef struct _WBSetSquare    //三角板
//{
//	CPoint        ptSetSquareLT;      //三角板左上点 
//	CPoint        ptSetSquareInLT;      //三角板左上内点
//	CPoint        ptSetSquareInRB;      //三角板右下内点
//	CPoint        ptSetSquareRB;      //三角板右下点
//	//CPoint        ptSetSquareInCT;      //三角内中直角点
//	CPoint        ptSetSquareCT;      //三角板中心点
//}WBSetSquareItem,*PWBSetSquareItem; 
class CWBSetSquare : public CWBObject  //三角板
{
public:
	CWBSetSquare(CPoint &ptLTpoint);
	//CWBSetSquare(const PWBSetSquareItem  pItem = NULL);
	~CWBSetSquare();

	void Draw(CDC *pDC);    //画三角板
	void setSetSquarePoint(const CPoint &ptSetSquareLT,const CPoint &ptSetSquareRB);  //设置点坐标
	int PointIn(CPoint point);   //判断区域
	int MouseDown(const CPoint &point);  //处理
	int MouseMove(const CPoint &point);  //处理
	int MouseUp(const CPoint &point);   //处理
	//判断点是否在对象的选择范围内
	void IsInSetSquare(CPoint point);//判断是否在三角板区域
	int IsInSelectArea(CPoint& pt)const;//点选

	virtual void MoveOffset(const CPoint& ptOffset); //移动
	void MoveDrawBtn(const CPoint &point);  //移动测量点
	void MoveLengthBtn(const CPoint &point); //缩放长度
	//建立选择框
	void CreateSelecrtFrame();
	//获得中心点
	void GetCenterPoint(CPoint& ptcenter);
	CPoint GetSetSquareCT(){return m_ptRotationCenter;}
	//设置单位
	int SetUnit(const int);
	BOOL GetIsInBotton();
	void SetIsInBotton();
	CWBSetSquare * m_pSetSquare;
	BOOL m_bInButton;

private:
	PWBSetSquareItem m_pSetSquareItem;  //数据结构
	ImageEx *m_pImageRotationBtn;  //旋转按钮
	ImageEx *m_pImageZoomBtn;   //缩放按钮
	ImageEx *m_pImageAttributeBtn; //属性按钮

	//int m_ncountnumber;   //刻度数 
	int m_nSetSquareUnit;//单位转换判断变量

	//double m_angle;
	CPoint m_ptPrePoint;  //按下的点
	CPoint m_ptCurPoint;  //移动的点
	bool m_binSetSquare; //判断是否在三角板区域内
	int m_nMousePos;    //鼠标位置
	CPoint m_ptcenter; //中心点
	CPoint m_ptOffCenter;//计算偏移中心点

};
class CWBCompass : public CWBObject  //圆规
{
public:
	CWBCompass(CPoint &ptLTpoint);
	~CWBCompass();

	void Draw(CDC *pDC);    //画圆规
	//void Draw_Arc();   //圆规作画
	void setCompassPoint(const CPoint &ptCompassLT,const CPoint &ptCompassRB);  //设置点坐标
	int PointIn(CPoint point);   //判断区域
	int MouseDown(const CPoint &point);  //处理
	int MouseMove(const CPoint &point);  //处理
	int MouseUp(const CPoint &point);   //处理
	//判断点是否在对象的选择范围内
	void IsInCompass(CPoint& point);//判断是否在圆规区域
	int IsInSelectArea(CPoint& point)const;//点选

	virtual void MoveOffset(const CPoint& ptOffset); //移动
	void HorizontalZoom(const CPoint& ptOffset);//水平拉伸
	void VerticalZoom(const CPoint& ptOffset); //垂直拉伸
	//void MoveDrawBtn(const CPoint &point);  //移动测量点
	//void MoveLengthBtn(const CPoint &point); //缩放长度
	////建立选择框
	void AdjustLT_RB(CPoint &ptLT,CPoint &ptRB); //调整左下和右上点坐标
	void CreateSelecrtFrame();
	////获得中心点
	void GetCenterPoint(CPoint& ptcenter);
	////设置单位
	//int SetUnit(const int);
	//BOOL GetIsInBotton();
	//void SetIsInBotton();
	BOOL m_bInButton;

private:
	PWBCompassItem m_pCompassItem;  //数据结构
	ImageEx *m_pImageAttributeBtn; //属性按钮

	//int m_ncountnumber;   //刻度数 
	int m_nCompassUnit;//单位转换判断变量

	//double m_angle;
	CPoint m_ptPrePoint;  //按下的点
	CPoint m_ptCurPoint;  //移动的点
	bool m_bInCompass; //判断是否在圆规区域内
	int m_nMousePos;    //鼠标位置
	CPoint m_ptcenter; //中心点
	CPoint m_ptOffCenter;//计算偏移中心点

	bool m_bIsDrawArc; //判断是否画圆弧
	double m_dStartAngle; //画圆弧起始角度
	double m_dSweepAngle;  //画圆弧偏移角度
	bool m_bMaxCompass;    //圆规半径达到最大，下次MOVE直接返回，知道UP后
	CPoint m_ptDrawStart;    //画圆弧或扇形选择框的START
	CPoint m_ptDrawEnd;     //画圆弧或是扇形选择框的END	 
   //用于代替圆规结果体里变量
	double  m_dRadian;             //弧度
	double  m_dAngle;				 //角度
	double  m_dRadius;			 //半径
	double  m_dLegLength;			 //支腿长

	CPoint	m_ptCompassCT;		 //旋转中心点
	CPoint  m_ptCompassMid;        //圆规中间点
	CPoint  m_ptCompassMidUp;      //圆规中间点Up点
	CPoint  m_ptCompassMidLT;   
	CPoint  m_ptCompassMidRT;
	CPoint  m_ptCompassLT;		 //左上点
	CPoint  m_ptCompassLTUp;       //左上Up点
	CPoint  m_ptCompassLB;         //左下点
	CPoint  m_ptCompassLBUp;       //左下点Up点
	CPoint  m_ptCompassRB;		 //右下点
	CPoint  m_ptCompassRBUp;		 //右下点Up点

	CPoint  m_ptCompassHPLB;		 //右下1号点-延伸固定画笔左下点
	CPoint  m_ptCompassHPLT;		 //右下1号点-延伸固定画笔的左上点
	CPoint  m_ptCompassHPRT;		//右下1号点-延伸固定画笔右上点
	CPoint  m_ptCompassHPRB;		//右下1号点-延伸固定画笔右下点
	
	CPoint  m_ptCompassPenLT;    //画笔的左上点
	CPoint  m_ptCompassPenLB;		//画笔的左下点
	CPoint  m_ptCompassPenRT;		//画笔的右上点
	CPoint  m_ptCompassPenRB;		//画笔的右下点

	CPoint	m_ptCompassRefillRT;   //画笔的右上点
	CPoint  m_ptCompassRefillLT;   //画笔的左上点
};

class CWBArc: public CWBObject  //圆弧类
{
public:
	CWBArc(double dStartAngle, double dSweepAngle, CPoint &pt,double dRidius, CPoint &ptSelectLT, CPoint &ptSelectRB);
	CWBArc(const PWBArcItem pItem = NULL);//撤销回来被删除的对象会调用
	CWBArc(const CWBArc& other);//拷贝构造函数
	~CWBArc();
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	CWBArc & operator=(const CWBArc& other); //赋值函数//上锁

	void Draw(CDC *pDC);    //画圆弧
	virtual BOOL Save(char *pBuf, DWORD& dwLen)const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength);//从文件读取

	//判断点是否在对象选择区域内
	virtual int IsInSelectArea(const CRect& rect) const; //框选 调用IsRegionIn()函数
	virtual BOOL IsInSelectArea(CPoint& pt) const;    //点选
	virtual BOOL IsRegionIn(const CRgn *pRgn) const; ////判断对象选择框的四条边的中点是否都在框选的区域内

	void SetItem(const PWBArcItem pItem);      //设置属性
	void GetItem(PWBArcItem pItem) const;      
	static PWBArcItem CopyItem(PWBArcItem pDesItem, const PWBArcItem pSrcItem);//数据拷贝

	virtual void CreateSelecrtFrame();   //创建选择框
	virtual void MoveOffset(const CPoint& ptOffset);    //移动

	BOOL ZoomByButton(const CPoint& ptOffset);//按钮缩放 调用父类
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale);//以任意中心点缩放

	void AdjustLT_RB( CPoint &ptLT,CPoint &ptRB );

		//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	int GetLineWidth() const;
private:
	PWBArcItem  m_pWBArcItem;  //圆弧结构体
	CPoint m_ptRotationCenter;
	XCritSec m_csArcItemLock;

};

class CWBSector: public CWBObject  //扇形类
{
public:
	CWBSector(double dStartAngle, double dSweepAngle, CPoint &pt,double dRidius, CPoint &ptSelectLT, CPoint &ptSelectRB);
	CWBSector(const PWBSectorItem pItem = NULL);//撤销回来被删除的对象会调用
	CWBSector(const CWBSector& other);//拷贝构造函数
	~CWBSector();

	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);//上锁

	CWBSector & operator=(const CWBSector& other); //赋值函数//上锁

	//操作
	void Draw(CDC *pDC);    //画扇形
	virtual BOOL Save(char *pBuf, DWORD& dwLen)const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength);//从文件读取

	//判断点是否在对象选择区域内
	virtual int IsInSelectArea(const CRect& rect)const; //框选 调用IsRegionIn()函数
	virtual BOOL IsInSelectArea(CPoint& pt)const;    //点选
	virtual BOOL IsRegionIn(const CRgn *pRgn)const; ////判断对象选择框的四条边的中点是否都在框选的区域内

	void SetItem(const PWBSectorItem pItem);      //设置属性
	void GetItem(PWBSectorItem pItem)const;      
	static PWBSectorItem CopyItem(PWBSectorItem pDesItem, const PWBSectorItem pSrcItem);//数据拷贝

	virtual void MoveOffset(const CPoint& ptOffset);    //移动
	BOOL ZoomByButton( const CPoint& ptOffset );
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale);//以任意中心点缩放
	void AdjustLT_RB( CPoint &ptLT,CPoint &ptRB );
	virtual void CreateSelecrtFrame();   //创建选择框

	void SetFill(BOOL bIsFill = TRUE);//设置是否填充
	void SetFillColor(COLORREF clrFill); //设置填充颜色
	
	//区域操作rgn
	BOOL CreateRgn(CRgn *pRgn)const; //创建区域   ---未使用
	BOOL PointIn(const CPoint& pt)const; //判断点是否在多边形(填充)范围内
	
	virtual BOOL IsSolid()const; //判断对象是否填充
	
	//同步设置于读取XML值
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);

	int GetLineWidth() const;
private:
	PWBSectorItem  m_pWBSectorItem;  //扇形结构体
	//double m_dStartAngle;
	//double m_dSweepAngle; 
	//CPoint m_ptSelectLT; //圆弧选择框起点
	//CPoint m_ptSelectRB;   //圆弧选择框终点
	//CPoint m_ptDrawStart;  //圆弧起点
	//CPoint m_ptDrawEnd;    //圆弧终点
	/*CPoint m_ptCT;*/
	//double m_dRadius;
	CPoint m_ptRotationCenter;
	XCritSec m_csSectorItemLock;
	//RectF m_rectf;  //扇形外切矩形
	BOOL m_bIsFill; //是否填充
	BOOL m_bHaveFillRgn; //是否有填充区域
};

typedef struct _WBDxfItem //DxfItem
{
	CString strFilePath;
	CRect rect; //位置;
    VIEW  View; 
	COLORREF BKColor;
}WBDxfItem,*PWBDxfItem;

class CWBDxf : public CWBObject //Dxf类
{
public:
	CWBDxf(CString strFilePath = NULL, CRect rct = CRect( 0, 0, 800, 600 ));
	CWBDxf(const CWBDxf&other);
	~CWBDxf();

public:
	//设置同步
	virtual BOOL SetXmlValue(XMemIni& xmlObj) const;
	virtual BOOL GetXmlValue(XMemIni& xmlObj);
	virtual BOOL Save(char *pBuf, DWORD& dwLen) const;
	virtual BOOL SaveToFile(CFile *pFile, LONGLONG& lOff) const;
	virtual BOOL Load(char *pBuf, DWORD& dwLen);
	virtual BOOL LoadFromFile(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //从文件读取
	virtual void Draw(CDC *pDC);
	CString GetFilePath() {return m_pItem->strFilePath;}

	//平移操作
	void SetOrgPoint(CPoint _orgPoint);
	void DxfDrag(CPoint _dragPoint);

	//缩放操作
	void CWBDxf::ZoomIn(float scale);
	void CWBDxf::ZoomOut(float scale);

	//数据拷贝
	static PWBDxfItem CopyItem(PWBDxfItem pDesItem, const PWBDxfItem pSrcItem);
	void SetItem(const PWBDxfItem pItem);
	void GetItem(PWBDxfItem pItem) const;

	//创建选择框
	virtual void CreateSelecrtFrame();

	//设置是否要重画DXF
	void SetIsNeedReDraw(BOOL bIsNeedReDraw = TRUE) { m_bIsNeedReDraw = bIsNeedReDraw; };
    BOOL GetIsNeedReDraw() { return m_bIsNeedReDraw; };

public:
	//更新选择框
	void UpdateSelectFrame(CRect rct);
	void ChangBKColor(COLORREF BKColor);

	//控制框是否显示
	//BOOL m_bIsHide;
	//BOOL GetIsHide() const { return m_bIsHide ;}
	//void SetHideState(BOOL isShow) { m_bIsHide = isShow; }
	virtual void MoveOffset(const CPoint& ptOffset); 

	//对象缩放
	virtual BOOL Zoom(const CPoint& ptZoomCenter, double dbXScale, double dbYScale); //以任意点位中心缩放
	virtual BOOL ZoomByButton(const CPoint& ptOffset);//根据缩放按钮偏移缩放对象
	virtual int IsInSelectArea(CPoint& pt);

private:
	mutable bool m_bSaveSameDxf;

	CDrawing m_Drawing;
	CPoint orgPoint;

	//dxf数据结构
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
	 int m_nDrawCount;//画的次数
 };


 typedef struct _WBMidiItem
 {
	 CString strFilePath;//midi文件路径
	 int mMidiWidth;//midi的宽度
	 int mMidiHeight;//midi的高度
	 CRect rect;//WBmidi对象显示的位置
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
	//int m_nPageTotal;//z总页数
	int m_nPageindex;
	PWBMidiItem m_pItem;
	CDC m_dcMidi;//保存midi文件读取的缓存
	HMODULE m_hmd;
	XCritSec m_csMidiItemLock;
	
	CPoint m_ptPreDraw;//上次使用到的点位置
	int m_nCopyView;//从缓存中拷贝的位置
	CDC dcMidi;
	float m_nScale; 
	
	bool m_bneedInit;
	std::map<int, CWBComponent*> m_comlist;//融合组件对象
	bool m_bDrawusedll;
	//CWBText *m_pPageIndexTip;
	bool m_bshowmagbtn;//是否显示放大按钮
	bool m_bshowredubtn;//是否显示缩小按钮
	
 };
