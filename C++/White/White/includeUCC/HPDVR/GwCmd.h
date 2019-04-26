#ifndef __GW_CMD_H__
#define __GW_CMD_H__

typedef struct tagGW_ENCODER_ITEM
{
public:
	tagGW_ENCODER_ITEM()
	{
		ulEID = "";
		szPass = "";
		szName = "";
		ulChannelNum = 0;
	}
	tagGW_ENCODER_ITEM& operator=(const tagGW_ENCODER_ITEM& rEncoderItem)
	{
		ulEID = rEncoderItem.ulEID;
		szPass = rEncoderItem.szPass;
		szName = rEncoderItem.szName;
		ulChannelNum = rEncoderItem.ulChannelNum;
		return *this;
	}
public:
	std::string ulEID;
	std::string szPass;
	std::string szName;
	ULONG  ulChannelNum;

}GW_ENCODER_ITEM,*PGW_ENCODER_ITEM;

typedef struct tagGW_ENCODER_CHANNEL_ITEM
{
public:
	tagGW_ENCODER_CHANNEL_ITEM()
	{
		eid = "";
		cid = 0;
		name = "";
		locaddr = "";
		tcpport = 0;
		udpport = 0;
		audioid = 0;
		videoid = 0;
		subvideoid = 0;
		mcuid = "";
		mcutype = "";
		mcuaddr = "";
		mcuport = 0;
		videownd=NULL;
		type=0;
		parentwnd=NULL;
	}
	tagGW_ENCODER_CHANNEL_ITEM& operator=(const tagGW_ENCODER_CHANNEL_ITEM& rChannelItem)
	{
		eid = rChannelItem.eid;
		cid = rChannelItem.cid;
		name = rChannelItem.name;
		locaddr = rChannelItem.locaddr;
		tcpport = rChannelItem.tcpport;
		udpport = rChannelItem.udpport;
		audioid = rChannelItem.audioid;
		videoid = rChannelItem.videoid;
		subvideoid = rChannelItem.subvideoid;
		mcuid = rChannelItem.mcuid;
		mcutype = rChannelItem.mcutype;
		mcuaddr = rChannelItem.mcuaddr;
		mcuport = rChannelItem.mcuport;
		videownd=rChannelItem.videownd;
		parentwnd=rChannelItem.parentwnd;
		type=rChannelItem.type;
		return *this;
	}
public:
	std::string	eid;
	ULONG cid;
    std::string	name;
	std::string locaddr;
	UINT tcpport;
	UINT udpport;		
	ULONG audioid;
	ULONG videoid;
	ULONG subvideoid;
	std::string mcuid;
	std::string	mcutype;
	std::string	mcuaddr;
	USHORT mcuport;
	UINT type; //±‡¬Î∆˜¿‡–Õ //whidbeycai20080506
	HWND videownd;
	HWND parentwnd;
}GW_ENCODER_CHANNEL_ITEM,*PGW_ENCODER_CHANNEL_ITEM;

typedef struct tagGW_ADDRESS
{
public:
	tagGW_ADDRESS()
	{
		szIP = "";
		usPort = 0;
		szGW_LoginID = "";
		szGW_LoginPasswd = "";
	}
	tagGW_ADDRESS& operator=(const tagGW_ADDRESS& rGwAddress)
	{
		szIP = rGwAddress.szIP;
		usPort = rGwAddress.usPort;
		szGW_LoginID = rGwAddress.szGW_LoginID;
		szGW_LoginPasswd = rGwAddress.szGW_LoginPasswd;
	}
public:
	std::string szIP;
	unsigned short usPort;
	std::string szGW_LoginID;
	std::string szGW_LoginPasswd;

}GW_ADDRESS,*PGW_ADDRESS;
#define CMD_GW_ENCODER_LOGIN	"gw_encoder_login"
#define CMD_GW_ENCODER_CHANNEL  "gw_encoder_channel"
#define CMD_GW_ENCODER_LOGOUT	"gw_encoder_logout"
#define CMD_GW_ENCODER_P2P		"gw_encoder_p2p"
#define CMD_GW_USER_LOGIN		"gw_user_login"
#define CMD_GW_ENCODER_ITEM		"gw_encoder_item"

#endif