
//PortMonitoring.h: ���˿��Ƿ�ռ��. ������XP���ϸ߼��汾.

#ifndef __PortMonitoring_H__
#define __PortMonitoring_H__

class CPortMonitoring  
{
public:
	CPortMonitoring();
	CPortMonitoring(bool bInitLib);
	virtual ~CPortMonitoring();
	//���˿��Ƿ�ռ�� true ռ�� false û��ռ��
	bool CheckTCPPort(int nPort);
	bool CheckUDPPort(int nPort);
	bool m_bInitSocketLib;
};

#endif // __PortMonitoring_H__