#ifndef _AVCON_MTLIB_INTERFACE_GESTURE_H_
#define _AVCON_MTLIB_INTERFACE_GESTURE_H_

namespace AVCON_SDK_MTTouch
{
	///手势类型
	typedef enum TouchGestureType{
		TGT_DYNAMIC,//动态手势（平移、缩放、旋转）
		TGT_ACLICK,
		TGT_DIRECTION,//方向性手势
		TGT_USER_DEFINE, //用户自定义
	}ETouchGestureType;


	///方向性标识(逆时针，水平右方向)
	typedef enum MTOrientationGes{
		MTOG_NONE,			//不定
		MTOG_RIGHT,			//右方向
		MTOG_RIGHT_UP_CORNER,//右上角方向
		MTOG_UP,				//上方向
		MTOG_LEFT_UP_CORNER,	//左上角方向
		MTOG_LEFT,				//左方向
		MTOG_LEFT_DOWN_CORNER,	//左下角方向
		MTOG_DOWN,				//下方向
		MTOG_RIGHT_DOWN_CORNER,	//右下角方向
	}EMTOrientationGes;
///组合方向手势按照4位有低位到高位依次存储。此为解析宏方法
#define	MTGETTGDIRCTION(l, n) ((DWORD)((((DWORD_PTR)(l)) >> (4 * (n - 1))) & 0x0f))

#define	MTGES_INERTIAL_BEGIN	(0x0408)
#define	MTGES_INERTIAL_END		(0x0808)
/*------------------------------------------------
					手势配置
				Touch Gesture Config
------------------------------------------------*/
	//点击类手势配置项
#define 	TGC_TAP			(0x01)	//Tap
#define 	TGC_CLICK		(0x02)	//单击
#define 	TGC_RCLICK		(0x04)	//右击（长按）
#define 	TGC_DBCLICK		(0x08)	//双击
#define 	TGC_ALL_CLICK		(TGC_TAP|TGC_CLICK|TGC_DBCLICK|TGC_RCLICK)	//所有点击手势
#define 	TGC_COMMON_CLICK	(TGC_TAP|TGC_CLICK|TGC_DBCLICK)	//所有点击手势

	//动态手势配置项
#define 	TGC_PAN			(0x01)	//平移
#define 	TGC_ZOOM		(0x02)	//缩放
#define 	TGC_ROTATE		(0x04)	//旋转
#define 	TGC_INERTIAL	(0x08)	//惯性
#define 	TGC_BASIC_DYNAMIC	(TGC_PAN|TGC_ZOOM|TGC_ROTATE)	//基本动态手势
#define 	TGC_ALL_DYNAMIC		(TGC_BASIC_DYNAMIC|TGC_INERTIAL)//所有动态手势

	//方向性手势配置项
//#define		TGC_FEEDBACK_WAIT		(0x00) //等到手指弹起时检查并反馈
//#define		TGC_FEEDBACK_IMMEDIATE	(0x01) //命中即反馈（不用等到手指弹起时）
	typedef struct MTDCData{
		bool bFeedbackImmediate;
		unsigned int nData;//if bFeedbackImmediate is true, the data will be ignored
		struct MTDCData(bool _bFeedbackImmediate=false, unsigned int _nData=0)
			: bFeedbackImmediate(_bFeedbackImmediate), nData(_nData){}
	}MTDCData, *PMTDCData;

/*------------------------------------------------
					常用结构体
------------------------------------------------*/
	typedef struct GestureTap {
		float xpos, ypos;
		GestureTap(float _xpos, float _ypos)
			: xpos(_xpos), ypos(_ypos) {}		
	}GestureTap, *PGestureTap;
	typedef struct GestureDoubleTap {
		float xpos, ypos;
		GestureDoubleTap(float _xpos, float _ypos)
			: xpos(_xpos), ypos(_ypos)	{}		
	}GestureDoubleTap, *PGestureDoubleTap;
	typedef struct GestureZoom {
		float deltaRatio;			// deltaRatio always be positive (delta/(last distance))
		float xanchor, yanchor;			// reference point(anchor point, ...)
		GestureZoom(float _xpos1, float _ypos1,float _deltaRatio)
			: xanchor(_xpos1), yanchor(_ypos1)
			, deltaRatio(_deltaRatio) {}
	}GestureZoom, *PGestureZoom;	
	typedef struct GesturePan {
		float xpos, ypos;
		GesturePan(float _xpos, float _ypos)
			: xpos(_xpos), ypos(_ypos) {}
	}GesturePan, *PGesturePan;
	typedef struct GestureRotate {
		float rotateAngle;					
		float xanchor, yanchor;	
		GestureRotate(float _xpos1, float _ypos1, float _rotateAngle)
			: xanchor(_xpos1), yanchor(_ypos1)
			, rotateAngle(_rotateAngle) {}
	}GestureRotate, *PGestureRotate;
	typedef struct GestureCombinePar {
		float xtrans, ytrans;	//x、y方向的平移量
		float deltaRatio;		//缩放比例
		float rotateAngle;		//旋转量（逆时针方向为正）
		float xanchor, yanchor;	//单次操作的支点
		float xTransCumulative;	//x方向的累计平移量
		float yTransCumulative;	//y方向的累计平移量
		float deltaRatioCumulative;	//累计缩放比例
		float rotateAngleCumulative;//累计旋转量
		unsigned short nFingerNum;	//当前触摸手指个数
		unsigned short nCurGesMark;	//平移，旋转，缩放的标识值
		GestureCombinePar(float _xtrans, float _ytrans, float _xanchor=0.0, float _yanchor=0.0, 
			float _deltaRatio = 1.0, float _rotateAngle = 0.0, float _xTransCumulative = 0.0,
			float _yTransCumulative=0.0, float _deltaRatioCumulative=0.0, float _rotateAngleCumulative=0.0,
			unsigned short _nFingerNum=1, unsigned short _nCurGesMark=TGC_PAN)
			: xtrans(_xtrans), ytrans(_ytrans), deltaRatio(_deltaRatio)
			, rotateAngle(_rotateAngle), xanchor(_xanchor), yanchor(_yanchor)
			, deltaRatioCumulative(_deltaRatioCumulative), rotateAngleCumulative(_rotateAngleCumulative)
			, xTransCumulative(_xTransCumulative), yTransCumulative(_yTransCumulative)
			, nFingerNum(_nFingerNum), nCurGesMark(_nCurGesMark){}
	}GestureCombinePar, *PGestureCombinePar;
	typedef struct GesturePressAndTap {
		float xpos1, ypos1;
		float xpos2, ypos2;
		GesturePressAndTap(float _xpos1, float _ypos1, float _xpos2, float _ypos2)
			: xpos1(_xpos1), ypos1(_ypos1), xpos2(_xpos2), ypos2(_ypos2) {}
	}GesturePressAndTap, *PGesturePressAndTap;
	typedef struct GestureTwoFingerTap {
		float xpos1, ypos1;
		float xpos2, ypos2;
		GestureTwoFingerTap(float _xpos1, float _ypos1, float _xpos2, float _ypos2)
			: xpos1(_xpos1), ypos1(_ypos1), xpos2(_xpos2), ypos2(_ypos2) {}
	}GestureTwoFingerTap, *PGestureTwoFingerTap;

	/*-------------------------------------------------
	*******		触摸点结构信息		*******
	--------------------------------------------------*/
	enum touchState{
		TOUCH_STATE_NONE,
		TOUCH_STATE_DOWN,
		TOUCH_STATE_UPDATE,
		TOUCH_STATE_UP,
	};
	//typedef struct touchState TouchState;

	struct Touch {
		Touch(int _id=0, float _xpos=0.0, float _ypos=0.0, long __time = 0) 
			: id(_id), xpos(_xpos), ypos(_ypos), time(__time) {}
		float xpos, ypos;	// screen coordinate
		long time;			// timestamp
		int id;				// use id to distinguish different points on the screen.
	};
	typedef Touch TouchDown;
	typedef Touch TouchMove;
	typedef Touch TouchUp;
	typedef Touch TouchPoint;
	typedef struct TouchStatePoint{
		TouchStatePoint(TouchPoint _touch, touchState tchState)
			: id(_touch.id), xpos(_touch.xpos), ypos(_touch.ypos), time(_touch.time) {}
		TouchStatePoint(int _id, touchState _state, float _xpos, float _ypos, long _time=0)
			: id(_id), state(_state), xpos(_xpos), ypos(_ypos),time(_time) {}
		float xpos, ypos;	// screen coordinate 
		int id;				// use id to distinguish different points on the screen.
		touchState state;	// touch state
		long time;			// time
	}TouchStatePoint, *PTouchStatePoint;
}
#endif //end of _AVCON_MTLIB_INTERFACE_gesture_H
