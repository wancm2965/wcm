
/*---------------------------------------------------------------*/
//NOTICE: Call H235 implementation
//DATE: 2011-11-22
/*---------------------------------------------------------------*/

//首先作generalID senderId 时间戳验证，如验证错误则作验证通过处理，不再验证密码，呼叫可以继续下去
//如果双方generalID senderId相同，但密码及消息验证不对则呼叫不再继续下去
//注，原是设置为此,如不让其验证通过可在h235Failure设置reason为h235FailReplyDeleteMessage

//注意 AnnexD I或AnnexD IA 与AnnexF IV不能同时用
//注意 用于注册到网守时不能使用除AnnexD I及AnnexD IA以外的其他规程
//注意 h235SetTimeSync(m_h235App, RV_TRUE, 86500); /*allow max 24 hours diff */
//注意 3DES H245 OID标识暂不确定，目前以{0x28,0xCD,0x7B,0x04,0x05}开始 待查到资料后再作变更
//注意 RvRtpEncryptionMode RV_RTPENCRYPTIONMODE_H235_PADDING仅此可用
#ifndef __H235MGR_H__
#define __H235MGR_H__

//use H235 Media
#include <RVINC/h235_api.h>
#include "H323Call.h"

#define MAXREASONLEN 256

class H235Mgr ;

struct SecurityCertificate
{    
	H235Mgr *ep;
	void    *rsa; //RSA of open SSL
};


class H235Mgr 
{
public:
	H235Mgr();
	~H235Mgr();

private:
	RvStatus H235SecurityAuthInfoSetting(char * senderId,char * generalId,char * password);
	RvStatus H235SecurityModeSetting();
	void InitData();
public:

	int  H235SecurityInit(HAPP hApp);
	int  H235SecurityEnd();
	//返回 h235Version 输出到 buffer
	void H235SecurityVersion(char * buffer,int &bufLen);

	int  H235SecurityCallInit(H323Call * call);
	int  H235SecurityCallEnd(H323Call    *call);
	void H235SecurityGetSenderId();
	void H235SecurityCallGenerateSharedSecret(H323Call    *call);

	//用于媒体流加密通道时修改OLC或OLC ACK消息信令加入安全信息
	int H235SecurityChannelAddEncryptionSync(H323Call *call,H323Channel * pChannel,RvPvtNodeId  nodeId);

	//用于媒体流加密通道时从接收到的OLC消息信令获取安全信息
	int H235SecurityChannelGetEncryptionSync(H323Call *call,H323Channel * pChannel,RvPvtNodeId  nodeId);

	//senderId 发送标识即本端标识 generalId接收标识即对端标识 password 验证密码
	RvStatus H235SecurityAuthSetting(char * senderId,char * generalId,char * password);
	RvStatus H235SecurityAuthSetting();

	//提交设置的参数 设置完参数后调用
	RvStatus H235SecuritySettingSubmit();
	//设置可用状态，预定义素数是否用于媒体加密共享密钥协商
	RvStatus H235SecurityUserPrimeSetting(bool enable);
	//设置对接收呼叫等消息进行验证检查状态
	RvStatus H235SecurityCheckIncomingMsgSetting(bool check);
	
	//设置H235 安全模式
	void H235SecurityAnnexISetting(bool enable);
	void H235SecurityAnnexIASetting(bool enable);
	void H235SecurityAnnexIISetting(bool enable);
	void H235SecurityAnnexIIISetting(bool enable);
	void H235SecurityAnnexIVSetting(bool enable);
	void H235SecurityMediaDH512Setting(bool enable);
	void H235SecurityMediaDH1024Setting(bool enable);
	void H235SecurityMediaDH1536Setting(bool enable);

	const char*  H235SecurityGetAuthCheckState(int &nState,int &nLength);
	int DecryptData(const unsigned char *  sharedSecret,int nKeyLen,unsigned char * vector,int dataLen,unsigned char * output);//iv=0
	int EncryptData(const unsigned char *  sharedSecret,int nKeyLen,unsigned char * vector,int dataLen,unsigned char * output);//iv=0

	HAPP					m_hApp;
	H235HAPP                m_h235App;           //H.235 stack 对象句柄
	char                    m_strKey[64];        //用于GK注册呼叫验证
	unsigned char           m_strPublicKey[256]; 
	int                     m_nPublicKeySize;
	SecurityCertificate     m_myRSA;             //RSA object from openSSL
	SecurityCertificate     m_gkRSA;             //RSA certificate from openSSL of the GK
	SecurityCertificate     m_tmpRSA;            //RSA certificate from temporary remote endpoints

	bool					m_bIsRegistered;     //注册到GK状态
	bool					m_bCheckIncoming;    //是否对接收到的呼叫消息进行检查验证
	h235Mode_t				m_nSecurityMode;     //安全模式

	char					m_strPassword[64];   //验证密码
	char					m_strSenderId[256];  //发送标识即本端标识
	char					m_strGeneralId[256]; //接收标识即对端标识
	char					m_strRemoteId[256];  //对端标识
	entityId_t				m_remoteId;
	entityId_t				m_senderId;
	entityId_t				m_generalId;

	bool m_bH235AnnexI;                          //annex D I 消息完整性及密码认证
	bool m_bH235AnnexIA;                         //annex D IA 仅密码认证
	bool m_bH235AnnexII;						 //annex E II 采用公钥/私钥对的数字签名
	bool m_bH235AnnexIII;						 //annex E III 端到端认证
	bool m_bH235AnnexIV;						 //annex F 规程IV 逐段转接的安全性 结合了规程I和规程II
	
	bool m_bUserPrimes;							//媒体流加密 使用预定义素数参与生成共享密钥
	bool m_bH235MediaDH512;						//媒体流加密 使用DH512生成7字节共享密钥 若都选择则按最长DH生成共享密钥
	bool m_bH235MediaDH1024;					//媒体流加密 使用DH1024生成21字节共享密钥
	bool m_bH235MediaDH1536;					//媒体流加密 使用DH1536生成21字节共享密钥

	int  m_nAuthCheckState;						//H235验证检查状态 记录呼叫验证,为网守时验证检查状态,媒体流共享密钥协商结果 
	                                            // 1 成功 2 失败 3 H235媒体流共享密钥协商失败 与IMXEC_CALL_H235AUTH_STATE定义对应
	
	char m_strCheckFailureReason[MAXREASONLEN]; //H235验证失败原因
	h235Reason_t m_nAuthFailureReason;			//H235验证失败类型

	int						m_nStarted;
	int						m_nShareSecretLen;	//DH协商生成的共享密钥长度

};



#endif	//#ifndef __H235MGR_H__