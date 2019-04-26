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

	//错误代码
	m_mapErrCode[ERR_NO]						= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NO"),					_T("成功登录"));

	//系统类或网络类错误代码
	m_mapErrCode[ERR_SYS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_PROTOCOL"),		_T("通讯协议版本不匹配"));
	m_mapErrCode[ERR_SYS_UNKNOWN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_UNKNOWN"),		_T("未知错误"));
	m_mapErrCode[ERR_SYS_COMMAND]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_COMMAND"),		_T("协议命令错误"));
	m_mapErrCode[ERR_SYS_DATABASE]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_DATABASE"), 		_T("数据库错误"));
	m_mapErrCode[ERR_SYS_NETWORK]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_NETWORK"),		_T("网络断开(网络通路断开)"));
	m_mapErrCode[ERR_SYS_SERVERTYPE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_SERVERTYPE"),		_T("服务器类型错误, 登录到错误类型的服务器"));

	//即时通讯类错误代码
	m_mapErrCode[ERR_IMS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_PROTOCOL"),			_T("通讯协议版本不匹配"));
	m_mapErrCode[ERR_IMS_INVALIDACCOUNT]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDACCOUNT"),	_T("无效的帐户(系统没有该帐户ID)"));
	m_mapErrCode[ERR_IMS_INVALIDPASSWORD]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDPASSWORD"),_T("无效的密码"));
	m_mapErrCode[ERR_IMS_DUPLICATELOGIN]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_DUPLICATELOGIN"),	_T("该帐户已经登录, 不得重复登录"));
	m_mapErrCode[ERR_IMS_REQUESTRELOGIN]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_REQUESTRELOGIN"),	_T("服务器要求使用获得的新地址重新登录"));
	m_mapErrCode[ERR_IMS_KICKOUT]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_KICKOUT"),		_T("被踢出(该帐户在其它地方已登录, 强制本地帐号退出)"));
	m_mapErrCode[ERR_IMS_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_MODULELIMIT"),	_T("服务器功能限制(服务端即时通讯模块授权没有启用)"));
	m_mapErrCode[ERR_IMS_CONNECTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_CONNECTLIMIT"),	_T("连接服务器到达最大用户数限制"));
	m_mapErrCode[ERR_IMS_ACCOUNTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_ACCOUNTLIMIT"),	_T("用户登录到达最大用户帐号数限制"));
	m_mapErrCode[ERR_IMS_SERVER]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_SERVER"),			_T("用户帐号在该服务器上不存在或该服务器没有加载帐号验证模块"));
	m_mapErrCode[ERR_IMS_SERVERLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_SERVERLIMIT"),	_T("服务器地址限定, 该用户不能在此服务器上登录"));
	m_mapErrCode[ERR_IMS_UNKNOWNAUTHTYPE]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_UNKNOWNAUTHTYPE"),_T("未知的帐户验证模式(NOT:DB/LDAP/RADIUS)"));
	m_mapErrCode[ERR_IMS_FORBIDDEN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_FORBIDDEN"),		_T("该帐户被禁用"));
	m_mapErrCode[ERR_IMS_CLIENTAUTH]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_CLIENTAUTH"),		_T("请求客户端验证"));
	m_mapErrCode[ERR_IMS_SERIALNOERROR]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_SERIALNOERROR"),	_T("序列号绑定错误"));
	m_mapErrCode[ERR_IMS_INVALIDRECORD]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDRECORD"),	_T("非录像服务器帐户错误"));
	m_mapErrCode[ERR_IMS_INVALIDIDENTIFYID]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_IMS_INVALIDIDENTIFYID"),_T("终端识别码错误"));

	//会议通讯类错误代码
	m_mapErrCode[ERR_MMS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_PROTOCOL"),		_T("通讯协议版本不匹配"));
	m_mapErrCode[ERR_MMS_INVALIDPASSWORD]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDPASSWORD"),_T("无效的会议密码"));
	m_mapErrCode[ERR_MMS_ALREADYLOGIN]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_ALREADYLOGIN"),		_T("该帐户已经进入会议"));
	m_mapErrCode[ERR_MMS_WORKGROUPLIMITS]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_WORKGROUPLIMITS"),_T("会议已经超过六人限制"));
	m_mapErrCode[ERR_MMS_NOMCU]					= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_NOMCU"),			_T("会议没有适当的MCU服务器"));
	m_mapErrCode[ERR_MMS_OWNERLEAVED]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_OWNERLEAVED"),	_T("会议所有者离开会议"));
	m_mapErrCode[ERR_MMS_ROOMLOCKED]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_ROOMLOCKED"),		_T("会议已经锁定"));
	m_mapErrCode[ERR_MMS_INVALIDROOMTYPE]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDROOMTYPE"),_T("会议室类型不明确"));
	m_mapErrCode[ERR_MMS_INVALIDMEMBERTYPE]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDMEMBERTYPE"),_T("登录者类型不明确"));
	m_mapErrCode[ERR_MMS_LOSSINVALID]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_LOSSINVALID"),	_T("定损报案号不存在"));
	m_mapErrCode[ERR_MMS_LOSSAGAIN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_LOSSAGAIN"),		_T("定损报案号已被其他人登记"));
	m_mapErrCode[ERR_MMS_LOSSHAVEEND]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_LOSSHAVEEND"),	_T("定损报案号已经定损结束"));
	m_mapErrCode[ERR_MMS_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_MODULELIMIT"),	_T("服务器功能限制(系统会议模块授权没有启用)"));
	m_mapErrCode[ERR_MMS_CONNECTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_CONNECTLIMIT"),	_T("系统到达最大会议成员数限制"));
	m_mapErrCode[ERR_MMS_HAVETV]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_HAVETV"),			_T("电视墙设备号已存在"));
	m_mapErrCode[ERR_MMS_INVALIDTV]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDTV"),		_T("非有效的电视墙设备号"));
	m_mapErrCode[ERR_MMS_HAVETVMATRIX]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_HAVETVMATRIX"),	_T("电视墙矩阵设备号已存在"));
	m_mapErrCode[ERR_MMS_INVALIDTVMATRIX]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVALIDTVMATRIX"),_T("非有效的电视墙矩阵设备号"));
	m_mapErrCode[ERR_MMS_SERVER]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_SERVER"),			_T("会议服务器断开连接"));
	m_mapErrCode[ERR_MMS_AUDITORLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_AUDITORLIMIT"),	_T("会议旁听者最大数量限制"));
	m_mapErrCode[ERR_MMS_DUPLICATEMPS]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_DUPLICATEMPS"),	_T("已经存在相同类型的MPS"));
	m_mapErrCode[ERR_MMS_UNAVAILABLE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_UNAVAILABLE"),	_T("不符合邀请MPS条件"));
	m_mapErrCode[ERR_MMS_NOAVAILABLEMPS]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_NOAVAILABLEMPS"),	_T("没有空闲的MPS"));
	m_mapErrCode[ERR_MMS_APPLYMEMORYFAILED]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_APPLYMEMORYFAILED"),	_T("申请内存失败"));
	m_mapErrCode[ERR_MMS_INVITEMPSSUCCESS]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MMS_INVITEMPSSUCCESS"),	_T("邀请MPS成功"));

	//短信类错误代码
	m_mapErrCode[ERR_SMS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_PROTOCOL"),		_T("通讯协议版本不匹配"));
	m_mapErrCode[ERR_SMS_GATEWAY]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_GATEWAY"),		_T("短信网关发送失败"));
	m_mapErrCode[ERR_SMS_INVALIDSEARIALNO]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_INVALIDSEARIALNO"),_T("序列号码为空或无效"));
	m_mapErrCode[ERR_SMS_NETWORK]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_NETWORK"),		_T("连接到短信网关发生网络故障"));
	m_mapErrCode[ERR_SMS_OTHERS]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_OTHERS"),			_T("连接到短信网关发生其他故障"));
	m_mapErrCode[ERR_SMS_ENTRYLIMITS]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_ENTRYLIMITS"),	_T("手机号码为空或者超过1000个"));
	m_mapErrCode[ERR_SMS_SPLITCHAR]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_SPLITCHAR"),		_T("手机号码分割符号不正确"));
	m_mapErrCode[ERR_SMS_ERRORNUMBER]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_ERRORNUMBER"),	_T("部分手机号码不正确，已删除，其余手机号码被发送"));
	m_mapErrCode[ERR_SMS_CONTENTOVERFLOW]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_CONTENTOVERFLOW"),_T("短信内容为空或超长(70个汉字)"));
	m_mapErrCode[ERR_SMS_BILLING]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_BILLING"),		_T("短信网关计费失败"));
	m_mapErrCode[ERR_SMS_UNKNOWN]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_UNKNOWN"),		_T("未知错误"));
	m_mapErrCode[ERR_SMS_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_MODULELIMIT"),	_T("服务器功能限制(系统短信模块授权没有启用)"));
	m_mapErrCode[ERR_SMS_INVALIDTYPE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_INVALIDTYPE"),	_T("短信类型未定义"));
	m_mapErrCode[ERR_SMS_EMPTYCODE]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SMS_EMPTYCODE"),		_T("发送号或接收号为空"));

	//监控类类错误代码
	m_mapErrCode[ERR_MON_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_PROTOCOL"),		_T("通讯协议版本不匹配"));
	m_mapErrCode[ERR_MON_REUSESERIAL]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_REUSESERIAL"),	_T("序列号已被其他帐户绑定"));
	m_mapErrCode[ERR_MON_INVALIDACCOUNT]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDACCOUNT"),	_T("无效的监控设备(或网关)帐户(系统没有该帐户ID)"));
	m_mapErrCode[ERR_MON_INVALIDPASSWORD]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDPASSWORD"),_T("无效的密码"));
	m_mapErrCode[ERR_MON_INVALIDSERIAL]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDSERIAL"),	_T("无效的序列号"));
	m_mapErrCode[ERR_MON_ALREADYLOGIN]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_ALREADYLOGIN"),	_T("已经登录"));
	m_mapErrCode[ERR_MON_INVALIDTYPE]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_INVALIDTYPE"),	_T("非法的终端类型"));
	m_mapErrCode[ERR_MON_MODULELIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_MODULELIMIT"),	_T("服务器功能限制(系统监控模块授权没有启用)"));
	m_mapErrCode[ERR_MON_CONNECTLIMIT]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_MON_CONNECTLIMIT"),	_T("系统到达最大通道数限制"));

	//离线文件类错误代码
	m_mapErrCode[ERR_NDS_PROTOCOL]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_PROTOCOL"),		_T("通讯协议版本不匹配"));
	m_mapErrCode[ERR_NDS_MCUDISKFULL]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_MCUDISKFULL"),	_T("MCU磁盘满"));
	m_mapErrCode[ERR_NDS_RECEIVERSPACEFULL]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_RECEIVERSPACEFULL"),_T("接收者空间满"));
	m_mapErrCode[ERR_NDS_MD5]					= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_MD5"),			_T("MD5验证失败"));
	m_mapErrCode[ERR_NDS_FILENOTEXIST]			= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_FILENOTEXIST"),	_T("文件不存在"));
	m_mapErrCode[ERR_NDS_FILEISUPLOADING]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_FILEISUPLOADING"),_T("文件正在上传中"));
	m_mapErrCode[ERR_NDS_FILEISDOWNLOADING]		= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_FILEISDOWNLOADING"),_T("文件正在下载中"));
	m_mapErrCode[ERR_NDS_NETERROR]				= GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_NDS_NETERROR"),		_T("网络错误"));
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
			return GetPluginU7Lang()->GetCommLangString(_T("ERROR_CODE"), _T("ERR_SYS_UNKNOWN"), _T("未知错误"));
		}
		else
		{
			return _T("");
		}
	}

	return it->second;
}
