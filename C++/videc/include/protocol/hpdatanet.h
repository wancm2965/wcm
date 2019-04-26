/*****************************************************************************
ģ����      : HPMediaNet
�ļ���      : HPMediaNet.h
����ļ�    : HPMediaNet.cpp
�ļ�ʵ�ֹ���: HPMediaSnd HPMediaRcv�ඨ��
����        : �� ��
�汾        : V1.0
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
2014/01/24  1.0         �� ��		 Create
******************************************************************************/


#ifndef _HPDATANET_H_
#define _HPDATANET_H_

#include "hptype.h"
#include "hpdef.h"
#include "ipc.h"


//ģ��汾��Ϣ�ͱ���ʱ�� �� �����Ŀ�İ汾�ͱ���ʱ��
API void HPdatanetver();
API void HPdatanetlog(u8 level);

//ģ�������Ϣ �� �����Ŀ�İ�����Ϣ
API void HPdatanethelp();


#define  DATA_TYPE     (u8)100//

#define  DATANET_NO_ERROR                   (u16)0//�ɹ�����ֵ  
#define  ERROR_DATA_NET_BASE                (u16)16000
#define  ERROR_SND_PARAM					(ERROR_DATA_NET_BASE+1)//���÷���ģ��������� 
#define  ERROR_SND_NOCREATE					(ERROR_DATA_NET_BASE+2)//����ģ��û�д���
#define  ERROR_SND_MEMORY					(ERROR_DATA_NET_BASE+3)//����ģ���ڴ��������
#define  ERROR_SND_CREATE_SOCK				(ERROR_DATA_NET_BASE+4)//����ģ�鴴��socket
#define  ERROR_RTP_SSRC                     (ERROR_DATA_NET_BASE+5)//RTPͬ��Դ����.
#define  ERROR_LOOPBUF_CREATE               (ERROR_DATA_NET_BASE+6)//��״���崴������  
#define  ERROR_RTP_NO_INIT                  (ERROR_DATA_NET_BASE+7)//RTP����Щ����û����
#define  ERROR_RTCP_NO_INIT                 (ERROR_DATA_NET_BASE+8)//RTP����Щ����û����
#define  ERROR_RTCP_SET_TIMER               (ERROR_DATA_NET_BASE+9)//RTCP���ö�ʱ������
#define  ERROR_RTP_SSRC_COLLISION           (ERROR_DATA_NET_BASE+10)//RTP ͬ��Դ����
#define  ERROR_SOCK_INIT                    (ERROR_DATA_NET_BASE+11)//socket û�г�ʼ��
#define  ERROR_H261_PACK_NUM                (ERROR_DATA_NET_BASE+12)//H261�İ���������
#define  ERROR_PACK_TOO_LEN                 (ERROR_DATA_NET_BASE+13)//���ݰ�̫��
#define  ERROR_H263_PACK_NUM                (ERROR_DATA_NET_BASE+14)//H263�İ���������
#define  ERROR_H263_PACK_TOOMUCH            (ERROR_DATA_NET_BASE+15)//H263�����ݰ�̫��

#define  ERROR_NET_RCV_PARAM                (ERROR_DATA_NET_BASE+100)//���ý���ģ���������
#define  ERROR_NET_RCV_NOCREATE             (ERROR_DATA_NET_BASE+101)//����ģ��û�д���
#define  ERROR_NET_RCV_MEMORY               (ERROR_DATA_NET_BASE+102)//����ģ���ڴ��������
#define  ERROR_RCV_RTP_CREATE               (ERROR_DATA_NET_BASE+103)//����ģ��RTP����ʧ��
#define  ERROR_RCV_RTP_CALLBACK             (ERROR_DATA_NET_BASE+104)//����ģ������RTP�ص�����ʧ��
#define  ERROR_RCV_RTP_SETREMOTEADDR        (ERROR_DATA_NET_BASE+105)//����ģ������RTPԶ�˵�ַʧ��
#define  ERROR_CREATE_LOOP_BUF              (ERROR_DATA_NET_BASE+106)//������״����ʧ��
#define  ERROR_RCV_NO_CREATE                (ERROR_DATA_NET_BASE+107)//����ģ����ն���û�д���

#define  ERROR_WSA_STARTUP                  (ERROR_DATA_NET_BASE+200)//wsastartup error
#define  ERROR_CREATE_SEMAPORE              (ERROR_DATA_NET_BASE+201)//create semapore error
#define  ERROR_SOCKET_CALL                  (ERROR_DATA_NET_BASE+202)//����socket() ��������
#define  ERROR_BIND_SOCKET                  (ERROR_DATA_NET_BASE+203)//socket �󶨳���
#define  ERROR_CREATE_THREAD                (ERROR_DATA_NET_BASE+204)//�����̳߳��� 

#define  ERROR_LOOPBUF_FULL                 (ERROR_DATA_NET_BASE+205)//ѭ��������


#define  ERROR_SET_DECRYPTKEY               (ERROR_DATA_NET_BASE+210)//���ý���keyʧ��
#define  ERROR_DECRYPT_FRAME                (ERROR_DATA_NET_BASE+212)//����һ֡ʧ��
#define  ERROR_SET_ENCRYPTKEY               (ERROR_DATA_NET_BASE+213)//���ü���keyʧ��
#define  ERROR_ENCRYPT_FRAME                (ERROR_DATA_NET_BASE+215)//����һ֡ʧ��


#define ERROR_SND_FRAME          (s32)-1;


#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE			(u8)0		//DES����ģʽ
#define AES_ENCRYPT_MODE			(u8)1		//AES����ģʽ
#define ENCRYPT_KEY_SIZE			(u8)32		//��Կ���� ȡ���еĽϴ�ֵ
#define DES_ENCRYPT_KEY_SIZE		(u8)8		//DES��Կ����
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16		//AES Mode-A ��Կ����
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24		//AES Mode-B ��Կ����
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32		//AES Mode-C ��Կ����
#endif


//Frame Header Structure
typedef struct tagDataHdr
{
    u8     *m_pData;       // ���ݻ���
    u32     m_dwDataSize;  // m_pDataָ���ʵ�ʻ����С�����С
    u32     m_dwTimeStamp; // ʱ���, ���ڽ��ն�
    u32     m_dwFrameId  ; // ����֡��ID, ���ڽ��ն�
    u32     m_dwSSRC;      // ͬ��Դ, ���ڽ��ն�
	u32     m_dwRemoteIP;  // ���Ͷ˵�ԴIP, ���ڽ��նˣ�������
}TDataHdr,*PTDataHdr;


#ifndef  MAX_NETSND_DEST_NUM
	#define  MAX_NETSND_DEST_NUM   5
#endif

#ifndef TNETSTRUCT//��ֹ��ַ�ṹ�ض���
#define TNETSTRUCT

/*�������*/
typedef struct tagNetAddr
{
    u32   m_dwRTPAddr;/*RTP��ַ(������)*/
    u16   m_wRTPPort;/*RTP�˿�(������)*/
}TNetAddr;

/*���緢�Ͳ���*/
typedef struct tagSndParam
{
    u8       m_byNum;	  //ʵ�ʵ�ַ����
    TNetAddr m_tLocalNet; //���ص�ַ��
    TNetAddr m_tRemoteNet[MAX_NETSND_DEST_NUM]; //Զ�˵�ַ��
}TSndParam;

#endif //end TNETSTRUCT


/*=============================================================================
	������		��HPDataNetInit
	����		��HP socket ��ʼ��,��ʹ���շ���֮ǰ����ô˺�����
	�㷨ʵ��	������ѡ�
	����ȫ�ֱ�����
	�������˵������
	              

	����ֵ˵�����μ������붨��
=============================================================================*/
u16 HPDataNetInit();

/*=============================================================================
	������		��HPDataNetFree
	����		���� HPDataNetInit ���Ӧ���������
	�㷨ʵ��	������ѡ�
	����ȫ�ֱ�������
	�������˵������
	              

	����ֵ˵������
=============================================================================*/
u16 HPDataNetFree();


//Frame Rcv CallBack Function
typedef   void (*PDATAPROC )(PTDataHdr pDataHdr, u32 dwContext);


class CHPInterSnd;
class CHPDataSnd
{
public:
    CHPDataSnd();
    ~CHPDataSnd();
public:
	//��������ģ��
    u16 Create (u32 dwMaxFrameSize);
	
	//�������緢�Ͳ���(���еײ��׽��ӵĴ������󶨶˿�,�Լ�����Ŀ���ַ���趨�ȶ���)
    u16 SetNetSndParam ( TSndParam tNetSndParam );

	//�Ƴ����緢�ͱ��ص�ַ����(���еײ��׽��ӵ�ɾ�����ͷŶ˿ڵȶ���)
    u16 RemoveNetSndLocalParam ();

	//���ü���key�ִ������������Ķ�̬�غ�PTֵ
    u16 SetEncryptKey( s8 *pszKeyBuf,  u16 wKeySize, 
		               u8 byEncryptPT, u8 byEncryptMode=DES_ENCRYPT_MODE );	
	//�������ݰ�
    u16 Send ( PTDataHdr pDataHdr );
    
private:
    CHPInterSnd *m_pcNetSnd;
    void         *m_hSndSynSem;  //���ڶ���ĵ��̲߳�����ͬ����
};

class CHPInterRcv;
class CHPDataRcv
{
public:
    CHPDataRcv();
    ~CHPDataRcv();
public:
	//��������ģ��
    u16 Create ( u32 dwMaxFrameSize, 
                 PDATAPROC pDataCallBackProc = NULL, 
                 u32 dwContext = 0 );
    
    //���ûص�����
    u16 SetCallBack(PDATAPROC pDataCallBackProc, u32 dwContext);

	//���ý��յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
    u16 SetNetRcvLocalParam( TNetAddr tNetParam );
	
	//�Ƴ����յ�ַ����(���еײ��׽��ӵ�ɾ�����ͷŶ˿ڵȶ���)
    u16 RemoveNetRcvLocalParam();

	//���� ��̬�غɵ� Playloadֵ
    u16 SetActivePT( u8 byRmtActivePT, u8 byRealPT );

	//���ý��ս���key�ִ�
    u16 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);
		
	//��ʼ����
    u16 StartRcv();
	//ֹͣ����
    u16 StopRcv();

private:
    CHPInterRcv *m_pcNetRcv;
    void         *m_hRcvSynSem;  //���ڶ���ĵ��̲߳�����ͬ����
};

//����ԶңTOSֵ
API s32 HPSetDataTOS(s32 nTOS);
API s32 HPGetDataTOS();

#endif

