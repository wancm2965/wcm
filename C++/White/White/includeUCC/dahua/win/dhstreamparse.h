#ifndef INCLUDED_DHSTREAMPARSE_H
#define INCLUDED_DHSTREAMPARSE_H

#ifdef WIN32
#define DHSTREAMPARSER_API __declspec(dllexport)
#define CALLMETHOD __stdcall
#else
#define __stdcall
#define CALLMETHOD
#define DHSTREAMPARSER_API
#endif

#define SAMPLE_FREQ_4000	1
#define SAMPLE_FREQ_8000	2
#define SAMPLE_FREQ_11025	3
#define SAMPLE_FREQ_16000	4
#define SAMPLE_FREQ_20000	5
#define SAMPLE_FREQ_22050	6
#define SAMPLE_FREQ_32000	7
#define SAMPLE_FREQ_44100	8
#define SAMPLE_FREQ_48000	9

#define INFOCOUNT_MAX_SIZE		200
#define MAX_BUFFER_SIZE 1028196

// Stream type:
#define DH_STREAM_UNKNOWN			0
#define DH_STREAM_MPEG4				1
#define DH_STREAM_DHPT				3
#define DH_STREAM_NEW				4
#define DH_STREAM_HB				5
#define DH_STREAM_AUDIO				6
#define  DH_STREAM_PS              7
#define  DH_STREAM_DHSTD            8
#define  DH_STREAM_ASF              9
// Frame Type and SubType
#define DH_FRAME_TYPE_UNKNOWN		0
#define DH_FRAME_TYPE_VIDEO			1
#define DH_FRAME_TYPE_AUDIO			2
#define DH_FRAME_TYPE_DATA			3

#define DH_FRAME_TYPE_VIDEO_I_FRAME	0
#define DH_FRAME_TYPE_VIDEO_P_FRAME	1
#define DH_FRAME_TYPE_VIDEO_B_FRAME	2
#define DH_FRAME_TYPE_VIDEO_S_FRAME	3
#define DH_FRAME_TYPE_DATA_TEXT    5
#define DH_FRAME_TYPE_DATA_INTL    6

// Encode type:
#define DH_ENCODE_UNKNOWN				0
#define DH_ENCODE_VIDEO_MPEG4			1
#define DH_ENCODE_VIDEO_HI_H264			2
#define DH_ENCODE_VIDEO_JPEG             3
#define DH_ENCODE_VIDEO_H264			4
#define DH_ENCODE_AUDIO_PCM8			7	// 8BITS,8K
#define DH_ENCODE_AUDIO_G729			8
#define DH_ENCODE_AUDIO_IMA				9
#define DH_ENCODE_AUDIO_PCM_MULAW		10
#define DH_ENCODE_AUDIO_G721			11
#define DH_ENCODE_AUDIO_PCM8_VWIS		12	// 16BITS,8K
#define DH_ENCODE_AUDIO_ADPCM			13	// 16BITS,8K/16K
#define DH_ENCODE_AUDIO_G711A			14	// 16BITS,8K
#define DH_ENCODE_AUDIO_TALK           30
#define STREAMPARSER_RESET_REFIND       0
#define STREAMPARSER_RESET_CONTINUE     1

//	ADI 264 resolution  

typedef struct 
{
	unsigned char* pHeader;  //����ͷ������ָ��
	unsigned char* pContent; //�������ݵ�ƫ��
	unsigned long nLength; //���ݳ���(����ͷ��)
	unsigned long nFrameLength; //���ݳ���.
	
	unsigned int nType; // VIDEO, AUDIO, DATA
	unsigned int nSubType; // I-FRAME, P-FRAME, etc.
	
	unsigned int nEncodeType; // MPEG4/H264, PCM, MSADPCM, etc.
	// ֻ��I֡���е�����
	unsigned long nYear;
	unsigned long nMonth;
	unsigned long nDay;
	unsigned long nHour;
	unsigned long nMinute;
	unsigned long nSecond;
	unsigned long nTimeStamp;
	
	unsigned int  nFrameRate;	//֡��
	int			  nWidth;  //ͼ����
	int			  nHeight; //ͼ��߶�
	unsigned long nRequence; //���к�
	// ��Ƶ���е�����
	unsigned int nChannels;
	unsigned int nBitsPerSample;
	unsigned int nSamplesPerSecond;
	
	unsigned long nParam1;		// ��չ��
	unsigned long nParam2;		// ��չ��
	
} DH_FRAME_INFO;

typedef void * PARSERHANDLE;
typedef unsigned long PARSERMSG;

#define STREAM_MSG_ERROR			1		// ��������
#define STREAM_DISCONTINUITY		2		// ʱ�䲻����
#define STREAM_ENCODE_CHANGED		3		// ͼ�������
#define STREAM_SIZE_CHANGED			4		// ������С���
#define STREAM_FRAMERATE_CHANGED	5		// ֡�ʷ����仯
#define STREAM_MSG_GET_ONE_FRAME	9		// ��ȡһ֡����(�첽)

typedef unsigned long (__stdcall * DHSP_CALLBACK)( \
	PARSERHANDLE hHandle, \
	PARSERMSG msg, \
	unsigned long dwParam1, \
	unsigned long dwParam2, \
	unsigned long dwUserData);


	
DHSTREAMPARSER_API PARSERHANDLE CALLMETHOD
DHSP_Init(DHSP_CALLBACK msg_cb, unsigned long dwUser);

DHSTREAMPARSER_API int CALLMETHOD
DHSP_Free(PARSERHANDLE hHandle);

DHSTREAMPARSER_API int CALLMETHOD
DHSP_InputData(PARSERHANDLE hHandle, unsigned char *byData, unsigned long dwLength);

DHSTREAMPARSER_API DH_FRAME_INFO * CALLMETHOD
DHSP_GetNextFrame(PARSERHANDLE hHandle);

DHSTREAMPARSER_API DH_FRAME_INFO * CALLMETHOD
DHSP_GetNextKeyFrame(PARSERHANDLE hHandle);

DHSTREAMPARSER_API void CALLMETHOD
DHSP_Reset(PARSERHANDLE hHandle, int nFlag);

#endif // INCLUDED_DHSTREAMPARSE_H
