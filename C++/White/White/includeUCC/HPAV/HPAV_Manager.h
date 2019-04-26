#pragma once

#include "HPAV_Player.h"


#include <map>
typedef std::map<std::string,HPAV_Player*>	MapHPAV_Player;
typedef MapHPAV_Player::iterator MapHPAV_Player_IT;


class HPAV_API HPAV_Manager
{
public:
	/*	�������ܣ����캯��
		��    ����	rNotify��		�ӿ�����
					ulSessionID��	���
		�� �� ֵ��*/
	HPAV_Manager(HPAV_PlayerNotify& rNotify, unsigned long ulSessionID = 0);
	virtual~ HPAV_Manager(void);

	void Destroy();

	/* ����HPTC_AVRPlayerʵ�� */
	HPAV_Player* CreateHPAV_Player(CWnd*pParent, const std::string& memberid, int nCardID);

	/*	�������ܣ�ǿ�ƹر�ָ���Ի���������Դ
		��    ����
		�� �� ֵ��*/
	void DestroyHPAV_Player(CWnd* pWindow);

	/* ����HPTC_AVRPlayerʵ�� */
	void DestroyHPAV_Player(const std::string& memberid, int nCardID);

	/* ����HPTC_AVRPlayerʵ�� playerid=memberid_did*/
	void DestroyHPAV_Player(const std::string& playerid);

	/* ��������HPTC_AVRPlayerʵ�� */
	void DestroyHPAV_Player(void);

	/* �õ�HPTC_AVRPlayerʵ�� */
	HPAV_Player* GetHPAV_Player(const std::string& memberid, int nCardID);

	/* �õ�HPTC_AVRPlayerʵ�� */
	HPAV_Player* GetHPAV_Player(const std::string& playerid);

	/* �õ�HPTC_AVRPlayerʵ�� */
	HPAV_Player* GetHPAV_Player(CWnd* pWnd);

	/* �õ�HPTC_AVRPlayerʵ��ӳ��� */
	MapHPAV_Player GetHPAV_PlayerMap(void) const;

	/* �õ�����Ƶ���õ��ܴ��� */
	unsigned long GetHPAV_Bandwidth(void);

	/* ����ץ��·��������·���� */
	void SetCapturePath(std::string strCapturePath);

	/* ����¼�񱣴�·�� */
	void SetRecordPath(std::string strRecordPath, std::string strLocalMemberID);

	/* �������ݿ�·�� */
	void SetDBPath(std::string strDBPath);

	/* �ı�PlayerID */
	bool ChangePlayerID(const std::string& strOldID, const std::string& memberid, int nCardID);

	/* Ӧ����Ƶ��� */
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

