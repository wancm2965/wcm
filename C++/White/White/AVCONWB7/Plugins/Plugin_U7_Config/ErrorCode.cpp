#include "StdAfx.h"
#include "ErrorCode.h"
#include "AVCONSys/KXNCP.h"
#include "Plugin_U7_Lang.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CErrorCode g_ErrorCode;
CErrorCode& CErrorCode::Instance(void)
{
	return g_ErrorCode;
}

CErrorCode::CErrorCode(void)
{
}

CErrorCode::~CErrorCode(void)
{
}

void CErrorCode::InitErrorCode(void)
{
	if (NULL == GetPluginU7Lang())
		return;

	m_mapErrCode.clear();

	//�������
	m_mapErrCode[ERR_NO]						= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NO"),					_T("�ɹ���¼"));

	//ϵͳ���������������
	m_mapErrCode[ERR_SYS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_PROTOCOL"),		_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_SYS_UNKNOWN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_UNKNOWN"),		_T("δ֪����"));
	m_mapErrCode[ERR_SYS_COMMAND]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_COMMAND"),		_T("Э���������"));
	m_mapErrCode[ERR_SYS_DATABASE]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_DATABASE"), 		_T("���ݿ����"));
	m_mapErrCode[ERR_SYS_NETWORK]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_NETWORK"),		_T("����Ͽ�(����ͨ·�Ͽ�)"));
	m_mapErrCode[ERR_SYS_SERVERTYPE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_SERVERTYPE"),		_T("���������ʹ���, ��¼���������͵ķ�����"));

	//��ʱͨѶ��������
	m_mapErrCode[ERR_IMS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_IMS_INVALIDACCOUNT]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDACCOUNT"),	_T("��Ч���ʻ�(ϵͳû�и��ʻ�ID)"));
	m_mapErrCode[ERR_IMS_INVALIDPASSWORD]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDPASSWORD"),_T("��Ч������"));
	m_mapErrCode[ERR_IMS_DUPLICATELOGIN]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_DUPLICATELOGIN"),	_T("���ʻ��Ѿ���¼, �����ظ���¼"));
	m_mapErrCode[ERR_IMS_REQUESTRELOGIN]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_REQUESTRELOGIN"),	_T("������Ҫ��ʹ�û�õ��µ�ַ���µ�¼"));
	m_mapErrCode[ERR_IMS_KICKOUT]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_KICKOUT"),		_T("���߳�(���ʻ��������ط��ѵ�¼, ǿ�Ʊ����ʺ��˳�)"));
	m_mapErrCode[ERR_IMS_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_MODULELIMIT"),	_T("��������������(����˼�ʱͨѶģ����Ȩû������)"));
	m_mapErrCode[ERR_IMS_CONNECTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_CONNECTLIMIT"),	_T("���ӷ�������������û�������"));
	m_mapErrCode[ERR_IMS_ACCOUNTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_ACCOUNTLIMIT"),	_T("�û���¼��������û��ʺ�������"));
	m_mapErrCode[ERR_IMS_SERVER]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_SERVER"),			_T("�û��ʺ��ڸ÷������ϲ����ڻ�÷�����û�м����ʺ���֤ģ��"));
	m_mapErrCode[ERR_IMS_SERVERLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_SERVERLIMIT"),	_T("��������ַ�޶�, ���û������ڴ˷������ϵ�¼"));
	m_mapErrCode[ERR_IMS_UNKNOWNAUTHTYPE]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_UNKNOWNAUTHTYPE"),_T("δ֪���ʻ���֤ģʽ(NOT:DB/LDAP/RADIUS)"));
	m_mapErrCode[ERR_IMS_FORBIDDEN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_FORBIDDEN"),		_T("���ʻ�������"));
	m_mapErrCode[ERR_IMS_CLIENTAUTH]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_CLIENTAUTH"),		_T("����ͻ�����֤"));
	m_mapErrCode[ERR_IMS_SERIALNOERROR]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_SERIALNOERROR"),	_T("���кŰ󶨴���"));
	m_mapErrCode[ERR_IMS_INVALIDRECORD]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDRECORD"),	_T("��¼��������ʻ�����"));
	m_mapErrCode[ERR_IMS_INVALIDIDENTIFYID]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDIDENTIFYID"),_T("�ն�ʶ�������"));

	//����ͨѶ��������
	m_mapErrCode[ERR_MMS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_PROTOCOL"),		_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_MMS_INVALIDPASSWORD]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDPASSWORD"),_T("��Ч�Ļ�������"));
	m_mapErrCode[ERR_MMS_ALREADYLOGIN]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_ALREADYLOGIN"),		_T("���ʻ��Ѿ��������"));
	m_mapErrCode[ERR_MMS_WORKGROUPLIMITS]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_WORKGROUPLIMITS"),_T("�����Ѿ�������������"));
	m_mapErrCode[ERR_MMS_NOMCU]					= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_NOMCU"),			_T("����û���ʵ���MCU������"));
	m_mapErrCode[ERR_MMS_OWNERLEAVED]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_OWNERLEAVED"),	_T("�����������뿪����"));
	m_mapErrCode[ERR_MMS_ROOMLOCKED]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_ROOMLOCKED"),		_T("�����Ѿ�����"));
	m_mapErrCode[ERR_MMS_INVALIDROOMTYPE]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDROOMTYPE"),_T("���������Ͳ���ȷ"));
	m_mapErrCode[ERR_MMS_INVALIDMEMBERTYPE]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDMEMBERTYPE"),_T("��¼�����Ͳ���ȷ"));
	m_mapErrCode[ERR_MMS_LOSSINVALID]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_LOSSINVALID"),	_T("���𱨰��Ų�����"));
	m_mapErrCode[ERR_MMS_LOSSAGAIN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_LOSSAGAIN"),		_T("���𱨰����ѱ������˵Ǽ�"));
	m_mapErrCode[ERR_MMS_LOSSHAVEEND]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_LOSSHAVEEND"),	_T("���𱨰����Ѿ��������"));
	m_mapErrCode[ERR_MMS_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_MODULELIMIT"),	_T("��������������(ϵͳ����ģ����Ȩû������)"));
	m_mapErrCode[ERR_MMS_CONNECTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_CONNECTLIMIT"),	_T("ϵͳ�����������Ա������"));
	m_mapErrCode[ERR_MMS_HAVETV]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_HAVETV"),			_T("����ǽ�豸���Ѵ���"));
	m_mapErrCode[ERR_MMS_INVALIDTV]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDTV"),		_T("����Ч�ĵ���ǽ�豸��"));
	m_mapErrCode[ERR_MMS_HAVETVMATRIX]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_HAVETVMATRIX"),	_T("����ǽ�����豸���Ѵ���"));
	m_mapErrCode[ERR_MMS_INVALIDTVMATRIX]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDTVMATRIX"),_T("����Ч�ĵ���ǽ�����豸��"));
	m_mapErrCode[ERR_MMS_SERVER]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_SERVER"),			_T("����������Ͽ�����"));
	m_mapErrCode[ERR_MMS_AUDITORLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_AUDITORLIMIT"),	_T("���������������������"));
	m_mapErrCode[ERR_MMS_DUPLICATEMPS]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_DUPLICATEMPS"),	_T("�Ѿ�������ͬ���͵�MPS"));
	m_mapErrCode[ERR_MMS_UNAVAILABLE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_UNAVAILABLE"),	_T("����������MPS����"));
	m_mapErrCode[ERR_MMS_NOAVAILABLEMPS]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_NOAVAILABLEMPS"),	_T("û�п��е�MPS"));
	m_mapErrCode[ERR_MMS_APPLYMEMORYFAILED]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_APPLYMEMORYFAILED"),	_T("�����ڴ�ʧ��"));
	m_mapErrCode[ERR_MMS_INVITEMPSSUCCESS]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVITEMPSSUCCESS"),	_T("����MPS�ɹ�"));

	//������������
	m_mapErrCode[ERR_SMS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_PROTOCOL"),		_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_SMS_GATEWAY]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_GATEWAY"),		_T("�������ط���ʧ��"));
	m_mapErrCode[ERR_SMS_INVALIDSEARIALNO]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_INVALIDSEARIALNO"),_T("���к���Ϊ�ջ���Ч"));
	m_mapErrCode[ERR_SMS_NETWORK]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_NETWORK"),		_T("���ӵ��������ط����������"));
	m_mapErrCode[ERR_SMS_OTHERS]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_OTHERS"),			_T("���ӵ��������ط�����������"));
	m_mapErrCode[ERR_SMS_ENTRYLIMITS]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_ENTRYLIMITS"),	_T("�ֻ�����Ϊ�ջ��߳���1000��"));
	m_mapErrCode[ERR_SMS_SPLITCHAR]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_SPLITCHAR"),		_T("�ֻ�����ָ���Ų���ȷ"));
	m_mapErrCode[ERR_SMS_ERRORNUMBER]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_ERRORNUMBER"),	_T("�����ֻ����벻��ȷ����ɾ���������ֻ����뱻����"));
	m_mapErrCode[ERR_SMS_CONTENTOVERFLOW]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_CONTENTOVERFLOW"),_T("��������Ϊ�ջ򳬳�(70������)"));
	m_mapErrCode[ERR_SMS_BILLING]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_BILLING"),		_T("�������ؼƷ�ʧ��"));
	m_mapErrCode[ERR_SMS_UNKNOWN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_UNKNOWN"),		_T("δ֪����"));
	m_mapErrCode[ERR_SMS_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_MODULELIMIT"),	_T("��������������(ϵͳ����ģ����Ȩû������)"));
	m_mapErrCode[ERR_SMS_INVALIDTYPE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_INVALIDTYPE"),	_T("��������δ����"));
	m_mapErrCode[ERR_SMS_EMPTYCODE]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_EMPTYCODE"),		_T("���ͺŻ���պ�Ϊ��"));

	//�������������
	m_mapErrCode[ERR_MON_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_PROTOCOL"),		_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_MON_REUSESERIAL]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_REUSESERIAL"),	_T("���к��ѱ������ʻ���"));
	m_mapErrCode[ERR_MON_INVALIDACCOUNT]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDACCOUNT"),	_T("��Ч�ļ���豸(������)�ʻ�(ϵͳû�и��ʻ�ID)"));
	m_mapErrCode[ERR_MON_INVALIDPASSWORD]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDPASSWORD"),_T("��Ч������"));
	m_mapErrCode[ERR_MON_INVALIDSERIAL]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDSERIAL"),	_T("��Ч�����к�"));
	m_mapErrCode[ERR_MON_ALREADYLOGIN]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_ALREADYLOGIN"),	_T("�Ѿ���¼"));
	m_mapErrCode[ERR_MON_INVALIDTYPE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDTYPE"),	_T("�Ƿ����ն�����"));
	m_mapErrCode[ERR_MON_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_MODULELIMIT"),	_T("��������������(ϵͳ���ģ����Ȩû������)"));
	m_mapErrCode[ERR_MON_CONNECTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_CONNECTLIMIT"),	_T("ϵͳ�������ͨ��������"));

	//�����ļ���������
	m_mapErrCode[ERR_NDS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_PROTOCOL"),		_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_NDS_MCUDISKFULL]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_MCUDISKFULL"),	_T("MCU������"));
	m_mapErrCode[ERR_NDS_RECEIVERSPACEFULL]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_RECEIVERSPACEFULL"),_T("�����߿ռ���"));
	m_mapErrCode[ERR_NDS_MD5]					= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_MD5"),			_T("MD5��֤ʧ��"));
	m_mapErrCode[ERR_NDS_FILENOTEXIST]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_FILENOTEXIST"),	_T("�ļ�������"));
	m_mapErrCode[ERR_NDS_FILEISUPLOADING]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_FILEISUPLOADING"),_T("�ļ������ϴ���"));
	m_mapErrCode[ERR_NDS_FILEISDOWNLOADING]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_FILEISDOWNLOADING"),_T("�ļ�����������"));
	m_mapErrCode[ERR_NDS_NETERROR]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_NETERROR"),		_T("�������"));
}

void CErrorCode::Release(void)
{
	m_mapErrCode.clear();
}

CString CErrorCode::GetErrCodeStr(unsigned long ulErrorCode)
{
	std::map<unsigned long,CString>::iterator it = m_mapErrCode.find(ulErrorCode);
	if (it == m_mapErrCode.end())
	{
		if (ulErrorCode != 0)
		{
			return GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_UNKNOWN"), _T("δ֪����"));
		}
		else
		{
			return _T("");
		}
	}

	return it->second;
}
