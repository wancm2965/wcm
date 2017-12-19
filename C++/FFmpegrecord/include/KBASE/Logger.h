﻿/* KLog.h */

#pragma once

#include "KBASE/KBASE_Export.h"
#include "CritSec.h"
#include "Utils.h"
#include <string>

#ifdef WIN32
#define snprintf _snprintf
#endif

//日志级别
typedef	enum _LOG_LEVEL
{
	LL_NONE			= 0,	//不显示日志信息
	LL_ERROR		= 1,    //显示错误信息级别
	LL_INFO     	= 2,    //显示运行信息级别(同时显示错误信息)
	LL_DEBUG		= 3,    //显示调试信息级别(同时显示错误信息和运行信息)
	LL_FILE         = 4,    //写入文件
}LOG_LEVEL;

//---------------------------------------------------------------------------------------
// KLogger日志类定义
class KBASE_API KLogger
{
public:
    KLogger(void);
    virtual ~KLogger();
    
    bool Open(LOG_LEVEL nLevel=LL_NONE);
	bool Open(const char* file,LOG_LEVEL nLevel=LL_NONE);
    void Close(void);
    void Write(const char* fmt,va_list args);

	void SetLevel(LOG_LEVEL nLevel);
    LOG_LEVEL GetLevel(void);

	void WriteFile(const char* fmt,va_list args);

	void CreateNewFile();
	void InitFd();
protected:
	LOG_LEVEL		m_nLevel;
	std::string		m_strFilePath;
	std::string		m_strFileDir;
};

//---------------------------------------------------------------------------------------
// LOG类定义
class KBASE_API LOG
{
public:
    LOG(void){};
    virtual ~LOG(){};

    static bool START(LOG_LEVEL nLevel);
	static bool START(LOG_LEVEL nLevel,const char* file);
    static void STOP(void);
	static void SETLEVEL(LOG_LEVEL nLevel);
    static void DBG(const char* fmt,...);
    static void INF(const char* fmt,...);
    static void ERR(const char* fmt,...);
};
