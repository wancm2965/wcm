#include "jsonconfig.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
CJsonConfig::CJsonConfig()
{

}

CJsonConfig::~CJsonConfig()
{

}

CJsonConfig& CJsonConfig::Ins()
{
    static CJsonConfig config;
    return config;
}

std::string CJsonConfig::GetContent()
{
    std::string strContent("");
    if (!m_strConfigPath.empty())
    {
        FILE* configFile = fopen(m_strConfigPath.c_str(), "r");
        if (configFile)
        {
            fseek(configFile, 0, SEEK_END);
            int size = ftell(configFile);
            char* chConfigContent = (char *)calloc(size + 1, sizeof(char));
            if (chConfigContent)
            {
                fseek(configFile, 0, SEEK_SET);
                fread(chConfigContent, 1, size, configFile);
                strContent = chConfigContent;

                if (chConfigContent)
                {
                    free(chConfigContent);
                }
                if (configFile)
                {
                    fclose(configFile);
                }
            }
        }
    }

    return strContent;
}

std::string CJsonConfig::GetValue(const std::string& strKey, bool bObject)
{
    std::string strValue("");
    if(!strKey.empty())
    {
        std::string strContent = GetContent();
        if (!strContent.empty())
        {
            strValue = GetValue(strContent, strKey, bObject);
        }
    }

    return strValue;
}

std::string CJsonConfig::GetValue(const std::string& strContent, const std::string& strKey, bool bObject)
{
    std::string strValue("");
    if (!strContent.empty() && !strKey.empty())
    {
        cJSON* jsonConfig = cJSON_Parse(strContent.c_str());
        if (jsonConfig)
        {
            cJSON* jsonValue = cJSON_GetObjectItem(jsonConfig, strKey.c_str());
            if (jsonValue)
            {
                if (bObject)
                {
                    strValue = cJSON_Print(jsonValue);
                }
                else if (!bObject && jsonValue->valuestring)
                {
                    strValue = jsonValue->valuestring;
                }
            }
            else
            {
                //printf("[CJsonConfig]error, GetValue key:\"%s\" not find \n", strKey.c_str());
            }

        }
        cJSON_Delete(jsonConfig);
    }
    return strValue;
}

double CJsonConfig::GetDoubleValue(const std::string& strContent, const std::string& strKey)
{
    double ret = 0;
    if (!strContent.empty() && !strKey.empty())
    {
        cJSON* jsonConfig = cJSON_Parse(strContent.c_str());
        if (jsonConfig)
        {
            cJSON* jsonValue = cJSON_GetObjectItem(jsonConfig, strKey.c_str());
            if (jsonValue)
            {
                ret = jsonValue->valuedouble;
            }

        }
        cJSON_Delete(jsonConfig);
    }
    return ret;
}

int CJsonConfig::GetIntValue(const std::string& strContent, const std::string& strKey)
{
    int ret = 0;
    if (!strContent.empty() && !strKey.empty())
    {
        cJSON* jsonConfig = cJSON_Parse(strContent.c_str());
        if (jsonConfig)
        {
            cJSON* jsonValue = cJSON_GetObjectItem(jsonConfig, strKey.c_str());
            if (jsonValue)
            {
                ret = jsonValue->valueint;
            }

        }
        cJSON_Delete(jsonConfig);
    }
    return ret;
}

int CJsonConfig::GetArraySize(const std::string& strContent)
{
    int ret = 0;
    if (!strContent.empty())
    {
        cJSON* jsonConfig = cJSON_Parse(strContent.c_str());
        if (jsonConfig)
        {
            ret = cJSON_GetArraySize(jsonConfig);
        }
    }
    return ret;
}

std::string CJsonConfig::GetArrayItem(const std::string& strContent, int nIndex)
{
    std::string strValue("");
    if (!strContent.empty())
    {
        if (nIndex >= 0 && nIndex < GetArraySize(strContent))
        {
            cJSON* jsonConfig = cJSON_Parse(strContent.c_str());
            if (jsonConfig)
            {
                cJSON* jsonValue = cJSON_GetArrayItem(jsonConfig, nIndex);
                if (jsonValue)
                {
                    strValue = cJSON_Print(jsonValue);
                }
            }
        }
    }

    return strValue;
}

bool CJsonConfig::Init(const std::string strConfigPath)
{
    m_strConfigPath = strConfigPath;
    if (m_strConfigPath.empty())
    {
        printf("[CJsonConfig]error, strConfigPath empty \n");
        return false;
    }

    return true;
}
