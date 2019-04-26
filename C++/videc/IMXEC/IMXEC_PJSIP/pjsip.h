

#pragma once
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include "SIPAPIType.h"

/* Ringtones*/
#define RINGBACK_FREQ1	    440	    /* 400 */
#define RINGBACK_FREQ2	    480	    /* 450 */
#define RINGBACK_ON	        2000    /* 400 */
#define RINGBACK_OFF	    4000    /* 200 */
#define RINGBACK_CNT	    1	    /* 2   */
#define RINGBACK_INTERVAL   4000    /* 2000 */

#define RING_FREQ1	    440      //800
#define RING_FREQ2	    400      //640
#define RING_ON		    1000     //200  
#define RING_OFF	    500      //100
#define RING_CNT	    3
#define RING_INTERVAL	3000

struct SipConfigStruct
{
	int listenPort;
	bool noUDP;
	bool noTCP;
	char stunAddress[255];
	char turnAddress[255]; //add
	bool publishEnabled;
	bool userICE;
	bool userTURN; //add
	bool userSTUN; //add

	int expires;

	bool VADEnabled;
	int ECTail;

	char nameServer[255];

	bool pollingEventsEnabled;
	//20120605
	char logFileName[255]; //add
	int logLevel;

	// IMS specifics
	bool imsEnabled;
	bool imsIPSecHeaders;
	bool imsIPSecTransport;

	bool useTLS;
	//notice key file put to exe path
	//Certificate of Authority (CA) list file.
	char	ca_list_file[256];
	//Public endpoint certificate file, which will be used as client-
	//side  certificate for outgoing TLS connection, and server-side
	//certificate for incoming TLS connection.
	char	cert_file[256];
	//Optional private key of the endpoint certificate to be used.
	char	privkey_file[256];
	//Password to open private key.
	char	password[256];

	//TLS protocol method from #pjsip_ssl_method, which can be:
	// PJSIP_SSL_UNSPECIFIED_METHOD(0): default (which will use PJSIP_SSL_DEFAULT_METHOD)
	// PJSIP_TLSV1_METHOD(1):	   TLSv1
	// PJSIP_SSLV2_METHOD(2):	   SSLv2
	// PJSIP_SSLV3_METHOD(3):	   SSL3
	// PJSIP_SSLV23_METHOD(23):	   SSL23
	//Default is PJSIP_SSL_UNSPECIFIED_METHOD (0), which in turn will
	//use PJSIP_SSL_DEFAULT_METHOD, which default value is 
	//PJSIP_TLSV1_METHOD.
	int		method;
    /**
     * Number of ciphers contained in the specified cipher preference. 
     * If this is set to zero, then default cipher list of the backend 
     * will be used.
     *
     * Default: 0 (zero).
     */
    unsigned ciphers_num;

    /**
     * Ciphers and order preference. The #pj_ssl_cipher_get_availables()
     * can be used to check the available ciphers supported by backend.
     */
    pj_ssl_cipher ciphers;

	//Specifies TLS transport behavior on the server TLS certificate 
	//verification result:
	//- If \a verify_server is disabled (set to PJ_FALSE), TLS transport 
	//  will just notify the application via #pjsip_tp_state_callback with
	//  state PJSIP_TP_STATE_CONNECTED regardless TLS verification result.
	//- If \a verify_server is enabled (set to PJ_TRUE), TLS transport 
	//  will be shutdown and application will be notified with state
	//  PJSIP_TP_STATE_DISCONNECTED whenever there is any TLS verification
	//  error, otherwise PJSIP_TP_STATE_CONNECTED will be notified.
	//In any cases, application can inspect #pjsip_tls_state_info in the
	//callback to see the verification detail.
	//Default value is PJ_FALSE.
	pj_bool_t	verify_server;
	//Specifies TLS transport behavior on the client TLS certificate 
	//verification result:
	//- If \a verify_client is disabled (set to PJ_FALSE), TLS transport 
	//  will just notify the application via #pjsip_tp_state_callback with
	//  state PJSIP_TP_STATE_CONNECTED regardless TLS verification result.
	//- If \a verify_client is enabled (set to PJ_TRUE), TLS transport 
	//  will be shutdown and application will be notified with state
	//  PJSIP_TP_STATE_DISCONNECTED whenever there is any TLS verification
	//  error, otherwise PJSIP_TP_STATE_CONNECTED will be notified.
	//In any cases, application can inspect #pjsip_tls_state_info in the
	//callback to see the verification detail.
	//Default value is PJ_FALSE.
	pj_bool_t	verify_client;
	//When acting as server (incoming TLS connections), reject inocming
	//connection if client doesn't supply a TLS certificate.
	//This setting corresponds to SSL_VERIFY_FAIL_IF_NO_PEER_CERT flag.
	//Default value is PJ_FALSE.
	pj_bool_t	require_client_cert;
	//TLS negotiation timeout to be applied for both outgoing and
	//incoming connection. If both sec and msec member is set to zero,
	//the SSL negotiation doesn't have a timeout.
	//pj_time_val	timeout;
	long    timeout_sec;
	long    timeout_msec;
	//QoS traffic type to be set on this transport. When application wants
	//to apply QoS tagging to the transport, it's preferable to set this
	//field rather than \a qos_param fields since this is more portable.
	//Default value is PJ_QOS_TYPE_BEST_EFFORT.
	int qos_type;

	////Set the low level QoS parameters to the transport. This is a lower
	////level operation than setting the \a qos_type field and may not be
	////supported on all platforms.
	////By default all settings in this structure are disabled.
	//pj_qos_params qos_params;

	//Specify if the transport should ignore any errors when setting the QoS
	//traffic type/parameters.
	//Default: PJ_TRUE
	pj_bool_t qos_ignore_error;

	//}}gaoshizhong 20120710 Add

};

    struct VideoCodec
    {
	char name[64];		
	unsigned port;			   
	unsigned  pt;	   // Payload type
	char param[32];	    // Parameter	   
    } ;

  std::string ANSI2UTF8(const std::string&str);
  std::string UTF82ANSI(const std::string&str);

  void setSipConfig(SipConfigStruct* config);

  //SDP negotiation has failed  Disconnect call upon error.
  int DisconnectCall(int callId, int code,pjmedia_sdp_session * sdp);

  int SipDestroy(void);
  int SipInit(void);
  int SipRegisterAccount(void * token,char* uri, char* reguri, char* domain, char* username, char* password, char* proxy, bool isdefault);
  //Delete account.
  pj_status_t SipUnRegisterAccount(void * token);
  //使用VIA映射地址再次注册
  int SipRegisterAccountReply(void * token);

  void sip_busy_sleep(unsigned msec);
  pj_caching_pool * GetSipCachingPool();
  pjsip_endpoint * GetSipEndpoint();
  pj_pool_t * GetSipPool();
  void sip_register_thread(void);
  void CallStateDestroyed(void*  pCall,char * callId,pjsip_status_code cause,int reason);

  pj_bool_t SendResponse200OKNOSDP(pjsip_rx_data *rdata ,int code=PJSIP_SC_OK);
  void pjsip_parse_media_type( pj_pool_t *pool,const pj_str_t *mime,pjsip_media_type *media_type);
