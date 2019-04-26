/*****************************************************************************
模块名      : HPMediaNet
文件名      : HPMediaNet.h
相关文件    : HPMediaNet.cpp
文件实现功能: HPMediaSnd HPMediaRcv类定义
作者        : 杨 俊
版本        : V1.0
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2014/01/24  1.0         杨 俊		 Create
******************************************************************************/


#ifndef _HPDATANET_H_
#define _HPDATANET_H_

#include "hptype.h"
#include "hpdef.h"
#include "ipc.h"


//模块版本信息和编译时间 及 依赖的库的版本和编译时间
API void HPdatanetver();
API void HPdatanetlog(u8 level);

//模块帮助信息 及 依赖的库的帮助信息
API void HPdatanethelp();


#define  DATA_TYPE     (u8)100//

#define  DATANET_NO_ERROR                   (u16)0//成功返回值  
#define  ERROR_DATA_NET_BASE                (u16)16000
#define  ERROR_SND_PARAM					(ERROR_DATA_NET_BASE+1)//设置发送模块参数出错 
#define  ERROR_SND_NOCREATE					(ERROR_DATA_NET_BASE+2)//发送模块没有创建
#define  ERROR_SND_MEMORY					(ERROR_DATA_NET_BASE+3)//发送模块内存操作出错
#define  ERROR_SND_CREATE_SOCK				(ERROR_DATA_NET_BASE+4)//发送模块创建socket
#define  ERROR_RTP_SSRC                     (ERROR_DATA_NET_BASE+5)//RTP同步源错误.
#define  ERROR_LOOPBUF_CREATE               (ERROR_DATA_NET_BASE+6)//环状缓冲创建错误  
#define  ERROR_RTP_NO_INIT                  (ERROR_DATA_NET_BASE+7)//RTP类有些对象没创建
#define  ERROR_RTCP_NO_INIT                 (ERROR_DATA_NET_BASE+8)//RTP类有些对象没创建
#define  ERROR_RTCP_SET_TIMER               (ERROR_DATA_NET_BASE+9)//RTCP设置定时器出错
#define  ERROR_RTP_SSRC_COLLISION           (ERROR_DATA_NET_BASE+10)//RTP 同步源出错
#define  ERROR_SOCK_INIT                    (ERROR_DATA_NET_BASE+11)//socket 没有初始化
#define  ERROR_H261_PACK_NUM                (ERROR_DATA_NET_BASE+12)//H261的包数不合理
#define  ERROR_PACK_TOO_LEN                 (ERROR_DATA_NET_BASE+13)//数据包太长
#define  ERROR_H263_PACK_NUM                (ERROR_DATA_NET_BASE+14)//H263的包数不合理
#define  ERROR_H263_PACK_TOOMUCH            (ERROR_DATA_NET_BASE+15)//H263的数据包太长

#define  ERROR_NET_RCV_PARAM                (ERROR_DATA_NET_BASE+100)//设置接收模块参数出错
#define  ERROR_NET_RCV_NOCREATE             (ERROR_DATA_NET_BASE+101)//接收模块没有创建
#define  ERROR_NET_RCV_MEMORY               (ERROR_DATA_NET_BASE+102)//接收模块内存操作出错
#define  ERROR_RCV_RTP_CREATE               (ERROR_DATA_NET_BASE+103)//接收模块RTP创建失败
#define  ERROR_RCV_RTP_CALLBACK             (ERROR_DATA_NET_BASE+104)//接收模块设置RTP回调函数失败
#define  ERROR_RCV_RTP_SETREMOTEADDR        (ERROR_DATA_NET_BASE+105)//接收模块设置RTP远端地址失败
#define  ERROR_CREATE_LOOP_BUF              (ERROR_DATA_NET_BASE+106)//创建环状缓冲失败
#define  ERROR_RCV_NO_CREATE                (ERROR_DATA_NET_BASE+107)//接收模块接收对象没有创建

#define  ERROR_WSA_STARTUP                  (ERROR_DATA_NET_BASE+200)//wsastartup error
#define  ERROR_CREATE_SEMAPORE              (ERROR_DATA_NET_BASE+201)//create semapore error
#define  ERROR_SOCKET_CALL                  (ERROR_DATA_NET_BASE+202)//调用socket() 函数出错
#define  ERROR_BIND_SOCKET                  (ERROR_DATA_NET_BASE+203)//socket 绑定出错
#define  ERROR_CREATE_THREAD                (ERROR_DATA_NET_BASE+204)//创建线程出错 

#define  ERROR_LOOPBUF_FULL                 (ERROR_DATA_NET_BASE+205)//循环缓冲满


#define  ERROR_SET_DECRYPTKEY               (ERROR_DATA_NET_BASE+210)//设置解密key失败
#define  ERROR_DECRYPT_FRAME                (ERROR_DATA_NET_BASE+212)//解密一帧失败
#define  ERROR_SET_ENCRYPTKEY               (ERROR_DATA_NET_BASE+213)//设置加密key失败
#define  ERROR_ENCRYPT_FRAME                (ERROR_DATA_NET_BASE+215)//加密一帧失败


#define ERROR_SND_FRAME          (s32)-1;


#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE			(u8)0		//DES加密模式
#define AES_ENCRYPT_MODE			(u8)1		//AES加密模式
#define ENCRYPT_KEY_SIZE			(u8)32		//密钥长度 取其中的较大值
#define DES_ENCRYPT_KEY_SIZE		(u8)8		//DES密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16		//AES Mode-A 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24		//AES Mode-B 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32		//AES Mode-C 密钥长度
#endif


//Frame Header Structure
typedef struct tagDataHdr
{
    u8     *m_pData;       // 数据缓冲
    u32     m_dwDataSize;  // m_pData指向的实际缓冲大小缓冲大小
    u32     m_dwTimeStamp; // 时间戳, 用于接收端
    u32     m_dwFrameId  ; // 数据帧的ID, 用于接收端
    u32     m_dwSSRC;      // 同步源, 用于接收端
	u32     m_dwRemoteIP;  // 发送端的源IP, 用于接收端（网络序）
}TDataHdr,*PTDataHdr;


#ifndef  MAX_NETSND_DEST_NUM
	#define  MAX_NETSND_DEST_NUM   5
#endif

#ifndef TNETSTRUCT//防止地址结构重定义
#define TNETSTRUCT

/*网络参数*/
typedef struct tagNetAddr
{
    u32   m_dwRTPAddr;/*RTP地址(网络序)*/
    u16   m_wRTPPort;/*RTP端口(本机序)*/
}TNetAddr;

/*网络发送参数*/
typedef struct tagSndParam
{
    u8       m_byNum;	  //实际地址对数
    TNetAddr m_tLocalNet; //当地地址对
    TNetAddr m_tRemoteNet[MAX_NETSND_DEST_NUM]; //远端地址对
}TSndParam;

#endif //end TNETSTRUCT


/*=============================================================================
	函数名		：HPDataNetInit
	功能		：HP socket 初始化,在使用收发类之前需调用此函数。
	算法实现	：（可选项）
	引用全局变量：
	输入参数说明：无
	              

	返回值说明：参见错误码定义
=============================================================================*/
u16 HPDataNetInit();

/*=============================================================================
	函数名		：HPDataNetFree
	功能		：和 HPDataNetInit 相对应的清除函数
	算法实现	：（可选项）
	引用全局变量：无
	输入参数说明：无
	              

	返回值说明：无
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
	//创建发送模块
    u16 Create (u32 dwMaxFrameSize);
	
	//设置网络发送参数(进行底层套结子的创建，绑定端口,以及发送目标地址的设定等动作)
    u16 SetNetSndParam ( TSndParam tNetSndParam );

	//移除网络发送本地地址参数(进行底层套结子的删除，释放端口等动作)
    u16 RemoveNetSndLocalParam ();

	//设置加密key字串及加密码流的动态载荷PT值
    u16 SetEncryptKey( s8 *pszKeyBuf,  u16 wKeySize, 
		               u8 byEncryptPT, u8 byEncryptMode=DES_ENCRYPT_MODE );	
	//发送数据包
    u16 Send ( PTDataHdr pDataHdr );
    
private:
    CHPInterSnd *m_pcNetSnd;
    void         *m_hSndSynSem;  //用于对象的单线程操作的同步量
};

class CHPInterRcv;
class CHPDataRcv
{
public:
    CHPDataRcv();
    ~CHPDataRcv();
public:
	//创建接收模块
    u16 Create ( u32 dwMaxFrameSize, 
                 PDATAPROC pDataCallBackProc = NULL, 
                 u32 dwContext = 0 );
    
    //设置回调函数
    u16 SetCallBack(PDATAPROC pDataCallBackProc, u32 dwContext);

	//设置接收地址参数(进行底层套结子的创建，绑定端口等动作)
    u16 SetNetRcvLocalParam( TNetAddr tNetParam );
	
	//移除接收地址参数(进行底层套结子的删除，释放端口等动作)
    u16 RemoveNetRcvLocalParam();

	//设置 动态载荷的 Playload值
    u16 SetActivePT( u8 byRmtActivePT, u8 byRealPT );

	//设置接收解密key字串
    u16 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);
		
	//开始接收
    u16 StartRcv();
	//停止接收
    u16 StopRcv();

private:
    CHPInterRcv *m_pcNetRcv;
    void         *m_hRcvSynSem;  //用于对象的单线程操作的同步量
};

//设置远遥TOS值
API s32 HPSetDataTOS(s32 nTOS);
API s32 HPGetDataTOS();

#endif

