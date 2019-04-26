
#pragma once


//========================================================================
//类名：设置视频调整窗口资源类
//功能：调整本地、远程或所有视频的亮度、对比度、饱和度、色调、伽马、锐化值
//作者：杜权胜
//时间：2008-09-10
//说明：通过设置回调函数地址实现参数回调
//--------------------------------------------------------------------------

#define ADJUST_TYPE_CANCEL		0	//用户取消调整
#define ADJUST_TYPE_APPLY		1	//用户要应用当前视频调整
#define ADJUST_TYPE_PREVIEW		2	//用户拖动滚动条
#define ADJUST_TYPE_DEFAULT		3	//用户取默认值

#define ADJUST_MODE_INCREASE	0	//调整幅度一致
#define ADJUST_MODE_RESULT		1	//调整后结果一致

typedef struct HPUI_VIDEO_PARAM
{
	int iBrightness;	//亮度
	int iContrast;		//对比度
	int iSaturation;	//饱和度
	int iHue;			//色调
	int iGamma;			//伽马
	int iSharpen;		//锐化值
	int iRed;
	int iGreen;
	int iBlue;
}Video_Param;


class AFX_EXT_CLASS HPDLG_VideoParamNotify
{
public:
	HPDLG_VideoParamNotify(){};
	~HPDLG_VideoParamNotify(){};
public:
	/* 函数：设置视频调整回调函数
	* 函数功能：调整本地、远程或所有视频的亮度、对比度、饱和度、色调、伽马、锐化值
	* 输入参数：
	*		memberid,	空:调整所有通道的视频
	*					本地通道号:仅调整本地视频
	*					其他值:调整远程视频
	*		iAdjustType ,0 :用户取消调整
	*					1 :用户要应用当前视频调整
	*					2 :用户拖动滚动条，此时处于预览模式
	*					3 :用户取默认值，处于预览模式
	*		iApplyMode,0:调整幅度一致
	*					1:调整后结果一致*/
	virtual void HPDLG_VideoParamSet(std::string memberid, unsigned long ulChannelID, 
		Video_Param iVideoParam, int iAdjustType, int iApplyMode) = 0;
};


class AFX_EXT_CLASS HPDLG_VideoParam
{	
public:
	HPDLG_VideoParam();
	virtual~ HPDLG_VideoParam(void);

	/* 函数：创建及显示对话框
	 * pParent, 父窗口句柄 
	 * bModal, 是否以模态显示*/

	bool Create(HPDLG_VideoParamNotify& rNotify, bool bAdvanced = true, CWnd* pParent = NULL);
	void ShowWindow(CWnd* pParent, bool bModal = true);

	/* 强制关闭对话框*/
	void Destroy();

public:

	/*设置默认参数
	* iVideoParam		参数的默认值
	* iVideoParamMin	参数的最小值
	* iVideoParamMax	参数的最大值*/
	void SetDefVideoParam(Video_Param iVideoParam, Video_Param iVideoParamMin, Video_Param iVideoParamMax);

	/*初始化调整参数
	* memberid			用户ID
	* ulChannelIndex	通道ID
	* iVideoParam		参数的初始值*/
	void InitVideoParam(std::string memberid, unsigned long ulChannelIndex, Video_Param iVideoParam);

	/* 函数：设置视频调整回调函数
	 * 函数功能：调整本地、远程或所有视频的亮度、对比度、饱和度、色调、伽马、锐化值
	 * 输入参数：
	 *		memberid,	空:调整所有通道的视频
	 *					本地通道号:仅调整本地视频
	 *					其他值:调整远程视频
	 *		iAdjustType ,0 :用户取消调整
	 *					1 :用户要应用当前视频调整
	 *					2 :用户拖动滚动条，此时处于预览模式
	 *					3 :用户取默认值，处于预览模式
	 *		iApplyMode,0:调整幅度一致
	 *					1:调整后结果一致
	void HPDLG_SetVParamCallBack(void(CALLBACK* pCallBack)(std::string memberid, unsigned long ulChannelID, 
		Video_Param iVideoParam, int iAdjustType, int iApplyMode));*/

private:
	void*		m_hWnd;
};

