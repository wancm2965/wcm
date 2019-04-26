
//PortMonitoring.h: 检测端口是否被占用. 适用于XP及较高级版本.

#ifndef __PortMonitoring_H__
#define __PortMonitoring_H__

class CPortMonitoring  
{
public:
	CPortMonitoring();
	CPortMonitoring(bool bInitLib);
	virtual ~CPortMonitoring();
	//检测端口是否被占用 true 占用 false 没有占用
	bool CheckTCPPort(int nPort);
	bool CheckUDPPort(int nPort);
	bool m_bInitSocketLib;
};

#endif // __PortMonitoring_H__