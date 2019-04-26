/*
 *	HCstruct.h
 */

#define DETECTPICLEFT 35
#define DETECTPICTOP 15
#define SCREEN_EDGE 30

#define MINSPACE		(200*1024*1024)

#define	CYCLE_TIMER			1		//循环显示时钟
#define	AUTORECORD_TIMER	2		//自动录像时钟
#define	CHECKSERVER_TIMER	3		//硬盘录像机校时时钟
#define	FREESPACE_TIMER		4		//检查硬盘时钟
#define	FILEINTERVAL_TIMER	5		//保存文件时间间隔时钟
#define PING_TIMER			6		// 灯显示时钟
#define UPGRADE_TIMER		7		//升级时钟
#define PLAY_TIMER			8		//本地回放时显示播放状态时钟
#define RPSTATE_TIMER		9		//远程回放文件播放状态时钟
#define DOWNSTATE_TIMER		10		//下载文件时钟
#define FORMAT_TIMER		11		//远程格式化硬盘
#define EVENTLOG_TIMER		12		//事件日志
#define PLAYBYTIME_TIMER	13		// 按时间回放
#define DOWNBYTIME_TIMER	14		//按时间下载
#define SERIAL_TIMER		15		//串口透明通道发送	

#define DEFAULTBRIGHTNESS 6
#define DEFAULTCONTRAST 6
#define DEFAULTSATURATION  6
#define DEFAULTHUE 6
#define DEFAULTVOLUME 50

#define MAXIPNUMBER 50			//最多管理的服务器数目
#define MAXCHANNUM 16			//每个服务器最多的监控通道数
#define MAXVIEWNUM	16			//最大的窗口数
#define MAXNUMBER   512			//可预览的最大数
#define	MAXALARMIN	16			//客户端最多可接收报警数
//#define MAXDISKNUM 8			//最大硬盘数	

typedef struct{
	CString	m_csServerName;
	CString m_csServerIP;
	DWORD	m_dwServerIPvalue;
	CString m_csServerMultiIP;
	DWORD	m_dwServerMultiIPvalue;
	CString m_csServerAdminName;
	CString m_csServerAdminPasswd;
	LONG	m_lServerID;
	LONG	m_lStartChannel;
	int		m_iServerChannelNumber;	//通道数
	WORD	m_wServerPort;			//服务器端口号
	WORD	m_wServerType;			//1 DVR, 2 ATMDVR, 3 DVS .......;
	CString	m_csSerialNumber;  //序列号
	DWORD	m_dwAlarmInNum;		//报警输入个数
	DWORD	m_dwAlarmOutNum;	//报警输出个数
	DWORD	m_dwDiskNum;			//硬盘个数
	DWORD	m_dwDevSoftVersion;		//设备软件版本
}SERVER_INFO, *PSERVER_INFO;

typedef struct{
	CString	m_csIP;					//报警服务器IP
	CString	m_csUsername;
	CString	m_csPassword;
	LONG	m_lServerID;
	int		m_iChannel;				//报警通道
	int		m_iPortIndex;			//报警输入编号
	int		m_iAlarmType;
	int		m_iAlarmLevel;
	WORD	m_wServerPort;
	LONG	m_lNetIndex;			//
}GENERALALARM_INFO,*PGENERALALARM_INFO;			//客户端报警设置结构

typedef struct {
	CString	m_csChanName;			//通道名称
	CString	m_csIP;					//视频服务器IP地址
	DWORD	m_dwIPvalue;
	int		m_iChannel;				//监控通道号
	DWORD	m_dwProtocol;			//网络协议
	DWORD	m_dwAddress;			//解码器地址
	CString m_csUser;				//用户名
	CString m_csPassword;			//密码
	LONG	m_lServerID;
	CString m_csMultiIP;			//
	DWORD	m_dwMultiIPvalue;
	DWORD	m_dwAlarmlevel;	
	WORD	m_wServerport;			//服务器端口号
	WORD	m_wJMQtype;				//解码器类型
	BOOL	m_bChoosed;				//该通道被选中网络播放
	int		m_iServerNum;			//对应服务器编号
	int		m_iChannelNum;			//对应监控通道编号
	int		m_iSequence;			//监控顺序
//	BOOL	m_bWiper;		//雨刷开关
//	BOOL	m_bLight;		//灯光开关
}IP_INFO,*PIP_INFO;

typedef struct{
	UINT	m_iBrightness;				//1-10
	UINT	m_iContrast;				//1-10
	UINT	m_iSaturation;				//1-10
	UINT	m_iHue;						//1-10
}VIDEO_INFO, *PVIDEO_INFO;

typedef struct{
    WORD	m_iStartHour;
	WORD	m_iStartMinute;
	WORD	m_iStopHour;
	WORD	m_iStopMinute;
	WORD	m_iStartTime;
	WORD	m_iStopTime;
}CLIENTRECORDTIME,*PCLIENTRECORDTIME;

typedef struct
{
	BOOL		m_bCyclePlay;				//是否循环播放
	int			m_iCycleTime;				//循环播放时间
	BOOL		m_bUseCard;					//是否采用解码卡
	BOOL		m_bNTSC;					//FALSE：Pal制  TRUE：NTSC制
	
	BOOL		m_bAutoRecord;				//是否自动录像;
	BOOL		m_bCycleRecord;				//是否循环录像
	int			m_iStartRecordDriver;		//客户端录像起始盘符
	int			m_iEndRecordDriver;			//客户端录像终止盘符
	int			m_iRecordFileInterval;		//录像文件打包时间
	CLIENTRECORDTIME	m_ClientRecordTime[7][4];
	CString		m_csClientRecordPath;		//客户端当前录像路径
	BOOL		m_bAllDiskFull;				//客户端硬盘满的标志
	
	CString		m_csDownLoadPath;			//远程文件下载保存路径
	CString		m_csPictureSavePath;		//抓图保存路径
	CString		m_csLogSavePath;			//日志保存路径
	
	BOOL		m_bPlaying;					//正在播放的标志
	BOOL		m_bCycling;					//正在循环播放的标志
	BOOL		m_bPaused;					//循环暂停
	BOOL		m_bNextPage;				//手动循环下一页
	BOOL		m_bFrontPage;				//手动循环上一页
	BOOL		m_bEnlarged;				//图像窗口放大
	BOOL		m_bFullScreen;				//图像全屏放大
	BOOL		m_bMultiScreen;				//多窗口全屏
	BOOL		m_bPlaySound[MAXVIEWNUM];	//窗口声音播放
	
	int			m_iAlarmHandle[MAXIPNUMBER];	//布防、撤防句柄
	
	VIDEO_INFO	m_VideoInfo[MAXVIEWNUM];		//视频参数
	UINT		m_Volume;						//音量
	
	BOOL		m_bAlarmPlaying;
	
	BOOL		m_bCheckServerTime;			//硬盘录像机校时
	long		m_iCheckServerTime;			//硬盘录像机校时时间间隔
}CLIENTPARAM, *PCLIENTPARAM;