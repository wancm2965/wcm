//========================================================================
//类名：监控客户端接口
//功能：音视频监视、对讲、云台控制、录像回放、电子地图等功能
//作者：龙国烜
//时间：2008-12-19
//说明：
//------------------------------------------------------------------------
using namespace std;
#pragma once
#include "KSYSClient.h"
#include "StructDefine.h"


/*
 *	监控客户端框架接口
 */
class HPMC_Client
{
public:
	HPMC_Client() {}
	virtual ~HPMC_Client() {}

public:
	/*	
	函数功能： 根据配置文件设置窗口显示的屏幕
	参    数： 
	返 回 值： 成功/失败
	*/
	virtual void ShowHPMC(bool bIsXPE = false) = 0;

	/*	
	函数功能： 显示窗口
	参    数： nCmdShow
	返 回 值： 成功/失败
	*/
	virtual BOOL ShowWindow(int nCmdShow) = 0;

	/*	
	函数功能： 设置窗口位置
	参    数： const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags
	返 回 值： 成功/失败
	*/
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;

	/*	
	函数功能： 获取窗口句柄
	参    数： 无
	返 回 值： 窗口句柄
	*/
	virtual HWND GetSafeHwnd() = 0;	

	/*	
	函数功能： 获取组信息结束
	参    数： 无
	返 回 值： 无
	*/
	virtual void OnGetGroupInfoEnd(const std::string& strDomain) = 0;

	/*	
	函数功能： 获取通道信息结束
	参    数： 无
	返 回 值： 无
	*/
	virtual void OnChannelInfoEnd(const std::string& strDomain) = 0;

	/*	
	函数功能：更新通道信息
	参    数： CHANNELINFO
	返 回 值： 无
	*/
	virtual void OnChannelInfo(const CHANNELINFO& item) = 0;

	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID) =0;


	/*
	函数功能： 返回监控P2P
	参    数： strDevID设备ID
	返 回 值： 无
	*/
	virtual void OnMonP2PData(const std::string& strDevID,KCmdUserDataPacket& packet) = 0;

	/*
	函数功能： 返回监控数据（需要服务器中转）
	参    数： 
	返 回 值： 无
	*/
	virtual void OnMonUserData(KCmdUserDataPacket& packet) = 0;

	/*
	函数功能： 监控报警
	参    数： strChannelID	通道ID
			   usType		报警类型
			   usSubtype	子类型
	返 回 值： 无
	*/
	virtual void OnMonAlertData(const std::string strChannelID,unsigned short usType,unsigned short usSubtype) = 0;

	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;

	virtual void OnUDPRcvGHJInfo(const UDP_RCV_GHJ_INFO& info)=0;

	//P2P透传
	virtual void OnUserData(KCmdUserDataPacket& packet,unsigned char ucObject) = 0;

	//定损
	virtual void OnLoss(MMS_LOSS& loss, unsigned short usErrorCode) = 0;

	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, unsigned long ulGroupID)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;


	//镜头分组
	virtual void OnDEVAddMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVDelMyGroup(const std::string& strTmpGrpID)=0;
	virtual void OnDEVRenameMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVAddMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;
	virtual void OnDEVDelMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;

	virtual void OnDEVGetMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVGetMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;

	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	//根据树节点ID得到关联的热点ID
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;

	//电视墙
	virtual void OnInsertTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;

	//电视墙控制器
	virtual void OnInsertTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;

	virtual void OnTVServerItem(const std::string& strTV,const std::string& strTVName,const std::string& strStatus)=0;
	virtual void OnTVMonitorItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVMatrixItem(unsigned short usIndex,const std::string& strData)=0;

	virtual void OnTVListEnd(void)=0;
	virtual void SetMonClientMCU(MMS_MCU* mcu)=0;
	virtual void OnSetAVCONType(int type)=0;
	virtual void OnScreenControlOnKeyDown(MSG* pMsg)=0;

	//电视墙新增
	virtual void OnRoomGroupItem(MMS_GROUP_EX& item)=0;
	virtual void OnRoomGroupListEnd(const std::string& strUpdateTime)=0;
	/* 收到会议成员项 */
	virtual void OnRoomMemberItem(MMS_MEMBER_EX& item,bool bAllGeted)=0;
	/* 收到会议成员项结束 */
	virtual void OnRoomMemberListEnd(void)=0;

	/* 收到预定窗口成员项 */
	virtual void OnWindowMemberItem(const std::string& strGroupID,unsigned int nScreenID,unsigned int nWindowID,const std::string& strMemberID)=0;

	/* 收到预定窗口成员项结束 */
	virtual void OnWindowMemberListEnd(void)=0;

	/****************************************************************
	* H323MP函数
	****************************************************************/
	// 根据MPID返回SIP设备

	// 根据MPID返回SIP设备列表结束
	virtual void OnGetSIPSetByMPEnd(const std::string& strMPID)=0;

	virtual void ProTVCInitTreeMember(const std::string& strRoomID,const std::string& strRoomName,const unsigned short& uRoomType)=0;

	virtual void OnStatus(const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType,
		const std::string& strSvrID,const std::string& strSvrAddr,unsigned short usSvrPort,const std::string& strSvrType,
		const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
		unsigned short usTermType,const std::string& strTermName,unsigned char ucStatus)=0;


	virtual void OnCardItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardName,
		unsigned int nChannelType,unsigned long ulChannnelID,
		const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
		const std::string& strDevInfo)=0;
	virtual void SetTVCMemberRoom(const string& strRoomID)=0;

	//错误提示
	virtual void PopuMsg(CString sMsg)=0;

	virtual void OnRcvP2PData(const std::string&strNodeID,KCmdPacket& packet)=0;
};

/*
函数功能： 创建监控客户端实例
参    数： 回调实例
返 回 值： 实例指针
*/
AFX_EXT_API HPMC_Client* Create_HPMC_Client(void);

/*	
函数功能： 销毁监控客户端实例
参    数： 实例句柄
返 回 值： 无
*/
AFX_EXT_API void Destroy_HPMC_Client(HPMC_Client*pClient);
