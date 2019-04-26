#pragma once
#include "stdafx.h"
#include "atltypes.h"
#include <vector>
#include <map>
#include <list>
#include <queue>
#include "common\WBCtrl.h"
#include "common\WBData.h"
#include "WBResourceDlg.h"
#include "WBResFunc.h"
#include "WBFunctionDlg.h"
#include "WBToolbarCtrlDlg.h"
#include "WBToolbarDlg.h"
#include "WBPromptFontSetDlg.h"
#include "WBRichEdit.h"
#include "UndoRedo.h"
#include "WBEagelEye.h"
#include "WBAboutDlg.h"
#include "WBOptionDlg.h"
//#include "WBResourceManager.h"
#include "DlgPrintLoadingMsg.h"
#include "Transparent_Dlg.h"
#include "WaterDlg.h"
#include "WBMTDrawObjectTally.h"
#include "LimitLocalObjOperate.h"
#include "MyDropTarget.h"
#include "WBRollCallDialog.h"
#include "WBRollCallShowDlg.h"
#include "ButtonCloseDlg.h"
#include "WBSavePictureDlg.h"
#include "GeometricRecognizer.h"
#include "PointRetain.h"
class CMedioVideoScreenDlg;

extern bool g_isLoacalWB;



int ReadMediaBitrate();//读取视频码流
void WriteMediaBitrate(int iValue);
int ReadMediaControlBitrate();//读取视频码流
void WriteMediaControlBitrate(int iValue);

#define MOUSEEVENTF_FROMTOUCH (0xFF515700)

#define UWM_TRACKPOPUPMENU					(WM_USER+250)	//自定义的弹出菜单消息
#define	UWM_CREATE_RESOURCE_DLG				(WM_USER+2786)	//发送创建资源窗体的消息

int ReadBroadCastStatus(); //读取广播光标状态，
void WriteBroadCastStatus(int nStatus); //写入配置文件

enum DICOM_ERROR_CODE
{
	DICOM_TOBMP_SUCCEES =1,      //转换成功
	DICOM_ERROR_OPENFAILED = -1 ,//文件打开失败
	DICOM_FILEWRITEFAIL = -2  ,    //文件转换失败
	FREEDISK_NOENOUGH = -3 ,  //保存文件时剩余磁盘空间不足
	NO_DICOM_FIEL = -4       //打开的不是dicom文件
};
//12-25添加 jin在快捷键中使用
#define  IsCTRLPressed() ((GetKeyState(VK_CONTROL) & (1<<(sizeof(SHORT)*8-1))) != 0)
//denghong
#define REFRESH_INTERVAL_TIME 40

class CTransparent_Dlg;
typedef std::map<string,CTransparent_Dlg*>MapCursorDlgPointer;  //用户广播的光标的透明对话框指针
typedef MapCursorDlgPointer::iterator IT_MapCursorDlgPointer;

class CWaterDlg;
typedef std::map<string,CWaterDlg*>MapWaterDlgPointer;  //用户广播的光标的透明对话框指针
typedef MapWaterDlgPointer::iterator IT_MapWaterDlgPointer;
// CWhiteBoardView 视图


typedef std::map<DWORD,HPMC_DROP*>MapDropCamInfo;
typedef MapDropCamInfo::iterator IT_MapDropCamInfo;//摄像头拖拽信息存储容器

typedef std::vector<DWORD> VECTORSPECPIC; //特殊图片对象ID
typedef VECTORSPECPIC::iterator IT_VectorSpepic;
class CDrawCtl;
class CWBSetSquare;
class CWBGlobalMediaCtrl;
class CWhiteBoardView : public CScrollView , public CWBMTObject
{
	DECLARE_DYNCREATE(CWhiteBoardView)
protected:
	CWhiteBoardView();           // 动态创建所使用的受保护的构造函数
	virtual ~CWhiteBoardView();

public:
	HWND GetHWND();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	///
	afx_msg LRESULT OnTrackPopupMenu(WPARAM wParam, LPARAM lParam);
	//菜单响应处理
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void ViewWindowFinalTreat();//在析构函数调用之前的处理工作，在白板的调用顺序是先调用此函数再调用OnDestroy（），后调用~CWhiteBoardView()

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	DECLARE_MESSAGE_MAP()


	/*-----------------------------------------------------------------------------------
	//菜单上的图片操作
	------------------------------------------------------------------------------------*/
public:
	void LoadPopupMenuBitmap();
	//删除当前选择的对象
	DWORD DeleteSelectedObjects();
	void CreateObjMenu(DWORD dwLayerIndex);	//创建菜单
	BOOL TranslateBitmapSize(  
		IN HDC hCurScreemDC,   //当前屏幕DC，可通过GetDC获得  
		IN HBITMAP hBmpSrc,    //原BITMAP句柄  
		IN int nSrcWidth,      //原BITMAP的宽度  
		IN int nSrcHeight,     //原BITMAP的高度  
		OUT HBITMAP &hBmpDst,  //改变大小后的BITMAP的句柄  
		IN int nDstWidth,      //改变大小后的BITMAP的宽度  
		IN int nDstHeight      //改变大小后的BITMAP的高度  
		);
	bool TranslateBitmapSizeToFitXP(CBitmap *pBitmap);	//改变CBitmap的大小用以适应XP,变小菜单图标以便于图标全部显示
private:
	CBitmap m_CheckedBmp;  //选中状态中
	CBitmap m_CutBmp;  //剪切
	CBitmap m_InfiniteCloneBmp;    //无限克隆
	CBitmap m_UnLockBmp;    //解锁
	CBitmap m_LockBmp;    //上锁
	//视频
	CBitmap m_MoviePlayBmp;    //播放
	CBitmap m_MoviePauseBmp;    //暂停
	CBitmap m_MovieStopBmp;    //停止
	
	CBitmap m_MediaCatchFrameBmp;   //截图
	CBitmap m_MediaSetmuteBmp;      //有音
	CBitmap m_MediaNoSetmuteBmp;      //静音
	//文档
	CBitmap m_DocPrePageBmp;    //上一页
	CBitmap m_DocFirstPageBmp;   //首页
	CBitmap m_DocNextPageBmp;      //下一页
	CBitmap m_DocLastPageBmp;      //末页
	CBitmap m_DocPrintCurBmp;		//打印当前页
	CBitmap m_DocPrintAllBmp;		//打印所有页

	//Dxf
	CBitmap m_DxfZoomInBmp;  //放大
	CBitmap m_DxfZoomOutBmp; //缩小
	CBitmap m_DxfDragBmp; //拖拽

	CBitmap m_SelectAllBmp;  //全选
	CBitmap m_UnselectBmp;      //取消选择
	CBitmap m_SckdBmp;           //生产刻度
	CBitmap m_EqualScaleZoomBmp;    //等比缩放
	CBitmap m_DeleteBackgroundBmp;    //删除背景图片
	CBitmap m_SetAsBackgroundBmp;    //设为背景
	CBitmap m_EditBmp;    //编辑
	CBitmap m_CopyBmp;  //复制
	CBitmap m_DeleteBmp;      //删除
	CBitmap m_DeleteAllBmp;  //删除全部
	CBitmap m_PasteBmp;    //粘贴
	CBitmap m_CloneBmp;    //克隆
	CBitmap m_CombineBmp;    //组合
	CBitmap m_UnCombineBmp;    //取消组合
	CBitmap m_TopLayerBmp;      //顶层
	CBitmap m_PreLayerBmp;    //上一层
	CBitmap m_NextLayerBmp;    //下一层
	CBitmap m_BottomLayerBmp;    //底层
	CBitmap m_AttributeBmp;    //属性

	CBitmap m_Deasil45Bmp;        //顺时针45度
	CBitmap m_Deasil90Bmp;        //顺时针90度
	CBitmap m_Widdershins45Bmp;   //逆时针45度
	CBitmap m_Widdershins90Bmp;   //逆时针90度
	CBitmap m_Rotation180Bmp;     //旋转180度
	CBitmap m_HirizontalBmp;      //水平放置
	CBitmap m_VerticalBmp;        //垂直放置

	CWBDocument *pDoc;

	/*-----------------------------------------------------------------------------------
	//触摸 与手势
	------------------------------------------------------------------------------------*/
public:

	///继承自AVCON_SDK_MTTouch::CMTObject
	//触摸按下
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//触摸移动
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//触摸弹起
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	///区域判断
	virtual bool BInRegion(const POINT &pt)const;
	/*---------------------------------------------------------------------------------------------
	参数一：低字节表示手势的大类[ETouchGestureType]，高字节表示手势的小类[如动态手势中的]
	参数二: 低字节表示x的坐标，高字节表示y方向的坐标. 单位为像素；坐标为屏幕坐标
	----------------------------------------------------------------------------------------------*/
	virtual void OnCallbackGesture(const unsigned int &nMessage, const unsigned int &nAppendMessage);
	/*---------------------------------------------------------------------------------------------
	参数为触摸过程中所发生的平移缩放旋转及相关量,详见参数数据结构
	----------------------------------------------------------------------------------------------*/
	virtual void OnGestureCombineHandle(const GestureCombinePar& gesPar);

	//覆盖系统的，不做任何处理
	afx_msg LRESULT OnGesture(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);

	///add by aolingwen 2012.11.16
	///用于触摸框选
	void SetRectSelect(CRect rectSelect);
	///用于触摸使用垃圾箱
	CWBDustbin* GetDustbin(){return m_pWBDustbin;}
	void MoveObjToDustbin(CPoint point,int id =0 );
	void SetObjectoutDustbin();
	void SetDustbinTouchID();
	///用于触摸使用放大镜
	CWBMagnifier* GetWBMagnifier(){return m_pWBRectMaqn;}
	void SetTouchEventId(unsigned long);
	//用于触摸无限克隆
	BOOL MTInfiniteClone();

	//白板与别的模版拖拽在U7下
	void SendDropNotify(HPMC_DROP *pBuffer);
	//外部拖拽视频，摄像头到白板
	void SendDropNotifyToU7(HPMC_DROP *pBuffer,bool bflag = true);

	void SendDropNotifyU7Message(HPMC_DROP *pBuffer,bool bflag = true);

	bool OpenCamaraFromU7(char *pbuff,int nLen);//摄像头由u7拖拽过来
	bool OpenCamaraFromU7(MediaCamaraU7Data *pMediaCamaraU7Data);//摄像头由u7拖拽过来


	bool OpenMediaFromU7(const char *pbuff);//视频
	CWBCamera *GetCamaraObjU7();
	void SetCamaraObjU7(CWBCamera *carama);

	void SetUserCameraName(string st) {m_strUserCurCameraName = st;}
	unsigned long GetchannelID() {return m_DropChanelID;}

private:
	CWBCamera *m_pCamaraObj;
	std::string m_strUserCurCameraName;
	unsigned long m_channelID; 

	unsigned long m_DropChanelID;
	//仅用于wm_touch消息
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;


	unsigned long m_ulTouchEventId;

private:
	CPoint m_pDownPoint;
	CPoint m_pMovePoint;
	///用于删除摄像头
	int m_nCameraObjId;	
	///end
	//用于触摸画图计数控制add by huanglongquan 2012年12月18日 
	CWBMTDrawObjectTally m_MTDrawObjectTally;

public:
	/*-----------------------------------------------------------------------------------
	//资源库
	------------------------------------------------------------------------------------*/
	//afx_msg HRESULT OnReceiveDragResouce(WPARAM wPar, LPARAM lPar);
	afx_msg HRESULT OnReceiveTouchResouce(WPARAM wPar, LPARAM lPar);
	CString GetResFilePath(){return m_sLastDragFileURL;}
	void	SetResFilePath();


	/*-----------------------------------------------------------------------------------
	//功能面板
	---------------------------------------------------------------------------------*/
public:
	// 获得功能对话框
	CWBFunctionDlg *GetWBFunctionDlg();
	//判断是否开览预功能面板
	BOOL IsOpenPreviewPanel();
	//判断是否打开功能面板
	BOOL IsOpenFunctionDlg();
	//更新所选对象的透明度
	void UpdateObjAlpha();
	//更新所选对象的颜色
	void UpdateObjColor();
	//更新所选对象的宽度
	void UpdateObjLineWidth();
	//更新所选所选对象的图标style
	void UpdateObjIcoStyle();
	//更新所选对象的线型
	void UpdateObjLineType();
	//更新所选对象的填充与否
	void UpdateObjSolidCheck();
	//更新所选对象的填充颜色
	void UpdateObjSolidColor();
	//更新所选对象的箭头
	void UpdateObjLineArrow();
	//更新对象的字体属性
	void UpdateObjFontFace();
	//更新对象的字体大小
	void UpdateObjFontSize();
	//更新对象的字体粗度
	void UpdateObjFontBold();
	//更新对象的字体倾斜
	void UpdateObjFontItalic();
	//更新所选对象的下划线
	void UpdateObjUnderline();
	//更新所选对象的输出排版
	void UpdateObjVertical();
	//更新对象的文本内容
	void UpdateObjTextStr();
	//更新手势操作属性（文档，视频截图）
	void UpdateMTOperate();
	//开启属性面板
	void OpenAttributeDlg();
	//删除文本编辑框
	void DeleteEdit();
	void DestroyRichWindow();
	void SetRichEditRect(CRect &rc);
	CRect GetRichEditRect();

	//合并两个文档
	bool CombineFile(CWBFile* dst, CWBFile* src);
private:
	CRect m_RichEditRect;
	bool  m_bIsconfernce;
private:
	//功能面板对话框
	CWBFunctionDlg	*m_pWBFunctionDlg;
	/*-----------------------------------------------------------------------------------
	//同步处理
	------------------------------------------------------------------------------------*/
public:
	void OnNETEC_NodeReceivedFromRouter(const char* pData,int nLen);   //将接收到的同步数据存入缓冲队列中
	//使用定时器处理接收到的同步数据
	static void CALLBACK  OnCallbackData(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	void _OnCallbackData();


	//同步
	/************************************************************************/
	//一般发送的数据是有效数据，可以重新解析画图（可以保存到录播 ，本地undoredo）

	void SendPacketData(char *pData, int nLen, bool bToNet = true); //发送同步数据

	//发送数据到白板dll外部，并不交给白板内部undoredo和录播文件 
	void SendPacketInfoOnlyToNet(char* pData,int nLen,bool bToNet = true); //数据不保存到
	//参数 bToNet ：false表示外部调用白板截获该数据  true ：数据发送到网上（服务器）上去实现同步功能
	/************************************************************************/

	void ProcessPacketData(const char *pData, int nLen); //处理接收到得同步数据
	void ProcessObj(XMemIni xmlObj, int nCmdType = CMD_ADD_ATTRIBUTE); //处理接收到得对象
	void ProcessRemoveObj(DWORD dwLayerIndex, unsigned long ulEventID); //处理删除对象
	void ProcessObjLayer(XMemIni xmlObj); //处理对象改变图层
	void ProcessMediaOperation(XMemIni xmlObj);	//处理对媒体的操作
	void GetAllObj(); //向服务器请求获得全部对象
	void GetUserColor();//请求用户颜色
	void SendAllObj(); //向服务器发送所有对象
	void RemoveAllObj(); //向服务器发送删除所有对象
	void RemovePageAllObj(int nDelPageId);//向服务器发送删除页全部消息
	void Broadcast_Data(XMemIni xmlobj); //广播临时数据
	void ProcBroadcast_Data(const char*pData,int nlen); //单独出来广播出来的数据,不进系统的缓冲池
	void OnBroadcast_Data(XMemIni xmlObj); //处理广播临时数据
	void AddObj(XMemIni xmlObj); //添加对象
	void RemoveObj(DWORD dwObjLayerIndex); //删除对象
	void RemoveDelCombineObj(DWORD dwObjLayerIndex);//删除组合对象
	void RemoveMediaObj(DWORD dwObjLayerIndex); //删被动方视频对象
	void RemoveDelComCurve(DWORD dwObjLayerIndex);//删除融合到文档、图片中的曲线
	void ChangeObj(XMemIni xmlObj, bool bchange = false); //修改对象
	void ChangeComObj(CWBObject *pObj); //修改组合对象
	void ChangeObjLayer(XMemIni xmlObj); //修改对象图层
	void MediaOperation(XMemIni xmlObj);	//对媒体的操作（文档，视频，FLASH等）
	//void ProcessGesture(CUIntArray &aLayerBox);	//处理手势修改的对象
	void Undo();//发送撤销命令给服务器来反馈，实现撤销功能
	void Redo();//发送重做命令给服务器来反馈，实现重做功能
	void ClearUndo(); //发送清空撤销队列命令，使撤销无效
	void MoveObj_Realtime_Synchronizatio(XMemIni xmlobj); //处理实时同步，该函数接受对象移动时候的广播出来的偏移量，进行移动本地的对象

	void SendLocalMessage(eCommend_Type eComType,BOOL bFullScreen = TRUE); //设置/退出全屏（本地消息）
	void ChangeFullScreen(BOOL bFullScreen = TRUE); //发送全屏网络消息
	void ProcessAttiObj(XMemIni xmlObj); //处理ID为0的对象消息

	void ProcessBroadcastAttiObj(XMemIni xmlObj); //广播同步消息

	void SendChangePage(); //发送翻页同步
	void SendScrollDraw(); //发送滚动同步
	void SendDeletePage(XMemIni xmlobj);//发送删除页同步
	void SendInsertPage(XMemIni xmlPage); //发送插入页
	void SendExchangePage(XMemIni xmlpage);

	void ProcessChangePage(int nPageIndex);  //处理翻页同步消息
	void ProcessDeletePage(XMemIni xmlObj, int nCmdType = CMD_DEL_PAGE);
	void ProcessInsertPage(XMemIni xmlObj, int nCmdType = CMD_INSERT_PAGE);
	void ProcessExchangPage(XMemIni xmlObj, int nCmdType = CMD_CHANGE_PAGE);


public:
	void processWBDTSData(AVDataInPacket& in); //网络不连通的时候，启动本地的服务器用来撤销重做

	void BroadcastUserCursor(CPoint pt,string str_username); //广播用户名和鼠标的位置
	void RejectBroadcastUserCursor(string str_name);//取消广播用户光标
	void OnRejectBroadcastUserCursor(string str_name); //处理取消广播用户光标
	void ReJectAllBroadcastUserCursor(); //销毁所有的广播光标
	afx_msg LRESULT ProcessMTUserCursor(WPARAM wParam,LPARAM lParam);

	void SetulIcoCurEvent(unsigned long ul) {m_ulIcoCurEvent = ul;}
	void SetdwIcoCurId(DWORD id) {m_dwIcoCurId = id;}
	DWORD GetdwIcoCurId() {return m_dwIcoCurId;}

	CTransparent_Dlg* GetTransparentDlg(){return m_pTransparent_dlg;}
	BOOL GetBroadcastStatus(){return m_bIsneedjionBroadcast;}
	void SetBroadcastUserCursor(bool IsBroad); //设置广播
	void HideUserCursor();
	std::string GetUserAddID() {return m_strUserID;}
	bool GetCameraUNDO() {return m_bIsCameraUNDO;}
	void ShowOrHideMediaCarema(UINT Index);
	void CloseBroadcastUserCursor(); //权限控制关闭水波纹和光标
private:
	bool m_bIsCameraUNDO;//摄像头撤销操作
	std::string m_strUserID;//添加摄像头的人员ID
	CWBDataModel m_WBDataModel;
	bool		  m_bBroadcastUserCursor; //广播用户光标判断
	CTransparent_Dlg*  m_pTransparent_dlg;//光标的对话框
	MapCursorDlgPointer m_MapCursorDlgPointer;
	BOOL         m_bIsneedjionBroadcast;
	BOOL                m_bEraseLineUp;//擦除弹起操作
	unsigned long	 m_ulIcoCurEvent;//图标曲线从文件加载事件
	DWORD m_dwIcoCurId;//图标曲线从文件加载对象id
private:
	XBufferPool m_tXBufferPool; //接收数据缓冲队列
	UINT_PTR	m_nTimerID;//接受网络同步数据缓冲队列定时器启动ID


	/*-----------------------------------------------------------------------------------
	//右键菜单命令处理
	------------------------------------------------------------------------------------*/
public:
	std::vector<string> m_pCaremaPicture;
	//设置当前字体及文本颜色
	void SetTextFontColor(const LOGFONT &lfFont, COLORREF clrText);
	//得到当前字体
	LOGFONT GetCurFont();
	//得到当前字体颜色
	COLORREF GetFontColor();
	//得到当前编辑框
	CWBRichEdit* GetCurEdit();
	void SetCurEdit(CWBRichEdit *pEdit = NULL);
	//设置编辑框内字体样式 luowei
	void SetEditCharFormat(CWBRichEdit *pEdit);

	//得到即时文字设置对话框
	CWBPromptFontSetDlg *GetPromptFontSetDlg();

	CWBOptionDlg *GetWBOptionDlg();
	void DeleteFuncPad();
	CRect GetEditRect(); //得到编辑框矩形
	void SetEditRect(CRect rectText);//设置编辑框矩形
	void SetEditTopLeft(CPoint ptTopLeft); //设置编辑框左上角的点
	BOOL GetIsEditing();
	CWBObject *GetEditObj();
	//设置当前字体
	void SetCurTextFont();
	//设置当前文本颜色
	void SetCurTextClr();
	HBITMAP GetSrcBit(HDC hDC,DWORD BitWidth, DWORD BitHeight);
	//图片、文档位图
	HBITMAP GetPictureBit(HDC hDC, DWORD BitTop, DWORD BitBottom, DWORD BitLeft, DWORD BitRight);

	void AllUnLock(); //解除全部锁定

	CRect GetEditTextRect(); //得到编辑文本框的大小
	BOOL Copy(); //复制剪切时的拷贝
	void CopyObject(); //复制对象
	void CutObject(); //剪切对象
	BOOL PasteObject(); //粘贴对象
	BOOL CloneObject(); //克隆对象
	void InfiniteCloneObj();//无限克隆
	CPoint LeftTop(); //计算 复制或剪切的对象指针列表 里最左最上的点
	BOOL GetIsCopy(); //得到是否复制了
	BOOL GetIsCut(); //得到是否剪切了

	void SetCurAnnotationsLayerIndex(DWORD dwLayerIndex); //设置当前选择的标注对象的图层号
	void SetCurTrapeziumLayerIndex(DWORD dwLayerIndex); //设置当前选择的梯形对象的图层号
	//旋转对象角度，顺时针为负，逆时针为正
	void RotateByAngle(int angle);	
	//水平、垂直旋转，水平为0度，垂直为90度
	void  RotateByHV(int HVAngle);

	void SetCurrentCursorPos();//设置view中鼠标箭头的坐标
	CPoint GetCurrentCursorPos();//得到view中鼠标箭头的坐标

	//图层操作
	void UnderLayer();
	void SuperLayer();
	void TopLayer();
	void BottomLayer();

	//判断是否为文档添加注释
	BOOL AddComponentToObj(CWBObject *pAddObj, unsigned long ulEventID);
	//判断是否为文档添加注释（选择框列表中的对象）
	void AddComListToObj();

	//关于对话框
	void OpenAboutDlg();
	void AddResToWBView(DRAW_STATE nDrawStatus,CString strResPath,RES_INPUT_METHOD nAddResMethod);//view中添加选中的资源
	void UpdateViewBackGround(CWBObject *pObj);//更新view背景
	void SetCloneMedia(BOOL CloneMedia) {m_bIsCloneMedia = CloneMedia;}
	BOOL GetCloneMedia() {return m_bIsCloneMedia;}

	bool AllowDeleteObj(DWORD dwObjId,unsigned long EventID); //是否删除对象是 正在操作的对象
	void BroadCostReleastLimitObjs();//互动方释放限制正在操作的对象，（解决实时同步的时候正在操作的对象是限制的，但有时候互动方一直没有接受到释放的消息，该对象就不能操作)
	void OnReleaseLimitObjs();//处理释放限制消息
	void SetPastePt(CPoint pt);
	void SetColorPaletteDlg(bool ColorPaletteDlg) {m_bIsColorPaletteDlg = ColorPaletteDlg;}
	bool GetColorPaletteDlg() {return m_bIsColorPaletteDlg;}
	
private:
	bool m_bIsColorPaletteDlg;//设置背景时候点击调色板上圈圈显示背景
	BOOL m_bIsCloneMedia;//克隆时候视频被动方位置不偏移
	BOOL m_bIsEditing; //是否在编辑文本
	CWBObject *m_pEditObj; //记录正在编辑的文本的指针

	DWORD m_dwCurObjLayerIndex; //当前对象的图层号
	DWORD m_dwCurSelectObjLayerIndex; //当前选择对象的图层号
	DWORD m_dwCurAnnotationsLayerIndex; //当前选择的标注对象的图层号
	DWORD m_dwCurTrapeziumLayerIndex; //当前选择的梯形对象的图层号
	CRect m_rectSelect; //选择框
	DWORD m_dwCurOperationObjId; //当前操作的对象唯一标识id  huanglongquan add by 2012年12月14日 用于互动方删除全部对象的时候（互动方不删除正常操作的对象） 本地不删除该对象
	//denghong
	CPoint m_moveFirstPoint;
	std::vector<CWBObject *> m_pObjCopyList; //复制或剪切的对象指针列表
	BOOL m_bIsCopy; //是否复制了
	BOOL m_bIsCut; //是否剪切了
	CPoint m_ptPaste; //粘贴时的鼠标位置

	//响应UP消息时，是否是触摸状态，TRUE是触摸状态，FALSE是鼠标状态
	BOOL m_bIsTouch;

	//工具条的相关变量
	//	int m_toolbarPosition; //初始值是-1，同时默认是在左边，如果为1则工具条是在view的右边
	BOOL m_bSDIsTransparent; //屏幕绘图是否玻璃化
	//	BOOL m_bSDIsFullScreen; //屏幕绘图是否全屏
	int m_nSDAlpha; //屏幕绘图透明度

	BOOL m_bIsAnnotations; //是标注还是文本，标注TURE，文本FALSE

	CWBOptionDlg *m_pWBOptionDlg;

	//用于记录编辑框的矩形
	CRect m_rectText;
	CPoint m_topLeft;

	/*-----------------------------------------------------------------------------------
	//工具箱操作
	------------------------------------------------------------------------------------*/
public:
	//添加遮挡板
	BOOL addBaffle();
	//添加功能面板
	BOOL addFunctionPanel();
	//设置TOOLBAR的窗口句柄
	void SetToolbarHWND(HWND hTOOLBARWnd);
	//聚光灯
	void AddSpotlight();
	HWND GetSpotLightHWND();
	BOOL CreateSpotlight(CRect screenRect = NULL);
	void CloseSpotlight();
	void SendSpotlight(); //发送聚光灯同步
	//鹰眼功能
	void EagleEyeFunction();
	BOOL CreateEagleEye();
	void EagleEyeSetposAndShow(CRect rect);
	//获得鹰眼对话框
	CWBEagelEye *GetWBEagleEyedlg();

	//矩形放大镜
	void AddMagnifier();  //放大镜
	void CloseRectMagn();   //关闭放大镜
	void SendMagn();  //发送放大镜同步
	BOOL MouseDownMagn(const CPoint& point);
	BOOL MouseMoveMagn(const CPoint& point);
	BOOL MouseUpMagn(const CPoint& point);	
	//void SetMaqnState(BOOL bMaqnState); //设置放大镜状态
	void AddRECScreen();
	BOOL DrawOnScreen(const CRect *rect = NULL);      //屏幕绘图
	BOOL FullScreenShift(bool isFullScreen);    //全屏化切换
	BOOL DestroyScreenDraw(void);               //销毁屏幕绘图
	void OpenCtrlDlg();
	BOOL ShiftGuise(BOOL isTransparent);  
	void SetSDAlpha(int nAlpha);

	void SendScreenDraw(); //发送屏幕绘图消息
	BOOL SaveSCDraw(char *pBuf, DWORD& dwLen); //保存屏幕绘图信息
	BOOL LoadSCDraw(CFile *pFile, LONGLONG& lOff, ULONGLONG lFileLength); //读取屏幕绘图信息

	void OpenSCDrawReStore(); //白板从最小化恢复时，如果标注窗口是打开的则打开标注窗口
	void SetIsMinSize(BOOL bIsMinSize); //设置白板窗口使是否最小化

	void SetFlashCurPage(BOOL FlashCurPage) {m_bIsFlashCurPage = FlashCurPage;}
	BOOL GetFlashCurPage() const {return m_bIsFlashCurPage;}

	//音视频会议录制，录制视频数据wangxiaobin
	void MediaAudioID(const char *pData,int nLen);
	void MediaConfVideoID(const char *pData,int nLen);

	//	std::map<int,DWORD> m_MapMediaIDPageID;
	void SetIsMedialubo(BOOL bl) {m_bIsMedialubo = bl;}
	BOOL GetIsMedialubo() {return m_bIsMedialubo;}
	//	std::vector<int> m_MediaPageID;
	void SetRECScreenIsShow(BOOL bRECScreenIsShow) {m_RECScreenIsShow = bRECScreenIsShow;}
	BOOL GetEraseLineMoveMsg() {return m_EraseLineMoveMsg;}

	void SetWBfunction(CWBFunctionDlg * fun);
private:
	BOOL  m_RECScreenIsShow;//录制对话框是否弹出来
	BOOL   m_bIsMedialubo;//是否打开录播文件
	DWORD  m_MediaID;
	BOOL m_bIsFlashCurPage;//FLASH是否在当前页，翻页时候用到
	BOOL m_bIsMinSize; //白板窗口是否最小化了
	BOOL m_bIsOpenSCDraw; //白板最小化时，是否同步打开了标注窗口
	BOOL m_bIsSDTransparent; //是否是透明效果

	DWORD m_dwTimeGetTime; //记录上一次鼠标移动时发送同步消息的时间
	BOOL  m_bMoveIsNeedSendData;//判断在MouseMove时是否需要发送同步消息

	//矩形放大镜
	CWBMagnifier*   m_pWBRectMaqn;
	CPoint m_CurrentCursorPos;//view中鼠标箭头的坐标

	//垃圾箱
	CPoint m_ptPreDustbinPoint;
	CPoint m_DustbinPoint;
	CWBDustbin *m_pWBDustbin;
	BOOL m_IsMoveDustbin;
	
	int  m_PreMousePos;//add by xiewenxiu 20111201，标识鼠标上一次位置是否在垃圾桶范围内
	DRAW_STATE    m_nDrawStatus;
	//聚光灯
	CWBSpotlight *m_WBSpotlight;
	BOOL m_bIsCloseScreenDlg;
	BOOL m_bIsTranspartentOrNot; //是否透明效果

	//屏幕录制
	CMedioVideoScreenDlg *m_MedioVideoScreenDlg;
	BOOL m_isFullScreen;
	//eagle eye鹰眼
	CWBEagelEye* m_pEagleEyeDlg;	

	//	CPoint m_MouseMovePrePoint;//mousemove两点距离没变化不进mousemove
	//	BOOL  m_bMouseMovePrePoint;
	BOOL m_EraseLineMoveMsg;//判断是否广播擦除消息

	int m_PrintX,m_PrintY;
	/*-----------------------------------------------------------------------------------
	//工具条上的操作
	------------------------------------------------------------------------------------*/
public:
	//添加图片
	BOOL AddPicture();
	BOOL AddPicture(CString strFilePath,CRect rect = CRect(0,0,0,0));
	BOOL AddDicom();
	BOOL AddDicom(CString strFilePath,CRect rect = CRect(0,0,0,0));
	//添加垃圾箱
	void AddDustbin();
	//添加视频
	BOOL AddMovie();
	BOOL AddMedia();//jiabangping 20110804
	BOOL AddMedia(CString strFilePath);
	BOOL AddCamera();//jiabangping 20110915
	UINT ClearObjectOfOffLineUser(CString sUCCUserID);//清除下线用户的一些对象(摄像头)
	//添加flash
	BOOL AddFlash();
	BOOL AddFlash(CString strFilePath,CRect rect = CRect(0,0,0,0));
	//添加文档
	BOOL AddDoc(CRect rect);
	BOOL AddDoc(CString strFilePath, CRect rect);
	//添加dxf
	BOOL AddDxf(CString strFilePath,CRect rect = CRect(0,0,0,0));
	BOOL AddDxf();
	//文档转换时的窗口
	BOOL DocConvertTip();
	void CreateFullScreen(CWBDocument *pWBDocument);
	void SetFullScreenPos();
	void HideFullScreen();
	void DestroyFullScreen();

	BOOL ScreenShoot(CString filepath);
	//打开资源库
	BOOL OpenResource();

private:
	CWBFullScreen* m_pFullScreen; //文档转换时创建的窗口
	CbuttonCloseDlg* m_pDocTip; //文档转换提示
	bool m_pEstimate;

public:
	friend class CScreenDrawDlg;

	/*-----------------------------------------------------------------------------------
	//白板视图有关操作（包括刷新、位置改变、滚动条改变、系统操作）
	------------------------------------------------------------------------------------*/
public:
	BOOL GetPageIsNeedDraw();
	void SetPageIsNeedDraw(BOOL PageIsNeedDraw);//设置当前的view的page是否需要进入draw()重画函数
	void SetViewIsNeedDraw(BOOL ViewIsNeedDraw);//设置当前的view是否需要让view重画
	//为view设置滚动条
	void setViewScroll();
	UINT getXnPos();
	UINT getYnPos();
	//获得白板视图的rect  2011/4/8
	CRect getWBViewRECT();    
	//设置滚动条的位置
	void setCurrentViewScroll(POINT pt);
	BOOL DeleteDirectory(CString DirName) ; //删除目录

	void SetComObj(CWBObject *pObj = NULL, DWORD dwLayerIndex = 0);
	DWORD GetComLayerIndex();

	UINT GetPrePage();
	void SetPrePage(int nPage);
	void ReductionPrimaryWB(); //还原白板最初的状态
	void HideResBar();//关闭资源目录

	//是否开启DOC转换
	void SetDocConvert(BOOL bIsDocConvert);
	BOOL GetDocConvert();

private:
	CPoint m_ptPointLineCurve;//保存鼠标按下时候直线q曲线的点坐标
	CRect m_rect,m_clientRect;                        // 绘图区域
	CBitmap m_bmpBuf;//view的临时位图
	CDC m_dcMem;
	BOOL m_pageIsNeedDraw;//判断是否需要进PAGE的draw()函数
	BOOL m_viewIsNeedDraw;//判断是否需要让view重画
	//滚动条移动偏移量
	UINT m_XnPos;
	UINT m_YnPos;
	//白板显示的大小与总大小的比例
	float m_fWbRatioX;
	float m_fWbRatioY;
	CRect m_RectWbSize; //白板显示的大小

	//初始滚动条view的大小
	int m_scrollX;
	int m_scrollY;
	XCritSec			m_csOnWbDeleteObjLock;
	XCritSec			m_csOnWbChangeObjLock;
	XCritSec			m_csAddObj;
	XCritSec			m_csConferenceRecord;
	XCritSec m_csDraw;
	BOOL m_bIsDocConvert; //是否开启DOC转换提示
private:
	//	WBLineItem m_wbCurLine;
	//	PWBLineItem m_pSelLine;
	//	BOOL m_bSelectLine;
	CPoint m_ptSelect;
	//	int m_nStatus;
	//	int m_nRoundType;


	CPoint m_ptPrePoint;
	CPoint m_ptMovePoint;  //上一次mousemove点的位置。在avcon6中onmousemove函数鼠标不动也会进 用来判断 
	long m_lmovecounts;

	CPoint m_ptDownPartRefresh;   //局部刷新中用来记录Down下的点
	CPoint m_ptMovePartRefresh;  //局部刷新中用来记录Move的点 
	CRect  m_rectRefresh;
	CRect  m_PreRectRefresh;     //记录上一次刷新区域矩形
	bool  m_bflag;
public:
	void ChangeLeftTop_RightBottomPoint (CPoint &ptLeft, CPoint &ptRight); //调整两个点
	bool GetIsRectRefresh();
	void SetIsRectRefresh( bool bl);

	void SetRefMoreSelectFrameRange( CRect rect );

	/*-----------------------------------------------------------------------------------
	//文件操作
	------------------------------------------------------------------------------------*/

public:
	CDlgPrintLoadingMsg*		m_pDlgPrintLoadingMsg; //加载打开文件时提示的对话框
	bool m_bLocalFileOperate; //判断是否操作文件   一般的话文件操作的话，要做一些处理比如刷新预览等等 
	bool m_IsGetAllObj;//是否在请求所有对象
	bool m_IsInsertPageOver;//是否成功插入页
public:
	//打印操作
	void PrintPage(/*CDC *pDC,BOOL m_IsPrint,int m_ScreenX,int m_ScreenY,CWBPage * pWBPrintPage*/);
	int SaveBitmapToFile(HBITMAP hBitmap, CString lpFileName);
	void PrintBmp(int count);
	void PrintBmp(HBITMAP hbitmap);

	//保存页为图片
	CWBSavePictureDlg *GetWBSavePictureDlg();

	//文件函数
	void DeleteAll();	//全部删除
	BOOL LoadLines(CString strFilePath, bool IsCombin=false);	
	BOOL SaveLines(CString strFilePath);
	void CloseLines();    //删除所有对象，当文件操作是本地操作是，就发送删除所有对象的同步消息
	bool CreateLoadingPictureDlg();
	bool DestoryLoadingPictureDlg();
	void SetPrintLoadingMsgPercent(float fpercent);
	HWND GetDlgPrintLoadingMsgHWND();
	//页操作
	void AddNewPage();
	void PrePage();
	void NextPage();
	void FirstPage();
	void LastPage();
	void GotoPage(UINT  nPage);
	void InsertNewPage();
	void InsertPrePage(UINT nPageIndex);
	void DeletePage();
	void DeletePrePage(UINT nPageIndex);
	void InterchangePage(UINT nFormerPage, UINT nTargetPage);
	void MovePage(UINT nSrcPageIndex, UINT nDesPageIndex); //移动页
private:
	CWBSavePictureDlg *m_pWBSavePictureDlg;//文件保存为图片
	//----------------------------------------------------------------------
	//鼠标操作
	//----------------------------------------------------------------------
public:
	std::vector<DWORD *> m_vecLineLayerIndexBeforeErase;  //保存擦除之前当前页所有直线对象的图层号

	void RecieveMsg(char* buff, int nRead);


	BOOL MoveMsg();
	BOOL MoveMsg(CPoint moveOffset);
	BOOL MoveMsg(CPoint moveOffset,unsigned long ulEvent);

	//BOOL MovInitBroadCastEventID(); //移动有选择框初始化事件Id    by  huanglongquan

	BOOL MovBroadCastMsg();	//广播有选择框移动偏移量       by   huanglongquan

	BOOL EraseLineMsg();

	BOOL EraseCurveMsg();

	BOOL EraseDocMsg();

	BOOL DeleteObjMsg();
	void	setFrameListAndObjLayerIndex();

	void SetIsLocalWhiteboard(bool isLocalWhiteboard);//设置白板中的本地白板变量
	bool GetIsLocalWhiteboard() {return g_isLoacalWB;}

	void SetTouchDustbinDelete(bool bflag) {m_bIsTouchUp = bflag;}
	bool m_bIsCombin;//组合打开文件
private:
	bool m_bIsTouchUp;//触摸垃圾箱删除对象弹起
	bool m_isLocalWhiteboard;//用来判断是否为单机调用电子白板，来实现本地白板的redo和 undo
	int m_XOffSetPicture;//avcon拍照图片位置
	int m_YOffSetPicture;
	unsigned long m_ulComEventID; //组合同步的事件ID
	int m_nComPageIndex; //组合同步的页号
	unsigned long m_ulComSenderID; //组合同步的客户端ID
	CWBObject *m_pComObj; //同步时的组合里的对象
	DWORD m_dwComLayerIndex; //同步时组合里的对象的ID
	int m_nComCount; //组合里的对象数，用于同步刷新
	//	bool m_bChangeObjectList;//判断改变对象列表的时候，不让刷新，当在改变或删除多个对象时只在最后一个对象后才让刷新
	//	bool m_bDrawView;

	UINT m_nPrePage; //改变页时记录之前的页号

	unsigned long m_ulMoveCounts;//移动对象时总得进入个数


	//橡皮擦矩形
	CRect m_rectBrush; //擦除框
	DWORD m_dwCurBrushTouchID; //当前擦除时的触摸屏ID
	CPoint m_ptErasePre; //擦除时前一个擦除框的中心店

	//区域删除曲线
	CWBCurve *m_pRgnDeleteWBCurve;
	std::map<int, CWBCurve*> m_pRgnDeleteWBCurveMap;

	//菜单操作对象的图层号
	DWORD m_dwMenuLayerIndex;

	//局部刷新使用的点
	//	CPoint m_ptValidatePre;


	//延展按钮
	//	CButton * m_viewButton;

	//工具条的窗口句柄
	HWND m_hTOOLBARWnd;

	CDrawCtl *m_ScreenDrawCtrl;

	unsigned long m_ulRecEventID; //接收同步事件ID
	unsigned long m_ulSendEventID; //发送事件ID，删除、擦除对象时使用
	unsigned long m_ulRecOldEventID; //上一次事件ID
	BOOL  m_bRefurbishPreview; //是否刷新预览框
	bool m_EraseRectDowm;//大小擦除按下时候

public:
	static int m_nViewCount; //视图个数
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg LRESULT OnWbDeleteobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbMaqnobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbSpotlightobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbCreateRichEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbEditRichText(WPARAM wParam, LPARAM lParam);//再次编辑文本
	afx_msg LRESULT OnWbDestroyRichText(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

protected:
	afx_msg LRESULT OnProcessScreenDraw(WPARAM wParam, LPARAM lParam); //处理屏幕绘图消息
	afx_msg LRESULT OnWbProcessobj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessAddFlash(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessAddMedia(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessAddCamera(WPARAM wParam, LPARAM lParam);
	//发送摄像头重新连接的同步消息，要求
	afx_msg LRESULT OnSendCameraConnectSynchronous(WPARAM wPar, LPARAM lPar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//处理放大镜点击关闭按钮关闭wangxiaobin
	afx_msg LRESULT OnProcessRectMagnClose(WPARAM wParam, LPARAM lParam);

public:

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);



	//touch
private:


	//触摸绘制参数结构
	typedef struct touchDrawingParameter
	{
	private:
		DWORD dwID;//此数据可以不用
		int nDrawType;
		CWBObject *pObject;
		//绘制多边形有用。
		DWORD dwTime;
		CPoint ptValid;
		CPoint pt;
		void *pData;
	public:
		touchDrawingParameter(DWORD _dwID, int _nDrawType, CWBObject*_pObject, CPoint _ptValid=CPoint(0,0), DWORD _dwTime=0, CPoint _pt=CPoint(0,0))
			: dwID(_dwID), pObject(_pObject), dwTime(_dwTime), ptValid(_ptValid), nDrawType(_nDrawType), pt(_pt){}
		void UpdateValidPt(CPoint _ptValid){ptValid=_ptValid;};
		void UpdatePt(CPoint _pt){pt=_pt;};
		void UpdateTime(DWORD _dwtime){dwTime=_dwtime;};
		void SetAppendData(void *_pData){pData=_pData;};
		int GetType(){return nDrawType;};
		CWBObject *GetWBObject(){return pObject;};
		DWORD GetTime(){return dwTime;};
		CPoint GetPt(){return pt;};
		CPoint GetValidPt(){return ptValid;};
		void *GetAppendData(){return pData;};
	}MTDrawPar, *PMTDrawPar;
	std::map<DWORD, MTDrawPar*> m_mapMTDrawPar;
	//最后一次触摸操作拖入的资源名称
	CString m_sLastDragFileURL;

	//获取触摸绘制映射中的指定绘制类型的对象个数（2012-3-16 jiabangping）
	unsigned int GetMapSizeOfSpecialDrawTypeObj(int nDrawType);

public:
	void ClearAllMTDrawMap();
	void ClearMTDrawMap(const DWORD &dwTouchID);

	unsigned long GetEventID() { return m_ulRecEventID;}

public:
	//新视频控制对话框相关
	//CWBGlobalMediaCtrl* GetMediaCtrl();
	//void SetMediaCtrl(CWBGlobalMediaCtrl* pMediaCtrl) { m_pMediaCtrl = pMediaCtrl ;}

	void SetEraseCurveLine(BOOL bErase) {m_EraseCurveLine = bErase;}
	BOOL GetEraseCurveLine() {return m_EraseCurveLine;}

private:
	BOOL m_EraseCurveLine;
	CWBGlobalMediaCtrl* m_pMediaCtrl;
	//	CDlgCamera* m_pCameraCtrl;
	DWORD m_MediaIDIsActive;//主动方

	bool m_bIsRButtonDown;//屏幕录制区域录制右键弹起

public:
	void SetMediaVideoRButtonDown(bool bl) {m_bIsRButtonDown = bl;}
	//存储所插入的新视频的指针
	std::vector<CWBMediaWindow*> m_MediaList;
	void RemoveAllMediaListDate();
	void DeleteMediaListDate( CWBObject* pObj );

	void DeleteIntnetMediaToSin();//删除单机下联机到单机被动方的视频
	std::map<DWORD,CString> m_MediaStringPath;//保存主动添加视频文件路径
public:
	//一个电子白板客户端仅只能有一个白板摄像头对象
	CWBCamera *m_pUniqueWBCamera;
	CWBPage *m_pPage;

protected:
	afx_msg LRESULT OnProcessFileOperate(WPARAM wParam, LPARAM lParam);
	//	afx_msg LRESULT OnWbDelAllAttribute(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNoticePraviewRefresh(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDelallobject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPraviewRefresh(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnClosewbfunctiondlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWbDelPageAllObj(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreatePrintLoadingMsg(WPARAM wParam, LPARAM lParam);	

public:
	//去除响应鼠标中间滚轮消息
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	void SendDataToConferenceRecord(XMemIni xmlobj ,int iCmdType = CMD_ADD_ATTRIBUTE ,bool bkeyFrame = true);
	bool SendFileToConferenceRecord(string str_path);
	bool	SendLocalDataToConferenceRecord(char* pData,int nLen,bool bKeyFrame = true);

	void SenMediadDataToConferenceRecord(unsigned char*pData,int nLen,int nKeyFrame,DWORD dwLayer = -1 );//主动放的视频回调
	void SenAudioDataToConferenceRecord(unsigned char*pData,int nLen,bool&bLoop ,DWORD dwLayer = -1);//主动放的音频回调
	void SenPlayMediadDataToConferenceRecord(unsigned char*pData,int nLen,int nKeyFrame,DWORD dwLayer = -1 );//被动放的视频回调
	void SenPlayAudioDataToConferenceRecord(unsigned char*pData,int nLen,bool&bLoop,DWORD dwLayer = -1 );//被动放的音频回调

private:
	long  m_nMoveObj_Realtime_Synchronizatio_Counts; //接受到移动对象时候控制刷新
public:
	void SetMoveIsNeedSendData(BOOL MoveIsNeedSendData);   //设置在move的时候需要刷新
	BOOL GetMoveIsNeedSendData();                          //获取在move的时候需要刷新的判断

	void SendMoveSelectedObjOffset(CPoint cpoffset,unsigned long ulEventId); //发送偏移量同步
protected:
	afx_msg LRESULT OnProcess_CurorTranDlg(WPARAM wParam, LPARAM lParam);

public:
	void CWhiteBoardView::BroadcastUserWater( CPoint pt,string str_username );

	CRect GetCamaraMediaRect() {return m_rcCamaraMediaRect;}
	//void  SetCamaraMediaRect(CRect rc) {m_rcCamaraMediaRect = rc;}
	bool GetCamaraMediaU7() {return m_bCamaraMedia;}
	DWORD GetCamaraMediaLayerID() {return m_CamaraMediaID;}
private:
	//unsigned long m_ulUserCompure;
	DWORD m_StartGetLayerId;
	DWORD m_CamaraMediaID;
	bool m_bCamaraMedia;//广播接收方
	CRect m_rcCamaraMediaRect;
	CRect m_rcCamaraMediaRectOpen;
	CWaterDlg *m_waterdlg;
	MapWaterDlgPointer m_MapWaterDlgPointer;
	std::string m_ReceiveWaterUserID;
	CPoint m_ReceiveWaterPoint;
public:
	void RejectBroadcastUserWater(string str_name);
	void OnRejectBroadcasstUserWater(string str_name);
	void ReJectAllBroadcastUserWater();
	void DeleteWaterCursorForAvcon6();
protected:
	afx_msg LRESULT OnSendPageDatatoConferenceRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcess_WaterDlg(WPARAM wParam, LPARAM lParam); //处理水波纹广播消息
	afx_msg LRESULT OnCreateWaterDlg(WPARAM wParam,LPARAM lParam);

public:
	CWBProtractor *m_WBProtractor;
	//BOOL MouseMoveProtarctor(const CPoint& point);
	//BOOL MouseDownProtarctor(const CPoint& point);
	//BOOL MouseUpProtarctor(const CPoint& point);
	void AddProtractor();

private:

	//private:
	//	CPoint m_nProtractorDownpt;
	//	CPoint m__nProtractorUppt;
public:
	CWBAngle *m_WBAngle;
	BOOL MouseMoveAngle(const CPoint& point);
	BOOL MouseDownAngle(const CPoint& point);
	BOOL MouseUpAngle(const CPoint& point);
	void AddAngle();


public:
	CWBRuler *m_WBRuler;
	//BOOL m_bRulerIsneedDraw;
	void CreateAnnRule(CPoint &pt,int number);
	void AddRuler();

	CWBSetSquare * m_WBSetSqaure; //定义一个三角板类对象
	BOOL AddSetSquare();
	//添加圆规
	CWBCompass * m_WBCompass; //定义一个圆规对象
	BOOL AddCompass();
	int GetCompassDrawShape();
	void SetCompassDrawShape(int num);
	//void CloseSetSquare();
	//判断圆规菜单是勾选扇形还是圆弧
	BOOL GetCompassOptMenu(){return m_bCompassOptMenu;}
	void SetCompassOptMenu(BOOL bl){m_bCompassOptMenu = bl;}
private:
	BOOL m_bCompassOptMenu;//圆规菜单勾选
	int m_nCompassDrawShape; //圆规画什么图形选择
	CButton m_btn;
	bool m_bIsCreated;

private:
	//GDI+
	ULONG_PTR	m_gdiplusToken;
		double m_dReduceRate;//缩放
	DWORD m_dwCheckItem;//记录缩放了多少
private:
	XMemIni m_xmlBroadCastAttiObj;
protected:
	afx_msg LRESULT OnProcessBroadcastObj(WPARAM wParam, LPARAM lParam);
public:
	void StartProReliefTimer(); //启动定时器
	void KillProReliefTimer();//关闭定时器
	static void CALLBACK  OnProRelief(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime); //定时器回调函数，每次判断是否有限制对象没有释放 huanglongquan
	void _OnProRelief();		//判断是否有限制对象没有释放 huanglongquan add
	void AddLimitObjId(unsigned long ulObjId );
	void AddLimitObjId(void* pWBObj);
	void RemoveLimitObjId(unsigned long ulObjId );
	void RemoveLimitObjId(void* pWBObj);
	void SetCheckItem(DWORD dItem);
	DWORD GetCheckItem();
private:
	//用于释放操作对象功能类实例
	CLimitLocalObjOperate  m_LimitLocalObj;
	UINT_PTR m_nTimerLimitObjPro;
public:
	void ChangViewSize(CRect rect,double dReduceRate);//改变view的大小
	void ChangeViewCoordinate(CDC *pDC);//改变view的坐标系
	void ChangeSize(CRect rect,double dReduceRate  );
afx_msg void OnReduce(WPARAM wParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
protected:
	afx_msg LRESULT OnPopmenu(WPARAM wParam, LPARAM lParam);
private:
	CString m_strPrompt;
	bool m_bCamaraMediaClk;

private:
	MapDropCamInfo m_MapDropCamInfo;
	CMyDropTarget m_myDropTarget;
	CPoint m_nLDBKPoint;
	BOOL m_bIsdrop;
	string m_strChannelID;
public:
	afx_msg	LRESULT OnDropNotify(WPARAM wParam,LPARAM lParam);
	void ProcessCaminfo(WPARAM wParam,CRect rc);  //U7拖拽
	void ReJectAllProcessCamInfo();   
	void DeleteCaminfo(DWORD nWBObjID);    //删除MAP
	BOOL GetIsdrop(){return m_bIsdrop;}
	void SetIsdrop(BOOL bIsTrue){m_bIsdrop = bIsTrue;}  //判断是否是拖拽生成的图片
	BOOL bIsInCamInfoMap(CWBObject* nDWBIndex); //查找MAP屏蔽按钮
	bool ProcessGetCamInfo(DWORD nObjID,CString str,string strname,unsigned long channelID); //被动接收拖拽信息图片

	//点名处理
public:
	void SendRollCallBroadCast(int UserStyle); //发送的广播（参数为类型）
	void ProcessRollCallBroadCast();           //被动方处理点名
	void ProcessRollCallLRSultBroadCast(string str);    //回收反馈信息

private:
	unsigned long m_nSendRollCallBroadCastID; //广播发起者
	CWBRollCallDialog *m_wbrcprodlg;
	CWBRollCallShowDlg *m_wbrcshowdlg;
	int m_nRollCallTimer;
	std::map<std::string ,DWORD>*m_pUserNameID;//拖拽用户名ID
	
public:
	std::vector<string> m_RollCallNameList;

public:
	void SendCurvePicFromFile(const char* strPath);
    std::map<DWORD, std::string> m_pRecvFileWBIcoCurveMap;
	BOOL m_bFirstLoginConference; //判断是否第一个进会的用户

private:
	CRect m_RcDrawText;
	bool  m_bUseToControlDraw;
	bool  m_bLimitCursorArea;
	int   m_ntouchID;
private:
	int m_nTriangleType;
	
public:
	void SetDrawTriangleType(int ntype){m_nTriangleType = ntype;}
	int GetDrawTriangleType(){return m_nTriangleType;}
	void ProcessCloseColorDlg();

public:

	void SetIsKeyDel(BOOL bIsTrue){m_ptIsKeyDel = bIsTrue;}
	BOOL GetIsKeyDel(){return m_ptIsKeyDel;}


	void SetIsRationButton(BOOL bIsTrue){m_ptIsRationButton = bIsTrue;}
	BOOL GetIsRationButton(){return m_ptIsRationButton;}
	void ExitRoomDeleteStreamObj();//退出会议时删除被动接收的流对象
	int GetDocCurPage(){return m_nDocCurPageIndex;}
	void SetDocCurPage(int Index){m_nDocCurPageIndex = Index;}//获得文档的当前页号
private:
	BOOL m_ptIsKeyDel;
	BOOL m_ptIsTouchCom;
	BOOL m_ptIsRationButton;
	int m_nDocCurPageIndex;
	BOOL m_bIsAllowDropCamera;   //U7下控制是否允许拖拽摄像头到白板
	CString m_strLocalNotDropCameraToWB; //本地不支持拖拽摄像头到白板字符串
	
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);//取消滚动条的右键弹出按钮
public:
	void ProcessMTMoveRationalCenter(CPoint ptOffset); //处理触摸时移动旋转中心点
	CFile m_cFile;
	CWBFile *m_pLoadWBFile;
private:
	bool m_bStartRecognition;//开启识别功能
	bool m_bRecogniMouseDown;
	bool m_bRecogniTouchDown;
	PointPath path2D;
	std::vector<CPoint> m_RecShowpoint;
	GeometricRecognizer *m_pGeoRecognizer;

public:
	void ChangeUserCurorTranDlgPosition(CPoint ptoffset); //处理U7中拖拽白板窗口后改变光标的位置

public:
	void SetNeedSetPageIndexForScreenDraw(bool bneed){m_bNeedSetPageIndexForScreenDraw = bneed;}//在有标注的时候为文件退出操作将页号标记为1（因为服务器的回发问题）
private:
	bool m_bNeedSetPageIndexForScreenDraw;
public:
	bool ProcessMediaPhotoEffect(CPoint ptRes,bool bRight);
	void SetWBPictureEffect(CWBPicture *pWBObj);
	void DeleteSpePicInMap(DWORD dwlayer); //白板内删除检查是否有记录
	bool DeleteSpePicFromResource(CString strPath);//资源库删除检查是否有记录，有就删除
private:
	CWBPicture* m_pWBPicture;	
	std::map<CString ,VECTORSPECPIC>*m_pResourcePic;//拖拽用户名ID
	std::vector<CWBPicture*> m_pDropPicObj;//资源库拖出的图片
	bool m_bIsDBClick; //双击了
	DWORD m_dDBClickobjlayer;//双击的对象
	CString m_strResourceUpdataPath;//更新资源库中图片的路径
	CString m_strDBClickSavePath;//双击保存编辑图片的路径
	int m_bSelectStatueChange;//发送选中对象状态变换
public:
	afx_msg HRESULT ProcessResourceMessageBox(WPARAM wParam,LPARAM lParam);//处理资源库删除文件错误信息
	void CheckDeleteObjInVector(CWBPicture* pWBPic); //检查删除的图片对象是否存在vector中。
	void CheckVectorBeforCloseWB();//白板退出时检查是否有需要保存的正在编辑的图片
	void AddPicToVector(CWBPicture* pWbPic);
	bool CheckDiskPlaceForPic();//检查路径磁盘大小




	void AddMidi(CString strPath);
	DWORD GetdwMenuLayerIndex();
	void SetdwMenuLayerIndex(DWORD dwMenuLayerIndex);
	int GetObjCopyListCount();
	bool SingleObjIsLock();//判断单个对象是否为锁定状态
	int GetWBObjCount();//获取对象个数
	void RefreshWB();	//刷新白板
	void PostMessageToWhiteBoard(UINT msg, WPARAM wparam, LPARAM lparam);//发异步消息给白板
};

typedef int (*pDicomToBmpDll)(CString,CString);//定义与动态链接库中的函数原型相同的原型
typedef BOOL (*GetBmpPath)(char *pszPath, int nLen);//定义与动态链接库中的函数原型相同的原型
