#include "stdafx.h"
#include <winsock2.h>
#include <mmsystem.h>
#include "RTTimer.h"

#include "CycleLogFile.h"
#include "VIDECFREHeader.h"

#pragma comment(lib,"winmm.lib")


CCycLogFile * m_gCycLogFile = NULL;

int g_nCoreLogLevel =2;
int g_nModuleLogLevel =1;

static FrameRateEventCtrl*	g_pFrameRateEventCtrl_10=NULL;
static FrameRateEventCtrl*	g_pFrameRateEventCtrl_15=NULL;
static FrameRateEventCtrl*	g_pFrameRateEventCtrl_20=NULL;
static FrameRateEventCtrl*	g_pFrameRateEventCtrl_25=NULL;
static FrameRateEventCtrl*	g_pFrameRateEventCtrl_30=NULL;
static FrameRateEventCtrl*	g_pFrameRateEventCtrl_50=NULL;
static FrameRateEventCtrl*	g_pFrameRateEventCtrl_60=NULL;
XCritSec					g_csFrameRateEventCtrl_10;
XCritSec					g_csFrameRateEventCtrl_15;
XCritSec					g_csFrameRateEventCtrl_20;
XCritSec					g_csFrameRateEventCtrl_25;
XCritSec					g_csFrameRateEventCtrl_30;
XCritSec					g_csFrameRateEventCtrl_50;
XCritSec					g_csFrameRateEventCtrl_60;

static int g_DebugLevel=-1;
static int g_nHWDecodeWidth=-1;
static int g_nHWDecodeHeight=-1;
static int g_nHWEncodeWidth=-1;
static int g_nHWEncodeHeight=-1;
static int g_nNVD3D=-1;
static int g_nD3D11Decoder = -1;
static int g_nPending = -1;
static int g_nDelayFrame = -1;
static int g_nDelayFrameRate = -1;
static int g_nD3D11Type = -1;
static int g_nD3D11Debug = -1;
static int g_nMaxXEncoderFrameI264Count=-1;
static int g_nForceIntelEncoder=-1;
static int g_nMaxPlayer=-1;
static int g_nD3D11Total = -1;
static int g_nSkipDisplay = -1;
static int g_nPIT = -1;


static int    g_nVIDECIPCPort =2160;

static unsigned long g_ulFreMaxKeyFrameInterval=300000; //ms



//using namespace nedalloc;
//int nSize = 1024 *1024 * 300;
//int nThreads =16;
//nedpool * pCreatePool = nedcreatepool(nSize, nThreads);
//if(pCreatePool)
//{
//	char * pGet =(char *)nedmalloc( nSize - 256 );
//	if(pGet)
//	{
//		nedfree(pGet);
//	}
//	neddestroypool(pCreatePool);
//	pCreatePool =NULL;
//}

//notice,nedmalloc release ram error ,not use nedmalloc pool 
//static nedpool * g_pCreatePool =NULL;

//use ipc pool
BOOL  g_bCreatePool =FALSE;

void CreateVPool(int nSize,int nThreads)
{
	g_bCreatePool =FALSE;

	//if( g_pCreatePool  ==NULL )
	//{
	//	g_pCreatePool = nedcreatepool(nSize, nThreads); //??
	//	if( g_pCreatePool ) g_bCreatePool = TRUE ;
	//}
	if( g_bCreatePool  == FALSE )
	{
		DebugOutput(logcoreerror ,"========= Create VPool ,Failed ========\n");
	}
}

void DestroyVPool(void)
{
	//if(g_pCreatePool)
	//{
	//	neddestroypool(g_pCreatePool);
	//	g_pCreatePool =NULL;
	//}
}


int  FREGetMemPoolSize(char * pApp,char * pKeyName, int nDefault)
{
	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");	

	int nCount=GetPrivateProfileInt( (pApp) , (pKeyName) , nDefault, chIniPath);
	IPCPrintf(1,0,"FREGetMemPoolSize section:%s,keyname:%s,count:%d \n",pApp,pKeyName,nCount);
	return nCount;
}

void  FRESetMemPoolSize(char * pApp,char * pKeyName ,int nCount)
{
	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");	

	char strValue[256];
	sprintf(strValue,("%d"),nCount);
	WritePrivateProfileString((pApp) , (pKeyName)  , strValue, chIniPath);
	IPCPrintf(1,0,"FRESetMemPoolSize section:%s,keyname:%s,count:%d \n",pApp,pKeyName,nCount);
}

void CreateIPCPool( )
{
	g_bCreatePool =FALSE;

	//g_bCreatePool = IPCPoolCreate( FREGetMemPoolSize("MEMPOOL","LESS128KBCount", 320 ) );
	//g_bCreatePool &= IPCPoolCreate128KB( FREGetMemPoolSize("MEMPOOL","128KBCount", 100 ) );
	//g_bCreatePool &= IPCPoolCreate256KB( FREGetMemPoolSize("MEMPOOL","256KBCount", 96 ) );
	//g_bCreatePool &= IPCPoolCreate512KB( FREGetMemPoolSize("MEMPOOL","512KBCount", 24 ) );
	//g_bCreatePool &= IPCPoolCreate1MB( FREGetMemPoolSize("MEMPOOL","1MBCount", 24 ) );
	//g_bCreatePool &= IPCPoolCreate2MB( FREGetMemPoolSize("MEMPOOL","2MBCount", 24 ) );
	//g_bCreatePool &= IPCPoolCreate4MB( FREGetMemPoolSize("MEMPOOL","4MBCount", 12 ) );
	//g_bCreatePool &= IPCPoolCreate8MB( FREGetMemPoolSize("MEMPOOL","8MBCount", 8 ) );
	//g_bCreatePool &= IPCPoolCreate16MB( FREGetMemPoolSize("MEMPOOL","16MBCount", 0 ) );

	if( g_bCreatePool  == FALSE )
	{
		DebugOutput(logcoreerror ,"========= Create IPCPool ,Failed ========\n");
	}
}


void * FreCalloc(  size_t no , size_t size,char * fileName,char * func,int nLine)
{
	void * p =NULL;
	if(g_bCreatePool)  p=IPCAllocMem(no * size);
	else p = calloc(no,size);
	if(p == NULL)
	{
		DebugOutput(logcoreerror ," Fre Calloc Failed,File:%s,Line:%d ,size:%d \n", fileName,  nLine,no*size);
	}
	return p;
}

void * FreRealloc( void * po,size_t size,char * fileName,char * func,int nLine)
{
	void * p =NULL;
	if(g_bCreatePool)
	{
		IPCFreeMem(po);
		p =IPCAllocMem(size); //notice ,no memcpy 
	}
	else p = realloc(po,size);
	if(p == NULL)
	{
		DebugOutput(logcoreerror ,"Fre realloc Failed,File:%s,Line:%d ,size:%d \n", fileName,  nLine,size);
	}
	return p;
}



void * FreMalloc( size_t size,char * fileName,char * func,int nLine)
{
	void * p =NULL;
	if(g_bCreatePool)  p=IPCAllocMem(size);
	else p = malloc(size);
	if(p == NULL)
	{
		DebugOutput(logcoreerror ,"Fre Malloc Failed,File:%s,Line:%d  ,size:%d \n", fileName,  nLine ,size);
	}
	//DebugOutput(logcoreerror ,"Fre Malloc Pointer:%p,File:%s,FUN:%s,Line:%d \n", p, fileName,   func,  nLine);
	return p;
}

void  FreFree(void * p,char * fileName,char * func,int nLine)
{
	if(p)
	{
		if(g_bCreatePool) IPCFreeMem(p);
		else free(p);
	}
}


unsigned long FRE_GetMaxKeyFrameInterval(void)
{
	return g_ulFreMaxKeyFrameInterval ;
}



//ipc func
void setharddecodercount(int nCount )
{
	int nOriCount =0;
	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");
	nOriCount = GetPrivateProfileInt(("VIDEC"), ("MaxDecoder") ,0, chIniPath);
	IPCPrintf(1,0,"Ori VIDEC HardWare MaxDecoder Count:%d\n",nOriCount);

	char strValue[64];
	sprintf(strValue,("%d"),nCount);
	WritePrivateProfileString(("VIDEC"), ("MaxDecoder")  , strValue, chIniPath);
	g_nMaxPlayer = nCount;

	nOriCount = GetPrivateProfileInt(("VIDEC"), ("MaxDecoder") ,0, chIniPath);
	IPCPrintf(1,0,"Current VIDEC HardWare MaxDecoder Count:%d\n",nOriCount);
}

void sethardencodercount(int nCount )
{
	int nOriCount =0;
	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");
	nOriCount = GetPrivateProfileInt(("VIDEC"), ("MaxEecoder") ,0, chIniPath);
	IPCPrintf(1,0,"Ori VIDEC HardWare MaxEncoder Count:%d\n",nOriCount);
	nOriCount = GetPrivateProfileInt(("VIDEC"), ("ForceIntelEncoder") ,0, chIniPath);
	IPCPrintf(1,0,"Ori VIDEC HardWare ForceIntelEncoder Count:%d\n",nOriCount);

	char strValue[64];
	sprintf(strValue,("%d"),nCount);
	WritePrivateProfileString(("VIDEC"), ("MaxEecoder")  , strValue, chIniPath);
	if(nCount >0 ) g_nForceIntelEncoder =1;
	else g_nForceIntelEncoder =0;
	sprintf(strValue,("%d"), g_nForceIntelEncoder);
	WritePrivateProfileString(("VIDEC"), ("ForceIntelEncoder")  , strValue, chIniPath);
	g_nMaxXEncoderFrameI264Count = nCount;

	nOriCount = GetPrivateProfileInt(("VIDEC"), ("MaxEecoder") ,0, chIniPath);
	IPCPrintf(1,0,"Current VIDEC HardWare MaxEncoder Count:%d\n",nOriCount);
}

void setd3ddisplay(int nUse )
{
	char chIniPath[1024]="";
	GetWindowsDirectory(chIniPath,sizeof(chIniPath));
	strcat(chIniPath,"\\VIDEC.ini");	

	int nUseD3D=GetPrivateProfileInt(("VIDEC"), ("UseD3D") , 0, chIniPath);
	IPCPrintf(1,0,"Ori VIDEC  UseD3D Count:%d\n",nUseD3D);

	nUseD3D=GetPrivateProfileInt(("VIDEC"), ("D3D11") , 0, chIniPath);
	IPCPrintf(1,0,"Ori VIDEC  D3D11 Count:%d\n",nUseD3D);

	char strValue[64];
	g_nNVD3D = nUse;
	sprintf(strValue,("%d"),g_nNVD3D);
	WritePrivateProfileString(("VIDEC"), ("UseD3D")  , strValue, chIniPath);

	g_nD3D11Total = nUse;
	g_nD3D11Debug =nUse;
	sprintf(strValue,("%d"),g_nD3D11Total);
	WritePrivateProfileString(("VIDEC"), ("D3D11")  , strValue, chIniPath);
	WritePrivateProfileString(("VIDEC"), ("D3D11Debug")  , strValue, chIniPath);

	nUseD3D=GetPrivateProfileInt(("VIDEC"), ("UseD3D") , 0, chIniPath);
	IPCPrintf(1,0,"Current VIDEC  UseD3D Count:%d\n",nUseD3D);
	nUseD3D=GetPrivateProfileInt(("VIDEC"), ("D3D11") , 0, chIniPath);
	IPCPrintf(1,0,"Ori VIDEC  D3D11 Count:%d\n",nUseD3D);

}

void fresethardalldisable()
{
	int nCount =0;
	 setharddecodercount(nCount);
	 sethardencodercount(nCount);
	 setd3ddisplay(nCount);
}

void fresetkeyframe(int nSecond)
{
	if (nSecond < 1)
		nSecond =1;
	else if (nSecond > 300 )
	{
		nSecond = 300 ;
	}

	IPCPrintf(1,0,"Ori FreMaxKeyFrameInterval:%ld(ms) \n",g_ulFreMaxKeyFrameInterval);
	g_ulFreMaxKeyFrameInterval  =nSecond *1000;
	IPCPrintf(1,0,"Current FreMaxKeyFrameInterval:%ld(ms) \n",g_ulFreMaxKeyFrameInterval);
}


void fresetlogcore(int nLevel)
{
	if(nLevel >0 && nLevel <99)
		g_nCoreLogLevel =nLevel;
}

void fresetlogmodule(int nLevel)
{
	if(nLevel >99)
		g_nModuleLogLevel =nLevel;
}


void frelogcode()
{
	IPCPrintf(1,0,"log level  1---99 core infomation\n");
	IPCPrintf(1,0,"log level  1 core error \n");
	IPCPrintf(1,0,"log level  2 core exception return\n");
	IPCPrintf(1,0,"log level  99  core other\n");
	IPCPrintf(1,0,"log level  100---999  module information\n");
	IPCPrintf(1,0,"log level  100  thread\n");
	IPCPrintf(1,0,"log level  101  soft encoder\n");
	IPCPrintf(1,0,"log level  102  soft decoder\n");
	IPCPrintf(1,0,"log level  103  hardware encoder\n");
	IPCPrintf(1,0,"log level  104  hardware decoder\n");
	IPCPrintf(1,0,"log level  105  directshow display\n");
	IPCPrintf(1,0,"log level  106  d3d display\n");
	IPCPrintf(1,0,"log level  107  capture\n");
	IPCPrintf(1,0,"log level  108  call statck \n");
	IPCPrintf(1,0,"log level  999  module other..\n");
}

enum MemPoolCount
{
	Mem16MBCount = 1 ,
	Mem8MBCount = 2 ,
	Mem4MBCount = 3 ,
	Mem2MBCount = 4 ,
	Mem1MBCount = 5 ,
	Mem512KBCount = 6 ,
	Mem256KBCount = 7 ,
	Mem128KBCount = 8 ,
	MemLESS128KBCount = 9 ,
	MemTotalCount = 10 
};

//ipc
int  fresetpoolcount(int nKeyNameIndex ,int count)
{
	char * section = "MEMPOOL";
	char  pKeyName[128] ={0};
	int nDefault  = 320 /16 ; //16M block , Max 320M 
	switch(nKeyNameIndex)
	{
	case Mem16MBCount :
		strcpy(pKeyName,"16MBCount");
		break;
	case Mem8MBCount :
		nDefault *= 2;
		strcpy(pKeyName,"8MBCount");
		break;
	case Mem4MBCount :
		nDefault *= 4;
		strcpy(pKeyName,"4MBCount");
		break;
	case Mem2MBCount :
		nDefault *= 8;
		strcpy(pKeyName,"2MBCount");
		break;
	case Mem1MBCount :
		nDefault *= 16;
		strcpy(pKeyName,"1MBCount");
		break;
	case Mem512KBCount :
		nDefault *= 32;
		strcpy(pKeyName,"512KBCount");
		break;
	case Mem256KBCount :
		nDefault *= 64;
		strcpy(pKeyName,"256KBCount");
		break;
	case Mem128KBCount :
		nDefault *= 128;
		strcpy(pKeyName,"128KBCount");
		break;
	case MemLESS128KBCount :
		nDefault *= 256;
		strcpy(pKeyName,"LESS128KBCount");
		break;
	default :
		return 0 ;
	}

	if( count > nDefault )  return 0 ;

	//FRESetMemPoolSize("MEMPOOL","16MBCount", 0 ) ;
	//FRESetMemPoolSize("MEMPOOL","8MBCount", 8 ) ;
	//FRESetMemPoolSize("MEMPOOL","4MBCount", 12 ) ;
	//FRESetMemPoolSize("MEMPOOL","2MBCount", 24 ) ;
	//FRESetMemPoolSize("MEMPOOL","1MBCount", 24 ) ;
	//FRESetMemPoolSize("MEMPOOL","512KBCount", 24 ) ;
	//FRESetMemPoolSize("MEMPOOL","256KBCount", 96 ) ;
	//FRESetMemPoolSize("MEMPOOL","128KBCount", 100 ) ;
	//FRESetMemPoolSize("MEMPOOL","LESS128KBCount", 320 ) ;

	FRESetMemPoolSize(section,pKeyName ,count);
	return 1;
}



void frehelp()
{
	IPCPrintf(1,0,"setharddecodercount count\n");
	IPCPrintf(1,0,"sethardencodercount count\n");
	IPCPrintf(1,0,"setd3ddisplay use\n");
	IPCPrintf(1,0,"fresethardalldisable \n");
	IPCPrintf(1,0,"fresetkeyframe second \n");
	IPCPrintf(1,0,"fresetlogcorelevel Level \n");
	IPCPrintf(1,0,"fresetlogmodule Level \n");
	IPCPrintf(1,0,"frelogcode   \n");
}


int FRE_GetD3D11Total(void)
{
	if (g_nD3D11Total == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");
		g_nD3D11Total=GetPrivateProfileInt(("VIDEC"), ("D3D11") , 1, chIniPath);
		char strValue[1024];
		sprintf(strValue,("%d"),g_nD3D11Total);		
		WritePrivateProfileString(("VIDEC"), ("D3D11")  , strValue, chIniPath);
	}
	return g_nD3D11Total;
}

int FRE_GetMaxDecoderCount(void)
{
	if (g_nMaxPlayer == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");
		g_nMaxPlayer=GetPrivateProfileInt(("VIDEC"), ("MaxDecoder") ,6, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nMaxPlayer);
		WritePrivateProfileString(("VIDEC"), ("MaxDecoder")  , strValue, chIniPath);
	}
	return g_nMaxPlayer;
}

int FRE_GetMaxEncoderCount(void)
{
	if (g_nMaxXEncoderFrameI264Count == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");
		g_nMaxXEncoderFrameI264Count=GetPrivateProfileInt(("VIDEC"), ("MaxEecoder") ,2, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nMaxXEncoderFrameI264Count);
		WritePrivateProfileString(("VIDEC"), ("MaxEecoder")  , strValue, chIniPath);
	}
	return g_nMaxXEncoderFrameI264Count;
}

int FRE_GetForceIntelEncoder(void)
{
	if (g_nForceIntelEncoder == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");
		g_nForceIntelEncoder=GetPrivateProfileInt(("VIDEC"), ("ForceIntelEncoder") ,0, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nForceIntelEncoder);
		WritePrivateProfileString(("VIDEC"), ("ForceIntelEncoder")  , strValue, chIniPath);
	}
	return g_nForceIntelEncoder;
}

int  FRE_GetHWDecodeWidth(void)
{
	if (g_nHWDecodeWidth == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nHWDecodeWidth=GetPrivateProfileInt(("VIDEC"), ("HWDecodeWidth") , 960, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nHWDecodeWidth);
		WritePrivateProfileString(("VIDEC"), ("HWDecodeWidth")  , strValue, chIniPath);
	}
	return g_nHWDecodeWidth;
}

int  FRE_GetHWDecodeHeight(void)
{
	if (g_nHWDecodeHeight == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nHWDecodeHeight=GetPrivateProfileInt(("VIDEC"), ("HWDecodeHeight") , 540, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nHWDecodeHeight);
		WritePrivateProfileString(("VIDEC"), ("HWDecodeHeight")  , strValue, chIniPath);
	}
	return g_nHWDecodeHeight;
}


int  FRE_GetHWEncodeWidth(void)
{
	if (g_nHWEncodeWidth == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nHWEncodeWidth=GetPrivateProfileInt(("VIDEC"), ("HWEncodeWidth") , 1280, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nHWEncodeWidth);
		WritePrivateProfileString(("VIDEC"), ("HWEncodeWidth")  , strValue, chIniPath);
	}
	return g_nHWEncodeWidth;
}

int  FRE_GetHWEncodeHeight(void)
{
	if (g_nHWEncodeHeight == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nHWEncodeHeight=GetPrivateProfileInt(("VIDEC"), ("HWEncodeHeight") , 720, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nHWEncodeHeight);
		WritePrivateProfileString(("VIDEC"), ("HWEncodeHeight")  , strValue, chIniPath);
	}
	return g_nHWEncodeHeight;
}

int  FRE_GetDebugLevel(void)
{
	if (g_DebugLevel == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_DebugLevel=GetPrivateProfileInt(("VIDEC"), ("DebugLevel") , 0, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_DebugLevel);
		WritePrivateProfileString(("VIDEC"), ("DebugLevel")  , strValue, chIniPath);
	}
	return g_DebugLevel;
}

int  FRE_GetUseD3D(void)
{
	if (g_nNVD3D == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nNVD3D=GetPrivateProfileInt(("VIDEC"), ("UseD3D") , 1, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nNVD3D);
		WritePrivateProfileString(("VIDEC"), ("UseD3D")  , strValue, chIniPath);
	}
	return g_nNVD3D;
}

int  FRE_GetD3D11Decoder(void)
{
	if (g_nD3D11Decoder == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nD3D11Decoder=GetPrivateProfileInt(("VIDEC"), ("D3D11Decoder") ,2, chIniPath);

/*		char strValue[1024];
		sprintf(strValue,("%d"),g_nD3D11Decoder);
		WritePrivateProfileString(("VIDEC"), ("D3D11Decoder")  , strValue, chIniPath);	*/	
	}
	return g_nD3D11Decoder;
}

int  FRE_GetPending(void)
{
	if (g_nPending == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nPending=GetPrivateProfileInt(("VIDEC"), ("Pending") , 0, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nPending);
		WritePrivateProfileString(("VIDEC"), ("Pending")  , strValue, chIniPath);		
	}
	return g_nPending;
}

int  FRE_GetDelayMilliSecond(void)
{
	if (g_nDelayFrame == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nDelayFrame=GetPrivateProfileInt(("VIDEC"), ("DelayMilliSecond") , 0, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nDelayFrame);
		WritePrivateProfileString(("VIDEC"), ("DelayMilliSecond")  , strValue, chIniPath);		
	}
	return g_nDelayFrame;
}

int  FRE_GetDelayFrameRate(void)
{
	if (g_nDelayFrameRate == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nDelayFrameRate=GetPrivateProfileInt(("VIDEC"), ("DelayFrameRate") , 30, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nDelayFrameRate);
		WritePrivateProfileString(("VIDEC"), ("DelayFrameRate")  , strValue, chIniPath);		
	}
	return g_nDelayFrameRate;
}

int  FRE_GetD3D11Type(void)
{
	if (g_nD3D11Type == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nD3D11Type=GetPrivateProfileInt(("VIDEC"), ("D3D11Type") , 0, chIniPath);

/*		char strValue[1024];
		sprintf(strValue,("%d"),g_nD3D11Type);
		WritePrivateProfileString(("VIDEC"), ("D3D11Type")  , strValue, chIniPath);	*/	
	}
	return g_nD3D11Type;
}

int  FRE_GetD3D11Debug(void)
{
	if (g_nD3D11Debug == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nD3D11Debug=GetPrivateProfileInt(("VIDEC"), ("D3D11Debug") , 0, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nD3D11Debug);
		WritePrivateProfileString(("VIDEC"), ("D3D11Debug")  , strValue, chIniPath);
	}
	return g_nD3D11Debug;
}

int  FRE_GetSkipDisplay(void)
{
	if (g_nSkipDisplay == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nSkipDisplay=GetPrivateProfileInt(("VIDEC"), ("SkipDisplay") , 1, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nSkipDisplay);
		WritePrivateProfileString(("VIDEC"), ("SkipDisplay")  , strValue, chIniPath);
	}
	return g_nSkipDisplay;
}

int  FRE_GetPIT(void)
{
	if (g_nPIT == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nPIT=GetPrivateProfileInt(("VIDEC"), ("PIT") , 0, chIniPath);

		char strValue[1024];
		sprintf(strValue,("%d"),g_nPIT);
		WritePrivateProfileString(("VIDEC"), ("PIT")  , strValue, chIniPath);
	}
	return g_nPIT;
}

void FREIPC_Init(void)
{
	//add ipc
	if (FALSE == IsIPCInitd())
	{
		if (FALSE == IPCInit(FALSE,g_nVIDECIPCPort))
		{
		}
	}
	IPCRegistModule("VIDECFRE.dll");
	IPCTelAuthor("admin","videcfre");

	CreateIPCPool( );

	//NEWRECORD( m_gCycLogFile , new CCycLogFile() )  //??
	NEWMALLOC( m_gCycLogFile ,   CCycLogFile , () ) 
}

void FREIPC_Terminate(void)
{
	if(m_gCycLogFile)
	{
		//delete m_gCycLogFile;//??
		DELETEFREE(m_gCycLogFile);
		m_gCycLogFile =NULL;
	}

	//IPCCloseLogFile();
	IPCQuit(FALSE);
}


void FRE_Init(void)
{
	//add ipc
	//FREIPC_Init();


	FRE_GetDebugLevel();
	FRE_GetPIT();
	FRE_GetMaxDecoderCount();
	FRE_GetMaxEncoderCount();
	FRE_GetForceIntelEncoder();	
	FRE_GetHWDecodeWidth();
	FRE_GetHWDecodeHeight();	
	FRE_GetHWEncodeWidth();
	FRE_GetHWEncodeHeight();
	FRE_GetDelayMilliSecond();
	FRE_GetDelayFrameRate();
	FRE_GetPending();	
	FRE_GetUseD3D();
	FRE_GetD3D11Total();
	//FRE_GetD3D11Decoder();	
	//FRE_GetD3D11Type();
	FRE_GetD3D11Debug();
	FRE_GetSkipDisplay();

	//NEWRECORD( m_gCycLogFile , new CCycLogFile() )

}

void FRE_Terminate(void)
{

	{
		XAutoLock l(g_csFrameRateEventCtrl_10);
		if (g_pFrameRateEventCtrl_10)
		{
			g_pFrameRateEventCtrl_10->Close();
			delete g_pFrameRateEventCtrl_10;
			g_pFrameRateEventCtrl_10=NULL;
		}
	}	
	{
		XAutoLock l(g_csFrameRateEventCtrl_15);
		if (g_pFrameRateEventCtrl_15)
		{
			g_pFrameRateEventCtrl_15->Close();
			delete g_pFrameRateEventCtrl_15;
			g_pFrameRateEventCtrl_15=NULL;
		}
	}	
	{
		XAutoLock l(g_csFrameRateEventCtrl_20);
		if (g_pFrameRateEventCtrl_20)
		{
			g_pFrameRateEventCtrl_20->Close();
			delete g_pFrameRateEventCtrl_20;
			g_pFrameRateEventCtrl_20=NULL;
		}
	}		
	{
		XAutoLock l(g_csFrameRateEventCtrl_25);
		if (g_pFrameRateEventCtrl_25)
		{
			g_pFrameRateEventCtrl_25->Close();
			delete g_pFrameRateEventCtrl_25;
			g_pFrameRateEventCtrl_25=NULL;
		}
	}
	{
		XAutoLock l(g_csFrameRateEventCtrl_30);
		if (g_pFrameRateEventCtrl_30)
		{
			g_pFrameRateEventCtrl_30->Close();
			delete g_pFrameRateEventCtrl_30;
			g_pFrameRateEventCtrl_30=NULL;
		}
	}
	{
		XAutoLock l(g_csFrameRateEventCtrl_50);
		if (g_pFrameRateEventCtrl_50)
		{
			g_pFrameRateEventCtrl_50->Close();
			delete g_pFrameRateEventCtrl_50;
			g_pFrameRateEventCtrl_50=NULL;
		}
	}
	{
		XAutoLock l(g_csFrameRateEventCtrl_60);
		if (g_pFrameRateEventCtrl_60)
		{
			g_pFrameRateEventCtrl_60->Close();
			delete g_pFrameRateEventCtrl_60;
			g_pFrameRateEventCtrl_60=NULL;
		}
	}


	//FREIPC_Terminate();

}

//void * __cdecl operator new(size_t s)
//{
//	return HeapAlloc( GetProcessHeap(), 0, s );
//}


//void * FreMalloc(void * p,size_t size,char * fileName,char * func,int nLine)
//{
//	if(p == NULL)
//	{
//		DebugOutput(logcoreerror ,"Fre Malloc Failed,File:%s,Line:%d \n", fileName,  nLine);
//		//DebugOutput(logcoreerror ,"Fre Malloc Failed,File:%s,FUN:%s,Line:%d \n", fileName,   func,  nLine);
//	}
//
//	//DebugOutput(logcoreerror ,"Fre Malloc Pointer:%p,File:%s,FUN:%s,Line:%d \n", p, fileName,   func,  nLine);
//	return p;
//}


void DebugOutput(int nLevel, char* msg)
{
	if(nLevel <=g_nCoreLogLevel || nLevel ==g_nModuleLogLevel)
	{
		char header[64];
		sprintf(header,"  [LOG%d]  ",nLevel) ;
		DebugOutput(msg,header);
	}
}

void DebugOutput(char* msg,char * header)
{
	static char OutputBuffer[2048]={0};
	int nIndex=0;

	if(m_gCycLogFile)
	{
		nIndex = m_gCycLogFile->GetWriteHeader(OutputBuffer,2048,header);
		strcpy(OutputBuffer+nIndex,msg);
		m_gCycLogFile->WriteFile(OutputBuffer,strlen(OutputBuffer) );
	}

	IPCPrintf(1,0,msg);
}

void InnerDebugOutput(const char* fmt,va_list args,char * headerBuf)
{
	static char buffer1[4096]={0};
	int nIndex=0;
	if(m_gCycLogFile) nIndex = m_gCycLogFile->GetWriteHeader(buffer1,4096,headerBuf);

#ifdef WIN32
	_vsnprintf(buffer1+nIndex,sizeof(buffer1) -nIndex -1,fmt,args);
#else
	vsnprintf(buffer1+nIndex,sizeof(buffer1) -nIndex -1,fmt,args);
#endif
#ifdef WIN32
	//OutputDebugStringA(buffer1);
#endif

	if(m_gCycLogFile)
	{
		m_gCycLogFile->WriteFile(buffer1,strlen(buffer1) );
	}

	IPCPrintf(1,0,buffer1+nIndex);

}



void DebugOutput(const char* fmt,...)
{
	va_list args;
	va_start(args, fmt);
	InnerDebugOutput(fmt,args,NULL);
	va_end(args);
}



void DebugOutput(int nLevel,const char* fmt,...)
{
	if(nLevel <=g_nCoreLogLevel || nLevel ==g_nModuleLogLevel)
	{
	va_list args;
	va_start(args, fmt);
	static char header[128];
	sprintf(header,"  [LOG%d]  ",nLevel) ;
	InnerDebugOutput(fmt,args,header);
	va_end(args);
	}
}


void AddFrameRateEvent(FrameRateEvent*pFrameRateEvent,int nFrameRate)
{	
	if (nFrameRate < 1)
	{
		return;
	}
	if (nFrameRate == 10)
	{
		XAutoLock l(g_csFrameRateEventCtrl_10);
		if (g_pFrameRateEventCtrl_10==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_10 ,new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_10->Open();
			g_pFrameRateEventCtrl_10->SetFrameRate(10);
		}
		g_pFrameRateEventCtrl_10->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_10->SetPause(false);
	}
	else if (nFrameRate == 15)
	{
		XAutoLock l(g_csFrameRateEventCtrl_15);
		if (g_pFrameRateEventCtrl_15==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_15 ,new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_15->Open();
			g_pFrameRateEventCtrl_15->SetFrameRate(15);
		}
		g_pFrameRateEventCtrl_15->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_15->SetPause(false);
	}	
	else if (nFrameRate == 20)
	{
		XAutoLock l(g_csFrameRateEventCtrl_20);
		if (g_pFrameRateEventCtrl_20==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_20 ,new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_20->Open();
			g_pFrameRateEventCtrl_20->SetFrameRate(20);
		}
		g_pFrameRateEventCtrl_20->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_20->SetPause(false);
	}	
	else if (nFrameRate == 25)
	{
		XAutoLock l(g_csFrameRateEventCtrl_25);
		if (g_pFrameRateEventCtrl_25==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_25 , new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_25->Open();
			g_pFrameRateEventCtrl_25->SetFrameRate(25);
		}
		g_pFrameRateEventCtrl_25->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_25->SetPause(false);
	}
	else  if (nFrameRate == 30)
	{
		XAutoLock l(g_csFrameRateEventCtrl_30);
		if (g_pFrameRateEventCtrl_30==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_30 ,new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_30->Open();
			g_pFrameRateEventCtrl_30->SetFrameRate(30);
		}
		g_pFrameRateEventCtrl_30->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_30->SetPause(false);
	}
	else if (nFrameRate == 50)
	{
		XAutoLock l(g_csFrameRateEventCtrl_50);
		if (g_pFrameRateEventCtrl_50==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_50 ,new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_50->Open();
			g_pFrameRateEventCtrl_50->SetFrameRate(50);
		}
		g_pFrameRateEventCtrl_50->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_50->SetPause(false);
	}
	else // if (nFrameRate == 60)
	{
		XAutoLock l(g_csFrameRateEventCtrl_60);
		if (g_pFrameRateEventCtrl_60==NULL)
		{
			NEWRECORD( g_pFrameRateEventCtrl_60 ,new FrameRateEventCtrl() )
			g_pFrameRateEventCtrl_60->Open();
			g_pFrameRateEventCtrl_60->SetFrameRate(60);
		}
		g_pFrameRateEventCtrl_60->AddFrameRateEvent(pFrameRateEvent);
		g_pFrameRateEventCtrl_60->SetPause(false);
	}
}

void RemoveFrameRateEvent(FrameRateEvent*pFrameRateEvent,int nFrameRate)
{
	if (nFrameRate < 1)
	{
		return;
	}
	if (nFrameRate == 10)
	{
		XAutoLock l(g_csFrameRateEventCtrl_10);
		if (g_pFrameRateEventCtrl_10)
		{
			g_pFrameRateEventCtrl_10->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_10->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_10->SetPause(true);
			}
		}
	}
	else if (nFrameRate == 15)
	{
		XAutoLock l(g_csFrameRateEventCtrl_15);
		if (g_pFrameRateEventCtrl_15)
		{
			g_pFrameRateEventCtrl_15->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_15->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_15->SetPause(true);
			}
		}
	}	
	else if (nFrameRate == 20)
	{
		XAutoLock l(g_csFrameRateEventCtrl_20);
		if (g_pFrameRateEventCtrl_20)
		{
			g_pFrameRateEventCtrl_20->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_20->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_20->SetPause(true);
			}
		}
	}	
	else if (nFrameRate == 25)
	{
		XAutoLock l(g_csFrameRateEventCtrl_25);
		if (g_pFrameRateEventCtrl_25)
		{
			g_pFrameRateEventCtrl_25->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_25->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_25->SetPause(true);
			}
		}
	}
	else if (nFrameRate == 30)
	{
		XAutoLock l(g_csFrameRateEventCtrl_30);
		if (g_pFrameRateEventCtrl_30)
		{
			g_pFrameRateEventCtrl_30->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_30->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_30->SetPause(true);
			}
		}
	}
	else if (nFrameRate == 50)
	{
		XAutoLock l(g_csFrameRateEventCtrl_50);
		if (g_pFrameRateEventCtrl_50)
		{
			g_pFrameRateEventCtrl_50->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_50->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_50->SetPause(true);
			}
		}
	}
	else //if (nFrameRate == 60)
	{
		XAutoLock l(g_csFrameRateEventCtrl_60);
		if (g_pFrameRateEventCtrl_60)
		{
			g_pFrameRateEventCtrl_60->RemoveFrameRateEvent(pFrameRateEvent);
			if (g_pFrameRateEventCtrl_60->GetEventCount() == 0)
			{
				g_pFrameRateEventCtrl_60->SetPause(true);
			}
		}
	}
}

FrameRateEvent::FrameRateEvent(void)
{
	m_hEventFrameRate = CreateEvent(NULL,FALSE,FALSE,NULL);
	SetEvent(m_hEventFrameRate);
	m_nFrameRate=0;
}


FrameRateEvent::~FrameRateEvent(void)
{
	if (m_hEventFrameRate)
	{
		CloseHandle(m_hEventFrameRate);
		m_hEventFrameRate=NULL;
	}
}

int FrameRateEvent::Open(void)
{
	SetFrameRate(60);
	return 0;
}

void FrameRateEvent::Close(void)
{
	if (m_hEventFrameRate)
	{
		SetEvent(m_hEventFrameRate);
	}

	RemoveFrameRateEvent(this,m_nFrameRate);	
}

void FrameRateEvent::WaitEvent(void)
{
	if (m_hEventFrameRate)
	{
		WaitForSingleObject(m_hEventFrameRate,100);
	}
}

void FrameRateEvent::OnEvent(void)
{
	if (m_hEventFrameRate)
	{
		SetEvent(m_hEventFrameRate);
	}
}

void FrameRateEvent::SetFrameRate(int nFrameRate)
{
	if (nFrameRate > 0 && m_nFrameRate != nFrameRate)
	{
		if (FRE_GetDebugLevel() > 0)
		{
			char szDebug[1024];
			sprintf(szDebug,"FrameRateCtrl::SetFrameRate nFrameRate=%u this = %u\n",nFrameRate,this);
			DebugOutput( logthread ,szDebug);
		}
		
		if (m_nFrameRate == nFrameRate)
		{
			return;
		}

		RemoveFrameRateEvent(this,m_nFrameRate);
		AddFrameRateEvent(this,nFrameRate);
		m_nFrameRate = nFrameRate;
	}
}

#if USE_PERFORMANCE

int		g_nPresentationFREType = -1;

FrameRateCtrl::FrameRateCtrl(void)
{
	m_bRunning=false;
	m_bPause = false;
	m_nFrameRate=0;
	m_uTimeid=0;
	m_nStepCount = 0;
	m_nFrameInterval = 0;
	m_nStepFrameInterval = 0;

	m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	if (g_nPresentationFREType == -1)
	{
		char chIniPath[1024]="";
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		strcat(chIniPath,"\\VIDEC.ini");	

		g_nPresentationFREType=GetPrivateProfileInt(("VIDEC"), ("FRE") , 0, chIniPath);

		//char strValue[1024];
		//sprintf(strValue,("%d"),g_nPresentationFREType);
		//WritePrivateProfileString(("VIDEC"), ("FRE")  , strValue, chIniPath);
	}

}

FrameRateCtrl::~FrameRateCtrl(void)
{
	if (m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent=NULL;
	}
}

int FrameRateCtrl::Open(void)
{
	if (m_bRunning)
	{
		return 0;
	}
	if (m_hEvent==NULL)
	{
		return -1;
	}

	unsigned int nInterval = 1;
	unsigned int nPeriod = 1;
	unsigned int nTimerType;
	MMRESULT nResult;

	nResult = timeBeginPeriod( nPeriod );
	if( nResult != TIMERR_NOERROR )
	{
		return -1;
	}

	// 启动多媒体定时器(多媒体定时器运行在它自己的线程中)
	nTimerType = 0;//TIME_ONESHOT//TIME_PERIODIC
	nTimerType |= TIME_PERIODIC;
	nTimerType |= TIME_CALLBACK_FUNCTION;
	nResult = timeSetEvent(nInterval, nPeriod, (LPTIMECALLBACK)RTTimmerFunc, (ULONG_PTR)this, nTimerType);
	if( nResult == NULL )
	if( nResult == NULL )
	{
		DWORD dwLastError = GetLastError();

		nResult = timeEndPeriod( nPeriod );
		if( nResult != TIMERR_NOERROR )
		{
		}
		return -1;
	}
	m_uTimeid = nResult;

	m_hirestiemr.Init();

	m_bRunning=true;
	if (!StartThread())
	{
		m_bRunning=false;
		return -1;
	}

	return 0;
}

void FrameRateCtrl::Close(void)
{
	m_bRunning=false;

	if (m_hEvent)
	{
		SetEvent(m_hEvent);
	}
	WaitForStop();

	if(m_uTimeid)
	{		
		timeEndPeriod(1);
		timeKillEvent(m_uTimeid);
		m_uTimeid = 0;
	}
}

void FrameRateCtrl::SetFrameRate(unsigned int nFrameRate)
{	
	m_nFrameRate=nFrameRate;
	m_nFrameInterval = 1000/nFrameRate;
	if (m_nFrameRate == 30)
	{
		m_nStepCount = 2;
		m_nFrameInterval = 33;
		m_nStepFrameInterval = 34;
	}
	else if (m_nFrameRate == 60)
	{
		m_nStepCount = 5;
		m_nFrameInterval = 17;
		m_nStepFrameInterval = 15;
	}
	else if (m_nFrameRate == 15)
	{
		m_nStepCount = 14;
		m_nFrameInterval = 67;
		m_nStepFrameInterval = 62;
	}
}

unsigned int FrameRateCtrl::GetFrameRate(void)
{
	return m_nFrameRate;
}

void FrameRateCtrl::OnTimer(void)
{
 	if (m_hEvent)
 	{
 		SetEvent(m_hEvent);
 	}
}

void FrameRateCtrl::SetPause(bool bPause)
{
	m_bPause = bPause;
}

void FrameRateCtrl::ThreadProcMain(void)
{
	unsigned long count = 0;
	unsigned long ulTime = timeGetTime();
	int nTimerTotal = 0;
	int nStepIndex = 0;
	int nChangeFPS=0;
	while (m_bRunning) {
		if (m_bPause)
		{
			Sleep(1);
			continue;
		}
		if (m_hEvent)
		{
			WaitForSingleObject(m_hEvent,100);
		}
		nTimerTotal++;

		if (m_hirestiemr.IsInit() && g_nPresentationFREType == 0)
		{
// 			if (nChangeFPS > 0)
// 			{
// 				if (nTimerTotal < 14)
// 				{	
// 					continue;
// 				}
// 				nChangeFPS--;
// 				m_hirestiemr.LockFPS(60,nChangeFPS);
// 			}
// 			else
			{
				if (nTimerTotal < (m_nFrameInterval - 2))
				{	
					continue;
				}
				nChangeFPS=0;
				m_hirestiemr.LockFPS(m_nFrameRate,nChangeFPS);
			}
#if (0)			
			if ((++count) == m_nFrameRate*20)
			{		
				unsigned long ulTime2 = timeGetTime();			
				char szDebug[1024];
				sprintf(szDebug,"FrameRateCtrl::ThreadProcMain Time=%u nFrameRate=%u this=%u\n",ulTime2 - ulTime,m_nFrameRate,this);
				DebugOutput( logthread ,szDebug);			
				ulTime = ulTime2;
				count=0;
			}
#endif
			OnFrameRate();
			nTimerTotal = 0;
		}
		else
		{
			if (m_nStepCount)
			{
				if (nStepIndex>=m_nStepCount)
				{
					if (nTimerTotal>=m_nStepFrameInterval)
					{
						nStepIndex=0;
						OnFrameRate();
						nTimerTotal=0;
					}
				}
				else
				{
					if (nTimerTotal>= m_nFrameInterval)
					{
						nStepIndex++;
						OnFrameRate();
						nTimerTotal=0;
					}
				}
			}
			else
			{
				if (nTimerTotal>= m_nFrameInterval)
				{
					OnFrameRate();
					nTimerTotal=0;
				}
			}
		}
	}
}

unsigned int __stdcall FrameRateCtrl::RTTimmerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	FrameRateCtrl*pRTTimer=(FrameRateCtrl*)dwUser;
	pRTTimer->OnTimer();
	return 0;
}


#else

FrameRateCtrl::FrameRateCtrl(void)
{
	m_nPeriod=1;
	m_bRunning=false;
	m_nTimerTotal=0;
	m_nStepIndex=0;
	m_nStepCount=0;
	m_nFrameRate=0;
	m_hSemaphore = CreateSemaphore(NULL,0,0xffff,NULL);//初时值为0
}

FrameRateCtrl::~FrameRateCtrl(void)
{
	CloseHandle(m_hSemaphore);
	m_hSemaphore = NULL;
}

int FrameRateCtrl::Open(void)
{
	if (m_bRunning)
	{
		return 0;
	}

	if (m_hSemaphore==NULL)
	{
		return -1;
	}

	RTTimer::Instance().AddFrameRateCtrl(this);

	m_nPeriod=RTTimer::Instance().GetPeriod();

	m_bRunning=true;
	if (!StartThread())
	{
		m_bRunning=false;
		return -1;
	}


	return 0;
}

void FrameRateCtrl::Close(void)
{
	RTTimer::Instance().RemoveFrameRateCtrl(this);

	m_bRunning=false;
	if (m_hSemaphore)
	{
		ReleaseSemaphore(m_hSemaphore,1,NULL);
	}
	WaitForStop();
}

void FrameRateCtrl::SetFrameRate(unsigned int nFrameRate)
{
	m_nFrameRate=nFrameRate;
	m_nFrameInterval=1000/m_nFrameRate;

	if (m_nPeriod==1 && nFrameRate==30)
	{
		m_nStepCount=2;
	}
	else
	{
		m_nStepCount=0;
	}
	m_nStepIndex=0;
}

unsigned int FrameRateCtrl::GetFrameRate(void)
{
	return m_nFrameRate;
}

void FrameRateCtrl::OnTimer(void)
{
	if (m_hSemaphore)
	{
		ReleaseSemaphore(m_hSemaphore,1,NULL);
	}
}

void FrameRateCtrl::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		DWORD dwRet = WaitForSingleObject(m_hSemaphore, INFINITE);
		switch (dwRet)
		{
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
			{
			}
			break;
		case WAIT_OBJECT_0:
			{
				m_nTimerTotal+=m_nPeriod;
				if (m_nTimerTotal>=m_nFrameInterval)
				{
					if (m_nStepCount)
					{
						m_nStepIndex++;
						if (m_nStepIndex>m_nStepCount)
						{
							if (m_nTimerTotal>=(m_nFrameInterval+1))
							{
								m_nStepIndex=0;
								OnFrameRate();
								m_nTimerTotal=0;
							}
						}
						else
						{
							OnFrameRate();
							m_nTimerTotal=0;
						}
					}
					else
					{
						OnFrameRate();
						m_nTimerTotal=0;
					}
				}
			}
			break;
		}
	}
}


static RTTimer g_RTTimer;
RTTimer&RTTimer::Instance(void)
{
	return g_RTTimer;
}

RTTimer::RTTimer(void)
{
	m_nTimerID=0;
	m_nPeriod=0;
}

RTTimer::~RTTimer(void)
{

}

int RTTimer::Start(void)
{
	if (m_nTimerID)
	{
		return 0;
	}

	TIMECAPS	 tTimeCaps;
	unsigned int nInterval;
	unsigned int nPeriod;
	unsigned int nTimerType;
	MMRESULT nResult;

	// 取得系统定时器的能力
	nResult = timeGetDevCaps( &tTimeCaps, sizeof(TIMECAPS) );
	if( nResult != TIMERR_NOERROR )
	{
		return -1;
	}

	nInterval = 1;
	if( nInterval < tTimeCaps.wPeriodMin )
	{
		nInterval = tTimeCaps.wPeriodMin;
	}
	if( nInterval > tTimeCaps.wPeriodMax )
	{
		nInterval = tTimeCaps.wPeriodMax;
	}

	// 为我们的应用程序设置定时器的最高精度
	nPeriod = min( max(tTimeCaps.wPeriodMin,1), tTimeCaps.wPeriodMax );
	nResult = timeBeginPeriod( nPeriod );
	if( nResult != TIMERR_NOERROR )
	{
		return -1;
	}

	// 启动多媒体定时器(多媒体定时器运行在它自己的线程中)
	nTimerType = 0;//TIME_ONESHOT//TIME_PERIODIC
	nTimerType |= TIME_PERIODIC;
	nTimerType |= TIME_CALLBACK_FUNCTION;
	nResult = timeSetEvent(nInterval,nPeriod,(LPTIMECALLBACK)RTTimmerFunc,(ULONG_PTR)this,nTimerType);
	if( nResult == NULL )
	{
		DWORD dwLastError = GetLastError();

		nResult = timeEndPeriod( nPeriod );
		if( nResult != TIMERR_NOERROR )
		{
		}

		return -1;
	}

	m_nTimerID = nResult;
	m_nPeriod = nPeriod;

	return 0;
}

void RTTimer::Stop(void)
{
	if ( m_nTimerID )
	{
		timeKillEvent( m_nTimerID );
		m_nTimerID = 0;

	}

	if ( m_nPeriod )
	{
		timeEndPeriod( m_nPeriod );
		m_nPeriod = 0;
	}
}

unsigned int RTTimer::GetPeriod(void)
{
	return m_nPeriod;
}

void RTTimer::OnTimer(void)
{
	XAutoLock l(m_csMapFrameRateCtrl);
	MapFrameRateCtrl::iterator iter;
	for (iter=m_MapFrameRateCtrl.begin();iter!=m_MapFrameRateCtrl.end();++iter)
	{
		FrameRateCtrl*pFrameRateCtrl=iter->second;
		pFrameRateCtrl->OnTimer();
	}
}

void RTTimer::AddFrameRateCtrl(FrameRateCtrl*pFrameRateCtrl)
{
	{
		XAutoLock l(m_csMapFrameRateCtrl);
		m_MapFrameRateCtrl[pFrameRateCtrl]=pFrameRateCtrl;
	}

	RTTimer::Instance().Start();
}

void RTTimer::RemoveFrameRateCtrl(FrameRateCtrl*pFrameRateCtrl)
{
	{
		XAutoLock l(m_csMapFrameRateCtrl);
		m_MapFrameRateCtrl.erase(pFrameRateCtrl);
	}
	if (m_MapFrameRateCtrl.size()==0)
	{
		RTTimer::Instance().Stop();
	}
}



unsigned int __stdcall RTTimer::RTTimmerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	RTTimer*pRTTimer=(RTTimer*)dwUser;
	pRTTimer->OnTimer();
	return 0;
}

#endif