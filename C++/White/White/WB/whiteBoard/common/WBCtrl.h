#pragma once
#include "../ImageEx.h"
#include "WBData.h"
#include "../WBToolTip.h"

#define  BUTTONSIZE 40
#define  FLASHBUTTONSIZE    60
#define  FLASHBUTTONOFFSET  20
#define  BAFFLE_BTN_WIDTH   74    //挡板按钮的宽度
#define  BAFFLE_BTN_HIGHT   20    //挡板按钮的高度
#define  BAFFLE_BTN_SPACE   5      //挡板按钮离挡板边上的距离
#define  BAFFLE_CLOSEBTN_WIDTH 40   //挡板关闭按钮的宽度
#define  BAFFLE_CLOSEBTN_HIGHT 40   //挡板关闭按钮的高度

class CWBObject;
//class CWBRectMaqn;
class CSpotlightDlg;

//图片按钮类
class CWBObject;
class CWBImageButton
{
public:
	CWBImageButton(CWBObject *pWBObject = NULL);
	virtual ~CWBImageButton();

	void Draw(CDC *pDC);	//画按钮	

	void ButtonDown(CPoint point);	//按钮按下进行的操作
	void ButtonMove(CPoint point);	//按钮移动进行的操作
	void ButtonUp(CPoint point);	//按钮弹起进行的操作

	BOOL IsInButtonArea(const CPoint& pt); //判断是否按中按钮 
	void setOffsetCenter(CPoint ptoffset);
	void ChangeRationCenterOperator();
	void SetObjRationCenterpt(CPoint pt);
	bool InitImageFromFile();	//初始化每个按钮的图片	
	bool InitImageFromResource();   //从资源初始化
	void ClearImage();         // 删除图片
	void GetObjRationCenterPt();
	CPoint GetMenuPt(); //得到点击菜单按钮的点
protected:	
	//按钮类型从1开始标识
	enum BTN_TYPE
	{	
		BTN_TYPE_BEGIN = 1,//直线起点按钮
		BTN_TYPE_END,      //直线终点按钮

		BTN_TYPE_LOCK,     //锁定按钮
		BTN_TYPE_ROTATION, //旋转按钮
		BTN_TYPE_MENU,     //菜单按钮
		BTN_TYPE_ZOOM,	   //缩放按钮
		BTN_TYPE_ROTATIONCENTER
	};

	int RotateXForm(double dbAngle); //旋转角度
	void ResumeXForm(int nGraphicsMode); //恢复角度
private:
	ImageEx *m_pImageLock; //锁定按钮图片
	ImageEx *m_pImageUnLock;	//解锁按钮图片
	ImageEx *m_pImageRotation;	//旋转按钮图片
	ImageEx *m_pImageMenu;	//菜单按钮图片
	ImageEx *m_pImageZoom;	//缩放或直线起点、终点按钮图片	
	ImageEx *m_pImageClone;  //无限克隆图片
	ImageEx *m_pImageAnnotations; //标注尖角图片
	ImageEx *m_pImageRotationCenter;

	CWBObject *m_pWBObject;	//按钮所对应的对象指针
	CDC *m_pDC;	//保存DC
	bool m_onlyone;
	int m_nType;
	BOOL m_bRotation; //旋转标识
	BOOL m_bZoom; //缩放标识
	CPoint m_ptRotationPre;// 旋转起点
	CPoint m_ptZoomPre;
	CPoint m_ptObjCenter; //对象中点
	CPoint m_ptRotationCenter; //旋转中心点
	CPoint m_ptMenu; //点击菜单按钮时的点
	XCritSec			m_csImageButtonMoveObjLock;	//组合构造上锁
	bool m_bIsImageMove;//图片按钮移动视频过程暂停视频播放
	BOOL m_bMoveRotionCenter; //移动旋转中心点标记
	CPoint m_ptMoveRotationCenter; //
	CPoint m_ptOffsetCenter;
	bool m_bCenterMove;
	CPoint m_ptcenterone;
	BOOL m_ptIsRorationType;
};
/*
//图片关闭按键类，用于放大镜关闭按键
class CWBImgCloseButton
{
public:
	CWBImgCloseButton();
	CWBImgCloseButton(CWBObject *pWBObject,UINT uBtnType);//制定操作类指针和按键类型，可以根据按键类型，确定操作类。
	//CWBImgCloseButton(CWBRectMaqn* WBRectMaqn,int nBtnCount);
	virtual ~CWBImgCloseButton();

	void Draw(CDC *pDC);	//画按钮	
	BOOL IsInButtonArea(const CPoint& pt); //判断是否按中按钮 
	void ButtonDown(CPoint point);	//按钮按下进行的操作
	void ButtonMove(CPoint point);	//按钮移动进行的操作
	void ButtonUp(CPoint point);	//按钮弹起进行的操作
	//void ClearImage();         // 删除图片
	void SetButtonImage(CString filename);//设置按钮图片

public:
	CWBObject *m_pWBObject;	//按钮所对应的对象指针
	//Bitmap *m_pImg;           //图像指针
	ImageEx *m_pImg;
	//int m_nBtnCount; 
	BOOL m_bMoveFlag;         //是否移动
	CPoint m_pCurPoint;       //当前光标位置
	BOOL m_IsUpOrDown;        //
	BOOL m_IsLeftOrRight;
	CDC *m_pDC;	//保存DC
};
*/

//对象控制按钮类（对视频或FLASH及文档的控制）
class CWBControlButton
{
public:
	CWBControlButton(CWBObject *pWBObject = NULL);
	virtual ~CWBControlButton();

	void Draw(CDC *pDC);	//画按钮	

	void ButtonDown(CPoint point);	//按钮按下进行的操作
	void ButtonMove(CPoint point);	//按钮移动进行的操作
	void ButtonUp(CPoint point);	//按钮弹起进行的操作
	void FlashTip(CPoint point);
	void DocTip(CPoint point);      //文档操作按钮提示
	BOOL IsInButtonArea(const CPoint& pt); //判断是否按中按钮 
	void MidiTip(CPoint point);
protected:	
	//按钮类型从1开始标识
	enum BTN_TYPE
	{	
		BTN_TYPE_START = 1,//播放按钮
		BTN_TYPE_STOP,      //停止按钮
		BTN_TYPE_CIRCLE,		//循环按钮
		BTN_TYPE_CATCH,		//视频截图按钮

		BTN_TYPE_FIRSTPAGE,    //文档的回到首页按钮
		BTN_TYPE_LASTPAGE,     //文档的尾页按钮
		BTN_TYPE_PREPAGE,      //文档的上一页按钮
		BTN_TYPE_NEXTPAGE,	   //文档的下一页按钮
		BTN_TYPE_PRINTPAGE,	   //文档的打印按钮

		BTN_TYPE_TOP,          //挡板顶部把手
		BTN_TYPE_LETE,         //挡板左边把手
		BTN_TYPE_RIGHT,        //挡板右边把手
		BTN_TYPE_BOTTOM,        //挡板底部把手
		BTN_TYPE_CLOSEPATETTE,   //挡板关闭按钮

		BTN_TYPE_MAQNCLOSE,  //放大器关闭按钮

		BTN_TYPE_DXF_ZOOMOUT,// dxf文件缩小
		BTN_TYPE_DXF_ZOOMIN, //dxf文件放
		BTN_TYPE_DXF_MOVE, //dxf文件移动

		BTN_TYPE_MIDI_UP,
		BTN_TYPE_MIDI_DOWN,
		BTN_TYPE_MIDI_PRE,
		BTN_TYPE_MIDI_NEXT,
		BTN_TYPE_MIDI_FIRST,
		BTN_TYPE_MIDI_LAST
	};

	void InitImageFromFile();	//从文件初始化图片
	void InitImageFromResource();	//从资源初始化图片

	int RotateXForm(double dbAngle); //旋转角度
	void ResumeXForm(int nGraphicsMode); //恢复角度
	 
private:
	ImageEx *m_pImageBackground;	//视频背景
	ImageEx *m_pImageStart;       //开始按钮图片
	ImageEx *m_pImagePushStart;	  //按下开始按钮的图片
	ImageEx *m_pImagePause;	      //暂停按钮图片
	ImageEx *m_pImagePushPause;	  //按下暂停按钮图片
	ImageEx *m_pImageStop;	      //停止按钮图片
	ImageEx *m_pImagePushStop;	  //按下暂停按钮图片
	ImageEx *m_pImageIsStop;	  //视频已经停止时显示的图片	
	ImageEx *m_pImageCircle;	  //循环按钮图片
	ImageEx *m_pImagePushCircle;	//按下循环按钮时的图片
	ImageEx *m_pImageIsCircle;	//视频已经处于循环状态时的图片
	ImageEx *m_pImageCatch;		//截取视频画面按钮的图片

	ImageEx *m_pImageFirstPage;	//文档首页按钮图片
	ImageEx *m_pImageLastPage;	//文档尾页按钮图片
	ImageEx *m_pImagePrePage;	//文档上一页按钮图片
	ImageEx *m_pImageNextPage;	//文档下一页按钮图片
	ImageEx *m_pImagePrintCur;	//打印文档当前页的图片

	ImageEx *m_pImageDxfZoomIn; //放大Dxf文件的按钮
	ImageEx *m_pImageDxfZoomOut; //缩小DXf文件的按钮
	ImageEx *m_pImageDxfMove; //Dxf文件的移动按钮

	ImageEx *m_pImageTopHandle;      //挡板顶部把手图片
	ImageEx *m_pImageLeftHandle;     //挡板左边把手图片
	ImageEx *m_pImageRightHandle;    //挡板右边把手图片
	ImageEx *m_pImageBottomHandle;   //挡板底部把手图片
	ImageEx *m_pImageBaffleClose;   //挡板关闭按钮

	ImageEx *m_pImageMaqnClose; //关闭放大镜的按钮

	ImageEx* m_pImageMIDIUp;
	ImageEx* m_pImageMIDIDown;
	ImageEx* m_pImageMidiPre;
	ImageEx* m_pImageMideNext;
	ImageEx* m_pImageMidiFirst;
	ImageEx* m_pImageMidiLast;
	ImageEx *m_pImageMidiPrint;
	CWBObject *m_pWBObject;	//按钮所对应的对象指针
	CDC *m_pDC;	//保存DC
	int m_nType;
	BOOL m_bIsButtonDown;	//记录鼠标是否按下
	//挡板按钮单击下去的点，即拖动挡板的开始点
	CPoint m_baffleBeginPt;
	int m_bShowDocTip;
};
