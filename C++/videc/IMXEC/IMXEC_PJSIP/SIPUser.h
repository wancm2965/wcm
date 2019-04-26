#ifndef __SIPUSER_H__
#define __SIPUSER_H__


#include "../DefineHeader/SIPDefineHeader.h"

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#include "SIPDAPCall.h"

#else
#include "SIPMediaCall.h"
#endif

#include <map>

#define SIPVIAMAXNUM 32
typedef struct tagSIPVIA
{
	unsigned int	m_nCapNum;	//有效数据能力集个数
	pjsip_via_hdr * m_Caps[SIPVIAMAXNUM];
	tagSIPVIA()
	{
		m_nCapNum = 0;
	}
}SIPVIA;

class SIPCallMgr;
class SIPUser
{
public:
	SIPUser(SIPCallMgr&rSIPUserMgr,int nLineID,const std::string&strUserID,const std::string&strUserName,const std::string&strPassword,const std::string&strSIPProxy,const std::string&strAuthUserID,const std::string&strDomain);
	virtual ~SIPUser(void);
	//use to ip call
	SIPUser(SIPCallMgr&rSIPUserMgr);

public:
	virtual int Open(void);
	virtual void Close(void);

	SIPMediaCall* MakeCall(const std::string&strUserID,ConfInfo *pConfInfo=NULL);

	virtual void HangupCall(SIPCall&rSIPCall);
	const std::string&GetUserID(void){return m_strUserID;};

	virtual int GetLineID(void);
	virtual bool IsRegistrationSuccess(void);
	virtual bool IsRegistrationError(void);
	virtual const std::string&GetRegistrationErrorCode(void);

	int GenerateSDP(SIPCall &rSIPMediaCall,pj_pool_t *pool,std::string strLocalIP,pjmedia_sdp_session **p_sdp);
public:
	//to fix get rtp address
	std::string GetHostAddress(void);
	std::string GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort);

public:
	SIPCallMgr&						m_rSIPCallMgr;

private:
	int								m_nLineID;			//m_nLineID起始为0
	std::string                     m_sipProtoName;
	std::string						m_strAuthUserID;
	std::string						m_strUserName;
	std::string						m_strPassword;
	std::string						m_strStunServer;
	bool							m_bUseStun;
	std::string						m_strSIPProxyHost;
	unsigned short					m_usProxyPort;
	std::string						m_strAor;
	int								m_nFlagAddedContact;
	bool							m_bActive;
	bool							m_bRegistrationSuccess;
	bool							m_bRegistrationError;
	std::string						m_strRegistrationErrorCode;
	bool							m_bAvaya;
	bool							m_bCanBeDeleted;

public:
	int								m_nAccountId;				//注册账号ID //ACC ID起始为1

public:
	void onRegistrationSuccess();
	void onRegistrationError(const std::string&strErrorCode);
	//strHost 仅用于未注册情况 IP呼叫时
	SIPMediaCall* onNewSession(const std::string & callID,std::string strHost);
	void GetRegContactUri(std::string & strContact);
	void GetCallContactUri(std::string strLocalIP,std::string & strContact);
	std::string GetLocalIPByRemoteIPPort(const std::string&strHostIPPort);
	bool CompareLocalIPEqual(const std::string&strHostIPPort);
	pj_status_t SIPInvSendMessage( pjsip_inv_session *inv,pjsip_tx_data *tdata,int type=1); //type 0 request ,1 response
	//add via header
	void SipHdrViaParamAdd(std::string & strTarget,pj_pool_t * pool);
	void SipHdrViaParamAdd2(pjsip_msg *msg,pj_pool_t * pool);
	void GetSIPViaRportViaAddr(pjsip_via_hdr * via, int & rport,pj_str_t * * via_addr,pjsip_transport *tp=NULL);
	//add subject header
	void SipHdrSubjectParamAdd(std::string & strTarget,pj_pool_t * pool,char * pTitleValue);
	//ICE SDP相关属性加入SDP中,	bOffer is true invite,is false answer
	void ICEEncodeInSDP(SIPCall & pSipCall,pj_pool_t * pool,bool bOffer,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp=NULL);
	void Init(void);

	//register use ,add via local header notice :pool not release
	void SipHdrViaParamLocalAdd(pjsip_msg *msg,pj_pool_t * pool);
	//add the add local via 
	void SipHdrViaParamLocalAddFirst(pjsip_msg *msg,pj_pool_t * pool,int nLocalPort);
	//generate via branch
	void GenerateVIABranchParam(pjsip_via_hdr * via,pj_pool_t * pool);

	void PlayRingTone();
	void CloseRingTone();

	unsigned						m_SipCredCount;			//Number of credentials in the credential array.
	pjsip_cred_info					m_SipCredInfo[PJSUA_ACC_MAX_PROXIES];

	std::string						m_strUserID;
	std::string						m_strSIPProxy;
	std::string						m_strDomainRealm;

	bool							m_bHasVIA;
	unsigned short					m_nViaRPort;
	unsigned short					m_nViaSendByPort;
	std::string						m_strViaSendByAddress;
	bool							m_bValid;	           //Is this account valid
	pjsip_regc *					m_pSipRegc;
	SIPVIA							m_SIPVia;
	bool							m_bReplyReg;
	std::string						m_strContactParams;
	std::string						m_strContactURIParams;
	unsigned						m_nUseRFC5626;
	unsigned						m_nRFC5626_status;      //SIP outbound status:0: not used 1: requested 2: acknowledged by servers   */
	std::string						m_strRFC5626_instprm;   //SIP outbound instance param.
	std::string						m_strRFC5626_regprm;    //SIP outbound reg param.
	pj_pool_t *						m_pPOOL;

    unsigned						m_nKaInterval;          //Set the interval for periodic keep-alive transmission for this account. Default: 15 (seconds)
    pj_str_t						m_strKaData;			//Specify the data to be transmitted as keep-alive packets. Default: CR-LF
	pj_timer_entry					m_KaTimer;				//Keep-alive timer for UDP
	pjsip_transport	*				m_pKaTransport;			//Transport for keep-alive
	pj_sockaddr						m_KaTarget;				//Destination address for K-A
	unsigned						m_nKaTarget_len;		//Length of ka_target
	int								m_nSIPREGFail;			//注册失败状态 1为失败状态 2成功状态
	std::string                     m_strLocalIP;
	int  							m_nRegSetting;          //1 注销 2 注册 0默认
	unsigned long                   m_nSIPCheckLastTimestamp; //
	unsigned long					m_nSIPRegFailLastTimestamp;
	bool                            m_bSendReg;                //true 发了注册信令 false没有发注册信令
	bool                            m_bTimeOutReg;             //true 注册超时 false默认
	int                             m_nLocalBindPort;

	void SetRequestLineAddr(pjsip_tx_data *tdata,char * addr,char * calleeid=NULL);
	//Add Require header
	void SIPMessageAddRequireCapability(pjsip_tx_data *tdata);
	//Add Date header
	void SIPMessageAddDateCapability(pjsip_tx_data *tdata);
	//Add User-Agent header
	void SIPMessageAddUserAgentCapability(pjsip_tx_data *tdata);
	void SIPHeaderAddCapability(pjsip_tx_data *tdata,char * pName,char * pValue);
	//Set MultipartSDP request message.
	pj_bool_t SIPSetMultipartSDPRequest(pjmedia_sdp_session * sdp,pjsip_tx_data *tdata,char * pwd,char * mimeType=NULL);
	void SIPRegCheckStatus(unsigned long nTimestamp);
	//add by sgb 20140709
	void GetCallContactUriUas( std::string strLocalIP,std::string & strContact, pjsip_rx_data *rdata );
	void SetLocalBindPort(int nLocalPort);
	//add the add local Contact use send request add
	void SipHdrContactParamLocalAddFirst(pjsip_msg *msg,pj_pool_t * pool,int nLocalPort);

};

#endif
