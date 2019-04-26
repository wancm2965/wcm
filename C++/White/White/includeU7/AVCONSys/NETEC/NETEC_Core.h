//NETEC_Core.h
#ifndef __NETEC_CORE_H__
#define __NETEC_CORE_H__

#include <NETEC/NETEC_Export.h>
#include <NETEC/XSocket.h>

class NETEC_API NETEC_Core
{
public:
	NETEC_Core(void);
	virtual~NETEC_Core(void);
public:
	//������������ں�
	//�ɹ�����0��ʧ�ܷ��ش�����
	static int32_t Start(unsigned short nMainPort=0,bool bHTTPPort=false,bool bAnyUDPPort=false,bool bHasMCU=true,unsigned short nSubPort=0);
	//ֹͣ��������ں�
	static void Stop(void);

	//����������� [20170209]
	static void ReFlashNet(void);

	static unsigned short GetUDPPort(void);
	static const char*GetLocalIP(void);
	static SOCKET GetSocketHandle(const char*cszLocalIP);

	static void SetUpdateFilePath(const char*cszFilePath);
	static bool HasMCU(void);
	static bool IsMCUClient(void);
	static bool IsMCUServer(void);

	static int32_t GetLocalIPByUDPSocket(SOCKET hSocket,char*szLocalIP,int32_t nBufferLen);
};

#endif
