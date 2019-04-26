#ifndef __CapalityNewType_H__
#define __CapalityNewType_H__

//==============================================================
//版权所有：版权所有(C) 2007，AVCON Ltd
//系统名称：H323 Protocol Stack
//作　　者：GaoShZh
//创建日期：2008-11-25
//功能说明：增加H264 Video Format Capability,用于SIP


#define H264_FMT_COUNT 12



//H264 Capability Table EntryNumber
//双流对应加20000
#define H264_ENTRY_QCIF  2641
#define H264_ENTRY_CIF   2642
#define H264_ENTRY_4CIF  2643
#define H264_ENTRY_16CIF 2644
#define H264_ENTRY_720I  2645
#define H264_ENTRY_720P  2646
#define H264_ENTRY_1080I 2647
#define H264_ENTRY_1080P 2648
#define H264_ENTRY_VGA   2649
#define H264_ENTRY_SVGA  2650
#define H264_ENTRY_XGA   2651
#define H264_ENTRY_SXGA  2652




////H264 Capability 最大宏块处理速度 MaxMBPS
//#define H264_ENTRY_QCIF  3000 //1.1
//#define H264_ENTRY_CIF   11880
//#define H264_ENTRY_4CIF  40500
//#define H264_ENTRY_16CIF 245760
//#define H264_ENTRY_720I  108000
//#define H264_ENTRY_720P  108000
//#define H264_ENTRY_1080I 216000
//#define H264_ENTRY_1080P 245760
//#define H264_ENTRY_VGA   40500
//#define H264_ENTRY_SVGA  108000
//#define H264_ENTRY_XGA   108000
//#define H264_ENTRY_SXGA  216000

//H264 Level Value base 16
#define H264_LEVEL_QCIF  0x0B //11      //1.1 30fps   3000 396 
#define H264_LEVEL_CIF   0x0D //13      //1.3 30fps   11880 396
#define H264_LEVEL_4CIF  0x1E //30      //3   25fps   40500 1620
#define H264_LEVEL_16CIF 0x28 //40      //4   39fps   245760 8192
#define H264_LEVEL_720I  0x1F //31      //3.1 60fps ? 108000 3600
#define H264_LEVEL_720P  0x1F //31      //3.1 30fps   108000 3600
#define H264_LEVEL_1080I 0x20 //32      //3.2 60fps ? 216000 5120
#define H264_LEVEL_1080P 0x28 //40      //4   30fps   245760 8192
#define H264_LEVEL_VGA   0x1E //30      //3   34fps   40500 1620
#define H264_LEVEL_SVGA  0x1F //31      //3.1 57fps   108000 3600
#define H264_LEVEL_XGA   0x1F //31      //3.1 35fps   108000 3600
#define H264_LEVEL_SXGA  0x20 //32      //3.2 42fps   216000 5120

//H264 Max Video Bit Rate 
#define H264_MBPS_QCIF  192        //1.1 30fps   192 kbit/s
#define H264_MBPS_CIF   768000        //1.3 30fps   768 kbit/s
#define H264_MBPS_4CIF  10000      //3   25fps   10  Mbit/s
#define H264_MBPS_16CIF 20000      //4   39fps   20  Mbit/s
#define H264_MBPS_720I  14000      //3.1 60fps ? 14 Mbit/s
#define H264_MBPS_720P  14000      //3.1 30fps   14 Mbit/s
#define H264_MBPS_1080I 20000      //3.2 60fps ? 20 Mbit/s
#define H264_MBPS_1080P 20000      //4   30fps   20 Mbit/s
#define H264_MBPS_VGA   10000      //3   34fps   10 Mbit/s
#define H264_MBPS_SVGA  14000      //3.1 57fps   14 Mbit/s
#define H264_MBPS_XGA   14000      //3.1 35fps   14 Mbit/s
#define H264_MBPS_SXGA  20000      //3.2 42fps   20 Mbit/s

//注意 tagCapability::n4CIF 在H264能力集中用于Level值

#define H263_MBPS_ALL	6144

#define	MAX_DATACAP_NUM		    20

typedef enum {
	DATA_CODEC_H224=0,		//unsupport	
	DATA_CODEC_COUNT
}DATA_CODEC_ID;


typedef struct tagDATA_CAPABILITY_ITEM
{
	DATA_CODEC_ID	    DataCodecID;
	int					nMaxBitrate;	

	tagDATA_CAPABILITY_ITEM(void)
	{
		DataCodecID=DATA_CODEC_H224;
		nMaxBitrate=480;
	}
}DATA_CAPABILITY_ITEM;


struct DataCap
{
	unsigned int	m_nCapNum;	//有效数据（远遥）能力集个数
	DATA_CAPABILITY_ITEM		m_Caps[MAX_DATACAP_NUM];

	DataCap()
	{
		m_nCapNum = 0;
	}
};



//用以唯一标识能力项,主要是用于SIP MaxFS
enum eCapNum{
	CAP_INVALID=0,
	CAP_G711A_64K,		//7118
	CAP_G711U_64K,		//7110
	CAP_G722_64K,		//722
	CAP_G7231,			//7231
	CAP_G729A,			//729
	CAP_G7221_24K,      //7221 24
	CAP_G7221_32K,      //7221 32
	CAP_H261,			//261
	CAP_H263,			//263
	CAP_H263v2,			//2632
	CAP_H263v3,			//2633
	CAP_H264,			//264 QCIF

	CAP_H264_QCIF=396, //264 QCIF
	CAP_H264_CIF=396+1,           //264 CIF
	CAP_H264_4CIF=1620,          //264 4CIF
	CAP_H264_16CIF=8192,         //264 4CIF
	CAP_H264_720I=3600,          //264 720I
	CAP_H264_720P=3600+1,
	CAP_H264_1080I=5120,
	CAP_H264_1080P=8192+1,
	CAP_H264_VGA=1620+1,
	CAP_H264_SVGA=3600+2,
	CAP_H264_XGA=3600+3,
	CAP_H264_SXGA=5120+1,         //264 SXGA


	CAP_H224,			//224
	CAP_H239CONTROL,	//239
	CAP_H261EXTEND,		//2261
	CAP_H263EXTEND,		//2263
	CAP_H263v2EXTEND,	//22632
	CAP_H263v3EXTEND,	//22633
	CAP_H264EXTEND,		//2264
	CAP_H264EXTEND_QCIF=CAP_H264EXTEND, //264 QCIF
	CAP_H264EXTEND_CIF,           //264 CIF
	CAP_H264EXTEND_4CIF,          //264 4CIF
	CAP_H264EXTEND_16CIF,         //264 4CIF
	CAP_H264EXTEND_720I,          //264 720I
	CAP_H264EXTEND_720P,
	CAP_H264EXTEND_1080I,
	CAP_H264EXTEND_1080P,
	CAP_H264EXTEND_VGA,
	CAP_H264EXTEND_SVGA,
	CAP_H264EXTEND_XGA,
	CAP_H264EXTEND_SXGA,         //264 SXGA
};

//channels表中各信道的编号，该值与Config.val(即Config.cpp)中的channels表顺序一一对应
enum eChnNum{
	CH_INVALID=0,
	CH_G711A_64K,
	CH_G711U_64K,
	CH_G722_64K,
	CH_G7231,
	CH_G729A,
	CH_G7221_24K,
	CH_G7221_32K,

	CH_H261,
	CH_H263,
	CH_H263v2,
	CH_H263v3,
	CH_H264,


	CH_H224,

	CH_H261EXTEND,
	CH_H263EXTEND,
	CH_H263v2EXTEND,
	CH_H263v3EXTEND,
	CH_H264EXTEND,
};



//目的：视频图像格式类型
typedef enum tagVideoFmtEn
{
	EN_VIDEO_FMT_CIF, /* CIF */
	EN_VIDEO_FMT_4CIF, /* 4CIF */
	EN_VIDEO_FMT_16CIF, /* 16CIF */
	EN_VIDEO_FMT_720I, /* 720I */
	EN_VIDEO_FMT_720P, /* 720P */
	EN_VIDEO_FMT_1080I, /* 1080I */
	EN_VIDEO_FMT_1080P, /* 1080P */
	EN_VIDEO_FMT_VGA, /* VGA */
	EN_VIDEO_FMT_SVGA, /* SVGA */
	EN_VIDEO_FMT_XGA, /* XGA */
	EN_VIDEO_FMT_SXGA, /* SXGA */
	EN_VIDEO_FMT_AUTO, /* 自动*/
	EN_VIDEO_FMT_COUNT
}VideoFmtEnDef;




#endif
