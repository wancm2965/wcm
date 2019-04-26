//CycLogFile.h
//1 循环写日志到文件

#ifndef __CYC_LOG_FILE_H__
#define __CYC_LOG_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XAutoLock.h"

//m_pRawFile = NULL;
//m_pRawFile = new CCycLogFile();
//if(m_pRawFile)
//{
//	delete m_pRawFile;
//	m_pRawFile =NULL;
//}


//到达最大文件数写下一轮
#define    MaxFileNum                 30     
//每个日志文件写入的最大时长
#define    MaxWriteLogLength    /*320*/  3200000      /*3M  1024*1024*3   */ 

#define    MaxCheckRunTime     20000   /*20s*/ 


typedef struct  tagCycLogFileInfoItem
{
	unsigned  int                   m_nWriteLength;    //LOG日志当前已写的文件总字节数
	FILE   *                             m_File;
}CycLogFileInfoItem;

typedef struct  tagCycLogFileInfo
{
	CycLogFileInfoItem  m_arrRaw[MaxFileNum];
}CycLogFileInfo;

typedef struct  tagFileInfoData
{
	unsigned int                 m_nWriteLength;
	unsigned int                    m_nLogIndex;
}FileInfoData;




class  CLogUtilC;

class  CCycLogFile
{
public:
	CCycLogFile();
	~CCycLogFile() ;

protected:
	bool                                 m_bRec;
	int                                    m_nStartWrite;
	CycLogFileInfo                 m_CycLogFileInfo;
	char                                 m_FilePath[256];
	char                                 m_PrefixName[64];
	char                                 m_FilePathName[256];
	int                                    m_nHeight;
	int                                    m_nWidth;
	unsigned long                 m_nDataWriteTS;
	unsigned long                 m_nObsID;
	unsigned int                    m_nFileIndex;
   CLogUtilC *                      m_pLogUtil;
   int                                    m_nReadCount;
   char                                 m_pReadFileName[256];
   FILE *                               m_pReadRawFile;
  int                                     m_nBmpCount;
  XCritSec                            m_csLogSec;
  FileInfoData                      m_logFileData;
  //char                                  m_strTS[256] ;

protected:
	void     Init(void);
	char *  GetFileName(void);

	//检查已写大小是否要循环到下一个文件
	void    CheckFileRotate(unsigned long ts);
	void    OpenFile(char * fileName,bool bEmpty=false);
	void    CloseFile(void);
	void    CloseAllFile(void);

public:
	void   WriteEnable(bool bEnable);
	void   SetFilePath(char * path) { if(path) strcpy(m_FilePath,path); };
	void   WriteFile(void * data,unsigned int nLen );
	void   LogOutput( char * info);
	int      GetWriteHeader(char * dstBuf,int nBufLen,char * header =NULL);

};



class  CLogUtilC
{
public:
	CLogUtilC(unsigned long nObsID);
	CLogUtilC(const char * pObsID);
	~CLogUtilC() ;

public:
	static void GetLogDir(char * pDirPath);

protected:
	char                      m_PrefixName[64];
	char                      m_FilePath[256];
	FILE *                    m_pLogFile;

	char                      m_strObsID[256];
	unsigned long     m_nRunningTS;

private:

	void  Init(void);
	void GetDataFile(FILE ** ppf );

public:
	void LogOutput( char * szFormat, ...);
	void LogOutputToFile(FILE ** ppf,const char * msg);
	//用于线程中间断输出信息
	bool CheckRunning(char * szFormat, ...);
	void SaveLogFileInfo( FileInfoData & data);
	bool GetLogFileInfo( FileInfoData & data);

};



#endif