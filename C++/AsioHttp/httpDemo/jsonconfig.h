#ifndef JSONCONFIG_H
#define JSONCONFIG_H
#include <string>

class CJsonConfig
{
public:
    CJsonConfig();
    virtual ~CJsonConfig();
public:
    static CJsonConfig& Ins();
    bool Init(const std::string strConfigPath);
    // get content from strConfigPath
    std::string GetContent();
    // get value by strKey
    std::string GetValue(const std::string& strKey, bool bObject);
    // get string value from strContent by strKey
    std::string GetValue(const std::string& strContent, const std::string& strKey, bool bObject);
    // get double value from strContent by strKey
    double GetDoubleValue(const std::string& strContent, const std::string& strKey);
    int GetIntValue(const std::string& strContent, const std::string& strKey);
    int GetArraySize(const std::string& strContent);
    std::string GetArrayItem(const std::string& strContent, int nIndex);
private:
    std::string m_strConfigPath;
};
#endif // JSONCONFIG_H
