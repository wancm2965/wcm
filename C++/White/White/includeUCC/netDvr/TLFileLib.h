#ifndef _TLFILE_H_
#define _TLFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void * TLFILE_t; 
typedef int BOOL;
typedef unsigned char BYTE;

#ifndef NULL
#define NULL 0
#endif
enum TL_FILE_OPENMODE{TL_FILE_READ = 0, TL_FILE_CREATE = 1};
enum TL_FILE_WRITEMODE{TL_FILE_WRITEWITHOUTBUF = 0, TL_FILE_WRITEWITHBUF = 1};

//////////////////////////////////////////////////////////////////////////
//	�򿪹ر��ļ�
//////////////////////////////////////////////////////////////////////////

//TL_OpenFile: ���ļ�,�����ļ����
//TL_CloseFile: �ر��ļ�
TLFILE_t __stdcall TL_OpenFile(char *filename, TL_FILE_OPENMODE mode = TL_FILE_READ, TL_FILE_WRITEMODE writemode = TL_FILE_WRITEWITHOUTBUF, char *writebuf = NULL, unsigned int maxwritebuflen = 0);
void __stdcall TL_CloseFile(TLFILE_t hFile);

//////////////////////////////////////////////////////////////////////////
//	���ļ������ӿ�
//////////////////////////////////////////////////////////////////////////

//TL_FileHasAudio: �ж��ļ��Ƿ�����Ƶ
BOOL __stdcall TL_FileHasAudio(TLFILE_t hFile);

//TL_FileTotalTime:�ļ���ʱ��,��λ-����
//TL_FileStartTime: �ļ���ʼʱ�� time_tʱ��
//TL_FileEndTime:�ļ�����ʱ�� time_tʱ��
int __stdcall TL_FileTotalTime(TLFILE_t hFile);
int __stdcall TL_FileStartTime(TLFILE_t hFile);
int __stdcall TL_FileEndTime(TLFILE_t hFile);

//TL_FileVideoFrameNum:�ļ�����Ƶ��֡��
//TL_FileAudioFrameNum:�ļ�����Ƶ��֡��
int __stdcall TL_FileVideoFrameNum(TLFILE_t hFile);
int __stdcall TL_FileAudioFrameNum(TLFILE_t hFile);

//TL_FileVideoWidth: ��Ƶ��
//TL_FileVideoHeight: ��Ƶ��
int __stdcall TL_FileVideoWidth(TLFILE_t hFile);
int __stdcall TL_FileVideoHeight(TLFILE_t hFile);

//TL_FileAudioBits: ��Ƶλ��
//TL_FileAudioSampleRate: ��Ƶ֡������
unsigned short __stdcall TL_FileAudioBits(TLFILE_t hFile);
unsigned int __stdcall TL_FileAudioSampleRate(TLFILE_t hFile);

//TL_FileReadOneMediaFrame: ������֡��˳�����ζ��ļ�,ÿ�ζ�һ֡
//����:meida_buffer ��Ŷ�ȡ���ݵ�buf; start_time ����ʱ���,�����ļ���ͷ�ĺ�����; 
//keyflag:֡��־; media_type:֡������, 0-video  1-audio
//����ֵ:��ȡ�ֽ���
int __stdcall TL_FileReadOneMediaFrame(TLFILE_t hFile,BYTE *meida_buffer,unsigned int *start_time,BYTE *keyflag,BYTE *media_type);

//TL_FileReadOneVideoFrame: ����Ƶ֡˳�����ζ��ļ�,ÿ�ζ�һ֡��Ƶ
//����: video_buffer �����Ƶ��buf, duration ������һ֡�ĳ���ʱ��,��λ-����; �������ͬTL_FileReadOneMediaFrame
//����ֵ:��ȡ�ֽ���
int __stdcall TL_FileReadOneVideoFrame(TLFILE_t hFile,BYTE *video_buffer,unsigned int *start_time,unsigned int *duration,BYTE *keyFlag);
//TL_FileReadOneAudioFrame: ����Ƶ֡˳�����ζ��ļ�,ÿ�ζ�һ֡��Ƶ
int __stdcall TL_FileReadOneAudioFrame(TLFILE_t hFile,BYTE *audio_buffer,unsigned int *start_time,unsigned int *duration);

//TL_FileSeekToPrevSeg: ��λ����һ��
//TL_FileSeekToNextSeg: ��λ����һ��
void __stdcall TL_FileSeekToPrevSeg(TLFILE_t hFile);
void __stdcall TL_FileSeekToNextSeg(TLFILE_t hFile);

//TL_FileSeekToSysTime: ��λ��systime (time_t��ʽ, ����)
void __stdcall TL_FileSeekToSysTime(TLFILE_t hFile, unsigned int systime);

//TL_FileDecVideoFrame: ����Ƶ֡
//TL_FILEDecAudioFrame: ����Ƶ֡
enum DECRET{DEC_ERR=-1, DEC_OK=0, DEC_MORE=1};
DECRET __stdcall TL_FileDecVideoFrame(TLFILE_t hFile, BYTE keyFlag, BYTE* encbuf, int enclen, BYTE* decbuf, int* pdeclen);
DECRET __stdcall TL_FILEDecAudioFrame(TLFILE_t hFile, BYTE* encbuf, int enclen, BYTE* decbuf, int* pdeclen);

//////////////////////////////////////////////////////////////////////////
//	д�ļ������ӿ�
//////////////////////////////////////////////////////////////////////////
enum VIDEOCOMPRESSOR{TLFILE_V_H264 = 0};
enum AUDIOCOMPRESSOR{TLFILE_A_GRAW = 0, TLFILE_A_ADPA, TLFILE_A_ADPB};

//TL_FileSetVideo: ������Ƶ����
//TL_FileSetAudio: ������Ƶ����
void __stdcall TL_FileSetVideo(TLFILE_t hFile, unsigned short width, unsigned short height, float frame_rate, VIDEOCOMPRESSOR compressor);
void __stdcall TL_FileSetAudio(TLFILE_t hFile, unsigned short channels, unsigned short bits, unsigned int sample_rate, AUDIOCOMPRESSOR compressor, unsigned int sample_size, unsigned int sample_duration);

//TL_FileWriteVideoFrame: д��Ƶ֡
//TL_FileWriteAudioFrame: д��Ƶ֡
int __stdcall TL_FileWriteVideoFrame(TLFILE_t hFile, BYTE *video_buffer,unsigned int bytes,unsigned int timestamp,BYTE keyflag,unsigned int frameID);
int __stdcall TL_FileWriteAudioFrame(TLFILE_t hFile, BYTE *audio_buffer,unsigned int bytes,unsigned int timestamp);

BOOL __stdcall SnapBMPFile(char* strbmpfilename, unsigned char* YV12Buf, unsigned short width, unsigned short height);
BOOL __stdcall SnapJPGFile(char* strjpgfilename, unsigned char* YV12Buf, unsigned short width, unsigned short height);

#ifdef __cplusplus
}
#endif

#endif