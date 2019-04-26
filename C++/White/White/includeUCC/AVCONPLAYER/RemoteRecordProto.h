#ifndef __REMOTE_RECORD_PROTO_H__
#define __REMOTE_RECORD_PROTO_H__
// Packet type
#define CMD_BINARY_DATA	100
#define CMD_DEFBIN_DATA	101
#define CMD_XMLSTR_DATA	102
#define CMD_ENCRYPT_KEY	103
#define CMD_MISC_DATA	104
#define CMD_KEEP_ALIVE	105
#define CMD_AUTH		106

static const char *SYS_XNCP		  = "SYS-XNCP";

static const char *CMD_RECORD_GETFILE	= "GETFILE"; //filename
static const char *CMD_RECORD_HEADER	= "HEADER";	//fileinfo
static const char *CMD_RECORD_BLOCK		= "BLOCK";//from to
static const char *CMD_RECORD_INDEX		= "INDEX";   //filename, [from,to]


//
static const char *ATTR_RECORD_SIZE		= "ATTR_SIZE";
static const char *ATTR_RECORD_COUNT	= "ATTR_COUNT";
static const char *ATTR_RECORD_FILENAME = "ATTR_FILENAME";
static const char *ATTR_RECORD_FROM	 = "ATTR_FROM";
static const char *ATTR_RECORD_TO		 = "ATTR_TO";
static const char *ATTR_RECORD_ERRMSG	 = "ATTR_ERRMSG";

#endif