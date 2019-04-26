
/*---------------------------------------------------------------*/
//NOTICE: Call H235 implementation
//DATE: 2011-11-22
/*---------------------------------------------------------------*/

//������generalID senderId ʱ�����֤������֤����������֤ͨ������������֤���룬���п��Լ�����ȥ
//���˫��generalID senderId��ͬ�������뼰��Ϣ��֤��������в��ټ�����ȥ
//ע��ԭ������Ϊ��,�粻������֤ͨ������h235Failure����reasonΪh235FailReplyDeleteMessage

//ע�� AnnexD I��AnnexD IA ��AnnexF IV����ͬʱ��
//ע�� ����ע�ᵽ����ʱ����ʹ�ó�AnnexD I��AnnexD IA������������
//ע�� h235SetTimeSync(m_h235App, RV_TRUE, 86500); /*allow max 24 hours diff */
//ע�� 3DES H245 OID��ʶ�ݲ�ȷ����Ŀǰ��{0x28,0xCD,0x7B,0x04,0x05}��ʼ ���鵽���Ϻ��������
//ע�� RvRtpEncryptionMode RV_RTPENCRYPTIONMODE_H235_PADDING���˿���
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
	//���� h235Version ����� buffer
	void H235SecurityVersion(char * buffer,int &bufLen);

	int  H235SecurityCallInit(H323Call * call);
	int  H235SecurityCallEnd(H323Call    *call);
	void H235SecurityGetSenderId();
	void H235SecurityCallGenerateSharedSecret(H323Call    *call);

	//����ý��������ͨ��ʱ�޸�OLC��OLC ACK��Ϣ������밲ȫ��Ϣ
	int H235SecurityChannelAddEncryptionSync(H323Call *call,H323Channel * pChannel,RvPvtNodeId  nodeId);

	//����ý��������ͨ��ʱ�ӽ��յ���OLC��Ϣ�����ȡ��ȫ��Ϣ
	int H235SecurityChannelGetEncryptionSync(H323Call *call,H323Channel * pChannel,RvPvtNodeId  nodeId);

	//senderId ���ͱ�ʶ�����˱�ʶ generalId���ձ�ʶ���Զ˱�ʶ password ��֤����
	RvStatus H235SecurityAuthSetting(char * senderId,char * generalId,char * password);
	RvStatus H235SecurityAuthSetting();

	//�ύ���õĲ��� ��������������
	RvStatus H235SecuritySettingSubmit();
	//���ÿ���״̬��Ԥ���������Ƿ�����ý����ܹ�����ԿЭ��
	RvStatus H235SecurityUserPrimeSetting(bool enable);
	//���öԽ��պ��е���Ϣ������֤���״̬
	RvStatus H235SecurityCheckIncomingMsgSetting(bool check);
	
	//����H235 ��ȫģʽ
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
	H235HAPP                m_h235App;           //H.235 stack ������
	char                    m_strKey[64];        //����GKע�������֤
	unsigned char           m_strPublicKey[256]; 
	int                     m_nPublicKeySize;
	SecurityCertificate     m_myRSA;             //RSA object from openSSL
	SecurityCertificate     m_gkRSA;             //RSA certificate from openSSL of the GK
	SecurityCertificate     m_tmpRSA;            //RSA certificate from temporary remote endpoints

	bool					m_bIsRegistered;     //ע�ᵽGK״̬
	bool					m_bCheckIncoming;    //�Ƿ�Խ��յ��ĺ�����Ϣ���м����֤
	h235Mode_t				m_nSecurityMode;     //��ȫģʽ

	char					m_strPassword[64];   //��֤����
	char					m_strSenderId[256];  //���ͱ�ʶ�����˱�ʶ
	char					m_strGeneralId[256]; //���ձ�ʶ���Զ˱�ʶ
	char					m_strRemoteId[256];  //�Զ˱�ʶ
	entityId_t				m_remoteId;
	entityId_t				m_senderId;
	entityId_t				m_generalId;

	bool m_bH235AnnexI;                          //annex D I ��Ϣ�����Լ�������֤
	bool m_bH235AnnexIA;                         //annex D IA ��������֤
	bool m_bH235AnnexII;						 //annex E II ���ù�Կ/˽Կ�Ե�����ǩ��
	bool m_bH235AnnexIII;						 //annex E III �˵�����֤
	bool m_bH235AnnexIV;						 //annex F ���IV ���ת�ӵİ�ȫ�� ����˹��I�͹��II
	
	bool m_bUserPrimes;							//ý�������� ʹ��Ԥ���������������ɹ�����Կ
	bool m_bH235MediaDH512;						//ý�������� ʹ��DH512����7�ֽڹ�����Կ ����ѡ�����DH���ɹ�����Կ
	bool m_bH235MediaDH1024;					//ý�������� ʹ��DH1024����21�ֽڹ�����Կ
	bool m_bH235MediaDH1536;					//ý�������� ʹ��DH1536����21�ֽڹ�����Կ

	int  m_nAuthCheckState;						//H235��֤���״̬ ��¼������֤,Ϊ����ʱ��֤���״̬,ý����������ԿЭ�̽�� 
	                                            // 1 �ɹ� 2 ʧ�� 3 H235ý����������ԿЭ��ʧ�� ��IMXEC_CALL_H235AUTH_STATE�����Ӧ
	
	char m_strCheckFailureReason[MAXREASONLEN]; //H235��֤ʧ��ԭ��
	h235Reason_t m_nAuthFailureReason;			//H235��֤ʧ������

	int						m_nStarted;
	int						m_nShareSecretLen;	//DHЭ�����ɵĹ�����Կ����

};



#endif	//#ifndef __H235MGR_H__