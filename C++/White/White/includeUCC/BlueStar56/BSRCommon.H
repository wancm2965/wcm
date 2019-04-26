/********************************************************************
*	Copyright (C) 2008 - BlueStar. All Rights Reserved
*
*	Date Created:2008-05-20
*	Filename	:BSRCommon.h
*	Author		:BlueStar luoyanhua
*	Description	:DVR Client&Player Common datatype and struct declare
*********************************************************************/
#ifndef __BSR_COMMON_H__
#define __BSR_COMMON_H__


//环境支持检测
//1-11位分别表示以下信息(位与为TRUE)表示支持
#define BSR_SUPPORT_DDRAW		0x00000001	//支持DirectDraw;如果不支持,则播放器不能工作;
#define BSR_SUPPORT_DSOUND		0x00000002	//支持DirectSound;如果不支持,则音频相关功能不能工作;
#define BSR_SUPPORT_YV12		0x00000004	//支持YV12图像格式;如果不支持,则播放器不能工作;
#define BSR_SUPPORT_BLT			0x00000008	//显卡支持BLT操作;如果不支持,则播放器不能工作;
#define BSR_SUPPORT_BLTFOURCC	0x00000010	//显卡BLT支持颜色转换;如果不支持,播放器会用软件方法作RGB转换,播放效率将降低(当前版本中暂不支持转换);
#define BSR_SUPPORT_BLTSHRINKX	0x00000020	//显卡BLT支持X轴缩小;如果不支持,系统会用软件方法转换;
#define BSR_SUPPORT_BLTSHRINKY	0x00000040	//显卡BLT支持Y轴缩小;如果不支持,系统会用软件方法转换;
#define BSR_SUPPORT_BLTSTRETCHX	0x00000080	//显卡BLT支持X轴放大;如果不支持,系统会用软件方法转换;
#define BSR_SUPPORT_BLTSTRETCHY	0x00000100	//显卡BLT支持Y轴放大;如果不支持,系统会用软件方法转换;
#define BSR_SUPPORT_SSE			0x00000200	//CPU支持SSE指令;Intel Pentium3以上支持SSE指令;
#define BSR_SUPPORT_MMX			0x00000400	//CPU支持MMX指令集;Intel Pentium3以上支持SSE指令;

//视频分辨率PAL制
typedef enum
{
  //QCIF
  XDIM_QCIF_PAL = 176,
  YDIM_QCIF_PAL = 144,
	//DCIF(1),对应录像机中DCIF(DCIF)
	XDIM_DCIF_PAL	= 528,
	YDIM_DCIF_PAL	= 288,

	//CIF(2),对应录像机中CIF(CIF)
	XDIM_CIF_PAL	= 352,
	YDIM_CIF_PAL	= 288,

	//2个CIF并排(3),对应录像机中2CIF(1/2D1)
	XDIM_2CIF_PAL	= 704,
	YDIM_2CIF_PAL	= 288,

	//D1(4)(D1)
	XDIM_D1_PAL		= 704,
	YDIM_D1_PAL		= 576,

	//2个CIF重叠(暂未使用)
	XDIM_HALFD1_PAL	= 352,
	YDIM_HALFD1_PAL	= 576,

	//2/3D1
	XDIM_23D1_PAL	= 512,
	YDIM_23D1_PAL	= 576
}BSR_VIDEORES_PAL;

//视频分辨率NTSC制
typedef enum
{
  XDIM_QCIF_NTSC = 176,
  YDIM_QCIF_NTSC = 144, //由于不能满足整数个宏块，暂时按照PAL处理
	//DCIF(1),对应录像机中DCIF(DCIF)
	XDIM_DCIF_NTSC		= 528,
	YDIM_DCIF_NTSC		= 240,

	//CIF(2),对应录像机中CIF(CIF)
	XDIM_CIF_NTSC		= 352,
	YDIM_CIF_NTSC		= 240,

	//2个CIF并排(3),对应录像机中2CIF(1/2D1)
	XDIM_2CIF_NTSC		= 704,
	YDIM_2CIF_NTSC		= 240,

	//D1(4)(D1)
	XDIM_D1_NTSC		= 704,
	YDIM_D1_NTSC		= 480,

	//2个CIF重叠(暂未使用)
	XDIM_HALFD1_NTSC	= 352,
	YDIM_HALFD1_NTSC	= 480,

	//2/3D1
	XDIM_23D1_NTSC		= 512,
	YDIM_23D1_NTSC		= 480
}BSR_VIDEORES_NTSC;

//本地调试输出级别
typedef enum
{
	DEBUGLEVEL_DONTOUT		= 0,		//无输出(默认值)
	DEBUGLEVEL_IMPORTANT,				//重大错误
	DEBUGLEVEL_GENERROR,				//一般错误
	DEBUGLEVEL_WARNING,					//警告信息
	DEBUGLEVEL_NOTICE,					//常规提示
	DEBUGLEVEL_STREAMINFO				//流信息(信息很多)
}BSR_DEBUGLEVEL;

#endif //__BSR_COMMON_H__