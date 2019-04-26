// UCC7-MM会议功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_MMSDefine.h"
#include "AVCONSys/KXNCP.h"

namespace AVCONPlugin
{
	// 音视频类型定义
	typedef enum _THPAVStream{
		HPAV_TYPE_NONE		= 0,
		HPAV_TYPE_VIDEO		= 100,	// 视频
		HPAV_TYPE_AUDIO,			// 音频
		HPAV_TYPE_AV,				// 音视频
	}THPAVStream;

	class IPlugin_U7_MM_Function_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_Function_Observer(){};

	public:
		virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType,const MMS_MEMBER_TYPE& memberType, const std::string& strTag){};
		virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType){};
		virtual void OnRecvChatMessage( const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat ){};

		/* 拖动窗口 */
		virtual void OnDragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID){};

		virtual void OnRoomDigitalComp(const _MMS_CHLTYPE& channelType){};
		/* 改变模板 */
		virtual void OnSetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber){};

		virtual void OnSetCurrentScreenID(int nCurScrID){}; 

		virtual void OnSetFullScreen(int nScreenID,int nWindowID,int nFullType){};

		virtual void OnTellFullScreen(int nCardIndex){};

		/* 浮动窗口 */
		virtual void OnSetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY){};

		virtual void OnSetTempPresider(const std::string& strMemberID,bool bSet){};

		/*会议锁定*/
		virtual void OnSetRight(int nRight, bool bEnable, const std::string& strMemberID){};

		// 触摸授权回调
		virtual void OnTouchScreenAuthorize(bool bAuthorize){};

		/* 申请或删除提问人回调 */
		// strMemberID：提问人ID
		// bRequest：true为申请，false为删除
		virtual void OnSetQuestioner(const std::string& strMemberID,bool bRequest){};

		/* 单接或取消单接提问人回调 */
		// strMemberID：提问人ID
		// bEnable：是否单接提问人
		virtual void OnEnableQuestioner(const std::string& strMemberID,bool bEnable){};

		// 公告标题
		virtual void OnAnnexData(int nType,const std::string& strData,const std::string& strFormat){};
		virtual void OnEnableAnnex(int nType,bool bEnable){};

		//对讲
		virtual void OnChAudioStart(std::string strFromUserID, std::string strUserData){};

		virtual void OnGetAlarmLogInfo( const AVCON_MON_ALARMLOG& alarmLoginfo ){};
		/* 开始投票回调
		 * strVoteID:	投票标识
		 * strInitiator: 发起人
		 * strTopic:	投票主题
		 * bType:		投票类型，记名投票为true，不记名为false
		 * ulTime:		投票时长，为了与AVCON统一，使用毫秒单位
		 * bMulti:		是否允许多选
		 * strItems:	选项列表 */
		 virtual void OnStartVote(const std::string& strVoteID,const std::string& strInitiator,const std::string& strTopic,bool bType,unsigned long ulTime,bool bMulti,MMS_VOTE_LST& strItems){};

		/* 投票错误回调
		 * nErrCode:	错误码 */
		 virtual void OnStartVoteErr(unsigned int nErrCode){};

		 /* 停止投票回调
		 * strVoteID:	投票标识
		 * strEndType:	投票结束方式，手动或自动（"hand" 或 "auto"）
		 * nSum:		应投票人数
		 * nAnswer:		响应人数
		 * strItems:	选项列表 */
		virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,int nSum,int nAnswer,MMS_VOTE_LST& strItems){};

		/* 开始举手，所有人都会收到 ulID：本次举手的ID; ulTime:举手时间 */
		virtual void OnStartHandle(unsigned long ulID, unsigned long ulTime){};
		/* 举手错误，nErrCode:错误码 */
		virtual void OnStartHandleErr(unsigned int nErrCode){};
		/* 停止举手，所有人都会收到 ulID：本次举手的ID; usTotal:总人数; usCount:举手人数*/
		virtual void OnStopHandle(unsigned long ulID, unsigned short usTotal, unsigned short usCount){};

		/* 开始点名,所有人都会收到 strMemberID:点名发起人ID */
		virtual void OnCalling(const std::string& strMemberID, unsigned int usTime){};
		virtual void OnCallingErr(unsigned int nErrCode){};
		virtual void OnCloseCalling(const std::string& strMemberID){};

		/* 模版预置参数回调 */
		virtual void OnSetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute){};
		/* 设置会议窗口属性
		* strAllAttribute所有窗口的属性串
		* strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
		virtual void OnSetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute=""){};


		/* 视频采集锁定 */
		virtual void OnSetCaptureLock(const std::string& strMemberID, int nDevID, bool bLock){};

		/* 自动点名 */
		virtual void OnSetAutomaticCall(int nEndTime, int nIntervalTime){};

		virtual void OnSetWndStatusInLive(const std::string& strParameter){};

		virtual void OnShowMeetingRemind(bool bShowMeetingRemind){};

		// 设置会议参数
		virtual void OnSetMeetingParameter(const std::string& strGroupID,const std::string& strKeyword, const std::string& strParameter, const std::string& strAdminID){};
		// 删除会议参数
		virtual void OnDelMeetingParameter(const std::string& strGroupID,const std::string& strKeyword){};

		//此回调未客户端自己手动调用！在会议中，自己的视频被广播了，此时视频设置被改变应该手动调用此回调，
		//目的是通知会议所有人自己的码流设置被改变，应该调整视频接受方式（主要是TR2会收到此消息，调整码流接受，预防小窗口解大码流）
		//nStream, int nResWidth, int nResHeight为高码流值和分辨率
		virtual void OnSetHighStreamResolution(const std::string& strMemberID, int nCardIndex, int nStream, int nResWidth, int nResHeight, bool bOpenCalc = true){};
	};

	// UCC7-MM会议功能实现插件
	class IPlugin_U7_MM_Function : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Function(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Function_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Function_Observer* pObserver) = 0;

		// 返回白板、文档共享、讨论的ChannelID
		virtual unsigned long PublishLocalCard(int nCardIndex, int nDevID = -1) = 0;

		// 用以音频通道ID改变时再次发布
		virtual void SetCardAudioItem(int nCardIndex,unsigned long ulAudioID) = 0;

		virtual void Broadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType,bool bMutex = true, const std::string& strTag = "CON_SYNC") = 0;
		virtual void CloseBroadcast(const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType) = 0;

		/* 设置临时主持人
		* bSet: true=设置;false=取消 */
		virtual void SetTempPresider(std::string& strMemberID,bool bPresider) = 0;

		virtual void SendMMMessage(const std::string& strMemberID,const std::string& strBody,const std::string& strFormat)= 0;

		/* 切换窗口 */
		virtual void DragWindow(int nFromCardID, int nToCardID, const std::string& strFromMemberID, const std::string& strToFromMemberID, int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID) = 0;

		/*设置当前激活屏*/
		virtual void SetActive(int nScreenID) = 0 ;

		/* 设置会议模板 */
		virtual void SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber) = 0;

		/* 设置窗口全屏（参数：nScreenID 模板号；nWindowID 窗口号；nFullType 是否全屏；
		* lpszMemberID 通道的所有者；nCardIndex 通道号）
		* 说明：当nFullType为0，即取消全屏时，参数lpszMemberID和nCardIndex无意义，可缺省；
		*		当nFullType为1，即设置窗口全屏时，必须指定通道所有者的ID以及通道号，
				，如果通道没有所有者（如讨论和白板），那么参数lpszMemberID可缺省。 */
		virtual void SetFullScreen(int nScreenID, int nWindowID, int nFullType, const char* lpszMemberID = "", int nCardIndex = 0) = 0;
		virtual void TellFullScreen(const std::string& strMemberID, int nCardIndex) = 0;
		/* 设置浮动窗口位置
		* x = 像素比例 * 100
		* y = 像素比例 * 100
		* cx= 宽度比例 * 100
		* cy= 高度比例 * 100 */
		virtual void SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY) = 0;

		/* 获取组同步信息 */
		virtual void GetGroupSynInfo(const std::string& strGroupID) = 0;

		virtual void ModifyCardName(const std::string& strMemberID, int nCardIndex, const std::string& strCardName) = 0;//lml

		virtual void SetRight(int nRight, bool bEnable, const std::string& strMemberID="") = 0;

		// 获取触摸授权
		virtual void GetTouchScreenAuthorize(void) = 0;
		// 释放触摸授权
		virtual void ReleaseTouchScreenAuthorize(void) = 0;

		virtual void BeginTalk(const std::string &strChannelID, int nCurrentTalkType, bool bForce=false) = 0;

		/* 申请或删除提问人 */
		// strMemberID为空则是申请提问，strMemberID不为空则删除提问人
		virtual void SetQuestioner(const std::string& strMemberID = "") = 0;

		/* 单接或取消单接提问人*/
		// strMemberID ：提问人ID
		// bEnable：是否单接
		virtual void EnableQuestioner(const std::string& strMemberID, bool bEnable) = 0; 

		/****************************************************************
		 * 会议标题、公告函数
		 ****************************************************************/

		/* 设置内容 */
		virtual void SetAnnexData(int nType,const std::string& strData,const std::string& strFormat) = 0;

		/* 设置 */
		virtual void EnableAnnex(int nType,bool bEnable=true) = 0;

		/* 	获取公告、标题内容	*/
		virtual void GetAnnexData(void) = 0;

		/****************************************************************
		 * 会议投票函数
		 ****************************************************************/

		/* 只有位于root组的主持人才可以发起投票,子组会议中不能发起
		 * 预投票 */
		virtual void PrepVote(void) = 0;

		/* 发起投票
		 * strTopic:	投票主题
		 * bType:		投票类型，记名投票为true，不记名为false
		 * ulTime:		投票时长，为了与AVCON统一，使用毫秒单位
		 * bMulti:		是否允许多选
		 * strItems:	选项列表*/
		virtual void StartVote(const std::string& strTopic, bool bType, unsigned long ulTime, bool bMulti, MMS_VOTE_LST& strItems) = 0;

		/* 结束投票 
		 * strVoteID:	投票标识ID
		 * strEndType:	投票结束方式，手动或自动（"hand" 或 "auto"）*/
		virtual void StopVote(const std::string& strVoteID, const std::string& strEndType) = 0;

		/* 投票
		 * strVoteID:	投票标识ID
		 * strItems:	列表中存有要投票的选项，如果弃权，则清空列表即可 */
		virtual void Vote(const std::string& strVoteID, MMS_VOTE_LST& strItems) = 0;

		/****************************************************************
		 * 会议举手函数
		 ****************************************************************/

		/* 开始举手:发起人调用 */
		virtual void StartHandle(unsigned long ulTime) = 0;

		/* 举手:举手人调用 */
		virtual void Handle(unsigned long ulID, bool bAgree) = 0;

		/* 停止举手:发起人调用 */
		virtual void StopHandle(unsigned long ulID) = 0;

		/****************************************************************
		 * 会议点名函数
		 ****************************************************************/

		/* 开始点名:发起人调用 */
		virtual void Calling(unsigned int usTime) = 0;

		/* 关闭点名:发起人调用 */
		virtual void CloseCalling() = 0;

		/* 向服务器发送确认图片:被点名人调用 */
		virtual void CallingInConference(const std::string& strFile) = 0;
		/* 设置会议屏属性
		 * strAllAttribute所有屏的属性串
		 * strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
		virtual void SetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="") = 0;
		
		/* 设置会议窗口属性
		* strAllAttribute所有窗口的属性串
		* strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
		virtual void SetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="") = 0;

		/*开启 MPS*/
		virtual void StartMP(void)=0;
		virtual void Start3GC(void)=0;
		/*结束 MPS*/
		virtual void StopMPS(const std::string& strMPSUserID)=0;

		/* 视频采集锁定 */
		virtual void SetCaptureLock(const std::string& strMemberID, int nDevID, bool bLock) = 0;

		// 设置自动点名
		virtual void SetAutomaticCall(int nEndTime, int nIntervalTime) = 0;

		virtual void SetWndStatusInLive(const std::string& strWndStatusInLive) = 0;

		//手动调用通知自己的高码流以及分辨率被改变！在会议中，自己的视频被广播了，此时视频设置被改变应该手动调用此回调，
		//目的是通知会议所有人自己的码流设置被改变，应该调整视频接受方式（主要是TR2会收到此消息，调整码流接受，预防小窗口解大码流）
		//nStream, int nResWidth, int nResHeight为高码流值和分辨率
		virtual bool SetHighStreamResolution(const std::string& strMemberID, int nCardIndex, int nStream, int nResWidth, int nResHeight, bool bOpenCalc = true) = 0;
		
		//发送会议属性通知所有人，视频设置码流以及分辨率被手动改变（适配于TR2，用于调整码流）
		virtual void SetStreamResolutionNotice(int nCardIndex, int nStream, int nResWidth, int nResHeight) = 0;

		// 是否显示会议提醒
		virtual void ShowMeetingRemind(bool bShow = true) = 0;

		// 设置会议参数
		virtual void SetMeetingParameter(const std::string& strGroupID, const std::string& strKeyword, const std::string& strParameter, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL) = 0;
		// 删除会议参数
		virtual void DelMeetingParameter(const std::string& strGroupID, const std::string& strKeyword, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL) = 0;

		//评价客服StrServiceID:客服ID,usSatisfaction:满意度（0:不予评价，1:不满意，2:基本满意，3:满意，4:非常满意），usServiceTime:服务时长（单位秒）
		virtual	void JudgeService(const std::string& strServiceID,const unsigned short& usSatisfaction=0,const unsigned long& ulServiceTime=0) = 0;

		// 上传事后总结 
		// business_type:业务类型
		// business_first_level:一级业务类型
		// business_second_level:二级业务类型
		// strDepart:客服自己所在的部门
		// starttime:客服开始电话服务开始时间
		// endtime:客服结束电话服务时间
		virtual void UploadServiceSumary(const int& business_type, const int& business_first_level, const int& business_second_level, const std::string& strDepart = "", const unsigned long& ulTalkTime = 0) = 0;
	};
}
