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



//����֪ͨ��Ϣ
class IKeyBoard_Notify
{
public:
	virtual bool keyboard_bind(int monitor, int camera)=0;							//��&���� һ��ͨ���󶨵�һ����Ļ


	//����ѡ��һ����Ļ��������̨���� idx:��Ļid 
	//param:>-1 ���Ͳ������� ��:һ��������  
	//		-1 ����һֱ����	 ��:ֹͣ
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