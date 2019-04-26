


#if !defined(AFX_WBDEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_WBDEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WB_API  _declspec(dllexport)
class IHPNetNotify;
enum DRAW_STATE   //运行状态
{
	
	DRAW_START = 0,   //初始状态
	DRAW_NORMAL,  //一般化状态
	TODUSTBIN_OVER,		//拖动至垃圾箱并删除成功
	
	DRAW_SELECTINT, //用矩形选择时画选择框
	ENTER_VIEW_BUTTON,	//按中白板上的按钮
	ENTER_BUTTON,	//按中选择框上的按钮

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
	ADDPICTUREFROMFOLDER//外部文件夹拖入图片
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
	CMD_SCREEN = 0,
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

//WBAPI 白板API函数声明
WB_API BOOL WB_ShowWhiteBoardDlg(void *pParent, void* pDataNotify, const CWnd* pWndInsertAfter, CRect rect, UINT nFlags);
//改变窗口位置
WB_API void WB_MoveBorad(const CWnd* pWndInsertAfter, CRect rect, UINT nFlags);
//设置数据发送窗口指针
WB_API void WB_SetDataNotify(void *pDataNotify);
//全屏
WB_API void WB_SetFullScreen(BOOL bFullScreen = TRUE);
//还原
WB_API void WB_SetNormalScreen( );
//设置运行状态
WB_API void WB_SetDrawStatus(DRAW_STATE nDrawStatus);
//字体
WB_API void WB_SetCharfont();
//设置橡皮擦尺寸
WB_API void WB_SetBrushSize(BRUSH_SIZE nBrushSize);
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
WB_API BOOL WB_AddPicture(const char *pFilePath);
// 添加新视频（jiabangping，临时用）
WB_API void WB_AddMedia();
WB_API void WB_AddCamera();
// 插入文档
WB_API void WB_AddDoc();
// 删除
WB_API void WB_DeleteSelectedObjects();
// 加载资源库
WB_API void WB_OpenResource();
//加载资源管理
WB_API void WB_OpenReManger();
//加载flash
WB_API void WB_AddFlash();
//保存
WB_API void WB_SaveWBFile();
WB_API void WB_SaveWBFile(const char* pFilePath);
//另存
WB_API void WB_Resavefile();
WB_API void WB_Resavefile(const char* pFilePath);
//打印
WB_API void WB_Printfile();
//打开文件
WB_API void WB_Openfile();//无参数默认是会弹出是否保存文件对话框以及打开文件路径对话框
WB_API void WB_Openfile(const char* pFilePath); 
//填充色、线型、线宽、箭头选择
//WB_API void WB_OpenAttributeDlg();
//新建文件
WB_API void WB_Newfile();//无参数默认是会弹出是否保存文件对话框
WB_API void WB_Newfile(bool bSaveFile);
//关闭文件
WB_API void WB_Closefile();//无参数默认是会弹出是否保存文件对话框
WB_API void WB_Closefile(bool bSaveFile);

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
WB_API BOOL WB_GetUndoNameList(CStringList *pUndoNameList);

//处理同步接收的数据
WB_API void WB_ProcessDataIn(const char *pData, int nLen);

//请求所有对象属性
WB_API void WB_ReqAllObj();
//发送所有对象属性
WB_API void WB_SendAllObj();

//检测当前硬件是否支持多点触摸功能(包括对avcon指定的硬件触摸屏的检测）
WB_API bool WB_BHardwareSupportMTTouch();

//是否授权触摸功能
WB_API bool WB_SetTouchAuthorization(BOOL bAuthorization);

//最终的授权情况
WB_API bool WB_GetTouchAuthorization();

//屏幕录制
WB_API void WB_RECScreen(void );

//挡板
WB_API void WB_Baffle(void );

//功能面板
WB_API void WB_FunctionPanel();

//设置存放临时文件的目录
WB_API void WB_SetWBTempDir(const LPCTSTR lpszWBTempDir);

//获取存放临时文件的目录
WB_API LPCTSTR WB_GetWBTempDir();
//PreTranslateMessage消息
WB_API BOOL  DllPreTranslateMessage(MSG *pMsg);

//修改默认线条颜色
WB_API void WB_SetDefaultLineColoe(int nRed, int nGreen, int nBlue);

WB_API void WB_DeletePage();

WB_API void WB_InsertNewPage();

//调用屏幕绘图
WB_API void WB_DrawScreen(const CRect* rect);
//全屏切换
WB_API void WB_FullScreenShift(bool isFullScreen);
//玻璃透明效果切换
WB_API void WB_GuiseShift(bool isTransparent);
//关闭屏幕绘图窗口
WB_API void WB_DestroyWnd();
//查询屏幕绘图窗口是否透明，透明的话返回true否则返回false
WB_API bool WB_IsScreenDrawTrans();
//查询是否全屏
WB_API bool WB_IsFullScreen();
//打开控制条
WB_API void WB_OnCtrlDlg();

//外层窗口从最小化恢复时，是否要打开白板标注窗口
WB_API void WB_OpenSCDraw();
//白板是否最小化了，最小化则为TRUE，否则为FALSE
WB_API void WB_SetIsMinSize(BOOL bIsMinSize);

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

//关于
WB_API void WB_AboutAvconWB();
//由AVCON提供的临时路径
WB_API void WB_SetFilePathsByavcon(const char *pLanguageFile,const char *pIniPath,const char *pTempPath);
//由AVCON设置是否锁定白板功能
WB_API void WB_AvconLock(bool bLock);
//由AVCON设置开始或结束会议录像
WB_API BOOL WB_StartConferenceRecord(bool bStart);
//设置会议录像指针
WB_API void WB_SetConferenceRecordNotify(void *pConferenceRecordNotify);
//设置白板的版本类型
WB_API void WB_SetTypeOfWB(WB_TYPE typeOfWB);
//关闭所有子工具栏
WB_API void WB_CloseAllChildBox();
#endif
