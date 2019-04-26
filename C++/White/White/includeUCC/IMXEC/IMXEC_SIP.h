//IMXEC_SIP.h
#ifndef __IMXEC_SIP_H__
#define __IMXEC_SIP_H__

#include <IMXEC/IMXEC_Export.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <IMXEC/IMXEC_AudioSetting.h>


//SIPЭ��
class IMXEC_API IMXEC_SIP
{
public:
	IMXEC_SIP(void);
	virtual~IMXEC_SIP(void);
public:
	//SIP����ͨ����������
	typedef enum
	{
		TRANS_UDP=0,				//UDP
		TRANS_TCP,					//TCP
		TRANS_TLS					//TLS
	}TRANS_TYPE;

public:
	/******************************************************************************
	* AddVirtualLine
	*���������������·
	*���룺	cszUserID		-�û��ʺ�
			cszUserName		-�û�����
			cszPassword		-�û�����
			cszHost			-SIP���������
	*�������
	*����ֵ���ɹ�����>=0[line id]��ʧ�ܷ���-1
	*****************************************************************************/
	static int AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID=NULL,const char*cszDomain=NULL);

	/******************************************************************************
	* RemoveVirtualLine
	*������ɾ��������·
	*���룺	nLineID			-��·��ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	static void RemoveVirtualLine(int nLineID);

	static int SetCurrentVirtualLine(int nLineID);
	static int GetCurrentVirtualLine(void);
	static int IsVirtualLineRegistrationSuccess(int nLineID);
	static int IsCurrentVirtualLineRegistrationSuccess(void);
	static int IsVirtualLineRegistrationError(int nLineID);
	static int IsCurrentVirtualLineRegistrationError(void);
	static const char*GetVirtualLineRegistrationErrorCode(int nLineID);
	static const char*GetCurrentVirtualLineRegistrationErrorCode(void);


	/******************************************************************************
	* SetSIPListenPort
	*���������������˿�
	*���룺	nSIPListenPort			-�����˿�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSIPListenPort(unsigned short nSIPListenPort);

	/******************************************************************************
	* SetLocalIP
	*����������ע���ʺ�
	*���룺	cszSIPID		-ע���ʺ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetLocalIP(const char*cszLocalIP);

	/******************************************************************************
	* SetRTPBasePort
	*���������������˿�
	*���룺	nRTPBasePort			-RTP��ʼ�˿�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetRTPBasePort(unsigned short nRTPBasePort);

	/******************************************************************************
	* SetAudioDevParam
	*����������������Ƶ�豸����
	*���룺 nAudioCodecID		-��Ƶ����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);

	/******************************************************************************
	* SetMainVideoDevParam
	*����������������Ƶ�豸����
	*���룺 nDevID				-�豸��ʶ
			nFrameRate			-֡��
			nBitrate			-����
			nVideoCodecType		-��Ƶ����
			nVideoFormatType	-��Ƶ��ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType);

	/******************************************************************************
	* Start
	*����������SIP����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int Start(void);

	/******************************************************************************
	* Stop
	*������ֹͣSIP����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int Stop(void);


	/******************************************************************************
	* SetSupportH239
	*�����������Ƿ�֧��H239
	*���룺nFlag		-�Ƿ�֧��H239
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSupportH239(int nSupportH239);

	/******************************************************************************
	* SetSecondVideoDevParam
	*���������ø�����Ƶ�豸����
	*���룺 nDevID				-�豸��ʶ
			nFrameRate			-֡��
			nBitrate			-����
			nVideoCodecType		-��Ƶ����
			nVideoFormatType	-��Ƶ��ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType);

	/******************************************************************************
	* SetSIPTransportType
	*����������SIP���������
	*���룺type			-����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type);

	/******************************************************************************
	* SetICETransportCount
	*���������ô�����ÿ��ý����ICEͨ����
	*���룺	nCount		-ÿ��ý����ICEͨ����(����ֻ��RTP����Ϊ1���绹��Ҫ��RTCP����Ϊ2)
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetICETransportCount(int nCount);
	/******************************************************************************
	* SetICEEnable
	*����������ICEӦ��״̬
	*���룺	bEnable		-ICEӦ��״̬ trueʹ�ã�false��ʹ�ã�Ĭ��false
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetICEEnable(bool bEnable);
	/******************************************************************************
	* SetTURNLoginAuth
	*���������õ�¼TURN�������˻�
	*���룺	cszTurnUser		-TURN�������˻���
	cszTurnPWD		-TURN�������˻�����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD);
	/******************************************************************************
	* SetTURNServerIPPort
	*����������TURN��������ַ,Ҳ����Ϊ������ַ
	*���룺	cszTurnIPPort		-TURN��������ַ ����IP:Port
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetTURNServerIPPort(const char*cszTurnIPPort);
	/******************************************************************************
	* SetSTUNServerIPPort
	*����������STUN��������ַ,Ҳ����Ϊ������ַ
	*���룺	cszStunIPPort		-STUN��������ַ ����IP:Port
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetSTUNServerIPPort(const char*cszStunIPPort);
	/******************************************************************************
	* SetTurnTransportType
	*����������TURN����ͨ������
	*���룺	nType		-TURN����ͨ������ 17 udp, 6 tcp, 255 tls
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetTurnTransportType(int nType);
	/******************************************************************************
	* SetDNSServerIP
	*����������DNS��������ַ ���������ЧDNS�Ͳ�Ҫ����
	*���룺	cszDNSIP		-����DNS��������ַ ����IP
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetDNSServerIP(const char*cszDNSIP);
	/******************************************************************************
	* SetDNSServerIP2
	*����������DNS��������ַ2 ���������ЧDNS�Ͳ�Ҫ����
	*���룺	cszDNSIP2		-����DNS��������ַ ����IP
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	static int SetDNSServerIP2(const char*cszDNSIP2);

};




#endif
