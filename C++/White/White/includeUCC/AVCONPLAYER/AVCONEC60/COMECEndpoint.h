#ifndef __COMECENDPOINT_H__
#define __COMECENDPOINT_H__

#include <string>
using namespace std;
#include "COMEC.h"

class COMEC_API COMECEndpoint
{
public:
	COMECEndpoint(void);
	virtual~COMECEndpoint(void);
public:
	//Methods
	virtual void COMEC_Attach(void);
	virtual void COMEC_Detach(void);

	virtual int COMEC_Register(const string&strAccountID);
	virtual int COMEC_Unregister(const string&strAccountID);

	virtual int COMEC_PlayDTMF(char dtmf);

	virtual int SIP_Init(void);
	virtual int SIP_Terminate(void);

	virtual int SIP_CreateAccountID(string&strAccountID);

	virtual int SIP_SetListenUDPPort(const string&strAccountID,unsigned short usUDPPort);
	virtual int SIP_GetListenUDPPort(const string&strAccountID,unsigned short&usUDPPort);
	
	virtual int SIP_SetUserName(const string& strAccountID,const string&strUserName);
	virtual int SIP_GetUserName(const string& strAccountID,string&strUserName);
	
	virtual int SIP_SetDisplayName(const string& strAccountID,const string&strDisplayName);
	virtual int SIP_GetDisplayName(const string& strAccountID,string&strDisplayName);
	
	virtual int SIP_SetAuthUser(const string& strAccountID,const string&strAuthUser);
	virtual int SIP_GetAuthUser(const string& strAccountID,string&strAuthUser);

	virtual int SIP_SetPassword(const string& strAccountID,const string&strPassword);
	virtual int SIP_GetPassword(const string& strAccountID,string&strPassword);

	virtual int SIP_SetDomainRealm(const string& strAccountID,const string&strDomainRealm);
	virtual int SIP_GetDomainRealm(const string& strAccountID,string&strDomainRealm);

	virtual int SIP_SetProxy(const string& strAccountID,const string&strProxy);
	virtual int SIP_GetProxy(const string& strAccountID,string&strProxy);

	virtual int SIP_SetStunServer(const string& strAccountID,const string&strStunServer);
	virtual int SIP_GetStunServer(const string& strAccountID,string&strStunServer);

	virtual int SIP_SetUseStun(const string& strAccountID,bool bUseStun);
	virtual int SIP_GetUseStun(const string& strAccountID,bool&bUseStun);

	virtual int SIP_SetOutboundProxy(const string& strAccountID,const string&strOutboundProxy);
	virtual int SIP_GetOutboundProxy(const string& strAccountID,string&strOutboundProxy);
	
	virtual int SIP_SetUseOutboundProxy(const string& strAccountID,bool bUseOutboundProxy);
	virtual int SIP_GetUseOutboundProxy(const string& strAccountID,bool&bUseOutboundProxy);
	
	virtual int SIP_SetTransport( const string& strAccountID,sip_protos proto );
	virtual int SIP_GetTransport( const string& strAccountID,sip_protos &proto );

	//Events
	virtual void OnCOMECEndpointRegistrationState(const string& strAccountID, bool bState) = 0;
};

#endif