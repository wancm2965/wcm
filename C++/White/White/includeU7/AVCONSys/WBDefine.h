


#if !defined(AFX_WBDEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_WBDEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WB_API  _declspec(dllexport)
class IHPNetNotify;
class IHPWBNotify;
enum DRAW_STATE   //运行状态
{
	
	DRAW_START = 0,   //初始状态
	DRAW_NORMAL,  //一般化状态
	TODUSTBIN_OVER,		//拖动至垃圾箱并删除成功
	MOVEING_RULERBTN,  //移动直尺圆
	ENTER_SETSQUARE,   //三角板
	ENTER_COMPASS,     //圆规
	DRAW_SELECTINT, //用矩形选择时画选择框
	ENTER_VIEW_BUTTON,	//按中白板上的按钮
	ENTER_BUTTON,	//按中选择框上的按钮
	ENTER_PROTRACTOR,//量角器

	ENTER_CONTROL,	//按中控制按钮
	DRAW_LINE,    //画直线状态
	DRAW_POLYLINE, //画任意线
	DRAWING_LINE, //正在画直线（鼠标左键已经按下）
	DRAWING_POLYLINE,  //正在画任意线
	DRAW_TEXT,		//输入文字
	DRAWING_TEXT,	//正在输入文字
	DRAWED_TEXT,    //文字和注释输入完毕
	DRAW_RECT,  //画矩形
	DRAWING_RECT, //正在画矩形
	DRAW_POLYGON, //画多边形
	DRAWING_POLYGON, //正在画多边形
	DRAWEND_POLYGON, //画一个多边形结束
	DRAW_ELLIPSE,	//画椭圆状态
	DRAWING_ELLIPSE,	//正在画椭圆
	DRAW_TRIANGLE,	//画三角形
	DRAWING_TRIANGLE,	//正在画三角形
	DRAW_RHOMBUS,	//画菱形
	DRAWING_RHOMBUS,	//正在画菱形
	MOVING_LINE,  //移动直线
	DRAW_ROUND,    //旋转状态
	DRAW_ERASE,   //擦除
	DRAW_ERASING,  //正在擦除
	DRAW_CLEANUP,  //清除
	DRAW_CLEANUPING, //正在清除
	DRAW_RGNDELETE, //区域删除
	DRAW_RGNDELETING, //正在区域删除
	ADDPICTURE,		//添加图片
	ADDDICOM,       //添加DICOM医疗图片
	ADDMOVIE,		//添加视频
	ADDNEWMOVIE,	//添加新视频
	ADDCAMERA,		//添加摄像头
	ADDDOC,			//添加文档
	ADDMUISC,		//添加音乐	
	ADDFLASH,		//添加flash
	ADD_ANNOTATIONS, //添加标注
	CHANGE_ANNOTATIONS, //改变标注
	DRAW_RECTMAQN,   //放大镜
	DRAW_SLIDER_MAQN,//放大镜上的滑动条
	MEDIA_CTRLING,     //正在控制视频
	MOVE_DUSTBIN,		//移动垃圾箱
	ADDFLASHFROMRESDLG,//资源面板拖入flash
	ADDDOCFROMRESDLG,//资源面板拖入文档
	ADDPICTUREFROMRESDLG,//资源面板拖入图片
	ADDNEWMOVIEFROMRESDLG,//资源面板拖入新视频
	ADDDOCFROMFOLDER,//外部文件夹拖入文档
	ADDFLASHFROMFOLDER,//外部文件夹拖入flash
	ADDNEWMOVIEFROMFOLDER,//外部文件夹拖入新视频
	ADDPICTUREFROMFOLDER,//外部文件夹拖入图片
    MOVE_CENTER_POINT,	//移动中心点	
	ADDRULER,//直尺
	DRAWEUP_TEXT, //文字和注释输入完毕,Up刷新
	DRAWANYTRIANGLE, //画任意三角形
	DRAWINGANYTRIANGLE, //正在画任意三角形
	EDITTRIANGLE,       //编辑三角形
	EDITINGTRIANGLE,     //正在编辑三角形
	DRAW_PALLELOGAM,       //画平行四边形
	DRAWING_PALLELOGAM,    //正在画平行四边形
	DRAW_PENTAGON,       //画五边形
	DRAWING_PENTAGON,    //正在画五边形
	DRAW_HEXAGON,       //画六边形
	DRAWING_HEXAGON,    //正在画六边形
	DRAW_FIVEPOINTSTART,       //画五角星
	DRAWING_FIVEPOINTSTART,    //正在画五角星
	DRAW_TRAPEZIUM,       //画梯形
	DRAWING_TRAPEZIUM,    //正在画梯形
	MOVE_TRAPEZIUMPOINT,   //移动梯形的顶点
	DRAW_POLYICON,   //画任意图标曲线
	DRAWING_POLYICON,   //正在画任意图标曲线
	ADDDXF,  //添加DXF文件
	DXF_DRAG, //DXF拖拽
	DXF_PREDRAG, //DXF拖拽前
	DRAW_MAGICLINE, //画魔术线
	DRAWING_MAGICLINE, //正在画魔术线
	MIDI_DRAG,
	MIDI_DRAGING
};

enum BRUSH_SIZE //橡皮擦尺寸
{
	BRUSH_SMALL = 10,
	BRUNSH_BIG = 20
};

enum WB_LAYER_CHARNG
{
	WB_LAYER_SUPER = 1, //图层向上
	WB_LAYER_UNDER, //图层向下
	WB_LAYER_TOP, //图层置顶
	WB_LAYER_BOTTOM, //图层置底
	WB_LAYER_BACKGROUND, //设为背景 
	WB_LAYER_DELETEBACKGROUND, //删除背景
	WB_LAYER_DONOTHING, //不处理 
};

typedef enum _eCommend_Type
{
	CMD_SCREEN = 0,//通知本地全屏
	CMD_SINGLE_NETWORK,//单机版中联网命令
	CMD_SINGLE_LOCAL,//单机版中断网到本地命令
	CMD_SINGLE_SHOTDOWN,//单机版中关机
	CMD_SINGLE_QUIT,//单机版中退出程序
	CMD_DROP_NOTIFY , //通知U7响应接收拖拽消息
	CMD_DROP_NOTIFY_TO_WB,  //通知U7响应接收拖拽消息到白板
	CMD_FOCUS_GET,   //获得编辑框焦点
	CMD_FOCUS_KILL   //失去编辑框焦点
}eCommend_Type;


enum WB_MEDIA_OPERATION
{
	WB_MOVIE_START	= 1,//视频、FLASH播放或暂停
	WB_MOVIE_PAUSE,
	WB_MOVIE_STOP,				//视频、FLASH停止
	WB_MOVIE_LOOP,				//视频、FLASH循环
	WB_MOVIE_SINGLEPLAY,        //FLASH单次播放
	WB_MOVIE_SYNCHRONISATION,	//视频、FLASH同步

	WB_DOCUMENT_PREPAGE,		//文档上一页
	WB_DOCUMENT_NEXTPAGE,		//文档下一页
	WB_DOCUMENT_FIRSTPAGE,		//文档第一页
	WB_DOCUMENT_LASTPAGE		//文档最后一页
};

enum WB_TYPE//版本
{
	WB_SINGLERVER = 1,		//单机版本
	WB_MULTIVER				//网络版本
};

enum RES_INPUT_METHOD
{
	NORMAL = 0,
	DOUBLE_CLK,//双击进资源
	DRAG_AND_DROP//拖拽资源
};
typedef struct tagMediaCamaraU7Data
{
	unsigned long nAudioID; //音频ID号
	unsigned long nVideoID; //视频ID号
	char strNodeID[MAX_PATH]; //节点ID
	char strNATIP[MAX_PATH];  //netID
	unsigned short nNATPort; // 网络端口
	char strLocalIP[MAX_PATH]; //本地IP地址
	unsigned short nLocalPort; //本地端口
	char strMCUID[MAX_PATH];    //MCU
	char strMCUIP[MAX_PATH];   //MCUIP
	unsigned short nServerPort; //服务器端口
	char m_strUCCUerID[MAX_PATH];  //视频拥有者ID
	bool bIsCaptureCarema;       //是主动(true)或被动（false）添加摄像头

}MediaCamaraU7Data;


//WBAPI 白板API函数声明
WB_API BOOL WB_ShowWhiteBoardDlg(void *pParent, IHPNetNotify* pDataNotify, const CWnd* pWndInsertAfter, CRect rect, UINT nFlags);

//I7调用白板接口
WB_API BOOL WB_ShowWhiteBoardDlgI7(HWND hWnd, IHPNetNotify* pDataNotify, RECT whiteboardRect, UINT nFlags);

//改变窗口位置
WB_API void WB_MoveBorad(CRect rect, UINT nFlags);

//改变白板窗口位置
WB_API void WB_MoveBorad(const CWnd* pWndInsertAfter, CRect rect, UINT nFlags);
//设置数据发送窗口指针
WB_API void WB_SetDataNotify(void *pDataNotify);
//全屏
WB_API void WB_SetFullScreen(BOOL bFullScreen = TRUE);
//还原
WB_API void WB_SetNormalScreen( );
//设置运行状态，其中包含直线、曲线、三角型等
WB_API void WB_SetDrawStatus(DRAW_STATE nDrawStatus);

//设置线型，比如实线、虚线、点划线等PS_SOLID、PS_DOT;当前系统中默认设置为PS_SOLID实线
WB_API void WB_SetWBLineType(int nWBLineType);

//设置线和图形的线宽，默认值为4
WB_API void WB_SetWBCurObjWidth(int nWBCurObjWidth);

//设置线冒
WB_API void WB_SetLineCap(int nStartLineCap,int nEndLineCap);

//设置对象透明度g_nTranspar
WB_API void WB_SetWBTranspar(float nTranspar);


//字体
WB_API void WB_SetCharfont();

WB_API void WB_Combinefile();		//合并文档


//水波纹特效处理
WB_API void WB_SetWaterStatus();

//设置橡皮擦尺寸
WB_API void WB_SetBrushSize(int nBrushSize);
//视图中新建一页
WB_API void WB_AddNewPage();
//视图中插入一页
WB_API void WB_InsertNewPage();
WB_API void WB_InsertNewPage(UINT nPageIndex);
//视图中的删除一页
WB_API void WB_DeletePage();
WB_API void WB_DeletePage(UINT nPageIndex);
//互换页
WB_API void WB_InterchangePage(UINT nFormerPage, UINT nTargetPage);
//视图中的上一页
WB_API void WB_PrePage();
//视图中的下一页
WB_API void WB_NextPage();
//视图中的第一页
WB_API void WB_FirstPage();
//视图中的最后页
WB_API void WB_LastPage();
// 图层上一层
WB_API void WB_SuperLayer();
// 图层下一层
WB_API void WB_UnderLayer();
// 图层底层
WB_API void WB_BottomLayer();
// 图层顶层
WB_API void WB_TopLayer();
// 添加图片
WB_API void WB_AddPicture();
WB_API BOOL WB_AddPicture(const char *pFilePathW,CRect rect = CRect(0,0,0,0));
// 添加新视频（jiabangping，临时用）
WB_API void WB_AddMedia();
WB_API BOOL WB_AddMedia(const char *pFilePathW);
WB_API void WB_AddCamera();
// 插入文档
WB_API void WB_AddDoc(CRect rect = CRect(0, 0, 0, 0));
WB_API BOOL WB_AddDoc(const char *pFilePathW, CRect rect = CRect(0, 0, 0, 0));
// 删除
WB_API void WB_DeleteSelectedObjects();
// 加载资源库
WB_API void WB_OpenResource();
//加载flash
WB_API void WB_AddFlash();
WB_API BOOL WB_AddFlash(const char *pFilePathW,CRect rect = CRect(0,0,0,0));
//加载dxf
WB_API BOOL WB_AddDxf(const char *pFilePath,CRect rect = CRect(0,0,0,0));
//保存
WB_API void WB_SaveWBFile();
WB_API void WB_SaveWBFile(const char* pFilePathW);
//保存成图片
WB_API void WB_SaveWBFileToPicture();
WB_API void WB_SaveWBFileToPicture(const char* pFilePath);
//另存
WB_API void WB_Resavefile();
WB_API void WB_Resavefile(const char* pFilePathW);
//打印
WB_API void WB_Printfile();
//打开文件
WB_API void WB_Openfile();//无参数默认是会弹出是否保存文件对话框以及打开文件路径对话框
WB_API void WB_Openfile(const char* pFilePathW); 
//填充色、线型、线宽、箭头选择
//WB_API void WB_OpenAttributeDlg();
//新建文件
WB_API void WB_Newfile();//无参数默认是会弹出是否保存文件对话框
WB_API void WB_Newfile(bool bSaveFile);
//关闭文件
WB_API void WB_Closefile();//无参数默认是会弹出是否保存文件对话框
WB_API void WB_Closefile(bool bSaveFile);
//是否显示全屏按钮 
WB_API void WB_ShowFullScreenBtn(bool show = true);
//单机版登入会议关闭白板
WB_API BOOL WB_SingleCloseFile();
//退出单机版白板
WB_API BOOL WB_SingleExit();

//关闭功能面板
WB_API BOOL WB_CloseFuntion();

//截屏
WB_API void WB_Cutscreen();
// 退出白板对话框
WB_API void WB_Close();
WB_API void WB_Undo(int );
WB_API void WB_Redo();

//处理同步接收的数据
WB_API void WB_ProcessDataIn(const char *pData, int nLen);

//请求所有对象属性
WB_API void WB_ReqAllObj();
//发送所有对象属性
WB_API void WB_SendAllObj();

 //删除所有对象，当文件操作是本地操作是，就发送删除所有对象的同步消息
WB_API void WB_CloseLine();

//检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
WB_API bool WB_BHardwareSupportMTTouch();

//是否授权触摸功能
WB_API bool WB_SetTouchAuthorization(bool bAuthorization);

//最终的授权情况
WB_API bool WB_GetTouchAuthorization();

//屏幕录制
WB_API void WB_RECScreen(void );

//挡板
WB_API void WB_Baffle(void );
//直尺
WB_API void WB_Ruler(void );
//三角板
WB_API void WB_SetSquare(void);
WB_API void WB_Protractor(void);//量角器
//圆规
WB_API void WB_Compass(void);
//功能面板
WB_API void WB_FunctionPanel();

//设置存放临时文件的目录
WB_API void WB_SetWBTempDir(const char *pWBTempDir);

//获取存放临时文件的目录
WB_API CString WB_GetWBTempDir();
//PreTranslateMessage消息
WB_API BOOL  DllPreTranslateMessage(MSG *pMsg);

//修改默认线条颜色
WB_API void WB_SetDefaultLineColoe(int nRed, int nGreen, int nBlue);

//设置是否填充颜色
WB_API void WB_SetIsSolid(bool isSolid);

//设置图形填充颜色
WB_API void WB_SetSolidColor(int nRed, int nGreen, int nBlue);

//设置白板背景颜色
WB_API void WB_SetWBPageColor(int nRed, int nGreen, int nBlue);

WB_API void WB_DeletePage();

WB_API void WB_InsertNewPage();

//调用屏幕绘图
WB_API void WB_DrawScreen(const CRect* rect);
//全屏切换
WB_API void WB_FullScreenShift(bool isFullScreen);
//玻璃透明效果切换
WB_API void WB_GuiseShift(bool isTransparent);
//关闭屏幕绘图窗口，默认发送同步消息，bIsSend为false时不发送
WB_API void WB_DestroyWnd(bool bIsSend = TRUE);
//查询屏幕绘图窗口是否透明，透明的话返回true否则返回false
WB_API bool WB_IsScreenDrawTrans();
//查询是否全屏
WB_API bool WB_IsFullScreen();
//打开控制条
WB_API void WB_OnCtrlDlg();
//白板所在屏为活动屏时可以接收广播
WB_API void WB_ShowBroadCast(bool bShow);
//外层窗口从最小化恢复时，是否要打开白板标注窗口
WB_API void WB_OpenSCDraw();
//白板是否最小化了，最小化则为TRUE，否则为FALSE
WB_API void WB_SetIsMinSize(BOOL bIsMinSize);
//当前窗口是否是全屏
WB_API void WB_IsWBFullScreen(bool isFullScreen);
//聚光灯
WB_API void WB_NewSpotlight();
//放大镜
WB_API void WB_Maqnifier();
//垃圾箱
WB_API void WB_Dustbin();
//鹰眼
WB_API void WB_EagleEye();
WB_API void WB_EagleEyeSetposAndShow(CRect rect);
//设置本地AVCON用户ID
WB_API void WB_SetLocalUserID(const char *pUserID);
//设置本地用户名称（广播光标的时候用）
WB_API void WB_SetLocalUserName(const char* str);
//AVCON用户下线通知
WB_API void WB_MemberOffline(const char *pUserID);

//对象角度旋转，逆时针为正，顺时针为负
WB_API void WB_RotateByAngle(int angle);

//对象水平垂直旋转，水平为0度，垂直为90度
WB_API void WB_RotateByHV(int HVAngle);

//克隆
WB_API void WB_CloneObject();
//无限克隆
 WB_API void  WB_InfiniteCloneObj();
//剪切
WB_API void WB_CutObject();
//复制
WB_API void WB_CopyObject();
//粘贴
WB_API void WB_PasteObject();
//删除全部
WB_API void WB_DeleteAll();
//回收站
WB_API void WB_Dustbin();
//调用DICOM接口
WB_API void WB_AddDICOM();
//调用DICOM接口,带路径
WB_API BOOL WB_AddDICOM(const char *pFilePathW,CRect rect = CRect(0,0,0,0));

//关于
WB_API void WB_AboutAvconWB();
//由AVCON提供的临时路径
WB_API void WB_SetFilePathsByavcon(const char *pLanguageFile,const char *pIniPath,const char *pTempPath);
//由AVCON设置是否锁定白板功能
WB_API void WB_AvconLock(bool bLock);
//由AVCON设置开始或结束会议录像
WB_API BOOL WB_StartConferenceRecord(bool bStart);
//设置会议录像指针
WB_API void WB_SetConferenceRecordNotify(IHPWBNotify *pConferenceRecordNotify);

//设置白板的版本类型
WB_API void WB_SetTypeOfWB(WB_TYPE typeOfWB);
//关闭所有子工具栏
WB_API void WB_CloseAllChildBox();
WB_API void WB_MediaSingelToNet();//视频网络到单机，被动方视频删除
WB_API void WB_SetMediaSinToIntnet( BOOL bMedia );
//设置存放U7的默认路径目录
WB_API void WB_SetU7FileDir(const char * lpszU7FileDir);

//外面传入页号保存白板中具体页位图给函数调用
WB_API BOOL WB_SaveWBPageToBitmap(UINT PageIndex,HBITMAP hBitmap);
WB_API UINT WB_GetCurPageCount();
//单机登陆成功与否
WB_API void WB_IsSuccessLoad(bool bLoadSuccess);
//设置是否为本地使用电子白板情况, 注意：请在判断硬件授权之前调用一次，之后还要
WB_API void WB_SetIsLoacalWhiteboard(bool isLoacalWhiteboard);
//设置登录服务器的时间,必须在判断硬件授权之前
WB_API void WB_SetServerTime(const char *pLoadServerTime);

//给U7调用的视频摄像头数据传输拖拽接口
WB_API void WB_SetDropNotifyMediaCamara(MediaCamaraU7Data *pMediaCamaraU7Data);
//处理U7中拖拽白板窗口后改变光标的位置
WB_API void WB_ChangCourTranDlgPosition(int x,int y); 

//获取当前选中白板对象数量
WB_API int WB_GetSelectWhiteBoardObjectCount();

//获取白板是否有复制或剪切对象
WB_API bool WB_GetIsCutOrCopyObject();

//获取选中对象中是否有组合对象
WB_API bool WB_GetIsCombinObject();

//获取当前最大图层号
WB_API int WB_GetPageMaxLayer();
//获取当前最小图层号
WB_API int WB_GetPageMinLayer();
//获取当前对象图层号
WB_API int WB_GetPageObjCurrentLayer();
//组合功能，在组合对象之前需保证对象是在选择状态
WB_API void WB_SetCombineObj();

//取消组合功能，在取消组合对象之前需保证被取消的组合对象是在选择状态
WB_API void WB_SetUnCombineObj();

//获取白板的宽度
WB_API int WB_GetWhiteboardWidth();
//获取白板的 高度
WB_API int WB_GetWhiteboardHeight();
//得到滚动条在当前视图中的起始点位置
WB_API POINT WB_GetScrollPosition();
//设置滚动条在当前视图中的起始点位置
WB_API void WB_SetScrollPosition(POINT scrollPoint);
//设置网络其他白板中滚动条位置
WB_API void WB_SendScrollPosToNet();
//判断单个对象是否锁定，只针对单个对象有效，多个对象时不处理
WB_API bool WB_GetSingleObjIsLock();
//获取白板对象个数
WB_API int WB_GetObjCount();
//刷新白板
WB_API void WB_Refresh();
//发送异步消息给白板
WB_API void WB_PostMessageToWhiteBoard(UINT msg, WPARAM wparam, LPARAM lparam);

#endif
