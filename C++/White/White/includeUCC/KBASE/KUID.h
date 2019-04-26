#pragma once

#include <string>
#include "Utils.h"

//=======================================================================================
// KUID����
class AFX_EXT_CLASS KUID
{
public:
	KUID(const std::string& strUID);
	KUID(const std::string& strUserID,const std::string& strDomain,const std::string& strResource);
	~KUID();

	std::string GetUserID(void);
	std::string GetDomain(void);
	std::string GetResource(void);
	std::string GetUID(void);

private:
	std::string m_strUserID;
	std::string m_strDomain;
	std::string m_strUID;
	std::string m_strResource;
};

//=======================================================================================
// KDomain����
class AFX_EXT_CLASS KDomain
{
public:
	KDomain(const std::string& strLocalDomain);
	~KDomain();

	std::string GetParentDomain(void);
	int GetParentDomainLevel(void);
	std::string GetLocalDomain(void);
	int GetLocalDomainLevel(void);
	STRING_MAP& GetFullDomain(void);
	void CompareDomainLevel(const std::string strDomain,int& nLevel); //���Լ�Ϊ���ա�0:ͬ�� 1:�ϼ� -1:�¼� 

private:
	std::string m_strParentDomain;
	std::string m_strLocalDomain;
	int m_nParentDomainLevel;
	int m_nLocalDomainLevel;
	STRING_MAP m_mapFullDomain;
};

//=======================================================================================
AFX_EXT_CLASS std::string GetUserDomain(const std::string& strUserID);
AFX_EXT_CLASS std::string GetUserIDWithDomain(const std::string& strUserID,const std::string& strDomain,const std::string& strResource);
AFX_EXT_CLASS int GetDomainLevel(const std::string strDomain);
AFX_EXT_CLASS void SplitThreeDomain(const std::string strDomainFull,std::string& strDomain1,std::string& strDomain2,std::string& strDomain3);