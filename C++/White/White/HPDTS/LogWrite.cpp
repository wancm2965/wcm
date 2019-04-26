#include "stdafx.h"
#include "LogWrite.h"
//#include "stdlib.h"

LogWrite::LogWrite()
{
}

LogWrite::LogWrite(std::string logDirectory, int MaxBuff, bool isOpen)
{
	m_strLogDrectory = logDirectory;
	m_nMaxBuff = MaxBuff;
	m_bIsOpen = isOpen;
	m_logBuff.clear();
	m_nBuffCount = 0;

	InitDirectory();
}

LogWrite::~LogWrite(void)
{
	//清空缓冲区，释放资源
	if (0 != m_nBuffCount)
	{
		WriteLog();
	}
}

bool LogWrite::InitDirectory()
{
#ifdef WIN32
	WIN32_FIND_DATA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(m_strLogDrectory.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true; 
	}
	FindClose(hFind);

	if (!rValue)
	{
		if (CreateDirectory(m_strLogDrectory.c_str(), NULL))
		{
			std::string msg = TEXT("HPDTS:LogWrite:SetLogDirectory Create directory '");
			msg.append(m_strLogDrectory.c_str());
			msg.append(TEXT("' succeed!\n"));
			TRACE(msg.c_str());
		}
		else
		{
			std::string msg = TEXT("HPDTS:LogWrite:SetLogDirectory Create directory '");
			msg.append(m_strLogDrectory.c_str());
			msg.append(TEXT("' failed!\n"));
			TRACE(msg.c_str());
			SetOpenState(false);
			return false;
		}
	}

#else
	bool rValue = false;
	if(!access(m_strLogDrectory.c_str(),0))
	{
		rValue = true;
		printf("HPDTS: directory existed!\n");
	}
	if (!rValue)
	{
		printf("HPDTS: creating directory\n");

		if (!mkdir(m_strLogDrectory.c_str(),777 ))
		{
			std::string msg = TEXT("HPDTS:LogWrite:InitDirectory Create directory '");
			msg.append(m_strLogDrectory.c_str());
			msg.append(TEXT("' succeed!\n"));
			TRACE(msg.c_str());
		}
		else
		{
			std::string msg = TEXT("HPDTS:LogWrite:InitDirectory Create directory '");
			msg.append(m_strLogDrectory.c_str());
			msg.append(TEXT("' failed!\n"));
			TRACE(msg.c_str());
			SetOpenState(false);
			return false;
		}

	}

#endif

	//判断文件夹路径末尾是否有一个分隔符
	if (m_strLogDrectory.at(m_strLogDrectory.length()-1) != PATH_SEPERATOR)
	{
		m_strLogDrectory += PATH_SEPERATOR;
	}

	return true;
}



bool LogWrite::WriteLog()
{
	XAutoLock l(m_csVectorLogBuff);
	//获取当前时间，构造日志文件名：
	std::string fileName = GetSystemDate() + TEXT(".log");
	std::string filePath = m_strLogDrectory + fileName;
	
	f.open(filePath.c_str(), ios_base::out|ios_base::app);	
	if (!f.is_open())
	{
		TRACE(TEXT("HPDTS:LogWrite:WriteLog function, open log file failed！\n"));
		return false;
	}
	f.seekp(0,ios_base::end);
	IT_LogBuff itLog = m_logBuff.begin();
	while (itLog != m_logBuff.end())
	{
		f.write(itLog->str, itLog->len);
		f.write(TEXT("\n"),strlen(TEXT("\n")));
		delete itLog->str;
		itLog->str = NULL;
		itLog->len = 0;
		itLog++;
	}
	f.flush();
	f.close();

	m_logBuff.clear();
	m_nBuffCount = 0;
	return true;
}

/*日志格式为：
时间：
数据：
*/
int LogWrite::Write(const char* strText, const int len)
{

	if (!m_bIsOpen)
	{
		return 3;
	}
	std::string curTime =  TEXT("time:  ") + GetSystemTime();
	std::string dataText = TEXT("data:  ");
	char nextLine = '\n';
	char endFlag = '\0';
	int timeLen = strlen(curTime.c_str());
	int dataTextLen = strlen(dataText.c_str());
	//               时间 +'\0'+'\n' +数据标识   +数据+'\0'+'\n'
	int totalLen = timeLen + 1 + 1 + dataTextLen + len + 1 + 1;
	char * buff = new char[totalLen];
	if (!buff)
	{
		TRACE(TEXT("HPDTS:LogWrite:Write new char failed!\n"));
		return 0;
	}
	memset(buff,0,totalLen);
	memcpy(buff,curTime.c_str(), timeLen+1);
	memcpy(buff+timeLen+1, &nextLine, 1);
	memcpy(buff+timeLen+1+1, dataText.c_str(), dataTextLen);
	memcpy(buff+timeLen+1+1+dataTextLen, strText, len);
	memcpy(buff+timeLen+1+1+dataTextLen+len, &endFlag, 1);
	memcpy(buff+timeLen+1+1+dataTextLen+len+1, &nextLine, 1);

	/*std::string dataText = TEXT("time: ") + GetSystemTime() + TEXT("\n") +
						   TEXT("data: ") + strText;//这里有问题！！，假如我的数据里面有0，那么后面的字节将不会显示
	int totalLen = strlen(dataText)+1;
	char * buff = new char[totalLen];
	memset(buff,0,totalLen);
	memcpy(buff,dataText.c_str(),totalLen);*/

	LOGNODE logNode;
	logNode.str = buff;
	logNode.len = totalLen;
	m_logBuff.push_back(logNode);
	m_nBuffCount++;

	if (m_nBuffCount >= m_nMaxBuff)
	{
		WriteLog();	
	}
	return 1;
}
/*日志格式为：
时间：
事件：
数据：
*/
int LogWrite::Write(const char* strEvent,const int eventLen,const char* strData, const int dataLen)
{
	if (!m_bIsOpen)
	{
		return 3;
	}
	std::string curTime =   TEXT("time:  ") + GetSystemTime();
	std::string eventTitle = TEXT("event: ");
	std::string dataTitle =  TEXT("data:  ");
	char lineFlag = '\n';
	char endFlag = '\0';
	int timeLen = strlen(curTime.c_str());
	int eventTitleLen = strlen(eventTitle.c_str());
	int dataTitleLen = strlen(dataTitle.c_str());
	//时间 +'\0'+'\n' + 事件标题 + 事件 + '\0' + '\n' + 数据标题 + 数据 + '\0' + '\n'
	int totalLen = timeLen + 1 + 1 + eventTitleLen + eventLen + 1 + 1 + dataTitleLen + dataLen + 1 + 1;
	char * buff = new char[totalLen];
	if (!buff)
	{
		TRACE(TEXT("HPDTS:LogWrite:Write new char failed!\n"));
		return 0;
	}
	memset(buff, 0, totalLen);
	memcpy(buff, curTime.c_str(), timeLen+1);
	memcpy(buff+timeLen+1, &lineFlag, 1);
	memcpy(buff+timeLen+1+1, eventTitle.c_str(), eventTitleLen);
	memcpy(buff+timeLen+1+1+eventTitleLen, strEvent, eventLen);
	memcpy(buff+timeLen+1+1+eventTitleLen+eventLen, &endFlag, 1);
	memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1, &lineFlag, 1);
	memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1, dataTitle.c_str(), dataTitleLen);
	memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1+dataTitleLen, strData, dataLen);
	memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1+dataTitleLen+dataLen, &endFlag, 1);
	memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1+dataTitleLen+dataLen+1, &lineFlag, 1);
	
	LOGNODE logNode;
	logNode.str = buff;
	logNode.len = totalLen;
	m_logBuff.push_back(logNode);
	m_nBuffCount++;

	if (m_nBuffCount >= m_nMaxBuff)
	{
		WriteLog();	
	}
	return 1;
}

/*日志格式为：
时间：
事件：
数据：
*/
int LogWrite::Write(const char* strEvent,const int eventLen,const char* strData, const int dataLen, bool isPacket)
{
	if (!m_bIsOpen)
	{
		return 3;
	}
	if (isPacket)
	{
		XMemIni xmlPacket;
		std::string strValue = strData;
		xmlPacket.ResetString(strValue);
		//对象ID，对象类型，事件ID，页号
		unsigned long objID = 0;
		unsigned long objType = 0;
		unsigned long eventID = 0;
		unsigned long pageID = 0;
		char str[128];
		memset(str,0,128);
		//unsigned long sessionID = 0;
		if (xmlPacket.Get(KEY_OBJ_ID,objID) 
			&& xmlPacket.Get(KEY_OBJ_TYPE,objType)
			&& xmlPacket.Get(KEY_EVENT_ID,eventID)
			&& xmlPacket.Get(KEY_OBJ_PAGE,pageID)
			)
		{
			sprintf(str,"objectID: %u, objectType: %u, eventID: %u, pageID: %u",objID,objType,eventID,pageID);

		}

		std::string curTime =   TEXT("time:  ") + GetSystemTime();
		std::string eventTitle = TEXT("event: ");
		std::string dataTitle =  TEXT("data:  ");
		char lineFlag = '\n';
		char endFlag = '\0';
		int timeLen = strlen(curTime.c_str());
		int eventTitleLen = strlen(eventTitle.c_str());
		int dataTitleLen = strlen(dataTitle.c_str());
		//时间 +'\0'+'\n' + 事件标题 + 事件 + '\0' + '\n' + 数据标题 + 数据 + '\0' + '\n'
		int totalLen = timeLen + 1 + 1 + eventTitleLen + eventLen + 1 + 1 + dataTitleLen + strlen(str) + 1 + 1;
		char * buff = new char[totalLen];
		if (!buff)
		{
			TRACE(TEXT("HPDTS:LogWrite:Write new char failed!  todalLen = %d\n "),totalLen);
			return 0;
		}
		memset(buff, 0, totalLen);
		memcpy(buff, curTime.c_str(), timeLen);
		memcpy(buff+timeLen, &endFlag, 1);
		memcpy(buff+timeLen+1, &lineFlag, 1);
		memcpy(buff+timeLen+1+1, eventTitle.c_str(), eventTitleLen);
		memcpy(buff+timeLen+1+1+eventTitleLen, strEvent, eventLen);
		memcpy(buff+timeLen+1+1+eventTitleLen+eventLen, &endFlag, 1);
		memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1, &lineFlag, 1);
		memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1, dataTitle.c_str(), dataTitleLen);
		memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1+dataTitleLen, str, strlen(str));
		memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1+dataTitleLen+strlen(str), &endFlag, 1);
		memcpy(buff+timeLen+1+1+eventTitleLen+eventLen+1+1+dataTitleLen+strlen(str)+1, &lineFlag, 1);

		LOGNODE logNode;
		logNode.str = buff;
		logNode.len = totalLen;
		m_logBuff.push_back(logNode);
		m_nBuffCount++;

		if (m_nBuffCount >= m_nMaxBuff)
		{
			WriteLog();	
		}
	}

	return 1;
}

std::string LogWrite::GetLogDirectory()
{
	return m_strLogDrectory;
}

void LogWrite::SetLogDirectory(std::string logDirectory)
{
	if (0 != m_nBuffCount)
	{
		WriteLog();
	}
	m_strLogDrectory = logDirectory;
	
	InitDirectory();
}

int LogWrite::GetMaxBuffCount()
{
	return m_nMaxBuff;
}

void LogWrite::SetMaxBuffCount(int maxBuff)
{
	m_nMaxBuff = maxBuff;
}

bool LogWrite::GetOpenState()
{
	return m_bIsOpen;
}

void LogWrite::SetOpenState(bool isOpen)
{
	m_bIsOpen = isOpen;
}

std::string LogWrite::GetSystemDate() 
{ 
	time_t timer; 
	struct tm* t_tm; 
	time(&timer); 
	t_tm = localtime(&timer); 
	char curTime[50];
	sprintf(curTime,"%4d_%02d_%02d",t_tm->tm_year+1900, t_tm->tm_mon+1, t_tm->tm_mday);
	return curTime; 
} 

std::string LogWrite::GetSystemTime() 
{ 
	time_t timer; 
	struct tm* t_tm; 
	time(&timer); 
	t_tm = localtime(&timer); 
	char curTime[50];
	sprintf(curTime,"%4d-%02d-%02d %d:%d:%d",t_tm->tm_year+1900, t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
	return curTime; 
} 

