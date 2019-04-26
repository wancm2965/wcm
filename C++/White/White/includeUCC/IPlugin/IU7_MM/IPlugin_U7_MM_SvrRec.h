// UCC7-MM������¼������ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_MM_SvrRec_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_SvrRec_Observer(){};

	public:
		/************************************************************************
		* ERRCODE ˵����
		*  ��                           ֵ                       ˵��
		* ERR_REC_SVRREC_BUSY			    70001					¼��������æ����û�п��õ�¼��������
		* ERR_REC_SVR_EXCEPTION				70002					¼���������쳣
		* REC_START_SUCCESS					0						��ʼ¼��
		* REC_START_RECMAXCNT				1						�ﵽ��Ȩ��
		* REC_STREAM_FAILED					2						����ʧ��
		* REC_START_EXIST					3						�Ѿ���¼
		* REC_START_DISKFULL				4						���̿ռ䲻��
		* REC_START_NODISE					5						û�д���
		* REC_START_OTHER					6						��������
		* ����и�����ο�HPRecordDefine.h -> RECERR_CODE����
		************************************************************************/
		virtual void OnRecServerStatus(const char* cszMemberID, int nCardIndex, int nErrorCode) = 0;
	};

	// UCC7-MM������¼������ʵ�ֲ��
	class IPlugin_U7_MM_SvrRec : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_SvrRec(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_SvrRec_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_SvrRec_Observer* pObserver) = 0;

		virtual bool StartChnRec(const char* cszMemberID, int nCardIndex) = 0;
		virtual bool StopChnRec(const char* cszMemberID, int nCardIndex) = 0;
		virtual bool StopAllRec(void) = 0;
	};
}
