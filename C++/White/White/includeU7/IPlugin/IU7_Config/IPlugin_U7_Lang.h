// UCC7-���԰���д���ܲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7���԰���д����
	class IPlugin_U7_Lang : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_Lang(){};

	public:
		// ��ȡ��ǰ����
		virtual CString GetCurLang(void) = 0;
		// ��ȡͨ�����԰��ַ���������Ƥ����ʹ��:-T,-C��
		virtual CString GetCommLangString(CString strSection, CString strKey, CString strDefValue) = 0;
		// ��ȡ����Դ����԰��ַ�����ֻ��һ��Ƥ����ʹ��:-D��
		virtual CString GetCommLangStringD(CString strSection, CString strKey, CString strDefValue) = 0;
		// ��ȡ����Դ����԰��ַ���������Ƥ����ʹ��:-T,-C��
		virtual CString GetPluginsLangString(CString strPluginName, CString strKey, CString strDefValue) = 0;
		// ��ȡ����Դ����԰��ַ�����ֻ��һ��Ƥ����ʹ��:-D��
		virtual CString GetPluginsLangStringD(CString strPluginName, CString strKey, CString strDefValue) = 0;
		// ��ȡ������ʾ
		virtual CString GetErrCodeStr(unsigned long ulErrorCode) = 0;
	};
}
