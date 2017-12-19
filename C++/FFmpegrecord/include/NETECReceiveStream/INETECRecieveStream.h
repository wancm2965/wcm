#ifndef __INETECRECIEVESTREAM__H__
#define __INETECRECIEVESTREAM__H__


#ifdef WIN32
#ifdef NETECRECEIVESTREAM_EXPORTS
#define NETECRECIEVESTREAM_API _declspec(dllexport)
#else
#define NETECRECIEVESTREAM_API _declspec(dllimport)
#endif // 
#else
#define NETECRECIEVESTREAM_API
#endif // WIN32

#if !defined(_MSC_VER)
#include <stdint.h>
#else
// Define C99 equivalent types, since MSVC doesn't provide stdint.h.
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef __int64             int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;
#endif

class CNETECRecievestreamCallback
{
public:
	virtual void OnNETECRecievestreamCallbackVideoData(unsigned char*pData, int nLen) = 0;
	virtual void OnNETECRecievestreamCallbackAudioData(unsigned char*pData, int nLen) = 0;
};


class NETECRECIEVESTREAM_API INETECRecievestream
{
public:
	INETECRecievestream(){};
	virtual ~INETECRecievestream(){};
public:
	/******************************************************************************
	* Start_NETECEngine
	*描述：开启NETEC引擎
	*输入：	strServerIP	-接流服务IP
	usServerPort	-接流服务端口	
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	static int Start_NETECEngine(const char* strServerIP, unsigned short usServerPort);
	/******************************************************************************
	* Stop_NETECEngine
	*描述：关闭NETEC引擎
	*输入：	无
	*输出： 无
	*返回值：无
	*****************************************************************************/
	static void Stop_NETECEngine(void);
	/******************************************************************************
	* Create
	*描述：创建接流实例
	*输入：	strUserID -用户ID
	pCallback	-回调类指针	
	*输出： 无
	*返回值：实例指针
	*****************************************************************************/
	static INETECRecievestream* Create(const char* strUserID,CNETECRecievestreamCallback*pCallback);


	/******************************************************************************
	* Close
	*描述：关闭
	*输入：	无
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void Close(void) = 0;

	/******************************************************************************
	* Open
	*描述：打开
	*输入：
	nAudChanID     -音频通道ID
	nVidChanID     -视频通道ID
	cszPeerNodeID	-对端节点标识
	cszPeerNATIP	-对端外网IP
	nPeerNATPort	-对端外网端口	
	cszPeerLocalIP	-对端本机IP
	nPeerLocalPort	-对端本机端口
	cszPeerMCUID	-对端MCU标识
	cszPeerMCUIP	-对端MCU地址
	usPeerMCUPort	-对端MCU端口
	nForceUseMCU    -强制使用mcu
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int32_t Open(unsigned long nAudChanID, unsigned long nVidChanID, const char*cszPeerNodeID, const char*cszPeerNATIP, unsigned short nPeerNATPort,const char*cszPeerLocalIP, unsigned short nPeerLocalPort, const char*cszPeerMCUID, const char*cszPeerMCUIP, unsigned short nPeerMCUPort, int nForceUseMCU = 0) = 0;

	virtual int32_t EnableAudio(int32_t nEnable/*0:不接收，1:接收*/)=0;
	virtual int32_t EnableVideo(int32_t nEnable/*0:不接收，1:接收*/)=0;

	virtual int32_t RequestKeyFrame() = 0;
};


#endif