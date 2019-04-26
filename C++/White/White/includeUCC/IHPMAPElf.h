#ifndef __HPMAPDLL_H_
#define __HPMAPDLL_H_

class HPMAPDLLNotify
{
public:
	HPMAPDLLNotify(){}
	virtual ~HPMAPDLLNotify(){}
	
	virtual void HPMAP_OnSaveOneMap(int code,CString strHash,CString strMsg)=0; //code回调成功或者错误类型,strHash保存的MD5,strMSG附加信息
	virtual void HPMAP_OnDownLoadOneMap(int code,CString strHash,CString strMsg)=0;//code回调成功或者错误类型,strHash保存的MD5,strMSG附加信息
	virtual void HPMAP_OnOpenOneMap(int code,CString strHash,CString strMsg)=0;//code回调成功或者错误类型,strHash保存的MD5,strMSG附加信息
	virtual void HPMAP_OnCloseOneMap(int code,CString strHash,CString strMsg)=0;//
	virtual void HPMAP_OnDeleteOneMap(int code,CString strHash,CString strMsg)=0;
	virtual void HPMAP_OnDeleteOneBZ(int id)=0;//删除一个标注
	virtual void HPMAP_OnObjDBClick(int id,UINT nFlags,CPoint point)=0; //双击一个标注
	virtual void HPMAP_OnObjSelect (int id)=0; //选择一个标注
	virtual void HPMAP_OnLevel(CString strHash)=0; //当点击了回到上一层的热点或者命令时触发。
	virtual void HPMAP_OnDragDown(HPMC_DROP* dropdata,HWND hWnd,int x,int y)=0;//拖拉一个东西到地图上，返回树上的treeid和地图x，y坐标。
	virtual void HPMAP_DoCloseMap()=0;//地图内部菜单命令，通知外部，关闭自己
	virtual void HPMAP_DoSaveMap()=0;
	virtual void HPMAP_DoEditMap()=0;
	virtual void HPMAP_OnUserBtnDown()=0;
public:

};

class HPMAPDLL
{
public:
	HPMAPDLL() {};
	virtual ~HPMAPDLL() {};

public:
	virtual HWND GetMapCtrlHwnd(void) = 0;
	virtual void ShowMapCtrl(BOOL bShow)=0;
	virtual void MoveMapCtrl(int x,int y,int w,int h)=0;
	virtual void SetMapCtrlPath(CString maproot,CString mapServer,CString uploadPath,unsigned short	sServerPort)=0;//maproot 地图根目录 mapServer 
	virtual BOOL OpenOneMap(CString strHash)=0; //打开一张地图
	virtual BOOL DownLoadMap(CString strHash)=0;//根据hash下载一张地图
	virtual BOOL DeleteMap(CString strHash)=0;//删除地图文件
	virtual void SaveOneMap()=0;//包含了保存和上传的操作
	virtual BOOL CloseOneMap()=0;//关闭一个地图
	virtual void SetMapState(int state)=0; //0 浏览 1 编辑 2 上传中
	virtual int  GetMapState()=0;
	virtual int  AddMapPoint(int type,CString strName,int x=50,int y=50)=0; //添加标注
	virtual int  ModMapPoint(int id,int type,CString strName)=0;//修改标注
	virtual BOOL QueryMapPoint(int id,int &type,CString &strName)=0;//查询标注
	virtual BOOL IsMapChanged()=0;
	virtual void SaveOneMapByTreeID(CString strMapName)=0;
	virtual CString GetMapMD5(CString strMapName)=0;
	virtual void SetFlash(int objIndex,BOOL bResh)=0;
	virtual	void SetMapPrivEdit(BOOL bPrivEdit,unsigned int uAviewType)=0;
	virtual void SetMapSaveName(string strMapName)=0;
	virtual string GetMapSaveName()=0;
	virtual void SetMapUseMode(int nMode)=0;//0:为监控模式1:为会议模式
	};

AFX_EXT_CLASS HPMAPDLL* HPMAPDLL_CreateHPMapCtrl(int nMode,HPMAPDLLNotify* pNotify,CWnd*pParent);
AFX_EXT_CLASS void HPMAPDLL_ReleaseHPMapCtrl();
 

#endif


/*
//////////////////////////////////接口说明////////////////////////////////////////
 
 GetMapCtrlHwnd 
 Description: 获得地图控件窗体WIN32句柄.
 Usage: virtual HWND GetMapCtrlHWnd ();;
 Parameters: None
 Return Value: HWND
 Example: None
 Remarks: None

 ShowMapCtrl  
 Description:显示或者隐藏地图控件.
 Usage: virtual void ShowMapCtrl(BOOL bShow);;
 Parameters: BOOL
 Return Value: void
 Example: ShowMapCtrl(False);//隐藏地图控件
 Remarks: None

 MoveMapCtrl 
 Description: 移动地图控件的位置
 Usage: virtual void MoveMapCtrl (int x,int y,int w,int h);
 Parameters: int x:控件窗体左上角移动的X坐标 
 int y:移动的Y坐标
 int w:控件窗体的宽度 
 int h:控件窗体的高度
 Return Value: None
 Example: None
 Remarks: 

 SetMapCtrlPath 
 Description: 设置地图控件路径信息
 Usage: virtual SetMapCtrlPath(Cstring maproot,CString mapServer ,CString uploadPath)
 Parameters: CString maproot: 地图文件的路径
 CString mapServer:地图下载的根目录
 CString uploadPath:地图上传的HTTP路径
 Return Value: None
 Example: None
 Remarks: 

 OpenOneMap 
 Description: 打开一张地图
 Usage: virtual BOOL OpenOneMap(CString strHash);
 Parameters: CString strHash 地图文件的哈希值，必须64位的字符串
 Return Value: 打开成功或者失败
 Example: None
 Remarks: 调用此办法后，立即收到回调OnOpenOneMap(int code,CString strHash,CString strMsg)事件，回调事件的参数说明如下：
 Int Code:回调事件码 1 表示打开成功 0表示地图不存在或者还没有下载
 CString strHash:该地图文件的哈希值
 Cstring strMsg:返回的提示字符串信息


 DownLoadMap () 
 Description: 下载一张地图
 Usage: virtual virtual BOOL DownLoadMap(CString strHash);
 Parameters: ulRoomID：房间ID
 Return Value: None
 Example: None
 Remarks: 调用此方法后，异步得到OnDownLoadOneMap(int code,CString strHash,CString strMsg)事件 回调事件的参数如下：
 Int Code:回调事件码 1 表示下载成功 0下载失败(网络中断或者取消或者地图服务器不存在该地图)
 CString strHash:该地图文件的哈希值
 Cstring strMsg:返回的提示字符串信息


 SaveOneMap  
 Description: 收到准备进入会议
 Usage: virtual void SaveOneMap ();
 Parameters: ulRoomID：房间ID
 Return Value: None
 Example: None
 Remarks: 调用SaveOneMap后，异步收到OnSaveOneMap事件。
 事件参数如下
 Int Code:回调事件码 1 表示保存并上传成功 0表示保存失败
 CString strHash:该地图文件新哈希值
 Cstring strMsg:返回的提示字符串信息

 CloseOneMap  
 Description: 关闭当前操作的地图，恢复到地图的初始状态
 Usage: virtual void CloseOneMap ();
 Parameters: None
 Return Value: None
 Example: None
 Remarks: 回调OnCloseOneMap(int code,CString strHash,CString strMsg)=0;//
 事件。事件参数如下
 Int Code:回调事件码 1 表示地图已经关闭 0表示关闭失败
 CString strHash:关闭的地图文件哈希值
 Cstring strMsg:返回的提示字符串信息

 DeleteMap  
 Description: 更新地图后，客户端通过此接口删除指定哈希的地图
 Usage: virtual void DeleteMap (Cstring strHash);
 Parameters: Cstring strHash 地图哈希值
 Return Value: BOOL TRUE 成功 FALSE 失败
 Example: None
 Remarks:

 SetMapState 
 Description: 设置地图状态
 Usage: virtual void SetMapState(int state);
 Parameters: int state 状态 0 浏览 1 编辑 2 上传中
 Return Value: None
 Example: None
 Remarks:。

 GetMapState  
 Description: 获取当前地图状态
 Usage: virtual int GetMapState ()
 Parameters: None
 Return Value: int state 状态 0 浏览 1 编辑 2 上传中


*/