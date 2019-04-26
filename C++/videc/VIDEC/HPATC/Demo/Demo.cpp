// Demo.cpp : 定义应用程序的入口点。
//

#include "Demo.h"
#include "player.h"
#include <string.h> 

void*		g_mkvHandle = NULL;


void VIDECMP4CallbackFilefull(const void*pUserData,void*pHandle,unsigned long ulFileTime,unsigned long ulFileSize)
{
	//printf("Demo => Total Time：%02d:%02d:%02d Total Size：%d \n",ulFileTime/(60*60),(ulFileTime%(60*60))/60,ulFileTime%60,ulFileSize);
}

int main(int argc,char* argv[])
{
ReInit:
	printf(" Input 'R' or 'D' Or 'E',R:Record File; D:Demux File; E:Exit!\n>");
	char line[256]={0};
	gets(line);
	if (strcmp(line,"E")==0 || strcmp(line,"e")==0)
	{
		goto DEMO_EXIT;
	}
	if (strcmp(line,"D")==0 || strcmp(line,"d")==0)
	{
ReDO:
		printf(" Input Demux Source File Path:\n>");
		gets(line);
		
		unsigned long ulTime  = XGetTimestamp();

#ifdef _DEBUG
#ifdef _WIN32
		char chPath[1024] = {0};
		sprintf(chPath,"D:/Work_Item/HPAVP/out_demux.mp4");
#else
		char chPath[1024] = {0};		
		sprintf(chPath,"/mnt/hgfs/HPAVP/out_demux_linux.mp4");
#endif // _WIN32
#else
#ifdef _WIN32
		char chPath[1024] = {0};
		sprintf(chPath,"D:/Work_Item/HPAVP/out_demux_%u.mp4",ulTime);
#else
		char chPath[1024] = {0};		
		sprintf(chPath,"/mnt/hgfs/HPAVP/out_demux_linux_%u.mp4",ulTime);
#endif // _WIN32
#endif // _DEBUG



		ulTime = XGetTimestamp();
		if (VIDEC_MP4_Demux_Open(line,chPath) != 0)
		{
			VIDEC_MP4_Demux_Close();
			printf(" File Path Error\n>");
			goto ReDO;
		}
		unsigned long ulTime2 = XGetTimestamp();
		ulTime2 = (ulTime2 - ulTime)/1000;
		printf("Demo => Demux_Open Time：%02d:%02d:%02d\n",ulTime2/(60*60),(ulTime2%(60*60))/60,ulTime2%60);

		ulTime = XGetTimestamp();
		char* pFileInfo = VIDEC_MP4_Demux_GetTrackInfo();
		printf("FileInfo:%s\n>",pFileInfo);
		int nTrackCount = VIDEC_MP4_Demux_GetTrackCount();
		for (int i = 1; i<= nTrackCount;i++)
		{
			printf("Track ID=%d  Type=%s\n>",i,VIDEC_MP4_Demux_GetTrackType(i));
		}

		ulTime2 = XGetTimestamp();
		ulTime2 = (ulTime2 - ulTime)/1000;
		printf("Demo => GetTrackInfo Time：%02d:%02d:%02d\n",ulTime2/(60*60),(ulTime2%(60*60))/60,ulTime2%60);

INPUT_TRACKID:
		printf(" Input TrackID  Or Input 's' Save File\n>");
		gets(line);
		if (!(strcmp(line,"s")==0 || strcmp(line,"S")==0))
		{
			int nTrackID = atol(line);
			ulTime = XGetTimestamp();
			if (0 < nTrackID && nTrackID <= nTrackCount)
			{
				VIDEC_MP4_Demux_AddTrackToDstFile(nTrackID);
			}
			ulTime2 = XGetTimestamp();
			ulTime2 = (ulTime2 - ulTime)/1000;
			printf("Demo => AddTrack Time：%02d:%02d:%02d\n",ulTime2/(60*60),(ulTime2%(60*60))/60,ulTime2%60);
			goto INPUT_TRACKID;
		}

		ulTime = XGetTimestamp();

		VIDEC_MP4_Demux_Close();

		ulTime2 = XGetTimestamp();
		ulTime2 = (ulTime2 - ulTime)/1000;
		printf("Demo => Demux_Close Time：%02d:%02d:%02d\n",ulTime2/(60*60),(ulTime2%(60*60))/60,ulTime2%60);

		goto ReInit;
	}
	else if (strcmp(line,"R")==0 || strcmp(line,"r")==0)
	{
		NETEC_Core::Start();
		//MP4
		if (g_mkvHandle == NULL/* && m_nVideoWidth != 0*/)
		{
			unsigned long ulTime  = XGetTimestamp();

#ifdef _DEBUG
#ifdef _WIN32
			char chPath[1024] = {0};
			sprintf(chPath,"D:/Work_Item/HPAVP/out.mp4");
#else
			char chPath[1024] = {0};		
			sprintf(chPath,"/mnt/hgfs/HPAVP/out_linux.mp4");
#endif // _WIN32
#else
#ifdef _WIN32
			char chPath[1024] = {0};
			sprintf(chPath,"D:/Work_Item/HPAVP/out_%u.mp4",ulTime);
#else
			char chPath[1024] = {0};		
			sprintf(chPath,"/mnt/hgfs/HPAVP/out_linux_%u.mp4",ulTime);
#endif // _WIN32
#endif // _DEBUG
			g_mkvHandle = VIDEC_MP4_Open(chPath);

			if (g_mkvHandle)
			{
				VIDEC_MP4_SetFileFullCallback(NULL,g_mkvHandle,VIDECMP4CallbackFilefull);
			}
		}
		printf("    STOP: Stop Rrecord.\n");
		printf("    TEXT: Input Text Track.\n");
		printf("    TIME: Input Rrecord Time.\n");
		printf("    MAXTIME: Input File Max Time.\n");
		printf("    MAXSIZE: Input File Max Size.\n");
		printf("    Orther: AV parameters.\n>");

		unsigned long ulTime = 0;
		int nCount = 0;
		std::map<int,player*> map_player;

		if (argc >1 && strlen(argv[1]) > 10)
		{
			player *pPlayer = new player;
			pPlayer->Start(argv[1],nCount);	

			printf("Demo => AV_%02d  parameters:%s\n>",nCount,argv[1]);
			map_player[nCount++] = pPlayer;
			if (ulTime == 0)
			{
				ulTime = XGetTimestamp();
			}
		}		

		char chTime[1024]= {0};
		unsigned long ulTotalTime = 0;
		while(true)
		{
			gets(line);

			std::string stdTemp = line;

			//char* szPrompt=strupr(line);
			if (strcmp(line,"MAXTIME")==0 || strcmp(line,"MAXSIZE")==0 || strcmp(line,"maxtime")==0 || strcmp(line,"maxsize")==0)
			{
				if (strcmp(line,"MAXTIME")==0 || strcmp(line,"maxtime")==0)
				{
					printf("Input File Max Time:\n>");
					gets(line);
					VIDEC_MP4_SetMaxTimeSize(atol(line),0);
					printf(">");
				}
				else
				{
					printf("Input File Max Size:\n>");
					gets(line);
					VIDEC_MP4_SetMaxTimeSize(0,atol(line)*1024*1024);
					printf(">");
				}
			}
			else if(strcmp(line,"STOP")==0 || strcmp(line,"stop")==0 || strcmp(line,"TIME")==0 || strcmp(line,"time")==0 || strcmp(line,"TEXT")==0 || strcmp(line,"text")==0)
			{
				if (strcmp(line,"TEXT")==0 || strcmp(line,"text")==0)
				{
					printf("Input Text Track:\n>");
					gets(line);
					if (g_mkvHandle)
					{
						VIDEC_MP4_WriteText(g_mkvHandle,line);
					}
					printf(">");
				}
				else
				{
					if (strcmp(line,"TIME")==0 || strcmp(line,"time")==0)
					{
						printf("Input:Record Time:\n>");
						gets(line);
						ulTotalTime = atol(line);
						if (ulTotalTime < 1 || ulTotalTime > 2*60*60)
						{
							ulTotalTime = 60*60;
						}			
						sprintf(chTime,"%02d:%02d:%02d",ulTotalTime/(60*60),(ulTotalTime%(60*60))/60,ulTotalTime%60);
						printf("Demo => Total Time：%s\n",chTime);
					}
					break;
				}
			}
			else 
			{
				if (stdTemp.length() > 10)
				{				
					player *pPlayer = new player;
					pPlayer->Start(stdTemp.c_str(),nCount);	
					printf("Demo => AV_%02d  parameters:%s\n>",nCount,line);
					map_player[nCount++] = pPlayer;
					if (ulTime == 0)
					{
						ulTime = XGetTimestamp();
					}			
				}
				else
				{
					printf(">");
				}
			}
		}

		if (ulTotalTime > 0)
		{
			unsigned long ulIndex = 0;
			//unsigned long ulTime = XGetTimestamp();
			unsigned long ulTime2 = ulTime;
			unsigned long ulTime3 = ulTime;
			while(ulTime2 - ulTime <= ulTotalTime*1000)
			{
				if(ulTime2 - ulTime3 >= 1000)
				{
					ulTime3 = ulTime2;

					ulIndex = (ulTime2 - ulTime)/1000;
					printf("Demo => Total Time：%s ，Current Time：%02d:%02d:%02d\n",chTime,ulIndex/(60*60),(ulIndex%(60*60))/60,ulIndex%60);
				}
				ulTime2 = XGetTimestamp();
				XSleep(1);
			}
			ulIndex = (ulTime2 - ulTime)/1000;
			printf("Demo => Total Time：%s ，Current Time：%02d:%02d:%02d\n",chTime,ulIndex/(60*60),(ulIndex%(60*60))/60,ulIndex%60);
		}

		printf("Demo => player::StopRecord\n");
		if (g_mkvHandle)
		{
			void* mkvHandle = g_mkvHandle;
			g_mkvHandle = NULL;
			VIDEC_MP4_Close(mkvHandle);
			mkvHandle = NULL;
		}

		if (ulTime != 0)
		{
			unsigned long ulTime2 = XGetTimestamp();
			ulTime2 = (ulTime2 - ulTime)/1000;

			printf("Demo => Total Time：%02d:%02d:%02d\n",ulTime2/(60*60),(ulTime2%(60*60))/60,ulTime2%60);
		}


		std::map<int,player*>::iterator it = map_player.begin();
		while(it != map_player.end())
		{
			player *pPlayer = it->second;
			if (pPlayer)
			{
				pPlayer->Stop();
				delete pPlayer;
				pPlayer = NULL;
			}
			it++;
		}
		map_player.clear();


		goto ReInit;
	}
	else
	{
		goto ReInit;
	}
DEMO_EXIT:
	printf("Demo => exit!\n");

	//printf("Demo => NETEC_Node::Stop\n");
	NETEC_Node::Stop();
	//printf("Demo => NETEC_Core::Stop\n");
	NETEC_Core::Stop();	


#ifdef _WIN32
#ifndef _DEBUG
	system("pause");
#endif // _DEBUG
#endif // _WIN32
	return 0;
}

