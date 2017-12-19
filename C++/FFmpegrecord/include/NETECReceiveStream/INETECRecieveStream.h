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
	*����������NETEC����
	*���룺	strServerIP	-��������IP
	usServerPort	-��������˿�	
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int Start_NETECEngine(const char* strServerIP, unsigned short usServerPort);
	/******************************************************************************
	* Stop_NETECEngine
	*�������ر�NETEC����
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	static void Stop_NETECEngine(void);
	/******************************************************************************
	* Create
	*��������������ʵ��
	*���룺	strUserID -�û�ID
	pCallback	-�ص���ָ��	
	*����� ��
	*����ֵ��ʵ��ָ��
	*****************************************************************************/
	static INETECRecievestream* Create(const char* strUserID,CNETECRecievestreamCallback*pCallback);


	/******************************************************************************
	* Close
	*�������ر�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Close(void) = 0;

	/******************************************************************************
	* Open
	*��������
	*���룺
	nAudChanID     -��Ƶͨ��ID
	nVidChanID     -��Ƶͨ��ID
	cszPeerNodeID	-�Զ˽ڵ��ʶ
	cszPeerNATIP	-�Զ�����IP
	nPeerNATPort	-�Զ������˿�	
	cszPeerLocalIP	-�Զ˱���IP
	nPeerLocalPort	-�Զ˱����˿�
	cszPeerMCUID	-�Զ�MCU��ʶ
	cszPeerMCUIP	-�Զ�MCU��ַ
	usPeerMCUPort	-�Զ�MCU�˿�
	nForceUseMCU    -ǿ��ʹ��mcu
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t Open(unsigned long nAudChanID, unsigned long nVidChanID, const char*cszPeerNodeID, const char*cszPeerNATIP, unsigned short nPeerNATPort,const char*cszPeerLocalIP, unsigned short nPeerLocalPort, const char*cszPeerMCUID, const char*cszPeerMCUIP, unsigned short nPeerMCUPort, int nForceUseMCU = 0) = 0;

	virtual int32_t EnableAudio(int32_t nEnable/*0:�����գ�1:����*/)=0;
	virtual int32_t EnableVideo(int32_t nEnable/*0:�����գ�1:����*/)=0;

	virtual int32_t RequestKeyFrame() = 0;
};


#endif