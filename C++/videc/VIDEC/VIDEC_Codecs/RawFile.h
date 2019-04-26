//RawFile.h
//1 循环写流到文件
//2 读流到BUF
//3 保存流到图片文件

#ifndef __Raw_FILE_H__
#define __Raw_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//到达最大文件数写下一轮
#define    MaxFileNum    30      
//收不到流检测超时时间
#define    MaxWriteTimeOut  180000   /*180s*/ 
//每个流文件写入的最大时长
#define    MaxWriteFileTime   600000    /*600s */  

#define    MaxCheckRunTime  20000   /*20s*/ 

//遇有关键帧并且每个流文件写入的最大时长
#define    MaxKeyWriteFileTime   300000    /*300s */  


#define   MaxBmpFileNum     1800 
#define   PerBmpFileNum       10  


typedef struct  tagRawFileInfoItem
{
	unsigned long                 m_nOpenTS;
	FILE   *                             m_File;
}RawFileInfoItem;

typedef struct  tagRawFileInfo
{
	RawFileInfoItem  m_arrRaw[MaxFileNum];
}RawFileInfo;

class  CRawFileCallBack
{
public:
	CRawFileCallBack() {};
	~CRawFileCallBack() {};
	virtual void OnRawFileCallBack(void) =0;
};

//test
class  CRawFileCall
	:public CRawFileCallBack
{
public:
	CRawFileCall() {};
	~CRawFileCall() {};
	virtual void OnRawFileCallBack(void) {};
};

////test
//m_pRawFile = NULL;
//CLogUtil * g_LogUtil = NULL;
//m_pRawFile = new CRawFile(/*RawFileCall*/);
//g_LogUtil = new CLogUtil(1);
//
//if(m_pRawFile)
//{
//	delete m_pRawFile;
//	m_pRawFile =NULL;
//}
//if(g_LogUtil)
//{
//	delete g_LogUtil;
//	g_LogUtil =NULL;
//}



class  CLogUtil;

class  CRawFile
{
public:
	//CRawFile(CRawFileCallBack & rCallBack);
	CRawFile();
	~CRawFile() ;

protected:
	//CRawFileCallBack &        m_rCallBack;
	bool                                m_bRec;
	int                                   m_nStartWrite;
	RawFileInfo                     m_RawFileInfo;
	char                                 m_FilePath[128];
	char                                 m_PrefixName[64];
	char                                 m_FilePathName[256];
	int                                    m_nHeight;
	int                                    m_nWidth;
	unsigned long                 m_nDataWriteTS;
	unsigned long                 m_nObsID;
	unsigned int                    m_nFileIndex;
   CLogUtil *                         m_pLogUtil;

   int                                     m_nReadCount;
   unsigned char  *              m_pReadRawBuf ;
   char                                  m_pReadFileName[128];
   FILE *                                m_pReadRawFile;

  int                                     m_nBmpCount;

protected:
	void Init(void);
	char * GetFileName(void);
	void CheckFileRotate(bool bKeyFrame,unsigned long ts);
	void OpenFile(char * fileName);
	void CloseFile(void);
	void CloseAllFile(void);

public:
	void WriteEnable(bool bEnable);
	void SetFilePath(char * path) { if(path) strcpy(m_FilePath,path); };
	void SetWidth(int w) {m_nWidth =w; };
	void SetHeight(int h) {m_nHeight =h; };
	void WriteFile(void * data,unsigned int nLen,bool bKeyFrame);
	bool CheckWrite(void);
	void LogOutput( char * info);


	//nMaxSection default 41
	bool  IsRawFileMaxSection(int nMaxSection=41);
	bool  GetRawFileData(unsigned char ** ppStreamData,int * pStreamLen,int nMaxSection=41);
	void  ReleaseRawDataBuf(void);
	void CopyAllFileDataToOneFile(void);

	void  ReadBMPToBuffer(char * pDest, char * bmp_file);
	void  SaveRGBToBMP(char * pdata, char * bmp_file, int width, int height );
	void  SaveToPictureFile(char * pBufferOut, int width, int height);
	void  SaveYUVToFile(char * pdata, char * yuv_file, int width, int height );

};




class  CLogUtil
{
public:
	CLogUtil(unsigned long nObsID);
	CLogUtil(const char * pObsID);
	~CLogUtil() ;

protected:
	char                      m_PrefixName[64];
	char                      m_FilePath[128];
	FILE *                    m_pLogFile;

	char                      m_strObsID[256];
	unsigned long     m_nRunningTS;

private:

	void  Init(void);

public:
	void LogOutput( char * szFormat, ...);
	void LogOutputToFile(FILE ** ppf,const char * msg);
	//用于线程中间断输出信息
	bool CheckRunning(char * szFormat, ...);

};



//test
//DeleteDirectory("D:\\RecH264\\capture");
//DeleteDirectory("D:\\RecH264");
BOOL DeleteDirectory(char* psDirName);















#endif