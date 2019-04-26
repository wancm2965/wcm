
一、代码结构
AVMSP:播放，网络点播，本地AVMS,动态加载
DATEC:数据会议，文档打印，PDF浏览
DRVINST：打印机安装程序
HPMP:A7合成
IMXEC：视频库、网络库、音频库、H323等的衔接层
MPVideo：通用版本MP合成
AVCONMP：通用版本MP合成调用层类似IMXEC
AVCONEC:老的视频库，企业版与播放器用到，改了AUDEC.lib VIDEC.lib 接口改后需要重编译
AH2K：采集卡对接，由采集卡项目组负责维护及发布,动态加载
AVCONAVC:软鞭软解，媒体播放库
DVC:ipcam接入模块，接入第三方可参照复制,动态加载
VIDECINL：Intel 硬编，合成库MPVIDEO用了里面的硬解。动态加载，依赖HPAVC、VIDECFRE
VIDECYNC：Intel 硬解，D3D显示入口。动态加载，依赖HPAVC、VIDECFRE
VIDECFRE：图像平滑显示控制
HPAVC：Intel 相关的底层调用，静态依赖HPAVC_Common（包括D3D代码）。


VIDECMKV：MKV录像
VIDECMP4:
VIDEC_VQD：图像识别，现在已经不用了。动态加载
VIDEC,解决方案:..\SDK\VIDEC\VIDEC_Win32\VIDEC_2005.sln
IMXEC,解决方案:..\SDK\IMXEC\IMXEC_Win32\IMXEC_PJSIP.sln
VIDEC_XF：消防特定代码，现在不用了，消防已经同步到主线代码库了


二、在win8.1平台下用VS2008编译VIDEC，按如下配置
1、安装MediaSDK2014R2Clients
2、nasm.exe 放到windows目录
3、修改：C:\Program Files (x86)\Windows Kits\8.1\Include\um\winnt.h  #include "basetsd.h"
4、VS2008 工程环境配置
	工具=》选项=》项目和解决方案=》VC++ 目录=>包含文件/库文件
	C:\Program Files (x86)\Windows Kits\8.1\Include\shared
	C:\Program Files (x86)\Windows Kits\8.1\Include\um
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\include
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\igfx_s3dcontrol\include

	C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\lib\win32
	C:\Program Files\Intel\Media SDK 2014 R2 for Clients\igfx_s3dcontrol\lib\win32

三、TestMPClient 调试
/////////////
[VIDEC]
DebugLevel=0 ;打印调试信息
PIT=0 ;平滑控制类型
MaxDecoder=6
MaxEecoder=2
ForceIntelEncoder=0
HWDecodeWidth=960
HWDecodeHeight=540
HWEncodeWidth=1280
HWEncodeHeight=720
DelayMilliSecond=0
DelayFrameRate=30
Pending=0 ;控制是否音视频同步
UseD3D=1
D3D11=1
D3D11Debug=0
SkipDisplay=1 ;显示跳帧
////////////////
VIDECYNC_PlayerOpen 硬解
VIDECINL_EncoderOpen  硬编
Capture2  Show=%d Send=%d Encode=%d ShowType=%d %d X %d bps 
视频显示类型ShowType 0：Unknow;1：DirectShow;2：D3D9;3：D3D11;4：D3D12
编码类型Encode：0:Unknown; 1:SoftWare; 2:Intel HardWare; 3:NV HardWare
Player2 Show=%d Recv=%d Decode=%d ShowType=%d %d X %d bps=%d
解码类型Decode：0:Unknown; 1:SoftWare; 2:Intel HardWare; 3:NV HardWare

四、发布库
VDEC版本号控制：#define VIDEC_FILEVERSION_STR       "1.7.2.20160302"
发布的库release与debug都是release的
PrinterDrv：第三方打印机安装
DS40xxSDK：海康编码依赖
HPATC.dll 音频转码
VIDECMP4 
VIDECVDP：新的视频缩放库 
VIDECICE.dll:图像增强
最新发布库目录
https://116.236.237.36:8443/svn/kernel/video/AVCONECSDK/trunk
稳定库目录
https://116.236.237.36:8443/svn/kernel/video/AVCONECSDK/tags
深圳维护的第三方解码库
https://116.236.237.36:8443/svn/kernel/video/AVCONECSDK/3party