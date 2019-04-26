#pragma once

typedef enum {
	CFSRC_TREE	= 0,
	CFSRC_MAP	= 1,
	CFSRC_AV	= 2,
	CFSRC_MEM	= 3,
	CFSRC_TM	= 99,
	CFSRC_TVTREE  = 100,
	SFSRC_TVSCREEN =101,

}CFSRC;

typedef struct _HPMC_DROP
{
    int				Type;	 		//0：监控树，1：地图，2：音视频播放窗口 3:会议人员树
    HWND        	hWnd;    		//拖动源的句柄，比如树上的节点HTREEITEM 或者地图的句柄或者AV窗体的句柄
    int				AreaID;	 		//（区域ID） 根据情况填充(0:区域，1：设备，2：通道)
    char			MemberID[255];	//根据情况填充
    unsigned long	ChannelID;		//根据情况填充
}HPMC_DROP;
