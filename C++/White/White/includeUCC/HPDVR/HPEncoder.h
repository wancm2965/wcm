#ifndef __HP_ENCODER_H__
#define __HP_ENCODER_H__

class IHPEncoderNotify
{
public:
	IHPEncoderNotify(){};
	virtual ~IHPEncoderNotify(){};

public: /*回调接口*/
	virtual void OnNotifyXMLDocumentElement(KXMLElement* t) = 0;
};

class IHPEncoder
{
public:
	IHPEncoder() {};
	virtual ~IHPEncoder() {};

public:
	virtual bool Connect() = 0;
	virtual void ReleaseConnection() = 0;

public: /*调用接口*/
	virtual void DoXMLDocumentElement(KXMLElement* t) = 0;
	virtual int CheckUser(const char * lpszUser,const char* lpszPwd) = 0;
	virtual bool GetUserRight(const char*lpszUser, COMMAND_TABLE_MAP&mapRight) = 0;
	virtual bool GetD2AConnectInfo(string&ip, int&port, string&uid, string&pwd) = 0;
	virtual string DEVID2EID(string&devid) = 0;
	virtual string EID2DEVID(string&eid) = 0;
	virtual string GetDvrEid() = 0;
	virtual string GetDvrName() = 0;
	virtual string GetDvrDevid() = 0;
	virtual int GetDevType(string&eid) = 0;
	virtual void LogCmd(bool result, const char *cmd, const char *desc, string userid) = 0;
};


AFX_EXT_CLASS IHPEncoder* CreateHPEncoder(IHPEncoderNotify& rNotify);


#endif //__HP_ENCODER_H__
