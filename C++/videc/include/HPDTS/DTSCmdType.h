
//========================================================================================
//版权所有: 版权所有(C) 2008，上海华平信息技术股份有限公司
//系统名称: AVCON7
//作　　者: duquansheng
//创建日期: 2008-3-31
//功能说明: DTS所有命令解析头定义及模块共用参数定义
//

#pragma once


#define LOCAL_DEBUG
#undef	LOCAL_DEBUG


#define FILE_TRANS_MAXLENGTH 8*1024	  //每个包的大小
#define FILE_TRANS_PACKETCOUNT 3      //第一次请求的个数

#define LOCAL_SESSIONID 1		  //本地SessionID
#define DEFNET_SESSIONID 2		  //默认网络SessionID
#define  HUBEI_MEDICAL_SDK   //湖北sdk版本 要定义这个宏 否则注释
//文件传输结构体定义
typedef struct tagDtsFileList
{
	uint32_t	ulFileID;
	uint32_t	ulFileLength;
	uint32_t	ulPosition;		
	uint32_t	ulFromSessionID;
	char			chSendUserID[255];
	char			chFileName[128];
	char			chFilePath[255];
	char			chDescription[255];//产品描述
	char			chStartTime[30];

}DTS_FILE_LIST,*PDTS_FILE_LIST;

//标注，白板对象结构体定义
typedef struct tag_ObjDataBuf
{
	char*			pData;
	int32_t				nLen;
	uint32_t	ulObjId;
}OBJ_DATA_BUF, *POBJ_DATA_BUF;

#ifndef _WIN32
typedef uint32_t	DWORD;
typedef uint32_t	UINT;
typedef uint32_t   COLORREF;
typedef struct tagPOINT
{
	int32_t  x;
	int32_t  y;
} POINT, *PPOINT;
typedef struct tagRECT
{
	int32_t    left;
	int32_t    top;
	int32_t    right;
	int32_t    bottom;
} RECT, *PRECT;
#endif

typedef struct tagDtsTemData
{	
	UINT			nCurPage;	
	UINT			nTotalPage;	
	POINT			ptFile;			//共享文件的大小
	POINT			ptClient;		//显示区域的大小
	UINT			nFlashStatus;	//flash 播放状态
	UINT			nFlashPosition;	//flash 播放位置
	UINT			nDocZoomSize;	//文档共享缩放比例
	UINT			nDocLeft;		//文档共享移动后位置
	UINT			nDocTop;		//文档共享移动后位置

}DTS_TEM_DATA,*PDTS_TEM_DATA;

//文件处理状态
enum enumFILESTATUS
{
	FILESTATUS_PAUSE = 0,			//暂停
	FILESTATUS_DONEUP,				//上传完成
	FILESTATUS_DONELOAD,			//下载完成
	FILESTATUS_WAIT,				//等待
	FILESTATUS_TRANSMIT,			//传输中
};

//DTS处理命令
enum enumDTSCOMMAND
{	
	WB_COMMAND_DATA = 0,			//白板数据

	/*－－文件传输界面发送命令－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	FILE_SEND_ADDFILE,				//发送添加文件
	FILE_SEND_DELETEFILE,			//发送删除命令
	FILE_REQUEST_DOWNLOAD,			//发送下载文件请求	
	FILE_PAUSE_DOWNLOAD,			//暂停下载文件
	FILE_CONTINUE_DOWNLOAD,			//继续下载文件
	FILE_PAUSE_UPLOAD,				//暂停上传文件
	FILE_CONTINUE_UPLOAD,			//继续上传文件	
	FILE_GET_FILEINFO,				//获取文件信息
	FILE_GET_POSITION,				//得到文件传输位置
	FILE_REQUEST_FILELIST,			//请求所有文件列表
	/*－－文件传输界面回调命令－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	FILE_INSERT_FILE,				 
	FILE_DELETE_FILE,				//删除文件信息
	FILE_UPDATE_STATUS,				//更新文件状态	
	FILE_UPLOAD_ERROR,				//上传文件错误
	FILE_DOWNLOAD_ERROR,			//下载文件错误
	FILE_ONGET_FILEINFO,			//回调文件信息
	FILE_ONGET_POSITION,			//回调文件传输位置

	/*－－文件传输内部处理命令－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	FILE_GET_FILELIST,				//获取所有文件列表
	FILE_ONGET_FILELIST,			//回调所有文件列表
	FILE_AUTO_DOWNLOAD,				//自动继续下载文件
	FILE_ONREQUEST_DOWNLOAD,		//收到下载文件请求
	FILE_ONDOWNLOAD_DATA,			//收到下载文件数据

	/*－－文档共享标注相关－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	FS_ADD_OBJ,						//添加对象
	FS_DELETE_OBJ,					//删除对象	
	FS_REQUEST_ALLOBJ,				//请求所有对象	
	FS_CHANGE_PAGE,					//Page改变
	FS_GETCUR_PAGEOBJ,				//得到当前Page对象
	FS_DELCUR_PAGEOBJ,				//删除当前Page对象
	FS_SET_FILERECT,				//设置文件及区域大小

	/*－－Flash共享相关－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	FLASH_SHARE_PLAY,				//开始播放Flash	
	FLASH_SHARE_STOP,				//停止播放Flash	
	FLASH_SHARE_PAUSE,				//暂停播放Flash
	FLASH_SHARE_POSITION,			//更新Flash播放位置
	FLASH_SHARE_SEEK,				//SeekFlash播放位置
	FLASH_SHARE_REQUEST,			//请求Flash播放位置

	/*－－2008.6.4 新加 文档共享相关－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	FS_SET_ZOOM,					//缩放	
	FS_MOVE_POS,					//移动位置	
	FS_GET_POS,						//获取位置	
	FS_ON_CURPAGEOBJ,				//回调当前页数据
	/*新白板属性服务*/
	WB_NEW_COMMAND_DATA,				//新白板服务

	/*－－20121120 新加会议数据相关－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
	MET_DTS_CONNECTED=1000,				//建立连接
	MET_DTS_ADD=1001,					//增加对象
	MET_DTS_UPDATE=1002,				//修改对象
	MET_DTS_DELETE=1003,				//删除对象
	MET_DTS_REQUEST=1004,				//请求对象
	MET_DTS_RESPONSE=1005,				//回调请求对象
	MET_DTS_CHATMSG=1006,				//会议讨论消息
	MET_DTS_READY=1007,					//请求前数据准备,通知客户端清理数据
};
//new wb
#define SHARE_OBJID      0


#define KEY_OBJ_BKG																			KEY_USER + 161 //对象背景属性


typedef enum _eAttributeKey
{
	KEY_OBJ_ID = 0,	//对象ID
	KEY_OBJ_TYPE,   //对象类型
	KEY_FILE_PATH,	//对象资源文件路径
	KEY_FILE_SIZE,	//对象资源文件大小
	KEY_FILE_TYPE,	//对象资源文件类型
	KEY_FILE_POS,	//对象资源文件下载的位置
	KEY_FILE_SENDER,//对象资源上传者
	KEY_EVENT_ID, //对象关联的事件ID
	KEY_OBJ_PAGE,//对象所在的页
	KEY_OBJ_LOCK	,//对象锁定属性 add by huanglongquan2011年12月3日   如果新的key的话就在这个后面、key_user之前添加
	KEY_DRAG_CAMERA_TO_WB,	//对象是从u7拖拽摄像头到白板

	KEY_FILE_ID,			//用于轨迹笔

	KEY_USER,			 //用户自定义
}eAttributeKey;

typedef enum _eWbCmdOperation
{
	CMD_ADD_ATTRIBUTE = 200,		 //添加属性
	CMD_DEL_ATTRIBUTE,			 //删除属性
	CMD_CHANGE_ATTRIBUTE,		 //改变属性
	CMD_REQUEST_ALL_ATTRIBUTE,	 //获取全部属性
	CMD_CLEANUP_ATTRIBUTE,		 //清空全部属性
	CMD_REQUEST_ALL_OBJ_ATTRIBUTE,//申请全部对象
	CMD_REQUEST_STATUS_INDEX,	 //获取状态索引
	CMD_REQUEST_STATUS_NUM,      //获取状态数量
	CMD_REDO_STATUS,			 //前进一步
	CMD_UNDO_STATUS,			 //后退一步
	CMD_SET_STATUS_NUM,			 //设置状态数量	
	CMD_P2P_DATA,				 //点对点临时数据
	CMD_BROADCAST_DATA,			 //广播临时数据
	CMD_P2MCU_DATA,				 //直接发给服务器
	CMD_START_STREAM,			 //开始发送数据
	CMD_RESTART_STREAM,			 //重新发送数据
	CMD_ASKFOR_STREAM,			 //申请数据包
	CMD_SAVE_STREAM,			 //写入数据包
	CMD_END_STREAM,				 //完成发送数据
	CMD_STOP_STREAM,			 //停止发送流
	CMD_RELATE_STREAM,			 //将属性和流关联
	CMD_ERROR_STREAM,		     //传输的数据有异常
	//被动方式
	CMD_PASSIVE_START,			 //开始被动传输数据流
	CMD_PASSIVE_SAVE,		     //被动接受数据流
	CMD_PASSIVE_END,
	CMD_PASSIVE_STOP,			 //终止被动接受数据
	CMD_PASSIVE_ERROR,

	CMD_DEL_ALL_ATTRIBUTE,		 //删除全部属性
	//页操作
	CMD_DEL_PAGE_ALL_ATTRIBUTE, //删除页的全部对象
	CMD_DEL_PAGE,                       //	删除page
	CMD_INSERT_PAGE,                  //插入页
	CMD_CHANGE_PAGE,               //交换页
	CMD_MOVE_PAGE,
	CMD_EXCHANGE_PAGE,

#ifdef HUBEI_MEDICAL_SDK
	//用户颜色操作定义
	CMD_ASKFOR_USERCOLOR,
	CMD_RELATIVE_USERCOLOR,
	CMD_BROADCAST_USERNAMECOLOR,
#endif

}eWbCmdOperation;

#ifdef HUBEI_MEDICAL_SDK
typedef enum _eUserColorWB
{
	Red_Color = 0,					//红色 RGB(255,0,0)
	Green_Color,						//绿色 RGB(0,255,0)
	Blue_Color,						//蓝色 RGB(0,0,255)
	Yellow_Color,					//黄色 RGB(255,255,0)
	Fuchsia_Color,					//紫色颜色 RGB(255,0,255)
	Aqua_Color,						//水绿色 RGB(0,255,255)
	MidnightBlue_Color,			//午夜蓝 RGB(25,25,112)
	SeaGreen_Color,				//海绿色 RGB(46,139,87)
	Teal_Color,						//水鸭色 RGB(0,128,128)
	DarkSlateGray_Color,		//墨绿色 RGB(47,79,79)
	DarkGreen_Color,				//青绿 RGB(0,100,0)
	SaddleBrown_Color,			//马鞍棕色 RGB(139,69,19)
	Maroon_Color,					//枣红 RGB(128,0,0)
	BlueViolet_Color,				//蓝紫罗兰 RGB(138,43,226)
	DarkOrchid_Color,			//暗兰花紫 RGB(153,50,24)
	Default_Color,					//默认颜色 RGB(0,0,0)
}eUserColorWB;
#endif

