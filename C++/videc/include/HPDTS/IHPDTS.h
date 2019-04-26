//IHPDTS.h
#ifndef __IHPDTS_H__
#define __IHPDTS_H__

//========================================================================================
//版权所有: 版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称: AVCON7
//作　　者: duquansheng
//创建日期: 2008-3-31
//功能说明: 数据传输服务器内核接口类定义
//

#ifdef HPDTS_EXPORT
#define HPDTS_API _declspec(dllexport)
#elif HPDTS_DLL
#define HPDTS_API _declspec(dllimport)
#else
#define HPDTS_API
#endif


//---------------------------------------------------------------------------------------
class HPDTS_API IHPDTSNotify
{
public:
	IHPDTSNotify(void){};
	virtual~ IHPDTSNotify(void){};
public:
	virtual void onDTSNetDataNotify(uint32_t ulSessionID,const char* pData,int32_t nLen)=0;  //回调网络数据
	virtual void onDTSUIDataNotify(const char* pData,int32_t nLen)=0;							  //回调界面数据
};


//---------------------------------------------------------------------------------------
class IHPDTS
{
public:
	IHPDTS(void){};
	virtual~IHPDTS(void){};

	virtual void Init(void) = 0;
	virtual void Release(void) = 0;
public:
	virtual void processUIData(const char* pData,int32_t nLen)=0;								//处理界面数据
	virtual void processDTSData(uint32_t ulSessionID,const char* pData,int32_t nLen)=0;	//处理网络数据
	virtual void AddDTSSession(uint32_t ulSessionID,bool bHost/*此ulSessionID是否为Host Server*/)=0; //增加DTS连接
	virtual void RemoveDTSSession(uint32_t ulSessionID)=0;								//删除DTS连接
};

//---------------------------------------------------------------------------------------
HPDTS_API IHPDTS*CreateIHPDTS(IHPDTSNotify& rIHPDTSNotify,bool bUI/*是否有UI*/);

HPDTS_API void HPDTSInit(const char* cszTemPath/*接收文件临时目录*/);	
HPDTS_API void HPDTSUnInit(void);
HPDTS_API void HPDTSSetSavePath(const char* cszSavePath);//客户端设置文件保存目录,可重复调用
HPDTS_API const char* GetFileTemPath(void);
HPDTS_API const char* GetFileSavePath(void);

#endif