//////////////////////////////////////////////////////////////////////////
// ˵����ʹ�ô˿�֮ǰ������س�ʼ�� COM ����⣬����
// Dialog::OnInitDialog() ����ӣ�
// CoInitialize(NULL);
// ���򣬲���¼��


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
    E_MMV_V9_ADV,       // Windows Media Video v9 Advanced Profile (deprecated as not VC-1����ȫ���ݡ�)
    E_MMV_V9_ADV2,      // Windows Media Video v9 Advanced Profile (VC-1����֧��)
    E_WMV_V7_SCREEN,    // Windows Media Video v7 Screen (ר����Ե�����Ļץȡ�Ż�)
    E_WMV_V9_SCREEN,    // Windows Media Video v9 Screen 
    E_WMV_V9_IMAGE,     // Windows Media Video v9 Image 
    E_WMV_V9_1_IMAGE,   // Windows Media Video v9.1 Image 
}EFourCC_Desc;

typedef struct tag_CapParam 
{
    tag_CapParam()
    {
        eFourCC = E_MMV_V9;									// Ĭ���� Windows Media Video v9 ����
        eContent_type = WMENC_CONTENT_ONE_AUDIO_ONE_VIDEO;  // Ĭ��Ϊ¼������Ƶ
        ulBitRate = 512;									// Ĭ��Ϊ 512kbps ������
        usFrameRate = 15;                                   // Ĭ��Ϊ 15֡/��
        usImageQuality = 100;                               // Ĭ��ͼ��������100
		usWidth = 640;										// Ĭ����Ļ�����640��0�򱣳ֲ���
		usHeight = 480;										// Ĭ����Ļ�߶ȣ�480��0�򱣳ֲ���
    }
    EFourCC_Desc eFourCC;						 // �����ʽ����
    WMENC_MEDIA_CONTENT eContent_type;			 // WMENC_CONTENT_ONE_VIDEO              : ����Ƶ
												 // WMENC_CONTENT_ONE_AUDIO_ONE_VIDEO    : ����Ƶ
    unsigned long ulBitRate;                     // �����ʣ���λ�� bps
    unsigned short usFrameRate;                  // ֡�ʣ���λ�� ֡/��
    unsigned short usImageQuality;               // ͼ��������1~100    

	unsigned short usWidth;						 // ��Ļ���
	unsigned short usHeight;					 // ��Ļ�߶�
}TCapParam, *PTCapParam;	

AFX_EXT_CLASS BOOL StartCap();							// ��ʼ¼��
AFX_EXT_CLASS BOOL StopCap();                           // ֹͣ¼��
AFX_EXT_CLASS BOOL PauseCap();                          // ��ͣ¼��
AFX_EXT_CLASS BOOL ResumeCap();                         // �ָ�¼��

AFX_EXT_CLASS BOOL SetupProfile( PTCapParam ptCapParam);//���ø�ʽ
AFX_EXT_CLASS BOOL SetupLive(std::string strServerName, unsigned short usPort, std::string strPubPoint); // ����ֱ��
AFX_EXT_CLASS BOOL SaveFile(LPCSTR szOutputFileName);	// �����ļ�,¼���ļ���ȫ������ʽֻ���� *.wmv���� D:\output.wmv


