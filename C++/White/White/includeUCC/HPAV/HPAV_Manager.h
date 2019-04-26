#pragma once

#include "HPAV_Player.h"


#include <map>
typedef std::map<std::string,HPAV_Player*>	MapHPAV_Player;
typedef MapHPAV_Player::iterator MapHPAV_Player_IT;


class HPAV_API HPAV_Manager
{
public:
	/*	函数功能：构造函数
		参    数：	rNotify：		接口引用
					ulSessionID：	标记
		返 回 值：*/
	HPAV_Manager(HPAV_PlayerNotify& rNotify, unsigned long ulSessionID = 0);
	virtual~ HPAV_Manager(void);

	void Destroy();

	/* 创建HPTC_AVRPlayer实例 */
	HPAV_Player* CreateHPAV_Player(CWnd*pParent, const std::string& memberid, int nCardID);

	/*	函数功能：强制关闭指定对话框并销毁资源
		参    数：
		返 回 值：*/
	void DestroyHPAV_Player(CWnd* pWindow);

	/* 销毁HPTC_AVRPlayer实例 */
	void DestroyHPAV_Player(const std::string& memberid, int nCardID);

	/* 销毁HPTC_AVRPlayer实例 playerid=memberid_did*/
	void DestroyHPAV_Player(const std::string& playerid);

	/* 销毁所有HPTC_AVRPlayer实例 */
	void DestroyHPAV_Player(void);

	/* 得到HPTC_AVRPlayer实例 */
	HPAV_Player* GetHPAV_Player(const std::string& memberid, int nCardID);

	/* 得到HPTC_AVRPlayer实例 */
	HPAV_Player* GetHPAV_Player(const std::string& playerid);

	/* 得到HPTC_AVRPlayer实例 */
	HPAV_Player* GetHPAV_Player(CWnd* pWnd);

	/* 得到HPTC_AVRPlayer实例映射表 */
	MapHPAV_Player GetHPAV_PlayerMap(void) const;

	/* 得到音视频所用的总带宽 */
	unsigned long GetHPAV_Bandwidth(void);

	/* 设置抓拍路径（绝对路径） */
	void SetCapturePath(std::string strCapturePath);

	/* 设置录像保存路径 */
	void SetRecordPath(std::string strRecordPath, std::string strLocalMemberID);

	/* 设置数据库路径 */
	void SetDBPath(std::string strDBPath);

	/* 改变PlayerID */
	bool ChangePlayerID(const std::string& strOldID, const std::string& memberid, int nCardID);

	/* 应用视频诊断 */
	void AppAVDiagnosis(bool bOpen);

private:
	MapHPAV_Player			m_PlayerMap;
	HPAV_PlayerNotify&		m_rNotify;
	std::string				m_strCapturePath;
	std::string				m_strRecordPath;
	std::string				m_strLocalMemberID;
	std::string				m_strDBPath;
};

HPAV_API HPAV_Manager* CreateHPAV_Manager(HPAV_PlayerNotify& rNotify, unsigned long ulSessionID);
HPAV_API void DestoryHPAV_Manager(void);
HPAV_API HPAV_Manager* GetHPAV_Manager(void);

