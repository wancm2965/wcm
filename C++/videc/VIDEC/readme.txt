
һ������ṹ
AVMSP:���ţ�����㲥������AVMS,��̬����
DATEC:���ݻ��飬�ĵ���ӡ��PDF���
DRVINST����ӡ����װ����
HPMP:A7�ϳ�
IMXEC����Ƶ�⡢����⡢��Ƶ�⡢H323�ȵ��νӲ�
MPVideo��ͨ�ð汾MP�ϳ�
AVCONMP��ͨ�ð汾MP�ϳɵ��ò�����IMXEC
AVCONEC:�ϵ���Ƶ�⣬��ҵ���벥�����õ�������AUDEC.lib VIDEC.lib �ӿڸĺ���Ҫ�ر���
AH2K���ɼ����Խӣ��ɲɼ�����Ŀ�鸺��ά��������,��̬����
AVCONAVC:�����⣬ý�岥�ſ�
DVC:ipcam����ģ�飬����������ɲ��ո���,��̬����
VIDECINL��Intel Ӳ�࣬�ϳɿ�MPVIDEO���������Ӳ�⡣��̬���أ�����HPAVC��VIDECFRE
VIDECYNC��Intel Ӳ�⣬D3D��ʾ��ڡ���̬���أ�����HPAVC��VIDECFRE
VIDECFRE��ͼ��ƽ����ʾ����
HPAVC��Intel ��صĵײ���ã���̬����HPAVC_Common������D3D���룩��


VIDECMKV��MKV¼��
VIDECMP4:
VIDEC_VQD��ͼ��ʶ�������Ѿ������ˡ���̬����
VIDEC,�������:..\SDK\VIDEC\VIDEC_Win32\VIDEC_2005.sln
IMXEC,�������:..\SDK\IMXEC\IMXEC_Win32\IMXEC_PJSIP.sln
VIDEC_XF�������ض����룬���ڲ����ˣ������Ѿ�ͬ�������ߴ������


������win8.1ƽ̨����VS2008����VIDEC������������
1����װMediaSDK2014R2Clients
2��nasm.exe �ŵ�windowsĿ¼
3���޸ģ�C:\Program Files (x86)\Windows Kits\8.1\Include\um\winnt.h  #include "basetsd.h"
4��VS2008 ���̻�������
	����=��ѡ��=����Ŀ�ͽ������=��VC++ Ŀ¼=>�����ļ�/���ļ�
	C:\Program Files (x86)\Windows Kits\8.1\Include\shared
	C:\Program Files (x86)\Windows Kits\8.1\Include\um
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\include
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\igfx_s3dcontrol\include

	C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\lib\win32
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\igfx_s3dcontrol\lib\win32

����TestMPClient ����
/////////////
[VIDEC]
DebugLevel=0 ;��ӡ������Ϣ
PIT=0 ;ƽ����������
MaxDecoder=6
MaxEecoder=2
ForceIntelEncoder=0
HWDecodeWidth=960
HWDecodeHeight=540
HWEncodeWidth=1280
HWEncodeHeight=720
DelayMilliSecond=0
DelayFrameRate=30
Pending=0 ;�����Ƿ�����Ƶͬ��
UseD3D=1
D3D11=1
D3D11Debug=0
SkipDisplay=1 ;��ʾ��֡
////////////////
VIDECYNC_PlayerOpen Ӳ��
VIDECINL_EncoderOpen  Ӳ��
Capture2  Show=%d Send=%d Encode=%d ShowType=%d %d X %d bps 
��Ƶ��ʾ����ShowType 0��Unknow;1��DirectShow;2��D3D9;3��D3D11;4��D3D12
��������Encode��0:Unknown; 1:SoftWare; 2:Intel HardWare; 3:NV HardWare
Player2 Show=%d Recv=%d Decode=%d ShowType=%d %d X %d bps=%d
��������Decode��0:Unknown; 1:SoftWare; 2:Intel HardWare; 3:NV HardWare

�ġ�������
VDEC�汾�ſ��ƣ�#define VIDEC_FILEVERSION_STR       "1.7.2.20160302"
�����Ŀ�release��debug����release��
PrinterDrv����������ӡ����װ
DS40xxSDK��������������
HPATC.dll ��Ƶת��
VIDECMP4 
VIDECVDP���µ���Ƶ���ſ� 
VIDECICE.dll:ͼ����ǿ
���·�����Ŀ¼
https://116.236.237.36:8443/svn/kernel/video/AVCONECSDK/trunk
�ȶ���Ŀ¼
https://116.236.237.36:8443/svn/kernel/video/AVCONECSDK/tags
����ά���ĵ����������
https://116.236.237.36:8443/svn/kernel/video/AVCONECSDK/3party