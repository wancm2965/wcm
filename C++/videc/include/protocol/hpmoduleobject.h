#ifndef _MODULE_MANANGER_H_
#define _MODULE_MANANGER_H_

#include "ipc.h"
#include "hptype.h"

extern u16 g_byModuleLogLevel;

#define MAX_MODULE_MSG_LEN 65532
#define MODULE_MSG_HEADER_LEN 4

#define ev_module_msg_init IPC_USEREVENT_BASE + 1
#define ev_module_msg_exit IPC_USEREVENT_BASE + 2


class CHpModuleMsg;
class CHpModuleObject : public CIPCInstance
{
public:
	CHpModuleObject();
	virtual ~CHpModuleObject();
protected:
	BOOL32    m_bInit;
private:
	void DaemonInstanceEntry( CIPCMessage *const pcMsg, CIPCApp* pcApp );							//daemon消息入口函数
	void InstanceEntry( CIPCMessage *const ptMsg );											//消息处理入口函数
protected:

	virtual void MessageProc( CHpModuleMsg *const ptMsg ) {};										//消息入口函数，此函数需要重载
	virtual void DaemonMessageProc( CHpModuleMsg *const ptMsg,CIPCApp* pcApp ){}					//daemon消息入口函数，此函数需要重载

	virtual BOOL32 DaemonObjectInit( u32 wParam, u32 lParam );								//daemon初始化函数，需要重载
	virtual void DaemonObjectExit( void );													//daemon退出函数，需要重载
	virtual BOOL32 PreDaemonMessageProc( CIPCMessage *const ptMsg )	{ return FALSE; }		    //daemon消息预处理函数，需要重载

protected:
	virtual BOOL32 ObjectInit( u32 wParam, u32 lParam ) = 0;									//object初始化函数，需要重载
	virtual void ObjectExit( void );															//object退出函数，需要重载

	virtual BOOL32 PreMessageProc( CIPCMessage *const ptMsg )	{ return FALSE; }			    //消息预处理函数，需要重载

public:
	BOOL32 PostToSelf( CHpModuleMsg &cMsg );
};


class CHpModuleMsg
{
protected:
	u16 m_wEvent;
	u16 m_wBodyLen;
	u32 m_dwSrcIId;
	u32 m_dwSrcNode;

	u8  m_abyMsg[ MAX_MODULE_MSG_LEN + MODULE_MSG_HEADER_LEN ];

	enum EmMsgCodeIndex
	{
		CODEC_FLAG        = 0, //该字节 = 0表示未编码 = 1表示编码数据 = 2表示xml编码
		CODEC_HOMEPLACE   = 1,
		CODEC_User        = 2,
		CODEC_RESERVER3   = 3,
		CODEC_BODY		  = 4,

	};
public:
	CHpModuleMsg(){ memset( this,0,sizeof(CHpModuleMsg) ); }
	CHpModuleMsg( const CIPCMessage* ptMsg )
	{
		memset( this,0,sizeof(CHpModuleMsg) );

		m_dwSrcIId  = ptMsg->srcid;
		m_dwSrcNode = ptMsg->srcnode;

		m_wEvent = ptMsg->event;
		m_wBodyLen = ptMsg->length;
		memcpy( m_abyMsg , ptMsg->content , ptMsg->length );

	}

	//获得消息体指针
	u8*	GetBody(){ return m_abyMsg; }

	//获得消息体长度
	u16	GetBodyLen() { return m_wBodyLen; }

	u8*	GetMsg(){ return m_abyMsg; }

	//获得消息长度
	u16	GetMsgLen() { return m_wBodyLen; }

	//设置消息体内容
	BOOL SetBody(const void* buf ,u16 len )
	{ 
		if( len > (u16)(MAX_MODULE_MSG_LEN) )
			return FALSE;
		if( NULL == buf || 0 == len )
		{
			m_wBodyLen = 0;
			return TRUE;
		}
		memcpy( m_abyMsg ,buf ,len );
		m_wBodyLen = len;
		return TRUE;
	}

	BOOL CatBody( const void* buf ,u16 len )
	{
		if( len + m_wBodyLen > (u16)(MAX_MODULE_MSG_LEN) )
			return FALSE;

		memcpy( m_abyMsg + m_wBodyLen ,buf ,len );
		m_wBodyLen += len;
		return TRUE;

	}

	void SetEvent(u16 event ) { m_wEvent = event; }
	u16  GetEvent() { return m_wEvent; }

	u32 GetSrcIId(){ return m_dwSrcIId; }
	u32 GetSrcNode(){ return m_dwSrcNode; }
};


typedef struct tagModuleInitParam
{
	u32 wParam;
	u32 lParam;	
	SEMHANDLE Sem;

	tagModuleInitParam()
	{
		memset( this , 0 , sizeof(tagModuleInitParam) );
		Sem = NULL;
	}
}TModuleInitParam;

//日志级别定义
#ifndef LOG_EXP
#define LOG_EXP			        (s32)0		//异常
#endif

#ifndef LOG_IMT
#define LOG_IMT			        (s32)1		//重要日志
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG		        (s32)2		//一级调试信息
#endif

#ifndef LOG_ALL
#define LOG_ALL                 (s32)3      //所有调试信息 
#endif



#define MODULE_PRINT_BUFFERSIZE  1024
#define IS_LOG(n) ( n < g_byModuleLogLevel )

void HpModuleLog( u16 byLogLevel, const char* pFormat, ... );


//读配置文件
BOOL32 GetConfigInt( const s8* lpszProfileName,
					const s8* lpszSectionName,
					const s8* lpszKeyName,
					const s32 sdwDefault,
					s32 *sdwReturnValue );

BOOL32 GetConfigStr( const s8* lpszProfileName,
					const s8* lpszSectionName,
					const s8* lpszKeyName,
					const s8* lpszDefault,
					s8* lpszReturnValue,
					u32 dwBufSize );

BOOL32 SetConfigInt( const s8* lpszProfileName,
					const s8* lpszSectionName, 
					const s8* lpszKeyName, 
					const s32 sdwValue   
					);

BOOL32 SetConfigStr( const s8* lpszProfileName, 
					const s8* lpszSectionName, 
					const s8* lpszKeyName, 
					const s8* lpszValue
					);

BOOL32 SetConfigSection( const s8* lpszProfileName, 
						const s8* lpszSectionName, 
						const s8* lpszValue,
						u16 wCurInSectionOffset ,
						const s8* lpSzSectionKeepKey
						);

BOOL32 DeleteConfigKey( const s8* lpszProfileName, 
					   const s8* lpszSectionName, 
					   const s8* lpszKeyName 
					   );


s32    IsConfigIntExist( const s8* lpszProfileName, 
						const s8* lpszSectionName, 
						const s8* lpszKeyName,
						s32 nValue,
						u8* index );


s32    IsConfigStrExist( const s8* lpszProfileName, 
						const s8* lpszSectionName, 
						const s8* lpszKeyName,
						s8* strValue,
						u8* index );


#endif
