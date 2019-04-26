
#ifndef DHSYSABLE_H
#define DHSYSABLE_H

typedef enum
{
	ABILITY_WIRELESS_CFG = 18,			//wireless配置能力
	ABILITY_DEVALL_INFO = 26,			//设备的能力列表
	ABILITY_CARD_QUERY = 0x100,			//卡号查询能力

} DH_SYS_ABILITY;


/************************************************************************
 * 结构体定义
************************************************************************/

// 设备支持功能列表
typedef struct 
{
	char IsFTPEnable;					//FTP
	char IsSMTPEnable;					//SMTP
	char IsNTPEnable;					//NTP
	char IsAutoMaintainEnable;			//自动维护
	char IsVideoCoverEnable;			//区域遮挡
	char IsAutoRegisterEnable;			//主动注册
	char IsDHCPEnable;					//DHCP
	char IsUPNPEnable;					//UPNP
	char IsCommSnifferEnable;			//串口抓包
	char IsNetSnifferEnable;			//网络抓包
	char IsBurnEnable;					//刻录功能
	char IsVideoMatrix;					//视频矩阵
	char IsRev[244];					//保留字段

}DH_DEV_ENABLE_INFO;

// 卡号查询能力结构体
typedef struct 
{
	char  IsCardQueryEnable;
	char  iRev[3];
} DH_CARD_QUERY_EN;

// wireless能力结构体
typedef struct 
{
	char  IsWirelessEnable;
	char  iRev[3];
}DH_WIRELESS_EN;

#endif // DHSYSABLE_H