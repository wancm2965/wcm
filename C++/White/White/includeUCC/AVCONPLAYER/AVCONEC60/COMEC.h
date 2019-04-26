//Comec.h
#ifndef __COMEC_H__
#define __COMEC_H__

#define COMEC_API AFX_EXT_CLASS

#define COMEC_ERR_OK						0
#define COMEC_ERR_UNSUPPORTED				-1
#define COMEC_ERR_MEMORY					-2
#define COMEC_ERR_NOT_INITIALIZED			-3

#define COMEC_ERR_CALL_NOT_FOUND			-2000
enum sip_protos { PROTO_NONE, PROTO_UDP, PROTO_TCP, PROTO_TLS, PROTO_SCTP };
#endif