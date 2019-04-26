#pragma once
#include <string>


// CDlgShowConnectInfo �Ի���

class CDlgShowConnectInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgShowConnectInfo)

public:
	CDlgShowConnectInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgShowConnectInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	unsigned long nAudioID;
	unsigned long nVideoID;
	std::string sNodeID;
	std::string sNATIP;
	unsigned short nNATPort;
	std::string sLocalIP;
	unsigned short nLocalPort;
	std::string sMCUID;
	std::string sMCUIP;
	unsigned short nServerPort;
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	void SetCreatorLable(BOOL bActive){m_bActiveCreator=bActive;}
private:
	BOOL m_bActiveCreator;
};
