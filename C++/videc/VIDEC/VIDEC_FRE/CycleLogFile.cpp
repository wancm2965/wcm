#include "stdafx.h"
#include "CycleLogFile.h"
#include <stdarg.h>

#ifdef _WIN32
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#endif

#include "VIDECFREHeader.h"

unsigned long XGetTimestamp(void)
{
#ifdef WIN32
	return ::timeGetTime();  //ºÁÃë
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}



CCycLogFile::CCycLogFile()
{
	Init();
}

CCycLogFile::~CCycLogFile()
{
	CloseAllFile();
	if(m_pLogUtil)
	{
		delete m_pLogUtil;
		m_pLogUtil =NULL;
	}
}

void CCycLogFile::Init(void)
{
	m_nDataWriteTS = XGetTimestamp();
	m_pLogUtil =NULL;
	//NEWRECORD( m_pLogUtil , new CLogUtilC("syslog") ) 
	memset(&m_CycLogFileInfo,0,sizeof(m_CycLogFileInfo) );

	char dirPath[256]={0};
	CLogUtilC::GetLogDir(dirPath);

	sprintf(m_FilePath,"%s\\",dirPath);
	strcpy(m_PrefixName,"Log");
	m_nFileIndex =0;
	m_bRec =true;
	m_nStartWrite =0;

	m_nReadCount =0;
	memset(m_pReadFileName,0,sizeof(m_pReadFileName));
	m_pReadRawFile= NULL;
	m_nBmpCount =0;

	memset(&m_logFileData,0,sizeof(m_logFileData) );
	if(m_pLogUtil) m_pLogUtil->LogOutput("Init Record.\r\n");
}

void CCycLogFile::WriteEnable(bool bEnable)
{
	m_bRec = bEnable;
}

char * CCycLogFile::GetFileName(void)
{
	sprintf(m_FilePathName,"%s%s_%d.log",m_FilePath,m_PrefixName,m_nFileIndex);
	return m_FilePathName;
}

void CCycLogFile::OpenFile(char * m_pReadFileName,bool bEmpty)
{
	FILE * pFile=NULL;
	//Çå¿Õ
	if(bEmpty)
	{
		pFile =fopen(m_pReadFileName,"wb");
		if(pFile) fclose(pFile);
	}

	pFile=fopen(m_pReadFileName,"a+b");
	if(pFile)
	{
		m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_File =pFile;
	}
}

void CCycLogFile::CheckFileRotate(unsigned long ts)
{
	if(m_nStartWrite ==1)
	{
		bool bGet =false;
		if(m_pLogUtil) bGet = m_pLogUtil->GetLogFileInfo(m_logFileData);
		m_nFileIndex =m_logFileData.m_nLogIndex ;

		OpenFile(GetFileName() , !bGet);
		m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_nWriteLength = m_logFileData.m_nWriteLength;

		m_nStartWrite =2;
		if(m_pLogUtil) m_pLogUtil->LogOutput("Start Record log index:%d,log position:%d.\r\n",m_logFileData.m_nLogIndex,m_logFileData.m_nWriteLength);
	}

	if(m_nStartWrite ==2)
	{
		if ( m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_nWriteLength >= MaxWriteLogLength )
		{
			CloseFile();
			m_nFileIndex +=1;
			m_nFileIndex = m_nFileIndex % MaxFileNum;
			OpenFile(GetFileName(),true);
			m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_nWriteLength = 0 ;
			if(m_pLogUtil) m_pLogUtil->LogOutput("File Rotate to index:%d.\r\n",m_nFileIndex);
		}
	}

}

void CCycLogFile::WriteFile(void * data,unsigned int nLen )
{
	if(! m_bRec) return;

	XAutoLock l(m_csLogSec);
	if(m_nStartWrite ==0) m_nStartWrite =1;
	CheckFileRotate( nLen);
	if(nLen >0)
	{
		FILE * pFile= m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_File;
		if(pFile)
		{
			//time_t now = time(NULL);
			//strftime(m_strTS, 255, ("[%m-%d %H:%M:%S]  "), localtime(&now));
			//if(header) strcat(m_strTS,header);
			//fwrite(m_strTS,strlen(m_strTS),1,pFile);

			fwrite(data,1,nLen,pFile);
			fflush(pFile);
			m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_nWriteLength += nLen ;
		}
	}

}

void CCycLogFile::CloseFile(void)
{
	if(m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_File)
	{
		fclose(m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_File);
		m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_File =NULL;
	}
}

void CCycLogFile::CloseAllFile(void)
{
	XAutoLock l(m_csLogSec);

	m_logFileData.m_nLogIndex =m_nFileIndex;
	m_logFileData.m_nWriteLength   =m_CycLogFileInfo.m_arrRaw[m_nFileIndex].m_nWriteLength;
	if(m_pLogUtil) m_pLogUtil->SaveLogFileInfo(m_logFileData);

	for(int i=0;i<MaxFileNum;i++)
	{
		if(m_CycLogFileInfo.m_arrRaw[i].m_File)
		{
			fclose(m_CycLogFileInfo.m_arrRaw[i].m_File);
			m_CycLogFileInfo.m_arrRaw[i].m_File =NULL;
		}
	}

	if(m_pReadRawFile )
	{
		fclose(m_pReadRawFile );
		m_pReadRawFile = NULL;
	}

	if(m_pLogUtil) m_pLogUtil->LogOutput("Close All Record.\r\n");

}

void CCycLogFile::LogOutput( char * info)
{
	if(m_pLogUtil) m_pLogUtil->LogOutput(info);
}

int CCycLogFile::GetWriteHeader(char * dstBuf,int nBufLen,char * header)
{
	time_t now = time(NULL);
	strftime(dstBuf, nBufLen -1, ("[%m-%d %H:%M:%S]  "), localtime(&now));
	if(header) strcat(dstBuf,header);
	int nRet =strlen(dstBuf);
	return nRet;
}

CLogUtilC::CLogUtilC(unsigned long nObsID)
{
	_snprintf(m_strObsID,255,"%d",nObsID);
	Init();
}

CLogUtilC::CLogUtilC(const char * pObsID)
{
	_snprintf(m_strObsID,255,"%s",pObsID);
	Init();
}

void CLogUtilC::GetLogDir(char * pDirPath)
{
	char *  pDir ="D:\\VidCoreLog";
    CFileFind tempFind;
	char  FilePath[256];
    sprintf(FilePath,"%s\\*.*",pDir);
	if( tempFind.FindFile(FilePath) !=TRUE)
	{
		if(CreateDirectory(pDir, NULL) != TRUE )
		{
			pDir ="D:";
		}
	}

	if(pDirPath) strcpy(pDirPath,pDir);
}

void CLogUtilC::Init(void)
{
	m_nRunningTS =0;
	m_pLogFile =NULL;

	char dirPath[256]={0};
	GetLogDir(dirPath);
	sprintf(m_FilePath,"%s\\%s_", dirPath,m_strObsID);
}

CLogUtilC::~CLogUtilC()
{
	if( m_pLogFile)
	{
		fclose( m_pLogFile);
		m_pLogFile =NULL;
	}
}

void CLogUtilC::LogOutput( char *szFormat, ...)
{

	va_list args;
	va_start(args, szFormat);
	static char buffer[1024]={0};

#ifdef _WIN32
	_vsnprintf(buffer,sizeof(buffer),szFormat,args);
	//DebugOutput(buffer);
	LogOutputToFile(&m_pLogFile,buffer);

#else
	vsnprintf(buffer,sizeof(buffer),szFormat,args);
	printf(buffer);
#endif

	va_end(args);

}

void CLogUtilC::LogOutputToFile(FILE ** ppf,const char * msg)
{
	if(*ppf ==NULL)
	{
		char logName[256]={0};
		sprintf(logName,"%s%s.log",m_FilePath,"Output");
		*ppf = fopen(logName, "ab+");
	}

	if (*ppf)
	{
		FILE * f =*ppf;
		int nLen =strlen(msg);
		char ts[128] ={0} ; time_t now = time(NULL);
		strftime(ts, 127, ("[%m-%d %H:%M:%S] "), localtime(&now));

		fwrite(ts,strlen(ts),1,f);
		fwrite(msg,nLen,1,f);
		fflush(f);
	}
}


bool CLogUtilC::CheckRunning(char * szFormat, ...)
{
	unsigned long ts = XGetTimestamp();
	if( ts - m_nRunningTS >= MaxCheckRunTime)
	{
		{
			va_list args;
			va_start(args, szFormat);
			static char buffer[1024]={0};

#ifdef _WIN32
			_vsnprintf(buffer,sizeof(buffer),szFormat,args);
			//DebugOutput(buffer);
			LogOutputToFile(&m_pLogFile,"Check Running");
			LogOutputToFile(&m_pLogFile,buffer);

#else
			vsnprintf(buffer,sizeof(buffer),szFormat,args);
			printf(buffer);
#endif

			va_end(args);
		}

		m_nRunningTS =ts;
		return true;
	}
	else return false;
}

void CLogUtilC::GetDataFile(FILE ** ppf )
{

}

void CLogUtilC::SaveLogFileInfo(FileInfoData & data)
{
	FILE * f=NULL;
	char logName[256]={0};
	sprintf(logName,"%s%s.dat",m_FilePath,"Output");
	f = fopen(logName, "wb");

	if ( f )
	{
		int nlen=sizeof(data);
		nlen=sizeof(FileInfoData);
		fwrite(&data,sizeof(data),1,f);
		fflush(f);
	}
}

bool CLogUtilC::GetLogFileInfo(FileInfoData & data)
{
	FILE * f=NULL;
	char logName[256]={0};
	sprintf(logName,"%s%s.dat",m_FilePath,"Output");
	f = fopen(logName, "rb");

	if ( f )
	{
		if( fread(&data,sizeof(data),1,f) >0) return true;
	}
	return false;
}
