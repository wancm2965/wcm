//========================================================================================
//版权所有：版权所有(C) 2010，上海华平信息技术股份有限公司
//系统名称：AVCON6
//作　　者: zhu yuyuan
//创建日期：2010-12-21
//功能说明：服务器端日志操作类
//日志每天生一个新的文件，以年月日命名，格式****_**_**.log

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <string>
#include "XMemIni.h"
#include "HPDTS/DTSCmdType.h"
#include "XAutoLock.h"
using namespace std;

//日志数据结构，保存日志数据和长度
typedef struct logNode
{
	char* str;
	//std::string str;
	int len;
}LOGNODE,*PLOGNODE;

//声明一个容器缓冲日志数据，当容器节点数目到达一定值时，再写入文件
typedef std::vector<LOGNODE> LogBuff;
typedef LogBuff::iterator IT_LogBuff;

class LogWrite
{
public:
	LogWrite();
	//参数分别为：日志文件路径，一次缓存的最大数目，是否打开日志写入
	LogWrite(std::string logPath, int MaxBuff, bool isOpen);
	virtual ~LogWrite(void);

private:
	bool WriteLog();
	bool InitDirectory();

public:
	//供外面调用的写入日志函数
	int Write(const char* strText, const int len);
	int Write(const char* strEvent,const int eventLen,const char* strData, const int dataLen);
	int Write(const char* strEvent,const int eventLen,const char* strData, const int dataLen, bool isPacket);

	//获取和设置日志文件路径
	std::string GetLogDirectory();
	void SetLogDirectory(std::string logPath);
	//获取和设置最大缓冲数
	int GetMaxBuffCount();
	void SetMaxBuffCount(int maxBuff);
	//获取和设置开关状态
	bool GetOpenState();
	void SetOpenState(bool isOpen);
	std::string GetSystemDate();
	std::string GetSystemTime();
private:
	fstream f;            //文件输入输出流
	std::string m_strLogDrectory;  //保存日志文件路径
	int m_nMaxBuff;		  //保存最大缓冲数
	int m_nBuffCount;	  //当前容器中的节点数
	bool m_bIsOpen;		  //是否打开日志功能
	LogBuff m_logBuff;	  //日志容器

	XCritSec m_csVectorLogBuff;
	//XCritSec m_csWriteBuff;
};




