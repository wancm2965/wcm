//AVCONRTC.h
#ifndef __AVCONRTC_H__
#define __AVCONRTC_H__

#ifdef AVCONRTC_EXPORTS
#define AVCONRTC_API __declspec(dllexport)
#elif AVCONRTC_DLL
#define AVCONRTC_API __declspec(dllimport)
#else
#define AVCONRTC_API
#endif


#define AVCONRTC_ERROR_OK				0x00000000
#define AVCONRTC_ERROR_USER_ID			0x80000001
#define AVCONRTC_ERROR_PASSWORD			0x80000002
#define AVCONRTC_ERROR_SERVER			0x80000003
#define AVCONRTC_ERROR_LOGIN			0x80000004
#define AVCONRTC_ERROR_LICENCE			0x80000005
#define AVCONRTC_ERROR_PARAM			0x80000006
#define AVCONRTC_ERROR_ROOM				0x80000007
#define AVCONRTC_ERROR_CONNECTION		0x80000008
#define AVCONRTC_ERROR_CHANNEL			0x80000009

class AVCONRTC_API AVCONRTC
{
public:
	AVCONRTC(void);
	virtual~AVCONRTC(void);
public:
	static void Initialize(void);
	static void UnInitialize(void);
public:
	/*
	���ܣ������û�
	���룺	cszUserID	�û��ʺ�
			cszPassword	�û�����
	�������
	���أ���
	*/
	virtual void SetUser(const char*cszUserID,const char*cszPassword);
	
	/*
	���ܣ����÷�����
	���룺	cszIP	��������ַ
			nPort	�������˿�
	�������
	���أ���
	*/
	virtual void SetServer(const char*cszIP,unsigned short nPort);

	/*
	���ܣ�����¼��·��
	���룺	cszPath	¼��·��
	�������
	���أ���
	*/
	virtual void SetRecordPath(const char*cszPath);

	/*
	���ܣ���������·��
	���룺	cszPath	����·��
	�������
	���أ���
	*/
	virtual void SetSnapshotPath(const char*cszPath);
	
	/*
	���ܣ�������Ƶ�豸
	���룺	nMicDevID	¼���豸ID��ȱʡΪ-1��Ĭ��¼���豸��
			nSpkDevID	�����豸ID��ȱʡΪ-1��Ĭ��¼���豸��
	�������
	���أ���
	*/
	virtual void SetAudioDev(int nMicDevID=-1,int nSpkDevID=-1);
	
	/*
	���ܣ�������Ƶ�豸
	���룺	nDevID		��Ƶ�ɼ��豸ID��ȱʡΪ0����һ���ɼ��豸��
			nWidth		��Ƶ��ȡ�176X144,320X240,352X288,640X480,704X576��
			nHeight		��Ƶ�߶ȡ�1280X720,1920X1080��
			nFrameRate	��Ƶ֡�ʡ�1��5,10,15,30��
			nBitrate	��Ƶ��������λΪKbps,����:32,64,128,256,384,512,768,1024��
	�������
	���أ���
	*/
	virtual void SetVideoDev(int nDevID=0,int nWidth=640,int nHeight=480,int nFrameRate=30,int nBitrate=512);
	
	/*
	���ܣ���¼
	���룺��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int Login(void);

	/*
	���ܣ�ע��
	���룺��
	�������
	���أ���
	*/
	virtual void Logout(void);

	/*
	���ܣ����뷿��
	���룺	cszRoomID	����ID
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int JoinRoom(const char*cszRoomID);
	
	/*
	���ܣ��뿪����
	���룺	cszRoomID	����ID
	�������
	���أ���
	*/
	virtual void LeaveRoom(const char*cszRoomID);
	
	/*
	���ܣ���ʼ���ض��û��Խ�
	���룺	cszUserID	�û��ʺ�
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int StartPushToTalkToUser(const char*cszUserID);
	
	/*
	���ܣ�ֹͣ���ض��û��Խ�
	���룺	cszUserID	�û��ʺ�
	�������
	���أ���
	*/
	virtual void StopPushToTalkToUser(const char*cszUserID);
	
	/*
	���ܣ���ʼ���ض�����˵��
	���룺	cszRoomID	����ID
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int StartPushToTalkToRoom(const char*cszRoomID);
	
	/*
	���ܣ�ֹͣ���ض�����˵��
	���룺	cszRoomID	����ID
	�������
	���أ���
	*/
	virtual void StopPushToTalkToRoom(const char*cszRoomID);

	/*
	���ܣ��򿪱����豸
	���룺	hParentWnd	��ʾ��Ƶ�ĸ����ھ��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int OpenLocalDev(HWND hParentWnd);
	
	/*
	���ܣ��رձ����豸
	���룺��
	�������
	���أ���
	*/
	virtual void CloseLocalDev(void);
	
	/*
	���ܣ����ñ�����Ƶ������ʾλ��
	���룺	x	���ڵ����λ��
			y	���ڵ��ϱ�λ��
			cx	���ڵĿ��
			cy	���ڵĸ߶�
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int SetLocalDevWndPos(int x,int y,int cx,int cy);
	
	/*
	���ܣ����û��豸
	���룺	cszUserID	�û��ʺ�
			hParentWnd	��ʾ��Ƶ�ĸ����ھ��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int OpenUserDev(const char*cszUserID,HWND hParentWnd);
	
	/*
	���ܣ��ر��û��豸
	���룺	cszUserID	�û��ʺ�
	�������
	���أ���
	*/
	virtual void CloseUserDev(const char*cszUserID);
	
	/*
	���ܣ������û���Ƶ������ʾλ��
	���룺	cszUserID	�û��ʺ�
			x	���ڵ����λ��
			y	���ڵ��ϱ�λ��
			cx	���ڵĿ��
			cy	���ڵĸ߶�
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int SetUserDevWndPos(const char*cszUserID,int x,int y,int cx,int cy);
	
	/*
	���ܣ�������Ƶ����
	���룺��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int Snapshot(void);
	
	/*
	���ܣ����ض��û�Զ����Ƶ���ա�ͼƬ�������û����ء�
	���룺	cszUserID	�û��ʺ�
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int Snapshot(const char*cszUserID);
	
	/*
	���ܣ���ʼ������Ƶ¼��
	���룺��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int StartRecording(void);
	
	/*
	���ܣ�ֹͣ������Ƶ¼��
	���룺��
	�������
	���أ���
	*/
	virtual void StopRecording(void);
	
	/*
	���ܣ���ʼ���ض��û���Ƶ¼��¼�񱣴����û����ء�
	���룺	cszUserID	�û��ʺ�
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int StartRecording(const char*cszUserID);
	
	/*
	���ܣ�ֹͣ���ض��û���Ƶ¼��
	���룺	cszUserID	�û��ʺ�
	�������
	���أ���
	*/
	virtual void StopRecording(const char*cszUserID);
	
	/*
	���ܣ��������ݸ��ض��û�
	���룺	cszUserID	�û��ʺ�
			cszData		�ַ�������ָ��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int SendDataToUser(const char*cszUserID,const char*cszData);
	
	/*
	���ܣ��������ݸ��ض�����
	���룺	cszRoomID	�����
			cszData		�ַ�������ָ��
	�������
	���أ��ɹ�����0�����򷵻ش�����
	*/
	virtual int SendDataToRoom(const char*cszRoomID,const char*cszData);

	/*
	���ܣ����������������¼�
	���룺��
	�������
	���أ���
	*/
	virtual void OnServerConnecting(void);

	/*
	���ܣ��������������¼�
	���룺��
	�������
	���أ���
	*/
	virtual void OnServerConnnected(void);

	/*
	���ܣ����������ӶϿ��¼�
	���룺��
	�������
	���أ���
	*/
	virtual void OnServerDisconnected(void);

	/*
	���ܣ���¼�ɹ��¼�
	���룺��
	�������
	���أ���
	*/
	virtual void OnLogin(void);

	/*
	���ܣ�ע���¼���������¼ʧ���¼���
	���룺	nCode			������
			cszDescription	��������
	�������
	���أ���
	*/
	virtual void OnLogout(unsigned long nCode,const char*cszDescription);

	/*
	���ܣ��û����뷿���¼��������Լ����뷿����¼���
	���룺	cszRoomID		�����
			cszUserID		�û��ʺ�
	�������
	���أ���
	*/
	virtual void OnUserJoinRoom(const char*cszRoomID,const char*cszUserID);

	/*
	���ܣ��û��뿪�����¼�
	���룺	cszRoomID		�����
			cszUserID		�û��ʺ�
	�������
	���أ���
	*/
	virtual void OnUserLeaveRoom(const char*cszRoomID,const char*cszUserID);
	
	/*
	���ܣ��û���ʼ�Խ��¼�
	���룺	cszUserID		�û��ʺ�
	�������
	���أ���
	*/
	virtual void OnStartPushToTalkFromUser(const char*cszUserID);
	
	/*
	���ܣ��û�ֹͣ�Խ��¼�
	���룺	cszUserID		�û��ʺ�
	�������
	���أ���
	*/
	virtual void OnStopPushToTalkFromUser(const char*cszUserID);
	
	/*
	���ܣ����俪ʼ˵���¼�
	���룺	cszRoomID		�������
			cszUserID		�û��ʺ�
	�������
	���أ���
	*/
	virtual void OnStartPushToTalkFromRoom(const char*cszRoomID,const char*cszUserID);
	
	/*
	���ܣ�����ֹͣ˵���¼�
	���룺	cszRoomID		�������
			cszUserID		�û��ʺ�
	�������
	���أ���
	*/
	virtual void OnStopPushToTalkFromRoom(const char*cszRoomID,const char*cszUserID);

	/*
	���ܣ���ʼ¼���¼�
	���룺	cszPathName		¼���ȫ·��
	�������
	���أ���
	*/
	virtual void OnStartRecording(const char*cszPathName);

	/*
	���ܣ�ֹͣ¼���¼�
	���룺	cszPathName		¼���ȫ·��
	�������
	���أ���
	*/
	virtual void OnStopRecording(const char*cszPathName);

	/*
	���ܣ������¼�
	���룺	cszPathName		ͼ����Ƭ��ȫ·��
	�������
	���أ���
	*/
	virtual void OnSnapshot(const char*cszPathName);
	
	/*
	���ܣ��յ��û����͹����������¼�
	���룺	cszUserID	�û��ʺ�
			pData		����ָ��
			nLen		���ݳ���
	�������
	���أ���
	*/
	virtual void OnRcvdDataFromUser(const char*cszUserID,const char*pData,int nLen);
	
	/*
	���ܣ��յ����鷢�͹����������¼�
	���룺	cszUserID	�û��ʺ�
			pData		����ָ��
			nLen		���ݳ���
	�������
	���أ���
	*/
	virtual void OnRcvdDataFromRoom(const char*cszRoomID,const char*cszUserID,const char*pData,int nLen);
};



#endif