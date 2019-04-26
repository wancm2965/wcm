#ifndef ITVWALL_H
#define ITVWALL_H
#ifdef WIN32
#ifdef TVWALL_EXPORTS
#define TVW_EXT_API __declspec(dllexport)
#else
#define TVW_EXT_API __declspec(dllimport)
#endif
#else
#define TVW_EXT_API extern 
#endif
typedef enum
{
	VIDEOIN_CAM=0,
	VIDEOIN_HP,
	VIDEOIN_HIK,
	VIDEOIN_DH
}videoin_type;
typedef struct 
{
	int camera;
	videoin_type type;
	int monitor;
	char *name;
}videoin_info;
/*
	ITVWall_Videoin		ITVWall_Server		ITVWall_Client

*/
//////////////////////////////////////////////////////////////////////////
//				�������
//////////////////////////////////////////////////////////////////////////
class ITVWall_Server_Notify
{
public:
	virtual void tvws_update(int camera)=0;
	virtual void tvws_alarm(int alarm/*�ź�Դ*/, int type/*�ź�����*/, int value/*�ź�ǿ��*/)=0;
	//Add 20080818 Jee
	virtual void tvws_decoder_in(const char*ip, unsigned short port)=0;
	virtual void tvws_decoder_out(const char*ip, unsigned short port)=0;
	//�����ͻ
	virtual void tvws_conflict(int idx, const char* ip, unsigned short port, const char* ip2, unsigned short port2)=0;
};

class ITVWall_Server
{
public:
	virtual bool connect(int camera_total, int monitor_total, int comport=1, const char *ip=NULL, unsigned short port=4231)=0;
	virtual void release()=0;

	// for client
	virtual bool scan(int monitor, int dur)=0;	//��ĳ������ѭ����ʾ��������
	virtual bool bind(int monitor, int camera)=0;	//��&����
	virtual bool ptzctrl(int camera, int cmd, int param)=0;

	virtual int get_camera_count()=0;
	virtual videoin_info *get_camera(int camera)=0;
	virtual int get_monitor_count()=0;
	virtual videoin_info *get_monitor(int monitor)=0;
	virtual void set_camera(videoin_info *p, int n)=0;
	//Add 20080818 Jee
	virtual void remove(int monitor)=0;
};

//////////////////////////////////////////////////////////////////////////
//			������Ƶ����
//////////////////////////////////////////////////////////////////////////
class ITVWall_Videoin_Notify
{
public:
	virtual void tvwv_disconnected()=0;
	virtual void tvwv_connected()=0;
	// idx=channel, cmd='L' 'R' 'U' 'D', param = speed
	virtual void tvwv_ptzctrl(int idx, int cmd, int param)=0;//����ǽ��������̨����

	//ok=falseʱΪͨ������ʧ�ܣ���ITVWall_Videoin::remove()���û����ͨ�������ͻ����
	//idx=-1Ϊ����ͨ��
	virtual void tvwv_update(int idx, int dev_type, bool ok)=0;
};
class ITVWall_Videoin
{
public:
	virtual bool connect(const char *serverip, unsigned short serverport=4231, const char *localip=NULL, unsigned short localport=4230, const char *username =NULL , const char *password =NULL)=0;
	virtual void release()=0;
	virtual void update(int idx, int dev_type, const char *name, bool enable_ptzctrl=false)=0;
	virtual void remove(int idx)=0;//idx=-1 RemoveAll
	//+ 20080902 Jee
	// keyboard
	virtual bool connect_keyboard(int nComport, int nBaud/*9600*/)=0;
	virtual void release_keyboard()=0;
	//
};

//////////////////////////////////////////////////////////////////////////
//			�������
//////////////////////////////////////////////////////////////////////////
//+ 20080902 Jee
// keyboard
class ITVWall_Client_Notify
{
public:
	virtual void tvwc_disconnected()=0;
	virtual void tvwc_connected()=0;
	virtual void tvwc_update(int camera, const char *param)=0;
	virtual void tvwc_alarm(int alarm/*�ź�Դ*/, int type/*�ź�����*/, int value/*�ź�ǿ��*/)=0;
	virtual void tvwc_decoder_in(const char*ip, unsigned short port)=0;
	virtual void tvwc_decoder_out(const char*ip, unsigned short port)=0;
	virtual void tvwc_conflict(int idx, const char* ip, unsigned short port, const char* ip2, unsigned short port2)=0;
};

class ITVWall_Client
{
public:
	virtual bool connect(const char *serverip, unsigned short serverport=4231, const char *username =NULL , const char *password =NULL, const char *localip=NULL, unsigned short localport=4232)=0;
	virtual void release()=0;

	// hard_keyboard
	virtual bool connect_keyboard(int nComport, int nBaud/*9600*/)=0;
	virtual void release_keyboard()=0;

	// soft_keyboard
	virtual bool scan(int monitor, int dur)=0;	//��ĳ������ѭ����ʾ��������
	virtual bool bind(int monitor, int camera)=0;	//��&����
	virtual bool ptzctrl(int camera, int cmd, int param)=0;
	virtual void remove(int monitor)=0;

	// TVWall_Server configure
	virtual int get_camera_count()=0;
	virtual videoin_info *get_camera(int camera)=0;
	virtual int get_monitor_count()=0;
	virtual videoin_info *get_monitor(int monitor)=0;
	virtual void set_camera(videoin_info *p, int n)=0;
};
TVW_EXT_API ITVWall_Server* CreateTVWServer(ITVWall_Server_Notify *p);
TVW_EXT_API ITVWall_Videoin* CreateTVWVideoin(ITVWall_Videoin_Notify *p=NULL);
TVW_EXT_API ITVWall_Videoin* CreateTVWClient(ITVWall_Client_Notify *p=NULL);
TVW_EXT_API void EnableTVWLog(bool enable);
#endif