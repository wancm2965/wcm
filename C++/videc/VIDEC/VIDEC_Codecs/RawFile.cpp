#include "stdafx.h"
#include "RawFile.h"
#include "XUtil.h"
#include <stdarg.h>
#include <time.h>
#include "VIDEC_Header.h"
#include <windows.h>
#include "../VIDEC_FRE/VIDECFREHeader.h"

//CRawFile::CRawFile(CRawFileCallBack & rCallBack)
//:m_rCallBack(rCallBack)
//{
//	Init();
//}

CRawFile::CRawFile()
{
	Init();
}

CRawFile::~CRawFile()
{
	CloseAllFile();
	ReleaseRawDataBuf();
	if(m_pLogUtil)
	{
		delete m_pLogUtil;
		m_pLogUtil =NULL;
	}
}

void CRawFile::Init(void)
{
	memset(&m_RawFileInfo,0,sizeof(m_RawFileInfo) );
	strcpy(m_FilePath,"D:\\RecH264\\");
	strcpy(m_PrefixName,"RawData");
	m_nFileIndex =0;
	m_bRec =true;
	m_nStartWrite =0;
	m_nDataWriteTS = XGetTimestamp();
	m_nObsID =  (unsigned long)m_nDataWriteTS +  (ULONG_PTR)this;
	m_pLogUtil =NULL;
	NEWRECORD( m_pLogUtil , new CLogUtil(m_nObsID) )
	//NULLRecord( m_pLogUtil )  

	m_nReadCount =0;
	m_pReadRawBuf = NULL;
	memset(m_pReadFileName,0,sizeof(m_pReadFileName));
	m_pReadRawFile= NULL;
	m_nBmpCount =0;

	if(m_pLogUtil) m_pLogUtil->LogOutput("Init Record.\r\n");
}

void CRawFile::WriteEnable(bool bEnable)
{
	m_bRec = bEnable;
}

char * CRawFile::GetFileName(void)
{
	sprintf(m_FilePathName,"%s%s%ld_%d.h264",m_FilePath,m_PrefixName,m_nObsID,m_nFileIndex);
	return m_FilePathName;
}

void CRawFile::OpenFile(char * m_pReadFileName)
{
	FILE * pFile=fopen(m_pReadFileName,"w+b");
	if(pFile)
	{
		m_RawFileInfo.m_arrRaw[m_nFileIndex].m_File =pFile;
	}
}

void CRawFile::CheckFileRotate(bool bKeyFrame,unsigned long ts)
{
	if(m_nStartWrite ==1)
	{
		////special 
		//sprintf(m_FilePathName,"%s%sFirst.h264",m_FilePath,m_PrefixName);
		//OpenFile(m_FilePathName);

		m_nFileIndex =0;
		OpenFile(GetFileName() );
		m_RawFileInfo.m_arrRaw[m_nFileIndex].m_nOpenTS =ts;
		m_nStartWrite =2;
		if(m_pLogUtil) m_pLogUtil->LogOutput("Start Record.\r\n");
	}
	else
	{
		if ( (bKeyFrame && (ts - m_RawFileInfo.m_arrRaw[m_nFileIndex].m_nOpenTS >= MaxKeyWriteFileTime) ) || (ts - m_RawFileInfo.m_arrRaw[m_nFileIndex].m_nOpenTS >= MaxWriteFileTime) )
		{
			CloseFile();
			m_nFileIndex +=1;
			m_nFileIndex = m_nFileIndex % MaxFileNum;
			OpenFile(GetFileName() );
			m_RawFileInfo.m_arrRaw[m_nFileIndex].m_nOpenTS =ts;
			if(m_pLogUtil) m_pLogUtil->LogOutput("File Rotate to index:%d.\r\n",m_nFileIndex);
		}
	}

}

void CRawFile::WriteFile(void * data,unsigned int nLen,bool bKeyFrame)
{
	if(! m_bRec) return;
	if(m_nStartWrite ==0) m_nStartWrite =1;
	unsigned long ts = XGetTimestamp();
	CheckFileRotate(bKeyFrame,ts);
	if(nLen >0)
	{
		m_nDataWriteTS =ts;
		FILE * pFile= m_RawFileInfo.m_arrRaw[m_nFileIndex].m_File;
		if(pFile)
		{
			fwrite(&nLen,1,4,pFile);
			fwrite(data,1,nLen,pFile);
			fflush(pFile);
		}
	}
	else CheckWrite();
}

bool CRawFile::CheckWrite(void)
{
	unsigned long ts = XGetTimestamp();
	if( ts - m_nDataWriteTS >= MaxWriteTimeOut)
	{
		if(m_pLogUtil) m_pLogUtil->LogOutput("RawFile Data Write TimeOut :%d \r\n",MaxWriteTimeOut);
		//m_rCallBack.OnRawFileCallBack();
		m_nDataWriteTS =ts;
		return true;
	}
	else return false;
}

void CRawFile::CloseFile(void)
{
	if(m_RawFileInfo.m_arrRaw[m_nFileIndex].m_File)
	{
		fclose(m_RawFileInfo.m_arrRaw[m_nFileIndex].m_File);
		m_RawFileInfo.m_arrRaw[m_nFileIndex].m_File =NULL;
	}
}

void CRawFile::CloseAllFile(void)
{
	for(int i=0;i<MaxFileNum;i++)
	{
		if(m_RawFileInfo.m_arrRaw[i].m_File)
		{
			fclose(m_RawFileInfo.m_arrRaw[i].m_File);
			m_RawFileInfo.m_arrRaw[i].m_File =NULL;
		}
	}

	if(m_pReadRawFile )
	{
		fclose(m_pReadRawFile );
		m_pReadRawFile = NULL;
	}

	if(m_pLogUtil) m_pLogUtil->LogOutput("Close All Record.\r\n");
}

void CRawFile::LogOutput( char * info)
{
	if(m_pLogUtil) m_pLogUtil->LogOutput(info);
}




bool  CRawFile::IsRawFileMaxSection(int nMaxSection)
{
	if(m_nReadCount >= nMaxSection) return true;
	else return false;
}

bool  CRawFile::GetRawFileData(unsigned char ** ppStreamData,int * pStreamLen,int nMaxSection)
{
	bool bRet =false;

	if(m_nReadCount >= nMaxSection) 
	{
		if(m_pLogUtil) m_pLogUtil->LogOutput("----------------------------Read File >= %d, finish------------------.\r\n",m_nReadCount);
		return false;
	}

	while(true)
	{
		if(m_pReadRawFile == NULL)
		{
			sprintf(m_pReadFileName,"D:\\RecH264\\test\\RawData1385974509_%d.h264",m_nReadCount);
			m_pReadRawFile =fopen(m_pReadFileName,"rb");
			if(m_pLogUtil) m_pLogUtil->LogOutput("Open RawData  File Name:%s \r\n",m_pReadFileName);
		}

		if(m_pReadRawFile )
		{
			m_pReadRawBuf = NULL;
			int nRet =-1; int nReadLen =0;
			nRet =fread(&nReadLen,4,1,m_pReadRawFile);
			if(nRet >0)
			{
				NEWRECORD( m_pReadRawBuf , new unsigned char[nReadLen +64] )
				NULLReturnRet( m_pReadRawBuf ,bRet )  
				nRet = fread(m_pReadRawBuf,nReadLen,1,m_pReadRawFile);
			}
			if(nRet >0 && m_pReadRawBuf)
			{
				*ppStreamData =m_pReadRawBuf;
				*pStreamLen =nReadLen;
				bRet =true;
				break; //break while
			}
			else
			{
				fclose(m_pReadRawFile );
				m_pReadRawFile = NULL;
				m_nReadCount ++;
				if(m_nReadCount >= nMaxSection) 
				{
					if(m_pLogUtil) m_pLogUtil->LogOutput("----------------------------Read File >= %d, finish------------------.\r\n",m_nReadCount);
					return false;
				}
			}

		}
	}

	return bRet;

}

void  CRawFile::ReleaseRawDataBuf(void)
{
	if(m_pReadRawBuf)
	{
		delete [] m_pReadRawBuf;
		m_pReadRawBuf =NULL;
	}
}

void CRawFile::CopyAllFileDataToOneFile(void)
{
	sprintf(m_FilePathName,"%s%sAll.h264",m_FilePath,m_PrefixName);
	FILE * pWriteFile=fopen(m_FilePathName,"w+b");
	if(pWriteFile)
	{
		m_nReadCount =0;
		unsigned char*pStreamData=NULL;
		int nStreamLen=0;
		while(true)
		{
			bool bGetRawData = GetRawFileData(&pStreamData,&nStreamLen);
			if(!bGetRawData) break;
			if( bGetRawData)
			{
				//fwrite(&nStreamLen,1,1,pWriteFile);
				fwrite(pStreamData,nStreamLen,1,pWriteFile);
				fflush(pWriteFile);
				ReleaseRawDataBuf();
			}
		}

		fclose(pWriteFile);
		pWriteFile =NULL;
		m_nReadCount =0;

		if(m_pLogUtil) m_pLogUtil->LogOutput("CopyAllFileDataToOneFile, finish------------------.\r\n");
	}

}


void CRawFile::ReadBMPToBuffer(char * pDest, char * bmp_file)
{
	//�ֱ�Ϊrgb���ݣ�Ҫ�����bmp�ļ�����ͼƬ����
	int size = 0; //width*height*3*sizeof(char); // ÿ�����ص���ֽ�
	//λͼ��һ���֣��ļ���Ϣ
	BITMAPFILEHEADER bfh;
	bfh.bfType = (WORD)0x4d42;  //bm
	bfh.bfSize = size  // data size
		+ sizeof( BITMAPFILEHEADER) // first section size
		+ sizeof( BITMAPFILEHEADER) ; // second section size
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER  )+ sizeof( BITMAPFILEHEADER  );//���������ݵ�λ��
	//λͼ�ڶ����֣�������Ϣ
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER );
	//bih.biWidth = width;
	//bih.biHeight = height; //BMPͼƬ�����һ���㿪ʼɨ�裬��ʾʱͼƬ�ǵ��ŵģ�������-height������ͼƬ������
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0; //��ѹ��
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 2835 ;//����ÿ��
	bih.biYPelsPerMeter = 2835 ;
	bih.biClrUsed = 0; //���ù�����ɫ��λ��Ϊ
	bih.biClrImportant = 0; //ÿ�����ض���Ҫ

	memset(&bfh,sizeof(bfh),0);
	memset(&bfh.bfReserved2,sizeof(&bfh.bfReserved2),0);
	memset(&bfh.bfOffBits,sizeof(&bfh.bfOffBits),0);
	memset(&bih,sizeof(bih),0);
	//char * pDest =new char[bih.biSizeImage +8] ;
	FILE * fp = NULL;
	fp = fopen( bmp_file,"rb" );
	if( !fp ) return;
	fseek(fp,0,SEEK_SET);
	int nRet =-1;
	nRet =fread( &bfh, 8, 1,  fp ); //����linux���ֽڶ��룬����Ϣͷ��СΪ�ֽڣ���һ�����ֽڣ��ڶ������ֽڣ����ԻὫ��һ���ֲ���Ϊ�Լ���ֱ����sizeof����ͼƬʱ�ͻ�����premature end-of-file encountered����
	nRet =fread(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, fp);
	nRet =fread(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, fp);
	nRet =fread( &bih, sizeof(BITMAPINFOHEADER ),1,fp );
	size = bih.biSizeImage;
	nRet =fread(pDest,size,1,fp);
	fclose( fp );
} 

void CRawFile::SaveRGBToBMP(char * pdata, char * bmp_file, int width, int height )
{
	//�ֱ�Ϊrgb���ݣ�Ҫ�����bmp�ļ�����ͼƬ����
	int size = width*height*3*sizeof(char); // ÿ�����ص�3���ֽ�
	//λͼ��һ���֣��ļ���Ϣ
	BITMAPFILEHEADER bfh;
	bfh.bfType = (WORD)0x4d42;  //bm
	bfh.bfSize = size  // data size
		+ sizeof( BITMAPFILEHEADER) // first section size
		+ sizeof( BITMAPFILEHEADER) ; // second section size
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER  )+ sizeof( BITMAPFILEHEADER  );//���������ݵ�λ��
	//λͼ�ڶ����֣�������Ϣ
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER );
	bih.biWidth = width;
	bih.biHeight = height; //BMPͼƬ�����һ���㿪ʼɨ�裬��ʾʱͼƬ�ǵ��ŵģ�������-height������ͼƬ������
	bih.biPlanes = 1; //Ϊ1
	bih.biBitCount = 24;
	bih.biCompression = 0; //��ѹ��
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 2835 ;//����ÿ��
	bih.biYPelsPerMeter = 2835 ;
	bih.biClrUsed = 0; //���ù�����ɫ��24λ��Ϊ0
	bih.biClrImportant = 0; //ÿ�����ض���Ҫ
	FILE * fp = fopen( bmp_file,"wb" );
	if( !fp ) return;
	fseek(fp,0,SEEK_SET);
	fwrite( &bfh, 8, 1,  fp ); //����linux��4�ֽڶ��룬����Ϣͷ��СΪ54�ֽڣ���һ����14�ֽڣ��ڶ�����40�ֽڣ����ԻὫ��һ���ֲ���Ϊ16�Լ���ֱ����sizeof����ͼƬʱ�ͻ�����premature end-of-file encountered����
	fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, fp);
	fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, fp);
	fwrite( &bih, sizeof(BITMAPINFOHEADER ),1,fp );
	fwrite(pdata,size,1,fp);
	fclose( fp );
}

void CRawFile::SaveToPictureFile(char * pBufferOut, int width, int height)
{
	m_nBmpCount ++;
	if(m_nBmpCount % PerBmpFileNum ==0 )
	{
		int nCount = m_nBmpCount % MaxBmpFileNum;
		static char path[256]={0};
		sprintf(path,"D:\\RecH264\\capture\\%p_%d_%d_%d.bmp",this,width,height,nCount );
		SaveRGBToBMP(  pBufferOut, path,width, height);
	}
}

void CRawFile::SaveYUVToFile(char * pdata, char * yuv_file, int width, int height )
{
	FILE * pFile =NULL;
	if(pFile ==NULL)
	{
		pFile =fopen(yuv_file,"wb");
		if(pFile)
		{
			fseek(pFile,0,SEEK_SET);
			fwrite(pdata,width * height *3/2,1,pFile);
			fclose(pFile);
			pFile =NULL;
		}
	}
}



CLogUtil::CLogUtil(unsigned long nObsID)
{
	_snprintf(m_strObsID,255,"%d",nObsID);
	Init();
}

CLogUtil::CLogUtil(const char * pObsID)
{
	_snprintf(m_strObsID,255,"%s",pObsID);
	Init();
}

void CLogUtil::Init(void)
{
	m_nRunningTS =0;
	m_pLogFile =NULL;
	sprintf(m_FilePath,"D:\\RecH264\\%s_", m_strObsID);
}

CLogUtil::~CLogUtil()
{
	if( m_pLogFile)
	{
		fclose( m_pLogFile);
		m_pLogFile =NULL;
	}
}

void CLogUtil::LogOutput( char *szFormat, ...)
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

void CLogUtil::LogOutputToFile(FILE ** ppf,const char * msg)
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


bool CLogUtil::CheckRunning(char * szFormat, ...)
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



//test
BOOL DeleteDirectory(char* psDirName)
{
    CFileFind tempFind;   
    char sTempFileFind[ _MAX_PATH ] = { 0 };   
    sprintf(sTempFileFind,"%s//*.*",psDirName);   
    BOOL IsFinded = tempFind.FindFile(sTempFileFind);   
    while (IsFinded)
    {
        IsFinded = tempFind.FindNextFile();
        if (!tempFind.IsDots())
        {
            char sFoundFileName[ _MAX_PATH ] = { 0 };
            strcpy(sFoundFileName,tempFind.GetFileName().GetBuffer(200));
            if (tempFind.IsDirectory())
            {
                char sTempDir[ _MAX_PATH ] = { 0 };
                sprintf(sTempDir,"%s//%s",psDirName,sFoundFileName);
                //DeleteDirectory(sTempDir);
            }
            else
			{
                char sTempFileName[ _MAX_PATH ] = { 0 };
                sprintf(sTempFileName,"%s//%s",psDirName,sFoundFileName);
                DeleteFile(sTempFileName);
            }
        }
    }

    tempFind.Close();
	//if(!RemoveDirectory(psDirName))
	//{
	//	return FALSE;
	//}
    return TRUE;
}



