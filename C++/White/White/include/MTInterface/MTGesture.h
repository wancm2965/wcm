#ifndef _AVCON_MTLIB_INTERFACE_GESTURE_H_
#define _AVCON_MTLIB_INTERFACE_GESTURE_H_

namespace AVCON_SDK_MTTouch
{
	///��������
	typedef enum TouchGestureType{
		TGT_DYNAMIC,//��̬���ƣ�ƽ�ơ����š���ת��
		TGT_ACLICK,
		TGT_DIRECTION,//����������
		TGT_USER_DEFINE, //�û��Զ���
	}ETouchGestureType;


	///�����Ա�ʶ(��ʱ�룬ˮƽ�ҷ���)
	typedef enum MTOrientationGes{
		MTOG_NONE,			//����
		MTOG_RIGHT,			//�ҷ���
		MTOG_RIGHT_UP_CORNER,//���ϽǷ���
		MTOG_UP,				//�Ϸ���
		MTOG_LEFT_UP_CORNER,	//���ϽǷ���
		MTOG_LEFT,				//����
		MTOG_LEFT_DOWN_CORNER,	//���½Ƿ���
		MTOG_DOWN,				//�·���
		MTOG_RIGHT_DOWN_CORNER,	//���½Ƿ���
	}EMTOrientationGes;
///��Ϸ������ư���4λ�е�λ����λ���δ洢����Ϊ�����귽��
#define	MTGETTGDIRCTION(l, n) ((DWORD)((((DWORD_PTR)(l)) >> (4 * (n - 1))) & 0x0f))

#define	MTGES_INERTIAL_BEGIN	(0x0408)
#define	MTGES_INERTIAL_END		(0x0808)
/*------------------------------------------------
					��������
				Touch Gesture Config
------------------------------------------------*/
	//���������������
#define 	TGC_TAP			(0x01)	//Tap
#define 	TGC_CLICK		(0x02)	//����
#define 	TGC_RCLICK		(0x04)	//�һ���������
#define 	TGC_DBCLICK		(0x08)	//˫��
#define 	TGC_ALL_CLICK		(TGC_TAP|TGC_CLICK|TGC_DBCLICK|TGC_RCLICK)	//���е������
#define 	TGC_COMMON_CLICK	(TGC_TAP|TGC_CLICK|TGC_DBCLICK)	//���е������

	//��̬����������
#define 	TGC_PAN			(0x01)	//ƽ��
#define 	TGC_ZOOM		(0x02)	//����
#define 	TGC_ROTATE		(0x04)	//��ת
#define 	TGC_INERTIAL	(0x08)	//����
#define 	TGC_BASIC_DYNAMIC	(TGC_PAN|TGC_ZOOM|TGC_ROTATE)	//������̬����
#define 	TGC_ALL_DYNAMIC		(TGC_BASIC_DYNAMIC|TGC_INERTIAL)//���ж�̬����

	//����������������
//#define		TGC_FEEDBACK_WAIT		(0x00) //�ȵ���ָ����ʱ��鲢����
//#define		TGC_FEEDBACK_IMMEDIATE	(0x01) //���м����������õȵ���ָ����ʱ��
	typedef struct MTDCData{
		bool bFeedbackImmediate;
		unsigned int nData;//if bFeedbackImmediate is true, the data will be ignored
		struct MTDCData(bool _bFeedbackImmediate=false, unsigned int _nData=0)
			: bFeedbackImmediate(_bFeedbackImmediate), nData(_nData){}
	}MTDCData, *PMTDCData;

/*------------------------------------------------
					���ýṹ��
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
		float xtrans, ytrans;	//x��y�����ƽ����
		float deltaRatio;		//���ű���
		float rotateAngle;		//��ת������ʱ�뷽��Ϊ����
		float xanchor, yanchor;	//���β�����֧��
		float xTransCumulative;	//x������ۼ�ƽ����
		float yTransCumulative;	//y������ۼ�ƽ����
		float deltaRatioCumulative;	//�ۼ����ű���
		float rotateAngleCumulative;//�ۼ���ת��
		unsigned short nFingerNum;	//��ǰ������ָ����
		unsigned short nCurGesMark;	//ƽ�ƣ���ת�����ŵı�ʶֵ
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
	*******		������ṹ��Ϣ		*******
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
