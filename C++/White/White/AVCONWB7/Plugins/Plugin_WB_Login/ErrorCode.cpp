#include "StdAfx.h"
#include "./ErrorCode.h"
#include "RWIniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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
	m_mapErrCode.clear();

	//�������
	m_mapErrCode[ERR_NO]						= _T(""); // CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NO"),					_T("��ȷ"));

	//ϵͳ���������������
	m_mapErrCode[ERR_SYS_PROTOCOL]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SYS_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_SYS_UNKNOWN]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SYS_UNKNOWN"),			_T("δ֪����"));
	m_mapErrCode[ERR_SYS_COMMAND]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SYS_COMMAND"),			_T("Э���������"));
	m_mapErrCode[ERR_SYS_DATABASE]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SYS_DATABASE"), 		_T("���ݿ����"));
	m_mapErrCode[ERR_SYS_NETWORK]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SYS_NETWORK"),			_T("����Ͽ�(����ͨ·�Ͽ�)"));
	m_mapErrCode[ERR_SYS_SERVERTYPE]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SYS_SERVERTYPE"),		_T("���������ʹ���, ��¼���������͵ķ�����"));

	//��ʱͨѶ��������
	m_mapErrCode[ERR_IMS_PROTOCOL]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_IMS_INVALIDACCOUNT]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDACCOUNT"),	_T("��Ч���˻�(ϵͳû�и��˻�ID)"));
	m_mapErrCode[ERR_IMS_INVALIDPASSWORD]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDPASSWORD"),	_T("��Ч������"));
	m_mapErrCode[ERR_IMS_DUPLICATELOGIN]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_DUPLICATELOGIN"),	_T("���˻��Ѿ���¼, �����ظ���¼"));
	m_mapErrCode[ERR_IMS_REQUESTRELOGIN]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_REQUESTRELOGIN"),	_T("������Ҫ��ʹ�û�õ��µ�ַ���µ�¼"));
	m_mapErrCode[ERR_IMS_KICKOUT]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_KICKOUT"),			_T("���߳�(���˻��������ط��ѵ�¼, ǿ�Ʊ����ʺ��˳�)"));
	m_mapErrCode[ERR_IMS_MODULELIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_MODULELIMIT"),		_T("��������������(����˼�ʱͨѶģ����Ȩû������)"));
	m_mapErrCode[ERR_IMS_CONNECTLIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_CONNECTLIMIT"),		_T("���ӷ�������������û�������"));
	m_mapErrCode[ERR_IMS_ACCOUNTLIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_ACCOUNTLIMIT"),		_T("�û���¼��������û��ʺ�������"));
	m_mapErrCode[ERR_IMS_SERVER]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_SERVER"),			_T("�û��ʺ��ڸ÷������ϲ����ڻ�÷�����û�м����ʺ���֤ģ��"));
	m_mapErrCode[ERR_IMS_SERVERLIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_SERVERLIMIT"),		_T("��������ַ�޶�, ���û������ڴ˷������ϵ�¼"));
	m_mapErrCode[ERR_IMS_UNKNOWNAUTHTYPE]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_UNKNOWNAUTHTYPE"),	_T("δ֪���ʻ���֤ģʽ(NOT:DB/LDAP/RADIUS)"));
	m_mapErrCode[ERR_IMS_FORBIDDEN]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_FORBIDDEN"),		_T("���˻�������"));
	m_mapErrCode[ERR_IMS_CLIENTAUTH]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_CLIENTAUTH"),		_T("����ͻ�����֤"));
	m_mapErrCode[ERR_IMS_SERIALNOERROR]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_SERIALNOERROR"),	_T("���кŰ󶨴���"));
	m_mapErrCode[ERR_IMS_INVALIDRECORD]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDRECORD"),	_T("��¼��������ʻ�����"));
	m_mapErrCode[ERR_IMS_ADDDISCUSSMEMBER]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_ADDDISCUSSMEMBER"),	_T("����������Ա����"));
	m_mapErrCode[ERR_IMS_WEBSERVICEDISCONNECT]	= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_WEBSERVICEDISCONNECT"),	_T("WebService����ʧ��"));
	m_mapErrCode[ERR_IMS_OTHERSERVERDISCONNECT]	= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_IMS_OTHERSERVERDISCONNECT"),	_T("����������������ʧ��"));

	//����ͨѶ��������
	m_mapErrCode[ERR_MMS_PROTOCOL]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_MMS_INVALIDPASSWORD]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDPASSWORD"),	_T("��Ч�Ļ�������"));
	m_mapErrCode[ERR_MMS_ALREADYLOGIN]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_ALREADYLOGIN"),		_T("���˻��Ѿ��������"));
	m_mapErrCode[ERR_MMS_WORKGROUPLIMITS]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_WORKGROUPLIMITS"),	_T("�����Ѿ�������������"));
	m_mapErrCode[ERR_MMS_NOMCU]					= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_NOMCU"),			_T("����û���ʵ���MCU������"));
	m_mapErrCode[ERR_MMS_OWNERLEAVED]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_OWNERLEAVED"),		_T("�����������뿪����"));
	m_mapErrCode[ERR_MMS_ROOMLOCKED]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_ROOMLOCKED"),		_T("�����Ѿ�����"));
	m_mapErrCode[ERR_MMS_INVALIDROOMTYPE]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDROOMTYPE"),	_T("���������Ͳ���ȷ"));
	m_mapErrCode[ERR_MMS_INVALIDMEMBERTYPE]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDMEMBERTYPE"),_T("��¼�����Ͳ���ȷ"));
	m_mapErrCode[ERR_MMS_LOSSINVALID]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_LOSSINVALID"),		_T("���𱨰��Ų�����"));
	m_mapErrCode[ERR_MMS_LOSSAGAIN]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_LOSSAGAIN"),		_T("���𱨰����ѱ������˵Ǽ�"));
	m_mapErrCode[ERR_MMS_LOSSHAVEEND]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_LOSSHAVEEND"),		_T("���𱨰����Ѿ��������"));
	m_mapErrCode[ERR_MMS_MODULELIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_MODULELIMIT"),		_T("��������������(ϵͳ����ģ����Ȩû������)"));
	m_mapErrCode[ERR_MMS_CONNECTLIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_CONNECTLIMIT"),		_T("ϵͳ�����������Ա������"));
	m_mapErrCode[ERR_MMS_HAVETV]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_HAVETV"),			_T("����ǽ�豸���Ѵ���"));
	m_mapErrCode[ERR_MMS_INVALIDTV]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDTV"),		_T("����Ч�ĵ���ǽ�豸��"));
	m_mapErrCode[ERR_MMS_HAVETVMATRIX]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_HAVETVMATRIX"),		_T("����ǽ�����豸���Ѵ���"));
	m_mapErrCode[ERR_MMS_INVALIDTVMATRIX]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDTVMATRIX"),	_T("����Ч�ĵ���ǽ�����豸��"));
	m_mapErrCode[ERR_MMS_SERVER]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_SERVER"),			_T("����������Ͽ�����"));
	m_mapErrCode[ERR_MMS_AUDITORLIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MMS_AUDITORLIMIT"),		_T("���������������������"));

	//������������
	m_mapErrCode[ERR_SMS_PROTOCOL]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_SMS_GATEWAY]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_GATEWAY"),			_T("�������ط���ʧ��"));
	m_mapErrCode[ERR_SMS_INVALIDSEARIALNO]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_INVALIDSEARIALNO"),	_T("���к���Ϊ�ջ���Ч"));
	m_mapErrCode[ERR_SMS_NETWORK]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_NETWORK"),			_T("���ӵ��������ط����������"));
	m_mapErrCode[ERR_SMS_OTHERS]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_OTHERS"),			_T("���ӵ��������ط�����������"));
	m_mapErrCode[ERR_SMS_ENTRYLIMITS]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_ENTRYLIMITS"),		_T("�ֻ�����Ϊ�ջ��߳���1000��"));
	m_mapErrCode[ERR_SMS_SPLITCHAR]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_SPLITCHAR"),		_T("�ֻ�����ָ���Ų���ȷ"));
	m_mapErrCode[ERR_SMS_ERRORNUMBER]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_ERRORNUMBER"),		_T("�����ֻ����벻��ȷ����ɾ���������ֻ����뱻����"));
	m_mapErrCode[ERR_SMS_CONTENTOVERFLOW]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_CONTENTOVERFLOW"),	_T("��������Ϊ�ջ򳬳�(70������)"));
	m_mapErrCode[ERR_SMS_BILLING]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_BILLING"),			_T("�������ؼƷ�ʧ��"));
	m_mapErrCode[ERR_SMS_UNKNOWN]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_UNKNOWN"),			_T("δ֪����"));
	m_mapErrCode[ERR_SMS_MODULELIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_MODULELIMIT"),		_T("��������������(ϵͳ����ģ����Ȩû������)"));
	m_mapErrCode[ERR_SMS_INVALIDTYPE]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_INVALIDTYPE"),		_T("��������δ����"));
	m_mapErrCode[ERR_SMS_EMPTYCODE]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_SMS_EMPTYCODE"),		_T("���ͺŻ���պ�Ϊ��"));

	//�������������
	m_mapErrCode[ERR_MON_PROTOCOL]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_MON_REUSESERIAL]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_REUSESERIAL"),		_T("���к��ѱ������ʻ���"));
	m_mapErrCode[ERR_MON_INVALIDACCOUNT]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_INVALIDACCOUNT"),	_T("��Ч�ļ���豸(������)�˻�(ϵͳû�и��˻�ID)"));
	m_mapErrCode[ERR_MON_INVALIDPASSWORD]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_INVALIDPASSWORD"),	_T("��Ч������"));
	m_mapErrCode[ERR_MON_INVALIDSERIAL]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_INVALIDSERIAL"),	_T("��Ч�����к�"));
	m_mapErrCode[ERR_MON_ALREADYLOGIN]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_ALREADYLOGIN"),		_T("�Ѿ���¼"));
	m_mapErrCode[ERR_MON_INVALIDTYPE]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_INVALIDTYPE"),		_T("�Ƿ����ն�����"));
	m_mapErrCode[ERR_MON_MODULELIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_MODULELIMIT"),		_T("��������������(ϵͳ���ģ����Ȩû������)"));
	m_mapErrCode[ERR_MON_CONNECTLIMIT]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_MON_CONNECTLIMIT"),		_T("ϵͳ�������ͨ��������"));

	//�����ļ���������
	m_mapErrCode[ERR_NDS_PROTOCOL]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_PROTOCOL"),			_T("ͨѶЭ��汾��ƥ��"));
	m_mapErrCode[ERR_NDS_MCUDISKFULL]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_MCUDISKFULL"),		_T("MCU������"));
	m_mapErrCode[ERR_NDS_RECEIVERSPACEFULL]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_RECEIVERSPACEFULL"),_T("�����߿ռ���"));
	m_mapErrCode[ERR_NDS_MD5]					= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_MD5"),				_T("MD5��֤ʧ��"));
	m_mapErrCode[ERR_NDS_FILENOTEXIST]			= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_FILENOTEXIST"),		_T("�ļ�������"));
	m_mapErrCode[ERR_NDS_FILEISUPLOADING]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_FILEISUPLOADING"),	_T("�ļ������ϴ���"));
	m_mapErrCode[ERR_NDS_FILEISDOWNLOADING]		= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_FILEISDOWNLOADING"),_T("�ļ�����������"));
	m_mapErrCode[ERR_NDS_NETERROR]				= CRWIniFile::ReadLangStr(_T("ERROR_CODE"), _T("ERR_NDS_NETERROR"),			_T("�������"));
	
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
		return _T("");
	}

	return it->second;
}
