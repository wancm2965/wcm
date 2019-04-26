/*****************************************************************************
模块名      : HpDataTrans
文件名      : HpDataTrans.h
相关文件    : HpDataTrans.cpp
文件实现功能: HpMediaSnd HpMediaRcv类定义
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
******************************************************************************/

#ifndef _HPDATATRANS_H_
#define _HPDATATRANS_H_



//模块版本信息和编译时间
API void hpdatatransver();
API void HpGetDatatransVer(char** ppVerionString);



#define  DATATRANS_NO_ERROR                  (u16)0//成功返回值  
#define  ERROR_DATA_TRANS_BASE               (u16)50000
#define  ERROR_SND_PARAM                    (ERROR_DATA_TRANS_BASE+1)//设置发送模块参数出错 
#define  ERROR_SND_NOCREATE	                (ERROR_DATA_TRANS_BASE+2)//发送模块没有创建
#define  ERROR_SND_MEMORY	                (ERROR_DATA_TRANS_BASE+3)//发送模块内存操作出错
#define  ERROR_SND_CREATE_SOCK	            (ERROR_DATA_TRANS_BASE+4)//发送模块创建socket
#define  ERROR_RTP_SSRC                     (ERROR_DATA_TRANS_BASE+5)//RTP同步源错误.
#define  ERROR_LOOPBUF_CREATE               (ERROR_DATA_TRANS_BASE+6)//环状缓冲创建错误  
#define  ERROR_RTP_NO_INIT                  (ERROR_DATA_TRANS_BASE+7)//RTP类有些对象没创建
#define  ERROR_RTCP_NO_INIT                 (ERROR_DATA_TRANS_BASE+8)//RTP类有些对象没创建
#define  ERROR_RTCP_SET_TIMER               (ERROR_DATA_TRANS_BASE+9)//RTCP设置定时器出错
#define  ERROR_RTP_SSRC_COLLISION           (ERROR_DATA_TRANS_BASE+10)//RTP 同步源出错
#define  ERROR_SOCK_INIT                    (ERROR_DATA_TRANS_BASE+11)//socket 没有初始化
#define  ERROR_H261_PACK_NUM                (ERROR_DATA_TRANS_BASE+12)//H261的包数不合理
#define  ERROR_PACK_TOO_LEN                 (ERROR_DATA_TRANS_BASE+13)//G.711的数举包太长
#define  ERROR_H263_PACK_NUM                (ERROR_DATA_TRANS_BASE+14)//H263的包数不合理
#define  ERROR_H263_PACK_TOOMUCH            (ERROR_DATA_TRANS_BASE+15)//H263的数据包太长

#define  ERROR_SND_INVALID_SOCK	            (ERROR_DATA_TRANS_BASE+16)//发送模块无效socket
#define  ERROR_SND_SEND_UDP	                (ERROR_DATA_TRANS_BASE+17)//发送模块数据包投递失败（目标可能不可达）
#define  ERROR_SND_FRAME                     (ERROR_DATA_TRANS_BASE+18)//发送模块帧拆包错误

#define  ERROR_NET_RCV_PARAM                (ERROR_DATA_TRANS_BASE+100)//设置接收模块参数出错
#define  ERROR_NET_RCV_NOCREATE             (ERROR_DATA_TRANS_BASE+101)//接收模块没有创建
#define  ERROR_NET_RCV_MEMORY               (ERROR_DATA_TRANS_BASE+102)//接收模块内存操作出错
#define  ERROR_RCV_RTP_CREATE               (ERROR_DATA_TRANS_BASE+103)//接收模块RTP创建失败
#define  ERROR_RCV_RTP_CALLBACK             (ERROR_DATA_TRANS_BASE+104)//接收模块设置RTP回调函数失败
#define  ERROR_RCV_RTP_SETREMOTEADDR        (ERROR_DATA_TRANS_BASE+105)//接收模块设置RTP远端地址失败
#define  ERROR_CREATE_LOOP_BUF              (ERROR_DATA_TRANS_BASE+106)//创建环状缓冲失败
#define  ERROR_RCV_NO_CREATE                (ERROR_DATA_TRANS_BASE+107)//接收模块接收对象没有创建

#define  ERROR_WSA_STARTUP                  (ERROR_DATA_TRANS_BASE+200)//wsastartup error
#define  ERROR_CREATE_SEMAPORE              (ERROR_DATA_TRANS_BASE+201)// create semapore error
#define  ERROR_SOCKET_CALL                  (ERROR_DATA_TRANS_BASE+202)//调用socket() 函数出错
#define  ERROR_BIND_SOCKET                  (ERROR_DATA_TRANS_BASE+203)//socket 绑定出错
#define  ERROR_CREATE_THREAD                (ERROR_DATA_TRANS_BASE+204)//创建线程出错 

#define  ERROR_LOOPBUF_FULL                 (ERROR_DATA_TRANS_BASE+205)//循环缓冲满
#define  ERROR_NULL_POINT                 (ERROR_DATA_TRANS_BASE+206)//空指针

#define  ERROR_SET_DECRYPTKEY               (ERROR_DATA_TRANS_BASE+210)//设置解密key失败
#define  ERROR_DECRYPT_FRAME                (ERROR_DATA_TRANS_BASE+212)//解密一帧失败
#define  ERROR_SET_ENCRYPTKEY               (ERROR_DATA_TRANS_BASE+213)//设置加密key失败
#define  ERROR_ENCRYPT_FRAME                (ERROR_DATA_TRANS_BASE+215)//加密一帧失败
#define  ERROR_SET_USERDATA                 (ERROR_DATA_TRANS_BASE+216)//加密一帧失败
#define  ERROR_SENDER_FEC_CRC	                 (ERROR_DATA_TRANS_BASE+217)//发送fec计算错误
#define  ERROR_RECEIVER_FEC_CRC	                 (ERROR_DATA_TRANS_BASE+218)//接收fec计算错误
#define  ERROR_RECEIVER_FEC_MODE_INVALID         (ERROR_DATA_TRANS_BASE+219)//接收fec模式错误
#define  ERROR_SENDER_FEC_INORDER         (ERROR_DATA_TRANS_BASE+220)//发送fec乱序
#define  ERROR_SENDER_FEC_INVALID_PARAM         (ERROR_DATA_TRANS_BASE+221)//发送fec错误参数

#define  ERROR_NET_BIND_ADDR_SOCKET					(ERROR_DATA_TRANS_BASE + 222) //设置网络参数IP绑定错误
#define  ERROR_NET_BIND_PORT_SOCKET					(ERROR_DATA_TRANS_BASE + 223) //设置网络参数端口绑定错误
#define  ERROR_NET_OTHER_BIND_ERROR					(ERROR_DATA_TRANS_BASE + 224) //设置网络其它绑定错误

#define  ERROR_NET_CREATE_ERROR_SOCKET			(ERROR_DATA_TRANS_BASE + 225) //设置网络参数时其他错误


#define  ERROR_H263_SEND_PARAM                (ERROR_DATA_TRANS_BASE + 226) //h263参数错误
#define  ERROR_H263_BAD_DATA                  (ERROR_DATA_TRANS_BASE + 227) //h263数据错误
#define  ERROR_H263_NOT_SUPPORTED             (ERROR_DATA_TRANS_BASE + 228) //h263模式不支持




// define the byte order type
#define  BYTE_ORDER_UNKNOWN   (u8)0
#define  BYTE_ORDER_BIG       (u8)1
#define  BYTE_ORDER_LITTLE    (u8)2


#define MIN_PRE_BUF_SIZE                    (s32)28// G.711需再加一字节

#define MAX_H261_HEADER_LEN      (s32)388//
#define MAX_H263_HEADER_LEN      (s32)3076//
#define MAX_H263PLUS_HEADER_LEN  (s32)3076//

#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE         (u8)0      //DES加密模式
#define AES_ENCRYPT_MODE         (u8)1      //AES加密模式
#define ENCRYPT_KEY_SIZE         (u8)32     //密钥长度 取其中的较大值
#define DES_ENCRYPT_KEY_SIZE     (u8)8      //DES密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16  //AES Mode-A 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24  //AES Mode-B 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32  //AES Mode-C 密钥长度
#endif

//fec raid5算法
#define FEC_MODE_RAID5	0
//fec raid6算法
#define FEC_MODE_RAID6	1	
//fec raid5 interlace
#define FEC_MODE_RAID5_INTERLEAVE	2
//fec raid6 interlace
#define FEC_MODE_RAID6_INTERLEAVE	3

//Frame Header Structure
typedef struct tagFrameHdr
{
    u8     m_byMediaType; //媒体类型
    u8    *m_pData;       //数据缓冲
    u32    m_dwPreBufSize;//m_pData缓冲前预留了多少空间，用于加
                          // RTP option的时候偏移指针一般为12+4+12
                          // (FIXED HEADER + Extence option + Extence bit)
    u32    m_dwDataSize;  //m_pData指向的实际缓冲大小缓冲大小
    u8     m_byFrameRate; //发送帧率,用于接收端
    u32    m_dwFrameID;   //帧标识，用于接收端
    u32    m_dwTimeStamp; //时间戳, 用于接收端
    u32    m_dwSSRC;      //同步源, 用于接收端

    u8    *m_pExtData;     //扩展数据缓冲指针
    u32    m_dwExtDataSize; //扩展数据缓冲大小

    union
    {
        struct{
                   BOOL32    m_bKeyFrame;    //频帧类型（I or P）
                   u16       m_wVideoWidth;  //视频帧宽
                   u16       m_wVideoHeight; //视频帧宽
                   BOOL32    m_bHighProfile; //H264 HP标识
              }m_tVideoParam;
        u8    m_byAudioMode;//音频模式
    };
	
}FRAMEHDR,*PFRAMEHDR;



#ifndef  MAX_NETSND_DEST_NUM
#define  MAX_NETSND_DEST_NUM   5 
#endif

#ifndef TNETSTRUCT//防止地址结构重定义
#define TNETSTRUCT


/*网络参数*/
typedef struct tagNetSession
{
    tagNetSession(){m_dwRTPAddr=0; m_wRTPPort=0; m_dwRTCPAddr=0; m_wRTCPPort=0;}
    
	u32   m_dwRTPAddr;  //RTP地址(网络序)
	u16   m_wRTPPort;   //RTP端口(本机序)
	u32   m_dwRTCPAddr; //RTCP地址(网络序)
	u16   m_wRTCPPort;  //RTCP端口(本机序)
}TNetSession;

/*本地网络参数*/
typedef struct tagLocalNetParam
{
	TNetSession m_tLocalNet;
	u32         m_dwRtcpBackAddr;//RTCP回发地址(网络序)
	u16         m_wRtcpBackPort; //RTCP回发端口（本机序）
}TLocalNetParam;

/*网络发送参数*/
typedef struct tagNetSndParam
{
	u8          m_byNum;	//实际地址对数
	TNetSession m_tLocalNet;//当地地址对
	TNetSession m_tRemoteNet[MAX_NETSND_DEST_NUM];//远端地址对
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

/*发送模块状态信息*/
typedef struct tagHpSndStatus
{
	u8           m_byMediaType;   //媒体类型
	u32          m_dwMaxFrameSize;//最大的帧大小
	u32  	     m_dwNetBand;     //发送带宽
	u32          m_dwFrameID;     //数据帧标识
	u8           m_byFrameRate;   //发送频率
	TNetSndParam m_tSendAddr;     //发送地址
	TFecParam	 m_tFecParam;
	BOOL32		 m_bReverseG7221c;
}THpSndStatus;

/*发送模块统计信息*/
typedef struct tagHpSndStatistics
{
	u32	m_dwPackSendNum; //已发送的包数
	u32	m_dwFrameNum;	 //已发送的帧数
	u32	m_dwFrameLoseNum;//由于缓冲满等原因造成的发送的丢帧数
}THpSndStatistics;

/*发送socket信息*/
typedef struct tagHpSndSocketInfo
{
    BOOL32 m_bUseRawSocket;
    u32    m_dwSrcIP;
    u32    m_wPort;
}THpSndSocketInfo;

/*接收模块状态信息*/
typedef struct tagHpRcvStatus
{
	BOOL32	       m_bRcvStart;  //是否开始接收
	u32            m_dwFrameID;  //数据帧ID
	TLocalNetParam m_tRcvAddr;   //接收当地地址
	u32            m_dwFecFrameId;
	BOOL32         m_bReverseG7221c;
	BOOL32         m_bH264DependMark; //H264组帧时是否依赖于MARK标志位
 }THpRcvStatus;

/*解码器统计信息*/
typedef struct tagHpRcvStatistics
{
	u32    m_dwPackNum;       //已接收的包数
	u32    m_dwPackLose;      //丟包数(为网卡上没有收到而导致的丢包数)
	u32    m_dwPackIndexError;//包乱序数
	u32    m_dwFrameNum;      //已接收的帧数
	u32		m_dwPhysicalPackLose;//真正的丢包数
	//	u32    m_dwFrameLoseNum;  //由于数据源切换清空队列等原因造成的接收的丢帧数

	u32 m_dwFecPackNum;//所收到的fec包数,  未启用
	u32 m_dwFecPackLose;//fec丢包总数，包括不可恢复的和可恢复的
	u32 m_dwFecRealFrameNum;//fec的帧数（即所收到的fec组数）
	u32 m_dwFecNeedRecovery;
	u32 m_dwFecRecoSucc;
	u32 m_dwFecRecoFail;
	u32 m_dwFecGroupFail;
}THpRcvStatistics;

/*=============================================================================
	函数名		：HpSocketStartUp
	功能		：hp socket 初始化,在使用收发类之前需调用此函数。
	算法实现	：（可选项）
	引用全局变量：
	输入参数说明：BOOL32 bStartupSock;  TRUE表示初始化SOCKET库(默认)
	                                    FALSE表示不初始化

	返回值说明：参见错误码定义
=============================================================================*/
u16 HpSocketStartup(BOOL32 bStartupSock=TRUE);

/*=============================================================================
	函数名		：HpSocketCleanUp
	功能		：和HpSocketStartup相对应的清除函数
	算法实现	：（可选项）
	引用全局变量：无
	输入参数说明：BOOL32 bCleanupSock;  TRUE表示关闭SOCKET库(默认)
	                                    FALSE表示不关闭
	              

	返回值说明：无
=============================================================================*/
u16 HpSocketCleanup(BOOL32 bCleanupSock=TRUE);


/*============================================================================
API BOOL32 HpSetUdpMaxLen(u32 dwMaxUdpPackLen):设置udp发送的最大包长
dwMaxUdpPackLen:最大包长,其值必须小于等于1450,大于等于1000
因为老版本有每帧最多96包的限制,又有每帧128*1024的限制,如果有128k的帧的话,
那最小udp包长必须比1370大
============================================================================*/
API BOOL32 HpSetUdpMaxLen(s32 dwMaxUdpPackLen);

API void setUseRawSend(BOOL32 bUsed);

/*============================================================================
开启平滑发送的API
============================================================================*/
API s32 SetSendUseTimer(s32 nMode);


/*=============================================================================
	函数名		：SetH264FrameHPFlag
	功能		：解析H264SPS信息，填入帧头结构的HighProfile标识字段。
	输入参数说明  ： PFRAMEHDR pFrmHdr， 指向H264帧头结构的指针
	返回值说明    ： DATATRANS_NO_ERROR为成功，其它为失败，请参考错误码定义
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
    u8	 m_byMark;			//是否帧边界1表示最后一包
    u8   m_byExtence;		//是否有扩展信息
    u8   m_byPadNum;		//可能的padding长度
    u8   m_byPayload;		//载荷
    u32  m_dwSSRC;			//同步源
    u16  m_wSequence;		//序列号
    u32  m_dwTimeStamp;		//时间戳
    u8  *m_pExData;         //扩展数据
    s32  m_nExSize;			//扩展大小：sizeof(u32)的倍数；
    u8  *m_pRealData;		//媒体数据 
    s32  m_nRealSize;		//数据大小 
    s32  m_nPreBufSize;		//m_pRealData前预分配的空间;
    u32  m_dwSrcIP;         //发送端(或者最后一个路由器)的IP地址,用于接收端
    u16  m_wSrcPort;        //发送端(或者最后一个路由器)的端口,用于接收端
}TRtpPack;

typedef struct tagRSParam
{
    u16  m_wFirstTimeSpan;   //第一个重传检测点
    u16  m_wSecondTimeSpan;  //第二个重传检测点
    u16  m_wThirdTimeSpan;   //第三个重传检测点
    u16  m_wRejectTimeSpan;  //过期丢弃的时间跨度
} TRSParam;

//发送端高级设置参数
typedef struct tagAdvancedSndInfo
{
    s32      m_nMaxSendNum;      // 根据带块计算的最大发送次数;	
    BOOL32   m_bRepeatSend;      // 对于 (mp4) 是否重发
    u16      m_wBufTimeSpan;
    BOOL32   m_bEncryption;      // 对于 (mp4/H.26X/Audio) 是否设置加密
    u8       m_byEncryptMode;    // 加密模式 Aes 或者 Des
    u16      m_wKeySize;         // 加密密钥长度
    s8       m_szKeyBuf[ENCRYPT_KEY_SIZE+1];   // 加密密钥缓冲
    u8       m_byLocalActivePT;  // 动态载荷PT值
    s32      m_nResendTimes;  //重发次数
    s32      m_nResendSpan;   //重发间隔
	BOOL32	 m_bHdFlag;//是否用于高清
	BOOL32	m_bFregmentSlice;//是否切片
	BOOL32  m_bSmoothSnd;//是否启用了平滑发送
	
	u32		m_nRLBUnitNum;      //重传缓存块数
	u32		m_dwPrintFlag;
	u32		m_dwSSRC;
} TAdvancedSndInfo;

//接收端高级设置参数
typedef struct tagAdvancedRcvInfo
{
    BOOL32    m_bConfuedAdjust;  // 对于 (mp3) 是否做乱序调整	
    BOOL32    m_bRepeatSend;     // 对于 (mp4) 是否重发
	BOOL32    m_bAutoAdjustRSParam; 
    TRSParam  m_tRSParam;
    BOOL32    m_bDecryption;     // 对于 (mp4/H.26X/Audio) 是否设置解密
    u8        m_byDecryptMode;   // 解密模式 Aes 或者 Des
    u16       m_wKeySize;        // 解密密钥长度
    s8        m_szKeyBuf[ENCRYPT_KEY_SIZE+1];   // 解密密钥缓冲
    u8        m_byRmtActivePT;   // 接收到的动态载荷的Playload值
    u8        m_byRealPT;        // 该动态载荷实际代表的的Playload类型，同于发送时的PT约定
	u8			m_byLastFrameType;
	BOOL32	 m_bHdFlag;//是否用于高清
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
	//创建发送模块
    u16 Create( u32 dwMaxFrameSize, u32 dwNetBand, 
                u8 ucFrameRate, u8 ucMediaType, u32 dwSSRC = 0);

	//设置是否高清的标志(高清h264帧结构与其他的不一样，在调用create后务必调用这个函数)
	u16	SetHDFlag( BOOL32 bHdFlag );

	//是否切片,当数据加rtp头超过(1456+12)时，是否切片标志，只对h264有效
	u16 SetFregmentSlice( BOOL32 bFregmentSlice );
	
	//设置网络发送参数(进行底层套结子的创建，绑定端口,以及发送目标地址的设定等动作)
    u16 SetNetSndParam( TNetSndParam tNetSndParam , BOOL32 bReuseAddr = FALSE);

	//移除网络发送本地地址参数(进行底层套结子的删除，释放端口等动作)
    u16 RemoveNetSndLocalParam();

	//设置 动态载荷的 Playload值, byLocalActivePT设置为0-表示清空 本端动态载荷标记
    u16 SetActivePT( u8 byLocalActivePT );

	//设置加密key字串及加密码流的动态载荷PT值, pszKeyBuf设置为NULL-表示不加密
    u16 SetEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE );	

	//重置帧ID
    u16 ResetFrameId();
	//重置同步源SSRC
    u16 ResetSSRC(u32 dwSSRC = 0);

	//重置发送端重传处理的开关,关闭后，将不对已经发送的数据包进行缓存
    u16 ResetRSFlag(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	//设置发送选项
    u16 SetSndInfo(u32 dwNetBand, u8 ucFrameRate);

	//重置发送选项
	u16 ResetSndInfo(void);

	//发送数据包
    u16 Send ( PFRAMEHDR pFrmHdr, BOOL32 bAvalid=TRUE );
	//用于转发H261或H263数据包
    u16 Send (TRtpPack *pRtpPack, BOOL32 bTrans = TRUE, BOOL32 bAvalid = TRUE);
 
	//得到状态
    u16 GetStatus ( THpSndStatus &tHpSndStatus );
	//得到统计
    u16 GetStatistics ( THpSndStatistics &tHpSndStatistics );	
	//得到发送端高级设置参数（重传等）
    u16 GetAdvancedSndInfo(TAdvancedSndInfo &tAdvancedSndInfo);
	
	//发送自测试
    u16 SelfTestSend (int nFrameLen, int nSndNum, int nSpan);

	//rtcp定时rtcp包上报, 内部使用，外部请勿调用
    u16 DealRtcpTimer();

    //设置源地址，用于IP伪装，只能在使用raw socket的时候才有效
    u16 SetSrcAddr(TNetSession tSrcNet);

    //得到socket相关的信息
    u16 GetSndSocketInfo(THpSndSocketInfo &tRtpSockInfo, THpSndSocketInfo &tRtcpSockInfo);

    u16 GetNetSndParam(TNetSndParam* ptNetSndParam);

    //设置音频重发nSendTimes：重发次数，nSendSpan：每次重发间隔，需要把发送重传打开
    u16 SetAudioResend(s32 nSendTimes, s32 nSendSpan);
	
	//调用顺序，可以只调用SetFecEnable，则默认raid5, 5+1, 切包988字节
	//SetFecMode,SetFecIntraFrame,SetFecXY应该在SetFecEnable之前调用,
	//如果在SetFecEnable之后调用，则这三个函数的任何一个都会导致前面ssrc的重置，后果是丢帧丢包
	//SetFecEnable函数的参数的true和false的每次切换都会导致丢帧丢包
	//SetFecPackLen可随时调用	
	//使能fec 发送
	u16 SetFecEnable( BOOL32 bEnableFec );
	//设置fec的切包长度
	u16 SetFecPackLen( u16 wPackLen );
	//是否帧内fec
	u16 SetFecIntraFrame( BOOL32 bIntraFrame );
	//设置fec算法 FEC_MODE_RAID5
	u16 SetFecMode( u8 byAlgorithm );
	//设置fec的x包数据包 + y包冗余包
	u16 SetFecXY(s32 nDataPackNum, s32 nCrcPackNum);

	u16 SetFecParam(const TFecParam& tPara);

	//设置反转g7221c
	u16 SetReverseG7221c( BOOL32 bReverse );

	// 设置字节序
	u16 SetCurByteOrderH261( u8 byOrder );


    u16 SetH263CodecDefineHdr(BOOL32 bCodeDefHdr);


	//打印信息
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
    void*       m_hSndSynSem;  //用于对象的单线程操作的同步量
};

class CHpNetRcv;
class CHpMediaRcv
{
public:
    CHpMediaRcv();
    ~CHpMediaRcv();
public:
	//创建接收模块
    u16 Create( u32 dwMaxFrameSize, 
                PFRAMEPROC pFrameCallBackProc, 
                u32 dwContext,
                u32 dwSSRC = 0);
    u16 Create( u32 dwMaxFrameSize,
                PRTPCALLBACK PRtpCallBackProc,
                u32 dwContext,
                u32 dwSSRC = 0);

	//设置是否高清的标志(高清h264回调帧的格式与其他不同，调用create后务必调用这个函数)	
	/*
	 *	帧结构，最大512个nal，每2字节代表nal的长度，这2字节为主机序，当长度为0时代表nal结束
	 *	在实际的数据之前预留2*512=1024字节
	 *	---------------------------------------------------------------------------
	 *	| Len0 | Len1 | Len2 | ... | Len511 | NALU0 | NALU1 | NALU2 | ... | NALUx |
	 *	---------------------------------------------------------------------------  
	 *	framehdr结构的m_pData指向Len0处，m_dwDataSize包含前面的1024字节
	 */
	u16	SetHDFlag( BOOL32 bHdFlag );
	
	//设置接收地址参数(进行底层套结子的创建，绑定端口等动作)
    u16 SetNetRcvLocalParam ( TLocalNetParam tLocalNetParam , BOOL32 bReuseAddr = FALSE);
	
	//移除接收地址参数(进行底层套结子的删除，释放端口等动作)
    u16 RemoveNetRcvLocalParam();

	//设置 动态载荷的 Playload值, byRmtActivePT设置为0-表示清空 本端动态载荷标记
    u16 SetActivePT( u8 byRmtActivePT, u8 byRealPT );

	//重置接收端对于mpeg4或者H.264采用的重传处理的开关,关闭后，将不发送重传请求
    //u16 ResetRSFlag(TRSParam tRSParam, BOOL32 bRepeatSnd = FALSE);
	u16 ResetRSFlag(TRSParam tRSParam, BOOL32 bRepeatSnd=FALSE, BOOL32 bAutoAdjustRSParam=FALSE);

	//重置接收端对于 (mp3) 是否采用乱序调整处理的开关, 关闭后，将不调整
    u16 ResetCAFlag(BOOL32 bConfuedAdjust = TRUE);
	
	//重置RTP回调接口信息
    u16 ResetRtpCalllback(PRTPCALLBACK pRtpCallBackProc, u32 dwContext);

	//设置接收解密key字串, pszKeyBuf设置为NULL-表示不加密
    u16 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);
		
	//开始接收
    u16 StartRcv();                       
	//停止接收
    u16 StopRcv();
	//得到状态
    u16 GetStatus ( THpRcvStatus &tHpRcvStatus );
	//得到统计
    u16 GetStatistics ( THpRcvStatistics &tHpRcvStatistics );
	//得到接收端高级设置参数（重传、乱序重排等）
    u16 GetAdvancedRcvInfo(TAdvancedRcvInfo &tAdvancedRcvInfo);
	//得到当前接收到的最大帧尺寸
    u16 GetMaxFrameSize(u32 &dwMaxFrameSize);
	//得到当前接收到的媒体类型
    u16 GetLocalMediaType(u8 &byMediaType);
	
	//rtcp定时rtcp包上报, 内部使用，外部请勿调用
    u16 DealRtcpTimer();

    u16 RelayRtpStart(u32 dwIP, u16 wPort);
    u16 RelayRtpStop();

	//重传过nat时，设置本机的rtp接收端口对应的公网地址,目的为使重传时不用广播
	//u32 dwRtpPublicIp:本地接收rtp对应的公网ip
	//u16 wRtpPublicPort:本地接收rtp对应的公网port
	u16 SetRtpPublicAddr(u32 dwRtpPublicIp, u16 wRtpPublicPort);

	//设置反转g7221c
	u16 SetReverseG7221c( BOOL32 bReverse );
	//记录帧信息，测试用
	u16 SaveSendData(u32 dwFrameNum);

    //设置H264组帧时是否依赖于MARK标志位, FALSE表示不依赖MARK, TRUE表示依赖, 默认值为TRUE.
    u16 SetRcvH264DependInMark( BOOL32 bH264DependMark );


	//打印信息
	void SetPrintFlag(u32 dwFlag);
	void AddPrintFlag(u32 dwFlag);
	u16	RsFecRcvStart();
	u16 RsFecRcvStop();
	void RsSetRepeatAndFecOn(BOOL32 bRepeat , BOOL32 bFecOn);
private:
    CHpNetRcv *m_pcNetRcv;
    void*       m_hRcvSynSem;  //用于对象的单线程操作的同步量
};


//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API s32 hpSetMediaTOS(s32 nTOS, s32 nType);
API s32 hpGetMediaTOS(s32 nType);

API void dtPrintf(char * szFormat,...);


#endif	//_HPDATATRANS_H_



