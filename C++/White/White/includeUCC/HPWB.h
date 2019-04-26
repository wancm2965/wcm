#ifndef _HPWB_H
#define _HPWB_H


typedef struct {	
	unsigned long ulObjId;	
	unsigned char byPageId;	
	unsigned char byTabId;
	unsigned char byType;
}WB_COMMAND_HEAD,*LPWB_COMMAND_HEAD, *PWB_COMMAND_HEAD;

enum TAB_CTRL_FLAG
{
	TAB_CTRL_WHITEBOARD = 0,	
	TAB_CTRL_SHAREVIDEO
};

typedef struct {
	RECT			rtToolBar;
	POINT			ptWhiteBoard;	
	DWORD			dwWhiteBoardPage;	
	DWORD			dwWhiteBoardTotalPage;	
	UCHAR			byTabId;
	BOOL			bFullScreen;
}CLIENT_WINDOW_POSITION;

#define WB_TOOL_BG_COLOR RGB(236,233,216)
//#define WB_TOOL_BG_COLOR RGB(183,183,183)
#define WB_TOOL_SELECT_BG_COLOR RGB(254,254,254)
#define DATA_TRANS_ADDOBJECT			1
#define DATA_TRANS_LOADFILE				2
#define DATA_TRANS_MOVEOBJ				3
#define DATA_TRANS_OBJ_RESIZE			4
#define DATA_TRANS_DELETE_ALL			5
#define DATA_TRANS_DELETE_OBJECT		6
#define DATA_TRANS_SCROLL				7
#define DATA_TRANS_COLORREF_CHANGED		8
#define DATA_TRANS_LINE_WIDTH_CHANGED	9
#define DATA_TRANS_FONT_CHANGED			10
#define DATA_TRANS_TEXT_CHANGED			11
#define DATA_TRANS_POINTER_CMD			12
#define DATA_TRANS_LOCK					13
#define DATA_TRANS_PAGE					14
#define DATA_TRANS_SET_BACKBITMAP		15
#define DATA_TRANS_SET_BACKCOLOR		16
#define DATA_TRANS_CLEAR_ALL			17
#define DATA_TRANS_OBJREQUEST			18
#define DATA_TRANS_OBJRESPONSE			19
#define DATA_TRANS_OBJECT_ALL			20
#define DATA_TRANS_DOC_PAGE_CHANGED		21
#define DATA_TRANS_DOC_CLOSE			22
#define DATA_TRANS_DOC_LOADFILE			23
#define DATA_TRANS_TABCTRL_CHANGE		24
#define DATA_TRANS_FULLSCREEN			25
#define DATA_TRANS_TOOLBARMOVE			26
#define DATA_TRANS_WINDOWMOVE			27
#define DATA_TRANS_LASTPOSITION			28		// 取得当前活动位置 data格式  posx + posy (len=sizeof(int) * 2)


#define DATA_TRANS_REQUESTTOKEN				29      //查询令牌状态  //ADD BY WZF 2006.04.21
#define DATA_TRANS_RESPONSETOKENSTATE		30      //反回令牌状态   //ADD BY WZF 2006.04.21
#define DATA_TRANS_FILEPRINTFINISH			31		//打印完成消息   //ADD BY WZF 2006.04.21
#define DATA_TRANS_FILEPRINTCANCEL			32		//文件打印退出

#define DATA_TRANS_REQUESTLASTPOSITION			33		// 取得当前活动位置 data格式  posx + posy (len=sizeof(int) * 2)
#define DATA_TRANS_RESPONSELASTPOSITION			34		// 取得当前活动位置 data格式  posx + posy (len=sizeof(int) * 2)

#define DATA_TRANS_SHAREFILE_END		37 //add by wzf on 2006.05.16
#define DATA_TRANS_REQUESTSHAREFILE		38
#define DATA_TRANS_RESPONSESHAREFILE	39

#define DATA_TRANS_DTOBJ_HEAD		40 //add by wzf on 2006.05.25
#define TRANS_REQUESTDTOBJDATA			41 //add by wzf on 2006.05.26
#define TRANS_RESPONSEDTOBJDATA			42 //add by wzf on 2006.05.26
#define TRANS_CLEAR_DTOBJDATA			44 //add by wzf on 2006.05.26
#define STOP_TRANS_SHARE_FILE			45

#define DATA_TRANS_ROATE				  46
#define DATA_TRANS_DELETE_OBJATTRIBUTE    47
#define DATA_TRANS_DELETE_ALLOBJATTRIBUTE 48
#define DATA_TRANS_PUT_OBJATTRIBUTE       49
#define DATA_TRANS_GET_OBJATTRIBUTE       50
#define DATA_TRANS_GET_ALLOBJATTRIBUTE    51
//命令
#define DATA_TRANS_PREMOVE_LAYOUT         52
#define DATA_TRANS_BACKMOVE_LAYOUT        53
#define DATA_TRANS_MOVETOHEAD_LAYOUT      54
#define DATA_TRANS_MOVETOTAIL_LAYOUT	  55
#define DATA_TRANS_BK_LAYOUT			  56
#define DATA_TRANS_REQUESTBKCOLOR         57
class IHPWhiteBoardNotify
{
public:
	/*本地白板数据网络发送接口。参数：a）白板内部自定义数据结构 b）数据长度*/
	virtual bool onWBDataOut(char* pData,long &nLen)= 0;
	virtual void onAllWBData(char* pData,long &nLen)= 0;

	//内部白板全屏后通知接口。 参数为改变后的屏幕显示状态
	virtual void onWBFullScreen(bool bFull) = 0;

	virtual void MyRemoveWindowSkin(HWND hWnd)=0;
	virtual void MyEnableWindowScrollbarSkin(HWND hWnd)=0;
	virtual void UpdateMainWindow()=0;//更新主程序窗口,主要用于白板抓屏
	virtual bool IsInMeeting()=0;//是否在会议中
	virtual bool IsMyAuditor()=0;  //旁听者
	virtual void ShowMainWindow(int nShowCmd)=0; //白板抓屏时主窗口的处理
	virtual void WBNotify_OnLButtonDown(UINT nFlags, CPoint point){};  //高级会议拖动白板用	

};

class IHPWhiteBoard
{
public:
	/*初始化白板。参数：a）父窗口句柄 b）外部接口*/
	virtual bool Connect(CWnd *pParent)=0;

	/*注销白板。*/
	virtual void releaseConnection()=0;

	/*处理白板内部自定义数据结构。参数：a）白板数据包 b）数据包长度*/
	virtual bool processWBData(char * pData,long nLen)=0;

	/*设置白板窗口位置*/
	virtual void setWBPosition(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)=0;

	/*设置工具栏窗口位置*/
	virtual void setTOOLPosition(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)=0;

	/*设置白板窗口可见属性*/
	virtual void setShowWindow(bool bShow)=0;

	//virtual void On_DataOut(char *pData, long nLen)=0;

	/*通过PageId, ObjId查询数据*/
	virtual void processWBObjRequest(unsigned char byPageId, unsigned long ulObjId)=0;

	//通知白板全屏
	virtual void processFullScreen(bool bFull) = 0;

	//是否为白板的发起者
	virtual void SetWBHost(bool bWBHost)=0;// add by wzf 2006.04.20

	virtual void SetJoinerNum(int nNum) =0;

	virtual void SetLocalUserID(CString strUserID)=0;

	virtual void On_DataOut(char *pData, long nLen) = 0;

	//获取所有白板数据
	virtual void RequestAllWBData()=0;
	/*设置白板窗口可见属性 //人寿专用*/
	virtual void setShowWBToolDlg(bool bShow)=0;

	/*Enables or Disables all whiteboard dialog*/
	virtual void EnableAllWBDlg(BOOL bEnable)=0;

	/*清楚所有白板数据*/
	virtual void ClearLocalData(void)=0;

	//added by luoning 2010-8-5
	virtual BOOL OpenPic(CString strPath) = 0;
	//end

	// 隐藏白板图片工具栏 [5/24/2011 %x.t%]
	virtual void HidePicBar() = 0;
};

AFX_EXT_CLASS IHPWhiteBoard * CreateWB(IHPWhiteBoardNotify * pNotify,CString strLanguagePath);

#endif