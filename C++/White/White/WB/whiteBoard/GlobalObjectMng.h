
#ifndef _AVCON_MUTITOUCH_CONFIG1_H
#define _AVCON_MUTITOUCH_CONFIG1_H

//对常见的三种手势的配置（config of three common gesture）
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
//	//获取实例
//	static  CC3CGestureType * GetInstance();
//	//配置
//	void Config(unsigned int type);
//	//具体配置项查询
//	BOOL IsTranslatePermit(){return (Gesture_Translate == (m_gestureConfig & Gesture_Translate));};
//	BOOL IsZoomPermit(){return (Gesture_Zoom == (m_gestureConfig & Gesture_Zoom));};
//	BOOL IsRotatePermit(){return (Gesture_Rotate == (m_gestureConfig & Gesture_Rotate));};
//
//private:
//	//当前实例
//	static CC3CGestureType m_instance;
//	//配置值
//	unsigned int m_gestureConfig;
//};

//一些全局性的对象管理,特别是那些一旦应用程序创建就不改变的变量
class CGlobalObjMng {
private:
	CGlobalObjMng();
	~CGlobalObjMng();

public:
	//获取实例
	static  CGlobalObjMng * GetInstance();
	//配置所有白板对象的所有父窗口句柄
	void ConfigWBParentHwnd(HWND hWnd);
	//具体配置项查询
	HWND GetWBParentHwnd(){return m_hWnd;}

	//触摸授权
	BOOL IsMTAuthorize(){return m_bMTAuthorize;}
	void AuthorizeMT(BOOL bFunctionEnabele){m_bMTAuthorize=bFunctionEnabele;};

private:
	//当前实例
	static CGlobalObjMng m_instance;
	//配置值
	HWND m_hWnd;

	//触摸授权功能
	BOOL m_bMTAuthorize;
};

#endif
