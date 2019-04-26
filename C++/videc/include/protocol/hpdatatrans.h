/*****************************************************************************
ģ����      : HpDataTrans
�ļ���      : HpDataTrans.h
����ļ�    : HpDataTrans.cpp
�ļ�ʵ�ֹ���: HpMediaSnd HpMediaRcv�ඨ��
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#ifndef _HPDATATRANS_H_
#define _HPDATATRANS_H_



//ģ��汾��Ϣ�ͱ���ʱ��
API void hpdatatransver();
API void HpGetDatatransVer(char** ppVerionString);



#define  DATATRANS_NO_ERROR                  (u16)0//�ɹ�����ֵ  
#define  ERROR_DATA_TRANS_BASE               (u16)50000
#define  ERROR_SND_PARAM                    (ERROR_DATA_TRANS_BASE+1)//���÷���ģ��������� 
#define  ERROR_SND_NOCREATE	                (ERROR_DATA_TRANS_BASE+2)//����ģ��û�д���
#define  ERROR_SND_MEMORY	                (ERROR_DATA_TRANS_BASE+3)//����ģ���ڴ��������
#define  ERROR_SND_CREATE_SOCK	            (ERROR_DATA_TRANS_BASE+4)//����ģ�鴴��socket
#define  ERROR_RTP_SSRC                     (ERROR_DATA_TRANS_BASE+5)//RTPͬ��Դ����.
#define  ERROR_LOOPBUF_CREATE               (ERROR_DATA_TRANS_BASE+6)//��״���崴������  
#define  ERROR_RTP_NO_INIT                  (ERROR_DATA_TRANS_BASE+7)//RTP����Щ����û����
#define  ERROR_RTCP_NO_INIT                 (ERROR_DATA_TRANS_BASE+8)//RTP����Щ����û����
#define  ERROR_RTCP_SET_TIMER               (ERROR_DATA_TRANS_BASE+9)//RTCP���ö�ʱ������
#define  ERROR_RTP_SSRC_COLLISION           (ERROR_DATA_TRANS_BASE+10)//RTP ͬ��Դ����
#define  ERROR_SOCK_INIT                    (ERROR_DATA_TRANS_BASE+11)//socket û�г�ʼ��
#define  ERROR_H261_PACK_NUM                (ERROR_DATA_TRANS_BASE+12)//H261�İ���������
#define  ERROR_PACK_TOO_LEN                 (ERROR_DATA_TRANS_BASE+13)//G.711�����ٰ�̫��
#define  ERROR_H263_PACK_NUM                (ERROR_DATA_TRANS_BASE+14)//H263�İ���������
#define  ERROR_H263_PACK_TOOMUCH            (ERROR_DATA_TRANS_BASE+15)//H263�����ݰ�̫��

#define  ERROR_SND_INVALID_SOCK	            (ERROR_DATA_TRANS_BASE+16)//����ģ����Чsocket
#define  ERROR_SND_SEND_UDP	                (ERROR_DATA_TRANS_BASE+17)//����ģ�����ݰ�Ͷ��ʧ�ܣ�Ŀ����ܲ��ɴ
#define  ERROR_SND_FRAME                     (ERROR_DATA_TRANS_BASE+18)//����ģ��֡�������

#define  ERROR_NET_RCV_PARAM                (ERROR_DATA_TRANS_BASE+100)//���ý���ģ���������
#define  ERROR_NET_RCV_NOCREATE             (ERROR_DATA_TRANS_BASE+101)//����ģ��û�д���
#define  ERROR_NET_RCV_MEMORY               (ERROR_DATA_TRANS_BASE+102)//����ģ���ڴ��������
#define  ERROR_RCV_RTP_CREATE               (ERROR_DATA_TRANS_BASE+103)//����ģ��RTP����ʧ��
#define  ERROR_RCV_RTP_CALLBACK             (ERROR_DATA_TRANS_BASE+104)//����ģ������RTP�ص�����ʧ��
#define  ERROR_RCV_RTP_SETREMOTEADDR        (ERROR_DATA_TRANS_BASE+105)//����ģ������RTPԶ�˵�ַʧ��
#define  ERROR_CREATE_LOOP_BUF              (ERROR_DATA_TRANS_BASE+106)//������״����ʧ��
#define  ERROR_RCV_NO_CREATE                (ERROR_DATA_TRANS_BASE+107)//����ģ����ն���û�д���

#define  ERROR_WSA_STARTUP                  (ERROR_DATA_TRANS_BASE+200)//wsastartup error
#define  ERROR_CREATE_SEMAPORE              (ERROR_DATA_TRANS_BASE+201)// create semapore error
#define  ERROR_SOCKET_CALL                  (ERROR_DATA_TRANS_BASE+202)//����socket() ��������
#define  ERROR_BIND_SOCKET                  (ERROR_DATA_TRANS_BASE+203)//socket �󶨳���
#define  ERROR_CREATE_THREAD                (ERROR_DATA_TRANS_BASE+204)//�����̳߳��� 

#define  ERROR_LOOPBUF_FULL                 (ERROR_DATA_TRANS_BASE+205)//ѭ��������
#define  ERROR_NULL_POINT                 (ERROR_DATA_TRANS_BASE+206)//��ָ��

#define  ERROR_SET_DECRYPTKEY               (ERROR_DATA_TRANS_BASE+210)//���ý���keyʧ��
#define  ERROR_DECRYPT_FRAME                (ERROR_DATA_TRANS_BASE+212)//����һ֡ʧ��
#define  ERROR_SET_ENCRYPTKEY               (ERROR_DATA_TRANS_BASE+213)//���ü���keyʧ��
#define  ERROR_ENCRYPT_FRAME                (ERROR_DATA_TRANS_BASE+215)//����һ֡ʧ��
#define  ERROR_SET_USERDATA                 (ERROR_DATA_TRANS_BASE+216)//����һ֡ʧ��
#define  ERROR_SENDER_FEC_CRC	                 (ERROR_DATA_TRANS_BASE+217)//����fec�������
#define  ERROR_RECEIVER_FEC_CRC	                 (ERROR_DATA_TRANS_BASE+218)//����fec�������
#define  ERROR_RECEIVER_FEC_MODE_INVALID         (ERROR_DATA_TRANS_BASE+219)//����fecģʽ����
#define  ERROR_SENDER_FEC_INORDER         (ERROR_DATA_TRANS_BASE+220)//����fec����
#define  ERROR_SENDER_FEC_INVALID_PARAM         (ERROR_DATA_TRANS_BASE+221)//����fec�������

#define  ERROR_NET_BIND_ADDR_SOCKET					(ERROR_DATA_TRANS_BASE + 222) //�����������IP�󶨴���
#define  ERROR_NET_BIND_PORT_SOCKET					(ERROR_DATA_TRANS_BASE + 223) //������������˿ڰ󶨴���
#define  ERROR_NET_OTHER_BIND_ERROR					(ERROR_DATA_TRANS_BASE + 224) //�������������󶨴���

#define  ERROR_NET_CREATE_ERROR_SOCKET			(ERROR_DATA_TRANS_BASE + 225) //�����������ʱ��������


#define  ERROR_H263_SEND_PARAM                (ERROR_DATA_TRANS_BASE + 226) //h263��������
#define  ERROR_H263_BAD_DATA                  (ERROR_DATA_TRANS_BASE + 227) //h263���ݴ���
#define  ERROR_H263_NOT_SUPPORTED             (ERROR_DATA_TRANS_BASE + 228) //h263ģʽ��֧��




// define the byte order type
#define  BYTE_ORDER_UNKNOWN   (u8)0
#define  BYTE_ORDER_BIG       (u8)1
#define  BYTE_ORDER_LITTLE    (u8)2


#define MIN_PRE_BUF_SIZE                    (s32)28// G.711���ټ�һ�ֽ�

#define MAX_H261_HEADER_LEN      (s32)388//
#define MAX_H263_HEADER_LEN      (s32)3076//
#define MAX_H263PLUS_HEADER_LEN  (s32)3076//

#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE         (u8)0      //DES����ģʽ
#define AES_ENCRYPT_MODE         (u8)1      //AES����ģʽ
#define ENCRYPT_KEY_SIZE         (u8)32     //��Կ���� ȡ���еĽϴ�ֵ
#define DES_ENCRYPT_KEY_SIZE     (u8)8      //DES��Կ����
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16  //AES Mode-A ��Կ����
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24  //AES Mode-B ��Կ����
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32  //AES Mode-C ��Կ����
#endif

//fec raid5�㷨
#define FEC_MODE_RAID5	0
//fec raid6�㷨
#define FEC_MODE_RAID6	1	
//fec raid5 interlace
#define FEC_MODE_RAID5_INTERLEAVE	2
//fec raid6 interlace
#define FEC_MODE_RAID6_INTERLEAVE	3

//Frame Header Structure
typedef struct tagFrameHdr
{
    u8     m_byMediaType; //ý������
    u8    *m_pData;       //���ݻ���
    u32    m_dwPreBufSize;//m_pData����ǰԤ���˶��ٿռ䣬���ڼ�
                          // RTP option��ʱ��ƫ��ָ��һ��Ϊ12+4+12
                          // (FIXED HEADER + Extence option + Extence bit)
    u32    m_dwDataSize;  //m_pDataָ���ʵ�ʻ����С�����С
    u8     m_byFrameRate; //����֡��,���ڽ��ն�
    u32    m_dwFrameID;   //֡��ʶ�����ڽ��ն�
    u32    m_dwTimeStamp; //ʱ���, ���ڽ��ն�
    u32    m_dwSSRC;      //ͬ��Դ, ���ڽ��ն�

    u8    *m_pExtData;     //��չ���ݻ���ָ��
    u32    m_dwExtDataSize; //��չ���ݻ����С

    union
    {
        struct{
                   BOOL32    m_bKeyFrame;    //Ƶ֡���ͣ�I or P��
                   u16       m_wVideoWidth;  //��Ƶ֡��
                   u16       m_wVideoHeight; //��Ƶ֡��
                   BOOL32    m_bHighProfile; //H264 HP��ʶ
              }m_tVideoParam;
        u8    m_byAudioMode;//��Ƶģʽ
    };
	
}FRAMEHDR,*PFRAMEHDR;



#ifndef  MAX_NETSND_DEST_NUM
#define  MAX_NETSND_DEST_NUM   5 
#endif

#ifndef TNETSTRUCT//��ֹ��ַ�ṹ�ض���
#define TNETSTRUCT


/*�������*/
typedef struct tagNetSession
{
    tagNetSession(){m_dwRTPAddr=0; m_wRTPPort=0; m_dwRTCPAddr=0; m_wRTCPPort=0;}
    
	u32   m_dwRTPAddr;  //RTP��ַ(������)
	u16   m_wRTPPort;   //RTP�˿�(������)
	u32   m_dwRTCPAddr; //RTCP��ַ(������)
	u16   m_wRTCPPort;  //RTCP�˿�(������)
}TNetSession;

/*�����������*/
typedef struct tagLocalNetParam
{
	TNetSession m_tLocalNet;
	u32         m_dwRtcpBackAddr;//RTCP�ط���ַ(������)
	u16         m_wRtcpBackPort; //RTCP�ط��˿ڣ�������
}TLocalNetParam;

/*���緢�Ͳ���*/
typedef struct tagNetSndParam
{
	u8          m_byNum;	//ʵ�ʵ�ַ����
	TNetSession m_tLocalNet;//���ص�ַ��
	TNetSession m_tRemoteNet[MAX_NETSND_DEST_NUM];//Զ�˵�ַ��
}TNetSndParam;

#endif //end TNETSTRUCT

typedef BOOL32 (*ChangeRate)(u32 dwContext  , u32 dwBand);

typedef struct tagFecParam
{
	BOOL32		m_bUseFec;
	BOOL32		m_bIntraFrame;
	u8			m_nFecMode;
	s32			m_nFecPacklen;
	s32			m_nFecDataPackNum;
	s32			m_nFecCrcPackNum;
}TFecParam;

/*����ģ��״̬��Ϣ*/
typedef struct tagHpSndStatus
{
	u8           m_byMediaType;   //ý������
	u32          m_dwMaxFrameSize;//����֡��С
	u32  	     m_dwNetBand;     //���ʹ���
	u32          m_dwFrameID;     //����֡��ʶ
	u8           m_byFrameRate;   //����Ƶ��
	TNetSndParam m_tSendAddr;     //���͵�ַ
	TFecParam	 m_tFecParam;
	BOOL32		 m_bReverseG7221c;
}THpSndStatus;

/*����ģ��ͳ����Ϣ*/
typedef struct tagHpSndStatistics
{
	u32	m_dwPackSendNum; //�ѷ��͵İ���
	u32	m_dwFrameNum;	 //�ѷ��͵�֡��
	u32	m_dwFrameLoseNum;//���ڻ�������ԭ����ɵķ��͵Ķ�֡��
}THpSndStatistics;

/*����socket��Ϣ*/
typedef struct tagHpSndSocketInfo
{
    BOOL32 m_bUseRawSocket;
    u32    m_dwSrcIP;
    u32    m_wPort;
}THpSndSocketInfo;

/*����ģ��״̬��Ϣ*/
typedef struct tagHpRcvStatus
{
	BOOL32	       m_bRcvStart;  //�Ƿ�ʼ����
	u32            m_dwFrameID;  //����֡ID
	TLocalNetParam m_tRcvAddr;   //���յ��ص�ַ
	u32            m_dwFecFrameId;
	BOOL32         m_bReverseG7221c;
	BOOL32         m_bH264DependMark; //H264��֡ʱ�Ƿ�������MARK��־λ
 }THpRcvStatus;

/*������ͳ����Ϣ*/
typedef struct tagHpRcvStatistics
{
	u32    m_dwPackNum;       //�ѽ��յİ���
	u32    m_dwPackLose;      //�G����(Ϊ������û���յ������µĶ�����)
	u32    m_dwPackIndexError;//��������
	u32    m_dwFrameNum;      //�ѽ��յ�֡��
	u32		m_dwPhysicalPackLose;//�����Ķ�����
	//	u32    m_dwFrameLoseNum;  //��������Դ�л���ն��е�ԭ����ɵĽ��յĶ�֡��

	u32 m_dwFecPackNum;//���յ���fec����,  δ����
	u32 m_dwFecPackLose;//fec�����������������ɻָ��ĺͿɻָ���
	u32 m_dwFecRealFrameNum;//fec��֡���������յ���fec������
	u32 m_dwFecNeedRecovery;
	u32 m_dwFecRecoSucc;
	u32 m_dwFecRecoFail;
	u32 m_dwFecGroupFail;
}THpRcvStatistics;

/*=============================================================================
	������		��HpSocketStartUp
	����		��hp socket ��ʼ��,��ʹ���շ���֮ǰ����ô˺�����
	�㷨ʵ��	������ѡ�
	����ȫ�ֱ�����
	�������˵����BOOL32 bStartupSock;  TRUE��ʾ��ʼ��SOCKET��(Ĭ��)
	                                    FALSE��ʾ����ʼ��

	����ֵ˵�����μ������붨��
=============================================================================*/
u16 HpSocketStartup(BOOL32 bStartupSock=TRUE);

/*=============================================================================
	������		��HpSocketCleanUp
	����		����HpSocketStartup���Ӧ���������
	�㷨ʵ��	������ѡ�
	����ȫ�ֱ�������
	�������˵����BOOL32 bCleanupSock;  TRUE��ʾ�ر�SOCKET��(Ĭ��)
	                                    FALSE��ʾ���ر�
	              

	����ֵ˵������
=============================================================================*/
u16 HpSocketCleanup(BOOL32 bCleanupSock=TRUE);


/*============================================================================
API BOOL32 HpSetUdpMaxLen(u32 dwMaxUdpPackLen):����udp���͵�������
dwMaxUdpPackLen:������,��ֵ����С�ڵ���1450,���ڵ���1000
��Ϊ�ϰ汾��ÿ֡���96��������,����ÿ֡128*1024������,�����128k��֡�Ļ�,
����Сudp���������1370��
============================================================================*/
API BOOL32 HpSetUdpMaxLen(s32 dwMaxUdpPackLen);

API void setUseRawSend(BOOL32 bUsed);

/*============================================================================
����ƽ�����͵�API
============================================================================*/
API s32 SetSendUseTimer(s32 nMode);


/*=============================================================================
	������		��SetH264FrameHPFlag
	����		������H264SPS��Ϣ������֡ͷ�ṹ��HighProfile��ʶ�ֶΡ�
	�������˵��  �� PFRAMEHDR pFrmHdr�� ָ��H264֡ͷ�ṹ��ָ��
	����ֵ˵��    �� DATATRANS_NO_ERRORΪ�ɹ�������Ϊʧ�ܣ���ο������붨��
=============================================================================*/
API u16  SetH264FrameHPFlag( PFRAMEHDR pFrmHdr );


// RTP 3500 
// RTP Header Fields
//
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |V=2|P|X|  CC   |M|     PT      |       sequence number         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           timestamp                           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |           synchronization source (SSRC) identifier            |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |            contributing source (CSRC) identifiers             |
// |                             ....                              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |      defined by profile       |           length              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        header extension                       |
// |                             ....                              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
typedef struct tagRtpPack
{
    u8	 m_byMark;			//�Ƿ�֡�߽�1��ʾ���һ��
    u8   m_byExtence;		//�Ƿ�����չ��Ϣ
    u8   m_byPadNum;		//���ܵ�padding����
    u8   m_byPayload;		//�غ�
    u32  m_dwSSRC;			//ͬ��Դ
    u16  m_wSequence;		//���к�
    u32  m_dwTimeStamp;		//ʱ���
    u8  *m_pExData;         //��չ����
    s32  m_nExSize;			//��չ��С��sizeof(u32)�ı�����
    u8  *m_pRealData;		//ý������ 
    s32  m_nRealSize;		//���ݴ�С 
    s32  m_nPreBufSize;		//m_pRealDataǰԤ����Ŀռ�;
    u32  m_dwSrcIP;         //���Ͷ�(�������һ��·����)��IP��ַ,���ڽ��ն�
    u16  m_wSrcPort;        //���Ͷ�(�������һ��·����)�Ķ˿�,���ڽ��ն�
}TRtpPack;

typedef struct tagRSParam
{
    u16  m_wFirstTimeSpan;   //��һ���ش�����
    u16  m_wSecondTimeSpan;  //�ڶ����ش�����
    u16  m_wThirdTimeSpan;   //�������ش�����
    u16  m_wRejectTimeSpan;  //���ڶ�����ʱ����
} TRSParam;

//���Ͷ˸߼����ò���
typedef struct tagAdvancedSndInfo
{
    s32      m_nMaxSendNum;      // ���ݴ�����������ʹ���;	
    BOOL32   m_bRepeatSend;      // ���� (mp4) �Ƿ��ط�
    u16      m_wBufTimeSpan;
    BOOL32   m_bEncryption;      // ���� (mp4/H.26X/Audio) �Ƿ����ü���
    u8       m_byEncryptMode;    // ����ģʽ Aes ���� Des
    u16      m_wKeySize;         // ������Կ����
    s8       m_szKeyBuf[ENCRYPT_KEY_SIZE+1];   // ������Կ����
    u8       m_byLocalActivePT;  // ��̬�غ�PTֵ
    s32      m_nResendTimes;  //�ط�����
    s32      m_nResendSpan;   //�ط����
	BOOL32	 m_bHdFlag;//�Ƿ����ڸ���
	BOOL32	m_bFregmentSlice;//�Ƿ���Ƭ
	BOOL32  m_bSmoothSnd;//�Ƿ�������ƽ������
	
	u32		m_nRLBUnitNum;      //�ش��������
	u32		m_dwPrintFlag;
	u32		m_dwSSRC;
} TAdvancedSndInfo;

//���ն˸߼����ò���
typedef struct tagAdvancedRcvInfo
{
    BOOL32    m_bConfuedAdjust;  // ���� (mp3) �Ƿ����������	
    BOOL32    m_bRepeatSend;     // ���� (mp4) �Ƿ��ط�
	BOOL32    m_bAutoAdjustRSParam; 
    TRSParam  m_tRSParam;
    BOOL32    m_bDecryption;     // ���� (mp4/H.26X/Audio) �Ƿ����ý���
    u8        m_byDecryptMode;   // ����ģʽ Aes ���� Des
    u16       m_wKeySize;        // ������Կ����
    s8        m_szKeyBuf[ENCRYPT_KEY_SIZE+1];   // ������Կ����
    u8        m_byRmtActivePT;   // ���յ��Ķ�̬�غɵ�Playloadֵ
    u8        m_byRealPT;        // �ö�̬�غ�ʵ�ʴ���ĵ�Playload���ͣ�ͬ�ڷ���ʱ��PTԼ��
	u8			m_byLastFrameType;
	BOOL32	 m_bHdFlag;//�Ƿ����ڸ���
	BOOL32   m_bSmoothSend;

	s32		  m_nPacketBuffNum;
	u32		  m_dwBufBlockSize;
	u32		  m_dwBufTotalNum;
	u32			m_dwPrintFlag;
	u32			m_dwSSRC;
} TAdvancedRcvInfo;

//Frame Rcv CallBack Function
typedef   void (*PFRAMEPROC )(PFRAMEHDR pFrmHdr, u32 dwContext);
//RTP PACK Call back
typedef   void (*PRTPCALLBACK)(TRtpPack *pRtpPack, u32 dwContext); 


class CHpNetSnd;
class CHpMediaSnd
{
public:
    CHpMediaSnd();
    ~CHpMediaSnd();
public:
	//��������ģ��
    u16 Create( u32 dwMaxFrameSize, u32 dwNetBand, 
                u8 ucFrameRate, u8 ucMediaType, u32 dwSSRC = 0);

	//�����Ƿ����ı�־(����h264֡�ṹ�������Ĳ�һ�����ڵ���create����ص����������)
	u16	SetHDFlag( BOOL32 bHdFlag );

	//�Ƿ���Ƭ,�����ݼ�rtpͷ����(1456+12)ʱ���Ƿ���Ƭ��־��ֻ��h264��Ч
	u16 SetFregmentSlice( BOOL32 bFregmentSlice );
	
	//�������緢�Ͳ���(���еײ��׽��ӵĴ������󶨶˿�,�Լ�����Ŀ���ַ���趨�ȶ���)
    u16 SetNetSndParam( TNetSndParam tNetSndParam , BOOL32 bReuseAddr = FALSE);

	//�Ƴ����緢�ͱ��ص�ַ����(���еײ��׽��ӵ�ɾ�����ͷŶ˿ڵȶ���)
    u16 RemoveNetSndLocalParam();

	//���� ��̬�غɵ� Playloadֵ, byLocalActivePT����Ϊ0-��ʾ��� ���˶�̬�غɱ��
    u16 SetActivePT( u8 byLocalActivePT );

	//���ü���key�ִ������������Ķ�̬�غ�PTֵ, pszKeyBuf����ΪNULL-��ʾ������
    u16 SetEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE );	

	//����֡ID
    u16 ResetFrameId();
	//����ͬ��ԴSSRC
    u16 ResetSSRC(u32 dwSSRC = 0);

	//���÷��Ͷ��ش�����Ŀ���,�رպ󣬽������Ѿ����͵����ݰ����л���
    u16 ResetRSFlag(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	//���÷���ѡ��
    u16 SetSndInfo(u32 dwNetBand, u8 ucFrameRate);

	//���÷���ѡ��
	u16 ResetSndInfo(void);

	//�������ݰ�
    u16 Send ( PFRAMEHDR pFrmHdr, BOOL32 bAvalid=TRUE );
	//����ת��H261��H263���ݰ�
    u16 Send (TRtpPack *pRtpPack, BOOL32 bTrans = TRUE, BOOL32 bAvalid = TRUE);
 
	//�õ�״̬
    u16 GetStatus ( THpSndStatus &tHpSndStatus );
	//�õ�ͳ��
    u16 GetStatistics ( THpSndStatistics &tHpSndStatistics );	
	//�õ����Ͷ˸߼����ò������ش��ȣ�
    u16 GetAdvancedSndInfo(TAdvancedSndInfo &tAdvancedSndInfo);
	
	//�����Բ���
    u16 SelfTestSend (int nFrameLen, int nSndNum, int nSpan);

	//rtcp��ʱrtcp���ϱ�, �ڲ�ʹ�ã��ⲿ�������
    u16 DealRtcpTimer();

    //����Դ��ַ������IPαװ��ֻ����ʹ��raw socket��ʱ�����Ч
    u16 SetSrcAddr(TNetSession tSrcNet);

    //�õ�socket��ص���Ϣ
    u16 GetSndSocketInfo(THpSndSocketInfo &tRtpSockInfo, THpSndSocketInfo &tRtcpSockInfo);

    u16 GetNetSndParam(TNetSndParam* ptNetSndParam);

    //������Ƶ�ط�nSendTimes���ط�������nSendSpan��ÿ���ط��������Ҫ�ѷ����ش���
    u16 SetAudioResend(s32 nSendTimes, s32 nSendSpan);
	
	//����˳�򣬿���ֻ����SetFecEnable����Ĭ��raid5, 5+1, �а�988�ֽ�
	//SetFecMode,SetFecIntraFrame,SetFecXYӦ����SetFecEnable֮ǰ����,
	//�����SetFecEnable֮����ã����������������κ�һ�����ᵼ��ǰ��ssrc�����ã�����Ƕ�֡����
	//SetFecEnable�����Ĳ�����true��false��ÿ���л����ᵼ�¶�֡����
	//SetFecPackLen����ʱ����	
	//ʹ��fec ����
	u16 SetFecEnable( BOOL32 bEnableFec );
	//����fec���а�����
	u16 SetFecPackLen( u16 wPackLen );
	//�Ƿ�֡��fec
	u16 SetFecIntraFrame( BOOL32 bIntraFrame );
	//����fec�㷨 FEC_MODE_RAID5
	u16 SetFecMode( u8 byAlgorithm );
	//����fec��x�����ݰ� + y�������
	u16 SetFecXY(s32 nDataPackNum, s32 nCrcPackNum);

	u16 SetFecParam(const TFecParam& tPara);

	//���÷�תg7221c
	u16 SetReverseG7221c( BOOL32 bReverse );

	// �����ֽ���
	u16 SetCurByteOrderH261( u8 byOrder );


    u16 SetH263CodecDefineHdr(BOOL32 bCodeDefHdr);


	//��ӡ��Ϣ
	void SetPrintFlag(u32 dwFlag);
	void AddPrintFlag(u32 dwFlag);
	void SaveSendData(u32 dwFrameNum);
	u16	RsFecSndStart(u32 dwContext = 0 , ChangeRate pFunc = NULL, u8 byDataNum = 20);
	u16 RsFecSndStop();

	void RsReSetPacketNum(int nNewPacketNum);
public:

	u16 SendRtcpStatisReq(u8* pBuf, u16 wLen, u32 dwTargetIp, u16 wTargetPort);	

private:
    CHpNetSnd *m_pcNetSnd;
    void*       m_hSndSynSem;  //���ڶ���ĵ��̲߳�����ͬ����
};

class CHpNetRcv;
class CHpMediaRcv
{
public:
    CHpMediaRcv();
    ~CHpMediaRcv();
public:
	//��������ģ��
    u16 Create( u32 dwMaxFrameSize, 
                PFRAMEPROC pFrameCallBackProc, 
                u32 dwContext,
                u32 dwSSRC = 0);
    u16 Create( u32 dwMaxFrameSize,
                PRTPCALLBACK PRtpCallBackProc,
                u32 dwContext,
                u32 dwSSRC = 0);

	//�����Ƿ����ı�־(����h264�ص�֡�ĸ�ʽ��������ͬ������create����ص����������)	
	/*
	 *	֡�ṹ�����512��nal��ÿ2�ֽڴ���nal�ĳ��ȣ���2�ֽ�Ϊ�����򣬵�����Ϊ0ʱ����nal����
	 *	��ʵ�ʵ�����֮ǰԤ��2*512=1024�ֽ�
	 *	---------------------------------------------------------------------------
	 *	| Len0 | Len1 | Len2 | ... | Len511 | NALU0 | NALU1 | NALU2 | ... | NALUx |
	 *	---------------------------------------------------------------------------  
	 *	framehdr�ṹ��m_pDataָ��Len0����m_dwDataSize����ǰ���1024�ֽ�
	 */
	u16	SetHDFlag( BOOL32 bHdFlag );
	
	//���ý��յ�ַ����(���еײ��׽��ӵĴ������󶨶˿ڵȶ���)
    u16 SetNetRcvLocalParam ( TLocalNetParam tLocalNetParam , BOOL32 bReuseAddr = FALSE);
	
	//�Ƴ����յ�ַ����(���еײ��׽��ӵ�ɾ�����ͷŶ˿ڵȶ���)
    u16 RemoveNetRcvLocalParam();

	//���� ��̬�غɵ� Playloadֵ, byRmtActivePT����Ϊ0-��ʾ��� ���˶�̬�غɱ��
    u16 SetActivePT( u8 byRmtActivePT, u8 byRealPT );

	//���ý��ն˶���mpeg4����H.264���õ��ش�����Ŀ���,�رպ󣬽��������ش�����
    //u16 ResetRSFlag(TRSParam tRSParam, BOOL32 bRepeatSnd = FALSE);
	u16 ResetRSFlag(TRSParam tRSParam, BOOL32 bRepeatSnd=FALSE, BOOL32 bAutoAdjustRSParam=FALSE);

	//���ý��ն˶��� (mp3) �Ƿ���������������Ŀ���, �رպ󣬽�������
    u16 ResetCAFlag(BOOL32 bConfuedAdjust = TRUE);
	
	//����RTP�ص��ӿ���Ϣ
    u16 ResetRtpCalllback(PRTPCALLBACK pRtpCallBackProc, u32 dwContext);

	//���ý��ս���key�ִ�, pszKeyBuf����ΪNULL-��ʾ������
    u16 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);
		
	//��ʼ����
    u16 StartRcv();                       
	//ֹͣ����
    u16 StopRcv();
	//�õ�״̬
    u16 GetStatus ( THpRcvStatus &tHpRcvStatus );
	//�õ�ͳ��
    u16 GetStatistics ( THpRcvStatistics &tHpRcvStatistics );
	//�õ����ն˸߼����ò������ش����������ŵȣ�
    u16 GetAdvancedRcvInfo(TAdvancedRcvInfo &tAdvancedRcvInfo);
	//�õ���ǰ���յ������֡�ߴ�
    u16 GetMaxFrameSize(u32 &dwMaxFrameSize);
	//�õ���ǰ���յ���ý������
    u16 GetLocalMediaType(u8 &byMediaType);
	
	//rtcp��ʱrtcp���ϱ�, �ڲ�ʹ�ã��ⲿ�������
    u16 DealRtcpTimer();

    u16 RelayRtpStart(u32 dwIP, u16 wPort);
    u16 RelayRtpStop();

	//�ش���natʱ�����ñ�����rtp���ն˿ڶ�Ӧ�Ĺ�����ַ,Ŀ��Ϊʹ�ش�ʱ���ù㲥
	//u32 dwRtpPublicIp:���ؽ���rtp��Ӧ�Ĺ���ip
	//u16 wRtpPublicPort:���ؽ���rtp��Ӧ�Ĺ���port
	u16 SetRtpPublicAddr(u32 dwRtpPublicIp, u16 wRtpPublicPort);

	//���÷�תg7221c
	u16 SetReverseG7221c( BOOL32 bReverse );
	//��¼֡��Ϣ��������
	u16 SaveSendData(u32 dwFrameNum);

    //����H264��֡ʱ�Ƿ�������MARK��־λ, FALSE��ʾ������MARK, TRUE��ʾ����, Ĭ��ֵΪTRUE.
    u16 SetRcvH264DependInMark( BOOL32 bH264DependMark );


	//��ӡ��Ϣ
	void SetPrintFlag(u32 dwFlag);
	void AddPrintFlag(u32 dwFlag);
	u16	RsFecRcvStart();
	u16 RsFecRcvStop();
	void RsSetRepeatAndFecOn(BOOL32 bRepeat , BOOL32 bFecOn);
private:
    CHpNetRcv *m_pcNetRcv;
    void*       m_hRcvSynSem;  //���ڶ���ĵ��̲߳�����ͬ����
};


//����ý��TOSֵ nType����0:ȫ�� 1:��Ƶ 2:��Ƶ 3: ����
API s32 hpSetMediaTOS(s32 nTOS, s32 nType);
API s32 hpGetMediaTOS(s32 nType);

API void dtPrintf(char * szFormat,...);


#endif	//_HPDATATRANS_H_



