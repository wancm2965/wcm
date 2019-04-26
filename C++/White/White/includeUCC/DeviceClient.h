#pragma once

class IDeviceClientNotify
{
public:
	IDeviceClientNotify(void){};
	~IDeviceClientNotify(void){};

	/*
	��¼ʧ�ܻص�
	nErrCode:�������
	strError:��������
	*/
	virtual void OnDeviceClientLogin(int nErrCode, CString strError)=0;

	/*
	�豸���߳ɹ��ص�
	�ڴ˽���ͨ������
	allow_count:��������ͨ������
	*/
	virtual void OnDeviceOnline(int allow_count)=0;

	/*
	�豸����֪ͨ
	*/
	virtual void OnDeviceOffline(void)=0;

	/*
	ͨ�����߳ɹ��ص�
	*/
	virtual void OnChannelOnline(void)=0;

	/*
	ͨ������֪ͨ
	*/
	virtual void OnChannelOffline(void)=0;

	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;
	virtual void OnUDPRcvGHJInfo(const UDP_RCV_GHJ_INFO& info)=0;
	virtual void OnP2P(const std::string& strUserID,KCmdUserDataPacket& packet)=0;
	virtual void OnYTControl(KCmdPacket& rCmdPacket)=0;
};

class AFX_EXT_CLASS IDeviceClient
{
public:
	/* ��¼
	strUserID		�豸ID
	strPassword		�豸����
	strClientType	�ͻ�������"SYS" "MON" "OCX" "MOB"
	strDevName		�豸����
	strSerialno		�豸Ψһ��ʾ��MAC��ַ�ȣ�
	usTermType		�ն�����:0-����ն� 1-Ӳ���ն� 2-�ƶ��ն�
	strTermName		�ն�����:����Ϊ�� */
	virtual void DeviceClientLogin(const std::string& strUserID, const std::string& strPassword, MON_DEVTYPE eDevType, const std::string& strDevName, 
		const std::string& strSerialno, unsigned short usTermType=0, const std::string& strTermName="")=0;

	/*�ǳ�*/
	virtual void DeviceClientLogout(void)=0;

	/* �����豸������Ϣ
	strDevType		�豸���ͣ�HPD_DVR��HIK_DVR�ȣ�
	strDevID		�豸ID
	strPwd			����
	strName			�豸����
	strMask			Ψһ��ʾ������Ϊ����MAC��ַ��
	strVersion		�豸�汾��Ϣ������������ɴ��գ��ڲ�����
	��ʽ��<version><system>ϵͳ�汾</system><software>����汾��<software></version>
	strDomain		�豸��ʵ�ַ�ʽ�޸ģ���ʱ����Ϊ�գ�
	*/
	virtual void DeviceOnline(MON_DEVTYPE eDevType, const std::string &strDevID, const std::string &strPwd,const std::string &strName,  const std::string &strMask="", 
		const std::string &strVersion="", const std::string &strDomain="")=0;

	/* �����豸������Ϣ
	strDevID		�豸ID
	strDomain		�豸��ʵ�ַ�ʽ�޸ģ���ʱ����Ϊ�գ�
	*/
	virtual void DeviceOffline(const std::string &strDevID, const std::string &strDomain="")=0;

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
	virtual void ChannelOnline(int nChannelIndex, const std::string &strChannelName, const std::string &mcuid, const std::string &mcuaddr, unsigned short mcuport, unsigned long ulAudioChannel, 
		unsigned long ulVideoChannel, unsigned long ulVideoChannelSub=0, const std::string &strDomain="", const std::string &strDevID="",const std::string& strDevName="")=0;

	/* ����DVRͨ��ע������
	strDevID		�豸ID
	nCHNum=-1		����ͨ��ע��
	strDomain		�豸��ʵ�ַ�ʽ�޸ģ���ʱ����Ϊ�գ�
	*/
	virtual void ChannelOffline(const std::string &strDevID,unsigned short nCHNum,const std::string &strDomain="")=0;

	/* �޸��豸����
	strDevID		�豸ID
	strName			�豸����
	strDomain		�豸��ʵ�ַ�ʽ�޸ģ���ʱ����Ϊ�գ�
	*/
	virtual void UpdateDeviceName(const std::string &strDevID,const std::string &strName, const std::string &strDomain="")=0;
};

