
#ifndef _AVCON_MUTITOUCH_CONFIG1_H
#define _AVCON_MUTITOUCH_CONFIG1_H

//�Գ������������Ƶ����ã�config of three common gesture��
typedef enum config3GestureType{
	Gesture_Translate = 1,
	Gesture_Zoom = (1<<1),
	Gesture_Rotate = (1<<2),
}EConfig3GestureType;


////config 3(three) common gesture type class.
//class CC3CGestureType {
//private:
//	CC3CGestureType();
//    ~CC3CGestureType();
//
//public:
//	//��ȡʵ��
//	static  CC3CGestureType * GetInstance();
//	//����
//	void Config(unsigned int type);
//	//�����������ѯ
//	BOOL IsTranslatePermit(){return (Gesture_Translate == (m_gestureConfig & Gesture_Translate));};
//	BOOL IsZoomPermit(){return (Gesture_Zoom == (m_gestureConfig & Gesture_Zoom));};
//	BOOL IsRotatePermit(){return (Gesture_Rotate == (m_gestureConfig & Gesture_Rotate));};
//
//private:
//	//��ǰʵ��
//	static CC3CGestureType m_instance;
//	//����ֵ
//	unsigned int m_gestureConfig;
//};

//һЩȫ���ԵĶ������,�ر�����Щһ��Ӧ�ó��򴴽��Ͳ��ı�ı���
class CGlobalObjMng {
private:
	CGlobalObjMng();
	~CGlobalObjMng();

public:
	//��ȡʵ��
	static  CGlobalObjMng * GetInstance();
	//�������аװ��������и����ھ��
	void ConfigWBParentHwnd(HWND hWnd);
	//�����������ѯ
	HWND GetWBParentHwnd(){return m_hWnd;}

	//������Ȩ
	BOOL IsMTAuthorize(){return m_bMTAuthorize;}
	void AuthorizeMT(BOOL bFunctionEnabele){m_bMTAuthorize=bFunctionEnabele;};

private:
	//��ǰʵ��
	static CGlobalObjMng m_instance;
	//����ֵ
	HWND m_hWnd;

	//������Ȩ����
	BOOL m_bMTAuthorize;
};

#endif
