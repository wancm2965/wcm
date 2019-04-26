#pragma once
#include "./deviceclient.h"
#include "../include/DeviceClient.h"

class CDeviceClientSession
	: public CDeviceClientNotify
	, public IDeviceClient
{
public:
	CDeviceClientSession(CDeviceClientSessionEvent& rNotify);
	~CDeviceClientSession(void);

private:
	virtual void OnLogin(void);
	virtual void OnLoginError(int nErrCode, CString strError);
	CDeviceClientSessionEvent& m_rNotify;
	CDeviceClient* m_pDeviceClient;

public:
	//��������ʼ��
	virtual void CreateDeviceClientSession(void);
	virtual void DestroyDeviceClientSession(void);

	/* ��¼
	strUserID		�豸ID
	strPassword		�豸����
	strDevType		�豸���ͣ�HPD_DVR��HIK_DVR�ȣ�
	strClientType	�ͻ�������"SYS" "MON" "OCX" "MOB"
	strDevName		�豸����
	strSerialno		�豸Ψһ��ʾ��MAC��ַ�ȣ�
	usTermType		�ն�����:0-����ն� 1-Ӳ���ն� 2-�ƶ��ն�
	strTermName		�ն�����:����Ϊ�� */
	virtual void DeviceClientLogin(const std::string& strUserID, const std::string& strPassword, const std::string &strDevType, const std::string& strClientType,
		const std::string& strDevName, const std::string& strSerialno, unsigned short usTermType=0, const std::string& strTermName="");

	/* ����ͨ��������Ϣ
	nChannelIndex		ͨ����
	strChannelName		ͨ������
	mcuid				MCU-ID
	mcuaddr				MCU��ַ
	mcuport				MCU�˿�
	ulAudioChannel		��Ƶ��
	ulVideoChannel		��Ƶ��
	ulVideoChannelSub	��Ƶ�����ţ�����Ϊ�գ�
	strDomain			�豸��ʵ�ַ�ʽ�޸ģ���ʱ����Ϊ�գ�
	*/
	virtual void DeviceClientChannelOnline(int nChannelIndex, const std::string &strChannelName, const std::string &mcuid, const std::string &mcuaddr, unsigned short mcuport, unsigned long ulAudioChannel, 
		unsigned long ulVideoChannel, unsigned long ulVideoChannelSub=0, const std::string &strDomain="");
};


