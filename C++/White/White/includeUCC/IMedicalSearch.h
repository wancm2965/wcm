//========================================================================
//类名：文件查询接口
//功能：avmsplayer 获取远程录像文件列表信息
//作者：朱 友
//时间：2011-2-16
//说明：
//--------------------------------------------------------------------------

#pragma once

//查询回调函数
class HPDLG_API IMedicalSearchNotify
{
public:
	IMedicalSearchNotify(){};
	virtual~ IMedicalSearchNotify(){};
	
	//播放视频文件(文件共享方式)
	virtual void PlayAVMSFile(CString strFilePath, CString strFileNameKey) = 0;

	virtual void OpenPlayAVMFileEnd() = 0;
};

//查询对话框
class AFX_EXT_CLASS HPDLG_MedicalSearch
{
public:
	HPDLG_MedicalSearch(void){};
	virtual~ HPDLG_MedicalSearch(void){};

	// 显示/隐藏对话框
	virtual void ShowWindow(int nCmdShow) = 0;

	//当前框口是否已打开
	virtual BOOL IsWindowShow() = 0;

	virtual CString PlayNextFile(CString strFileNameKey) = 0;
};	

// 创建对话框
HPDLG_API HPDLG_MedicalSearch* CreateMedicalSearchDlg(IMedicalSearchNotify& rNotify ,CWnd* pParent = NULL);

// 获得指针
HPDLG_API HPDLG_MedicalSearch* GetMedicalSearchDlg(void);

// 销毁对话框
HPDLG_API void DestroyMedicalSearchDlg(void);
