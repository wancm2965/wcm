
#pragma once


//========================================================================
//������ͨ���Խ�������Դ��
//���ܣ���ʾ�Խ�ͨ������������Ϣ�������Խ�������С
//���ߣ�����
//ʱ�䣺2008-09-27
//˵����ͨ�����ûص�������ַʵ�ֲ����ص�
//--------------------------------------------------------------------------


#define TALK_STATUS_CONNECT		0
#define TALK_STATUS_LOAD		1
#define TALK_STATUS_OUTPUT		2
#define TALK_STATUS_FAILED		3
#define TALK_STATUS_CALLING		4

#ifndef _HPDLG_TALK_TYPE
#define _HPDLG_TALK_TYPE
enum HPMC_TALK_TYPE
{
	HPMC_TALK_TYPE_NOR,
	HPMC_TALK_TYPE_TEST,
};
#endif

class AFX_EXT_CLASS HPDLG_TalkViewNotify
{
public:
	HPDLG_TalkViewNotify(){};
	~HPDLG_TalkViewNotify(){};
public:
	/*
	�������ܣ��رնԽ����ڻص��¼�
	��    ����strMemberID	�Խ���ID
	strDevID	�豸ID
	nCloseType �Ҷ����� 0�����Ҷϴ������ 1:�ܾ��Ҷ� 2:60������Ƶ�Զ��Ҷ�
	�� �� ֵ����
	*/
	virtual void CloseTalkViewNotify(std::string strMemberID, std::string strDevID,int nCloseType = 0)=0;

	/*
	�������ܣ����������ص��¼�
	��    ����strMemberID	�Խ���ID
	strDevID	�豸ID
	nVolumePos	��������
	�� �� ֵ����
	*/
	virtual void SetVolNotify(std::string strMemberID, std::string m_strDevID, int nVolumePos)=0;

	/*
	�������ܣ�ȡ�������Ͷ�����
	��    ����uiAudBitrate		��Ƶ����
	AudioLostCountRate	������
	�� �� ֵ����
	*/
	virtual void GetCodeText(UINT& uiAudBitrate, double& AudioLostCountRate)=0;

	/*
	�������ܣ�ȡ�����в���
	��    ����nAudioLevel	����
	�� �� ֵ����
	*/
	virtual void GetAudioLevel(int& nAudioLevel)=0;

	/*
	�������ܣ����öԽ�״̬
	��    ����nStatus		�Խ�״̬
	�� �� ֵ����
	*/
	virtual void GetTalkStatus(int& nStatus)=0;

	//ǿ����ֹ�Խ������¿�����ضԽ�
	virtual void ReStartTalk(string strDevId) = 0;

	//������ģʽʹ��	
	virtual void RecTalkViewNotify(std::string strMemberID, std::string strDevID){};
};

class AFX_EXT_CLASS HPDLG_TalkView
{
public:
	HPDLG_TalkView(void);
	virtual~ HPDLG_TalkView(void);

	/* ��������������ʾ�Ի���
	* pParent, �����ھ�� 
	* bModal, �Ƿ���ģ̬��ʾ*/

	bool Create(HPDLG_TalkViewNotify& rNotify, CWnd* pParent);
	void ShowWindow(int nCmdShow);

	/* ǿ�ƹرնԻ���*/
	void Destroy();

public:
	//���öԽ��豸����(�Ի�������ʾ)
	void SetDevName(CString strDevName);

	//���öԽ��豸ID(�رնԽ���)
	void SetDevid(std::string strDevid);

	//���öԽ���ID(��P2P����)
	void SetMemberID(std::string strMemberid);

	/*���õ�ǰ����ֵ
	* nAudioLevel		����ֵ*/
	void SetVolumn(int nAudioLevel);
	/*���õ�ǰ������Ϣ
	* uiAudBitrate			��Ƶ����
	* uiAudioLostCount		������*/
	void SetCodeInfo(UINT uiAudBitrate, UINT uiAudioLostCount);

	void SetTalkType(const HPMC_TALK_TYPE& eTalkType);
	/*�Խ����ӳɹ�*/
	void SetTalkConnected(void);

	/*���õ�ǰ�Խ�״̬
	* iStatus			״ֵ̬
	*		TALK_STATUS_CONNECT		0		��������
	*		TALK_STATUS_LOAD		1		����������Ƶ
	*		TALK_STATUS_OUTPUT		2		����ͨ��
	*		TALK_STATUS_FAILED		3		����ʧ��
	*		TALK_STATUS_CALLING		4		���ں���
	*		TALK_STATUS_BUSY		5		��æ*/
	void SetTalkStatus(int iStatus);
	void SetTalkBusy(CString csUserName);
	/*����ģʽ����֮ǰ����0:Ϊ��������ģʽ1:Ϊ������ģʽ*/
	void SetMode(int nMode);

private:
	void*		m_hWnd;
	bool		m_bWndCreated;
	BOOL		m_bCheckMuteText;
	HPMC_TALK_TYPE		m_eTalkType;
	int			m_nMode;

	
};

