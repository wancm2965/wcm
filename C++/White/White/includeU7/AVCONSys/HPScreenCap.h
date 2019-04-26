//////////////////////////////////////////////////////////////////////////
// 说明：使用此库之前，请务必初始化 COM 组件库，如在
// Dialog::OnInitDialog() 中添加：
// CoInitialize(NULL);
// 否则，不能录像


#pragma once
#include <wmencode.h>
#include <string>
using namespace std;
//////////////////////////////////////////////////////////////////////////
// typedef
typedef enum e_fourcc_description
{
    E_WMV_V7,           // Windows Media Video v7
    E_MPEG_4_V3,        // Microsoft MPEG-4 Video Codec v3
    E_MMV_V8,           // Windows Media Video v8
    E_MPEG_4_V2,        // Microsoft MPEG-4 Video Codec v2
    E_MPEG_4_V1,        // Microsoft ISO MPEG-4 Video Codec v1
    E_MMV_V9,           // Windows Media Video v9
    E_MMV_V9_ADV,       // Windows Media Video v9 Advanced Profile (deprecated as not VC-1不完全相容。)
    E_MMV_V9_ADV2,      // Windows Media Video v9 Advanced Profile (VC-1完整支持)
    E_WMV_V7_SCREEN,    // Windows Media Video v7 Screen (专门针对低速屏幕抓取优化)
    E_WMV_V9_SCREEN,    // Windows Media Video v9 Screen 
    E_WMV_V9_IMAGE,     // Windows Media Video v9 Image 
    E_WMV_V9_1_IMAGE,   // Windows Media Video v9.1 Image 
}EFourCC_Desc;

typedef struct tag_CapParam 
{
    tag_CapParam()
    {
        eFourCC = E_MMV_V9;									// 默认用 Windows Media Video v9 编码
        eContent_type = WMENC_CONTENT_ONE_AUDIO_ONE_VIDEO;  // 默认为录制视音频
        ulBitRate = 512;									// 默认为 512kbps 比特率
        usFrameRate = 15;                                   // 默认为 15帧/秒
        usImageQuality = 100;                               // 默认图像质量：100
		usWidth = 640;										// 默认屏幕宽带：640，0则保持不变
		usHeight = 480;										// 默认屏幕高度：480，0则保持不变
    }
    EFourCC_Desc eFourCC;						 // 编码格式描述
    WMENC_MEDIA_CONTENT eContent_type;			 // WMENC_CONTENT_ONE_VIDEO              : 仅视频
												 // WMENC_CONTENT_ONE_AUDIO_ONE_VIDEO    : 视音频
    unsigned long ulBitRate;                     // 比特率，单位是 bps
    unsigned short usFrameRate;                  // 帧率，单位是 帧/秒
    unsigned short usImageQuality;               // 图像质量，1~100    

	unsigned short usWidth;						 // 屏幕宽带
	unsigned short usHeight;					 // 屏幕高度
}TCapParam, *PTCapParam;	

AFX_EXT_CLASS BOOL StartCap();							// 开始录像
AFX_EXT_CLASS BOOL StopCap();                           // 停止录像
AFX_EXT_CLASS BOOL PauseCap();                          // 暂停录像
AFX_EXT_CLASS BOOL ResumeCap();                         // 恢复录像

AFX_EXT_CLASS BOOL SetupProfile( PTCapParam ptCapParam);//设置格式
AFX_EXT_CLASS BOOL SetupLive(std::string strServerName, unsigned short usPort, std::string strPubPoint); // 设置直播
AFX_EXT_CLASS BOOL SaveFile(LPCSTR szOutputFileName);	// 保存文件,录像文件名全名，格式只允许 *.wmv，例 D:\output.wmv


