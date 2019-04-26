//========================================================================
//类名：模板
//功能：适应各种需要的模板
//作者：戴磊
//时间：2008-12-8
//说明：
//--------------------------------------------------------------------------

#pragma once
/*#include "HPDLG_Base.h"*/

typedef enum
{
	TM_TYPE_NONE	= -1,	//无类型
	TM_TYPE_4X3		= 0,	//4：3普通屏
	TM_TYPE_16X9			//16：9宽屏
}HPTM_TM_TYPE;

typedef enum
{
	TM_SUBTYPE_0_NONE		= -1,
	TM_SUBTYPE_1_1X1		= 0,
	TM_SUBTYPE_2_1X2_SPACE,
	TM_SUBTYPE_2_1X2_FULL,
	TM_SUBTYPE_3_L1_R2,
	TM_SUBTYPE_4_2X2,
	TM_SUBTYPE_4_L1_R3,
	TM_SUBTYPE_6_LT1_RD5,
	TM_SUBTYPE_8_LT1_RD7,
	TM_SUBTYPE_9_3X3,
	TM_SUBTYPE_10_T2_B2X4,
	TM_SUBTYPE_13_IN1_RD12,
	TM_SUBTYPE_16_4X4,
	TM_SUBTYPE_2_PIC_IN_PIC,

	TM_SUBTYPE_4_B1_T3,
	TM_SUBTYPE_5_L1_R4,
	TM_SUBTYPE_5_B1_T4,
	TM_SUBTYPE_6_T2_B4,
	TM_SUBTYPE_7_L2X2_R3,
	TM_SUBTYPE_10_LT1_RD9,
	TM_SUBTYPE_12_3X4,
	TM_SUBTYPE_24_LT1_RD5_X4,
	TM_SUBTYPE_25_5X5,
	TM_SUBTYPE_36_6X6,

	TM_SUBTYPE_3_T1_B2		= 50,
	TM_SUBTYPE_7_L1_R6,
	TVWALL_TYPE_TEST		= 100,
}HPTM_TM_SUBTYPE;

#define HPTM_MAX_WIN_COUNT	36
#define WM_CHILE_ADDED	(WM_USER + 4321)

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
	int						Type;	 			//0：监控树，1：地图，2：音视频播放窗口 3:会议人员树
	HWND				hWnd;    		//拖动源的句柄，比如树上的节点HTREEITEM 或者地图的句柄或者AV窗体的句柄
	int						AreaID;	 		//（区域ID） 根据情况填充(0:区域，1：设备，2：通道)
	char					MemberID[255];		//根据情况填充
	unsigned long	ChannelID;				//根据情况填充
}HPMC_DROP;


#define HPTM_EXPORT
#ifdef HPTM_EXPORT
#define HPTM_API _declspec(dllexport)
#elif HPTM_DLL
#define HPTM_API _declspec(dllimport)
#else
#define HPTM_API
#endif


class HPTM_API HPTM_TempletNotify
{
public:
	HPTM_TempletNotify(){};
	~HPTM_TempletNotify(){};
public:
	/*	函数功能：拖放事件
		参    数：	wParam：		拖放参数
					lParam：		拖放参数
					iTMIndex：		目标模板的序号
					iParent：		目标模板上父窗体的序号
					ulSessionID：	标记
		返 回 值：*/
	virtual void HPTM_TempletDropNotify(WPARAM wParam, LPARAM lParam, int iTMIndex, int iParent, unsigned long ulSessionID) = 0;

	/*	函数功能：选择模板事件
		参    数：	iTMIndex：		该模板的序号
					iTMSubType：	该模板的模板编号
					ulSessionID：	标记
		返 回 值：*/
	virtual void HPTM_TempletSelectedNotify(int iTMIndex, int iTMSubType, unsigned long ulSessionID) = 0;

	/*	函数功能：交换或移动子窗口事件
		参    数：	iSrcTM：		源模板的序号
					iSrcParent：	源父窗口的序号
					iTagTM：		目标模板的序号
					iTagParent：	目标父窗口的序号
					ulSessionID：	标记
		返 回 值：*/
	virtual void HPTM_TempletSwapNotify(int iSrcTM, int iSrcParent, int iTagTM, int iTagParent, unsigned long ulSessionID) = 0;

	/*	函数功能：单击事件
		参    数：	iTMIndex：		模板的序号
					iParentIndex：	父窗口的序号
					ulSessionID：	标记
		返 回 值：*/
	virtual void HPTM_TempletLButtonDownNotify(int iTMIndex, int iParentIndex, unsigned long ulSessionID) = 0;

	/*	函数功能：双击事件
		参    数：	iTMIndex：		模板的序号
					iParentIndex：	父窗口的序号
					iResult：		双击后的状态（0：原始状态，1：最大化到模板，2：最大化到全屏）
					ulSessionID：	标记
		返 回 值：*/
	virtual void HPTM_TempletldbClickNotify(int iTMIndex, int iParentIndex, int iResult, unsigned long ulSessionID) = 0;

	/*	函数功能：模板类型或子类型改变
		参    数：	iTMIndex：		模板的序号
					type：			模板的类型
					subtype：		模板的子类型
					ulSessionID：	标记
		返 回 值：*/
	virtual void HPTM_TempletlTypeChangedNotify(int iTMIndex, HPTM_TM_TYPE type, HPTM_TM_SUBTYPE subtype, unsigned long ulSessionID, BOOL bAppAll=FALSE) = 0;
	virtual void HPTM_TempletLButtonUpNotify(int iTMIndex, int iParentIndex, unsigned long ulSessionID) = 0;
	virtual void HPTM_TempletRButtonDowmNotify(int iTMIndex, int iParentIndex, unsigned long ulSessionID) = 0; 
	virtual void HPTM_TempletAddWindowNotify(int iTMIndwx, int IParentIndex) = 0;
	virtual void HPTM_TempletDelWindowNotify(HWND hWnd, int nTMIndex, int nParentIndex) = 0;
	virtual void HPTM_TempletOnMouseHover(int iTMIndex, int iParentIndex, unsigned long ulSessionID){};
	virtual void HPTM_TempletOnMouseLeave(int iTMIndex, int iParentIndex, unsigned long ulSessionID){};
	virtual void HPTM_TempletOnEsc(){};
	/*	函数功能：模板窗口开始选择区域事件
	参    数：	iTMIndex：		模板的序号
	iParentIndex：	父窗口的序号
	point:	鼠标当前位置(客户区坐标)
	ulSessionID：	标记
	返 回 值：*/
	virtual void HPTM_TempletBeginSelectNotify(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
	/*	函数功能：模板窗口选择区域事件
	参    数：	iTMIndex：		模板的序号
	iParentIndex：	父窗口的序号
	point:	鼠标当前位置(客户区坐标)
	ulSessionID：	标记
	返 回 值：*/
	virtual void HPTM_TempletOnSelectNotify(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
	/*	函数功能：模板窗口结束选择区域事件
	参    数：	iTMIndex：		模板的序号
	iParentIndex：	父窗口的序号
	point:	鼠标当前位置(客户区坐标)
	ulSessionID：	标记
	返 回 值：*/
	virtual void HPTM_TempletEndSelectNotify(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
	/*	函数功能：模板窗口选择区域定时事件(每隔100ms)
	参    数：	iTMIndex：		模板的序号
	iParentIndex：	父窗口的序号
	point:	鼠标当前位置(客户区坐标)
	ulSessionID：	标记
	返 回 值：*/
	virtual void HPTM_TempletSelectTimerNotify(int iTMIndex, int iParentIndex,unsigned long ulSessionID){};
	virtual void HPTM_TempletOnMouseMove(int iTMIndex, int iParentIndex, POINT point,unsigned long ulSessionID){};
};

class HPTM_API HPTM_Templet
{
public:
	HPTM_Templet(void){};
	virtual~ HPTM_Templet(void){};

public:
	//窗体操作
	virtual HWND GetSafeHwnd() const = 0;
	virtual BOOL ShowWindow(int nCmdShow) = 0;
	virtual BOOL SetWindowPos(const HWND pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;
	virtual void GetWindowRect(LPRECT lpRect) const = 0;
	virtual BOOL IsWindowVisible(void) = 0;
	virtual HWND SetParent(HWND pWndNewParent) = 0;
	virtual HWND GetParent() = 0;

public:
	//模板操作
	/*	函数功能：向模板上指定父窗口添加窗口
		参    数：	iParent：	模板上父窗体的序号
					pWindow：	添加进模板的窗体指针
		返 回 值：*/
	virtual bool AddWindow(int iParent, HWND pWindow) = 0;

	/*	函数功能：向模板上第一个空闲的父窗口添加窗口
		参    数：pWindow：	添加进模板的窗体指针
		返 回 值：*/
	virtual bool AddWindow(HWND pWindow) = 0;

	/*	函数功能：向模板上被选中的父窗口添加窗口
		参    数：pWindow：	添加进模板的窗体指针
		返 回 值：*/
	virtual bool AddWin2Select(HWND pWindow) = 0;

	/*	函数功能：删除模板上的窗口
		参    数：pWindow：	添加进模板的窗体指针
		返 回 值：添加进模板的窗体指针*/
	virtual void DelWindow(HWND pWindow) = 0;

	/*	函数功能：删除模板上的所有窗口
		参    数：
		返 回 值：*/
	virtual void DelWindow() = 0;

	/*	函数功能：查看指定的窗口在哪个父窗口上
		参    数：pWindow：	指定的窗口句柄
		返 回 值：父窗口的序号（未找到返回-1）*/
	virtual int FindWindow(HWND pWindow) = 0;

	/*	函数功能：获取模板上父窗体的个数
		参    数：
		返 回 值：模板上父窗体的个数*/
	virtual int GetParentCount() = 0;

	/*	函数功能：获取模板上已使用的父窗体个数
		参    数：
		返 回 值：模板上已使用的父窗体个数*/
	virtual int GetUsedTMCount() = 0;

	/*	函数功能：获取添加在模板父窗体上窗口的指针
		参    数：pWindow：	添加进模板的窗体指针
		返 回 值：模板上父窗体的序号*/
	virtual int GetParentIndex(HWND pWindow) = 0;

	/*	函数功能：获取添加在模板父窗体上窗口的指针
		参    数：	iParentIndex：	模板上父窗体的序号
		返 回 值：添加进模板的窗体指针*/
	virtual HWND GetChildWindow(int iParentIndex) = 0;

	/*	函数功能：设置模板类型
		参    数：tmType：	模板类型（TM_TYPE_4X3,TM_TYPE_16X9）
		返 回 值：*/
	virtual void SetTMType(HPTM_TM_TYPE tmType) = 0;

	/*	函数功能：设置模板类型
		参    数：
		返 回 值：模板类型（TM_TYPE_4X3,TM_TYPE_16X9）*/
	virtual HPTM_TM_TYPE GetTMType() = 0;

	/*	函数功能：设置模板子类型
		参    数：tmSubType：	模板子类型
		返 回 值：*/
	virtual void SetTMSubType(HPTM_TM_SUBTYPE tmSubType) = 0;

	/*	函数功能：设置模板子类型
		参    数：
		返 回 值：模板子类型*/
	virtual HPTM_TM_SUBTYPE GetTMSubType() = 0;

	/*	函数功能：交换同一个模板下的两个父窗口
		参    数：	iTMIndex：	模板的序号
					iSrcParent：源模板上父窗体的的序号
					iTagParent：目标模板上父窗体的的序号
		返 回 值：*/
	virtual void SwapChild(int iSrcParent, int iTagParent) = 0;

	/*	函数功能：使父窗口全屏到模板区域
		参    数：isFullTemplet：	是否全屏
		返 回 值：*/
	virtual void SetParentFullTemplet(int nParentIndex, bool isFullTemplet) = 0;

	/*	函数功能：使模板区域全屏或取消全屏
		参    数：isFullScreen：	是否全屏
		返 回 值：*/
	virtual void SetTempletFullScreen(bool isFullScreen) = 0;
	virtual bool IsTempletFullScreen() = 0;

	/*	函数功能：使模板区域全屏或取消全屏
		参    数：iParent：模板上父窗体的的序号
		返 回 值：*/
	virtual bool IsBigTemplet(int iParent) = 0;

	/*	函数功能：设置报警
		参    数：	iParent：模板上父窗体的的序号
					bIsAlarm：是否报警（true：报警，边框闪烁；false：取消报警）
		返 回 值：*/
	virtual void SetAlarm(int iParent, bool bIsAlarm) = 0;

	/*	函数功能：设置当前模板的名称
		参    数：
		返 回 值：*/
	virtual void SetTMName(std::string strName) = 0;

	/*	函数功能：获取当前模板的名称
		参    数：
		返 回 值：*/
	virtual std::string GetTMName() = 0;

	/*	函数功能：获取当前模板的序号
		参    数：
		返 回 值：*/
	virtual int GetTMIndex() = 0;

	/*	函数功能：获取全屏模板的序号
		参    数：
		返 回 值：*/
	virtual int GetFullTMIndex() = 0;

	/*	函数功能：设置模板颜色
		参    数：
		返 回 值：*/
	virtual void SetColors(COLORREF BackColor, COLORREF FrameColor, COLORREF TextColor, COLORREF SelectedColor) = 0;

	/*	函数功能：设置模板字体大小
	参    数：
	返 回 值：*/
	virtual void SetFontSize(int nFontSize = 250) = 0;

	/*	函数功能：设置模板浮动窗口位置
		参    数：
		返 回 值：*/
	virtual void SetFloatWndPosition(int nX, int nY, int nCX, int nCY) = 0;

	/*  函数功能: 切屏时使用 x.t
		参    数: bChangeScreen 是否切屏
		返 回 值: 
	*/
	virtual void SetChangeScreenStatus(bool bChangeScreen) = 0;

/*  函数功能: 切屏时使用 x.t
		参    数: bChangeScreen 是否切屏
		返 回 值: 
	*/
	virtual void SetWndText(int nWndID, LPCTSTR strWndText) = 0;

	/*	函数功能：获取模板上指定父窗口的大小
	参    数：	iParent：	模板上父窗体的序号
	返 回 值：*/
	virtual void GetWndRect(int iParent, int &x, int &y, int &cx, int &cy) = 0;
};


class HPTM_API HPTM_Manager
{
public:
	/*	函数功能：构造函数
		参    数：
		返 回 值：*/
	HPTM_Manager(void){};
	virtual~ HPTM_Manager(void){};

	/*	函数功能：获取指定子类型的父窗体的个数
		参    数：
		返 回 值：父窗体的个数*/
	virtual int GetSubTypeWinCount(HPTM_TM_SUBTYPE nSubType) = 0;

	/*	函数功能：添加模板
		参    数：	rect：		模板显示区域
					pParent：	模板父窗体指针（默认为空）
					tmType：	模板类型（HPTM_TM_TYPE中定义的值，默认为TM_TYPE_4X3）
		返 回 值：模板的序号*/
	virtual int AddTemplet(RECT &rect, HWND hParent = NULL, bool bIsPopWindow = false, 
		HPTM_TM_TYPE tmType = TM_TYPE_4X3, std::string flag = "CF_HPMC") = 0;

	/*	函数功能：获取模板句柄
		参    数：
		返 回 值：*/
	virtual HPTM_Templet* GetTemplet(int iTMIndex) = 0;

	/*	函数功能：删除所有模板
		参    数：
		返 回 值：*/
	virtual void DelTemplet() = 0;

	/*	函数功能：删除指定模板
		参    数：
		返 回 值：*/
	virtual void DelTemplet(int iTMIndex) = 0;

	/*	函数功能：获取模板的个数
		参    数：
		返 回 值：模板的个数*/
	virtual int GetTMCount() = 0;

	/*	函数功能：删除模板上的窗口
		参    数：pWindow：添加进模板的窗体指针
		返 回 值：添加进模板的窗体指针*/
	virtual void DelWindow(HWND pWindow) = 0;

	/*	函数功能：删除所有模板上的所有窗口
		参    数：
		返 回 值：*/
	virtual void DelWindow() = 0;

	/*	函数功能：设置总是显示选中边框
		参    数：bAwaysShow：默认为false，仅在有窗口时才显示选中，设为true则总是显示
		返 回 值：*/
	virtual void AwaysShowSelectedFrame(bool bAwaysShow) = 0;

	/*	函数功能：获取指定窗口所在的模板序号
		参    数：pWindow：添加进模板的窗体指针
		返 回 值：模板的序号*/
	virtual int GetTMIndex(HWND pWindow) = 0;

	/*	函数功能：设置当前激活的模板
		参    数：iTMIndex：设置当前激活的模板序号
		返 回 值：*/
	virtual void SetActiveTM(int iTMIndex) = 0;

	/*	函数功能：交换不同同主模板下的两个子模板
		参    数：	iSrcTM：	源模板的序号
					iSrcParent：源模板上父窗体的的序号
					iTagTM：	目标模板的序号
					iTagParent：目标模板上父窗体的的序号
		返 回 值：*/
	virtual void SwapChild(int iSrcTM, int iSrcParent, int iTagTM, int iTagParent) = 0;

	/*	函数功能：显示模板选择对话框
		参    数：
		返 回 值：*/
	virtual void ShowSelTMDlg() = 0;

	/*	函数功能：获取模板的Map表中第一个模板
		参    数：pTM：模板指针
		返 回 值：Map表中是否有第一项*/
	virtual bool GetFirstTM(HPTM_Templet** pTM) = 0;

	/*	函数功能：获取模板的Map表中下一个模板
		参    数：pTM：模板指针
		返 回 值：Map表中是否有下一项*/
	virtual bool GetNextTM(HPTM_Templet** pTM) = 0;

	/*	函数功能：在修改模板类型对话框修改模板后是否立即更改模板
		参    数：bImmediately：是否立即更改模板
		返 回 值：*/
	virtual void ChangeTMImmediately(bool bImmediately = true) = 0;

	/*	函数功能：获取当前选中的模板号
		参    数：	nTMIndex：		模板序号
					nParentIndex：	窗体序号
		返 回 值：*/
	virtual void GetSelTMIndex(int& nTMIndex, int& nParentIndex) = 0;

	//禁用所有操作
	virtual void SetEnable(bool bEnable = true) = 0;

	/*	函数功能：选中模板窗口
	参    数：	nTMID：		模板序号
				nParentID：	窗体序号
	返 回 值：*/
	virtual void SelectWnd(int nTMID, int nParentID) = 0;

	/*	函数功能：设置模板窗口是否可以拖动
	参    数：	nTMID：		模板序号
	nParentID：	窗体序号
	bDrag		true为可拖动,false为不可拖动
	返 回 值：*/
	virtual void SetWndDrag(int nTMID, int nParentID, bool bDrag) = 0;

	/*	函数功能：获得模板窗口是否可以拖动
	参    数：	nTMID：		模板序号
	nParentID：	窗体序号
	返 回 值：*/
	virtual bool GetWndDrag(int nTMID, int nParentID) const = 0 ;

	/*	函数功能：设置模板窗口是否要进行数码缩放
	参    数：	nTMID：		模板序号
	nParentID：	窗体序号
	bZoom		TRUE为要进行缩放,FALSE为停止缩放
	返 回 值：*/
	virtual void SetWndZoom(int nTMID, int nParentID, BOOL bZoom) = 0;

	/*	函数功能：获得模板窗口是否正在缩放
	参    数：	nTMID：		模板序号
	nParentID：	窗体序号
	返 回 值：*/
	virtual BOOL GetWndZoom(int nTMID, int nParentID) const = 0 ;
};

HPTM_API HPTM_Manager* CreateHPTM_Manager(HPTM_TempletNotify& rNotify, unsigned long ulSessionID);
HPTM_API HPTM_Manager* GetHPTM_Manager(unsigned long ulSessionID);
HPTM_API void DestoryHPTM_Manager(HPTM_Manager* pHPTM_Manager);