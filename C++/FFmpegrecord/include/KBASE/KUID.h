#pragma once

#include "KBASE/KBASE_Export.h"
#include <string>
#include "Utils.h"

//=======================================================================================
// KUID定义
class KBASE_API KUID
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
// KDomain定义
class KBASE_API KDomain
{
public:
	KDomain(const std::string& strLocalDomain);
	~KDomain();

	std::string GetParentDomain(void);
	int GetParentDomainLevel(void);
	std::string GetLocalDomain(void);
	int GetLocalDomainLevel(void);
	STRING_MAP& GetFullDomain(void);
	void CompareDomainLevel(const std::string strDomain,int& nLevel); //以自己为参照　0:同级 1:上级 -1:下级 

private:
	std::string m_strParentDomain;
	std::string m_strLocalDomain;
	int m_nParentDomainLevel;
	int m_nLocalDomainLevel;
	STRING_MAP m_mapFullDomain;
};

//=======================================================================================
KBASE_API std::string GetUserDomain(const std::string& strUserID);
KBASE_API std::string GetLowestCommonParentDomain(const std::string& strDomain1,const std::string& strDomain2);
KBASE_API std::string GetUserResource(const std::string& strUserID);
KBASE_API std::string GetUserIDWithDomain(const std::string& strUserID,const std::string& strDomain,const std::string& strResource);
KBASE_API int GetDomainLevel(const std::string strDomain);
KBASE_API void SplitThreeDomain(const std::string strDomainFull,std::string& strDomain1,std::string& strDomain2,std::string& strDomain3);