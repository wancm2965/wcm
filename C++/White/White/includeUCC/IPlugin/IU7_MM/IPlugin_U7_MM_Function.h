// UCC7-MM会议功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

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
		virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,int nChannelType){};
		virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType){};
		virtual void OnRecvChatMessage( const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat ){};

		/* 拖动窗口 */
		virtual void OnDragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID){};

		/* 改变模板 */
		virtual void OnSetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber){};

		virtual void OnSetCurrentScreenID(int nCurScrID){}; 

		virtual void OnSetFullScreen(const std::string& strMemberID,int nCardIndex,int nScreenID,int nWindowID,int nFullType){};

		/* 浮动窗口 */
		virtual void OnSetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY){};

		virtual void OnSetTempPresider(std::string& strMemberID,bool bSet){};

		/*会议锁定*/
		virtual void OnLockMeet(bool bLock){};

		// 触摸授权回调
		virtual void OnTouchScreenAuthorize(bool bAuthorize){};

		//对讲
		virtual void OnChAudioStart(std::string strFromUserID, std::string strUserData){};

		// 投票
		/* 预投票回调
		 * strMemberID: 投票发起人
		 * strVoteID:	投票标识
		 * bAllow:		是否允许投票，如果当前会议中已经存在了一个投票，则此值为false */
		virtual void OnPrepVote(const std::string& strMemberID,const std::string& strVoteID,bool bAllow){};
		
		/* 开始投票回调
		 * strVoteID:	投票标识
		 * strTopic:	投票主题
		 * bType:		投票类型，记名投票为true，不记名为false
		 * ulTime:		投票时长，为了与AVCON统一，使用毫秒单位
		 * bMulti:		是否允许多选
		 * strItems:	选项列表 */
		 virtual void OnStartVote(const std::string& strVoteID,const std::string& strTopic,bool bType,unsigned long ulTime,bool bMulti,MMS_VOTE_LST& strItems){};
		
		 /* 停止投票回调
		 * strVoteID:	投票标识
		 * strEndType:	投票结束方式，手动或自动（"hand" 或 "auto"）
		 * nSum:		应投票人数
		 * nAnswer:		响应人数
		 * strItems:	选项列表 */
		virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,int nSum,int nAnswer,MMS_VOTE_LST& strItems){};

		/* 开始举手，所有人都会收到 ulID：本次举手的ID; ulTime:举手时间 */
		virtual void OnStartHandle(unsigned long ulID, unsigned long ulTime){};
		/* 停止举手，所有人都会收到 ulID：本次举手的ID; usTotal:总人数; usCount:举手人数*/
		virtual void OnStopHandle(unsigned long ulID, unsigned short usTotal, unsigned short usCount){};

		/* 开始点名,所有人都会收到 strMemberID:点名发起人ID */
		virtual void OnCalling(const std::string& strMemberID, unsigned int usTime){};
		virtual void OnCloseCalling(const std::string& strMemberID){};
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
		virtual unsigned long PublishLocalCard(int nCardIndex) = 0;
		// 用以音频通道ID改变时再次发布
		virtual void SetCardAudioItem(int nCardIndex,unsigned long ulAudioID) = 0;

		virtual void Broadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType) = 0;
		virtual void CloseBroadcast(const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType) = 0;

		/* 设置临时主持人
		* bSet: true=设置;false=取消 */
		virtual void SetTempPresider(std::string& strMemberID,bool bPresider) = 0;

		virtual void SendMMMessage(const std::string& strMemberID,const std::string& strBody,const std::string& strFormat)= 0;

		/* 切换窗口 */
		virtual void DragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID) = 0;

		/*设置当前激活屏*/
		virtual void SetActive(int nScreenID) = 0 ;

		/* 设置会议模板 */
		virtual void SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber) = 0;

		virtual void SetFullScreen(const std::string& strMemberID, int nCardIndex, int nScreenID, int nWindowID, int nFullType) = 0;
		/* 设置浮动窗口位置
		* x = 像素比例 * 100
		* y = 像素比例 * 100
		* cx= 宽度比例 * 100
		* cy= 高度比例 * 100 */
		virtual void SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY) = 0;

		virtual void ModifyCardName(const std::string& strMemberID, int nCardIndex, const std::string& strCardName) = 0;//lml

		virtual void LockMeet(bool bLock) = 0;

		// 获取触摸授权
		virtual void GetTouchScreenAuthorize(void) = 0;
		// 释放触摸授权
		virtual void ReleaseTouchScreenAuthorize(void) = 0;

		virtual void BeginTalk(const std::string strChannelID, int nCurrentTalkType, bool bForce=false) = 0;

		/****************************************************************
		 * 会议投票函数
		 ****************************************************************/

		/* 只有位于root组的主持人才可以发起投票,子组会议中不能发起
		 * 预投票 */
		virtual void PrepVote(void) = 0;

		/* 发起投票 
		 * strVoteID:	投票标识
		 * strTopic:	投票主题
		 * bType:		投票类型，记名投票为true，不记名为false
		 * ulTime:		投票时长，为了与AVCON统一，使用毫秒单位
		 * bMulti:		是否允许多选
		 * strItems:	选项列表*/
		virtual void StartVote(const std::string& strVoteID, const std::string& strTopic,
			bool bType, unsigned long ulTime, bool bMulti, MMS_VOTE_LST& strItems) = 0;

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
	};
}
