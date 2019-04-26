#pragma once

class XInVoIPNotify
{
public:
	virtual void SetAreaCode(CString strAreaCode){};
	virtual void SetCurrentCallID(string strCurrentCallID){};
	virtual void OnAddVoIPCallRecord(string username, string userid, ULONG typeflag, ULONG statusflag, string comefrom, string callnumber) = 0;
	virtual void OnAddVoIPCallRecord(string strCallto,bool bSelfHangup,string strUsedTime) = 0;
	virtual void OnShowAddress() = 0;
	virtual bool GetUserIDAndNameFromPhoneNo(const string& strPhone,string&strUerID,string&strUserName)=0;
};

class XInVoIP
{
public:
	virtual void ReleaseConnections(void)=0;
	virtual void SetWindowVisible(BOOL bVisible = TRUE)=0;
	virtual void ProcessVoIPCall(string callto)=0;
	virtual void SetAreaCode(CString strAreaCode)=0;
	virtual void SetUSBPhone_Ring(BOOL bOpen)=0;
	virtual bool HandupVoipCall()=0;
};

AFX_EXT_CLASS XInVoIP* CreateXInVoIP(XInVoIPNotify* pXInVoIPNotify,const std::string& strSIPProxyHost, unsigned short usSIPProxyPort, 
							const std::string& strSIPAccount,const std::string&strPassword, CString strUserID, CString strDBPath,string strDomainID,string strDomain, CWnd* pParentWnd);

AFX_EXT_CLASS XInVoIP* CreateSIPPhone(XInVoIPNotify* pXInVoIPNotify, std::string strProxy, std::string strAccount, std::string strPassword, std::string strBindPhone, 
									  std::string strBindPwd, std::string strAccessNum, std::string strRealm, std::string strCountry, int nSIPPort, int nRTPPort, CString strDBPath, CWnd* pParentWnd);
