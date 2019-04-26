#ifndef IKEYBOARD_H
#define IKEYBOARD_H
#ifdef WIN32
#ifdef KEYBOARD_EXPORTS
#define KEYBOARD_EXT_API __declspec(dllexport)
#else
#define KEYBOARD_EXT_API __declspec(dllimport)
#endif
#else
#define KEYBOARD_EXT_API extern 
#endif



//键盘通知消息
class IKeyBoard_Notify
{
public:
	virtual bool keyboard_bind(int monitor, int camera)=0;							//流&屏绑定 一个通道绑定到一个屏幕


	//键盘选择一个屏幕触发的云台控制 idx:屏幕id 
	//param:>-1 发送操作命令 如:一个左命令  
	//		-1 代表一直操作	 如:停止
	virtual void keyboard_ptzctrl(int camera, int cmd, int param)=0;			

};

class IKeyBoard
{
public:
	virtual bool Connect()=0;							
	virtual void Release()=0;
	virtual bool	StartIO(int nComport, int nBaud)=0;
	virtual int		StopIO()=0;

};

KEYBOARD_EXT_API IKeyBoard* CreateKeyBoard(IKeyBoard_Notify *pNotify=0);

#endif