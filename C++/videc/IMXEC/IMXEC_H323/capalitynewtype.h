#ifndef __CapalityNewType_H__
#define __CapalityNewType_H__

//==============================================================
//版权所有：版权所有(C) 2007，AVCON Ltd
//系统名称：H323 Protocol Stack
//作　　者：GaoShZh
//创建日期：2008-11-25
//功能说明：增加H264 Video Format Capability

#ifndef HAS_H323
typedef  void *  HPVT ;
typedef  void *  HPST ;
typedef  void *  HCALL ;
#endif

//Audio Capability Table EntryNumber
#define G711A_ENTRY_64K  7118
#define G711U_ENTRY_64K  7110
#define G722_ENTRY_64K   722
#define G7231_ENTRY      7231
#define G729A_ENTRY      729
#define G729_ENTRY       7290
#define G7221_ENTRY_24K  7224 
#define G7221_ENTRY_32K  7225 
#define G7221C_ENTRY_24K 17221  
#define G7221C_ENTRY_32K 17222
#define G7221C_ENTRY_48K 17223

//notice 20111227 update
#define G719_ENTRY_32K 7191
#define G719_ENTRY_48K 7192
#define G719_ENTRY_64K 7193

//H224 Capability Table EntryNumber
#define H224_ENTRY       224
//notice add 20120202
#define GSM_ENTRY        745

#define H261_ENTRY   261
#define H263_ENTRY   263
#define H263P_ENTRY  2632
#define H263PP_ENTRY 2633


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

//3000; //以3开头的整数作为H235 EntryNumber

#define H264_ENTRY_HIG720P   2653    /* 720P */
#define H264_ENTRY_HIG1080P      2654    /* 1080P */

#define DTMF_ENTRY       2833	//add by Jan for DTMF [20140730]

#define  MPEG4_ENTRY     2780   // use to call kedacom

//add tai er ru wang test use

#define TAIERLAB 0

//add by Jan for add cap [20130903]//add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

#define MPE4_ENTRY       280
#define CONF_ENTRY       100
#define USERINPUT_ENTRY  200
#define G728_ENTRY       728
#endif
//end [20130903]

#define H264_ENTRY_BASE    2100  
#define H264HP_ENTRY_BASE  2130  
#define H263_ENTRY_BASE    2160  
#define H263P_ENTRY_BASE   2190  
#define H263PP_ENTRY_BASE  2220  
#define H261_ENTRY_BASE    2250  

#define ENTRY_QCIF  1 
#define ENTRY_CIF   2 
#define ENTRY_VGA   3 
#define ENTRY_4CIF  4 
#define ENTRY_720I  5
#define ENTRY_SVGA  6 
#define ENTRY_XGA   7 
#define ENTRY_720P  8 
#define ENTRY_1080I 9 
#define ENTRY_SXGA  10
#define ENTRY_16CIF 11
#define ENTRY_1080P 12

//H264 Level Value
//#define H264_LEVEL_QCIF  22      //1.1 30fps
//#define H264_LEVEL_CIF   36      //1.3 30fps
//#define H264_LEVEL_4CIF  64      //3   25fps
//#define H264_LEVEL_16CIF 85      //4   39fps
//#define H264_LEVEL_720I  71      //3.1 60fps ?
//#define H264_LEVEL_720P  71      //3.1 30fps
//#define H264_LEVEL_1080I 78      //3.2 60fps ?
//#define H264_LEVEL_1080P 85      //4   30fps
//#define H264_LEVEL_VGA   64      //3   34fps
//#define H264_LEVEL_SVGA  71      //3.1 57fps
//#define H264_LEVEL_XGA   71      //3.1 35fps
//#define H264_LEVEL_SXGA  78      //3.2 42fps
//H264 Level Value 因适应RADVision ViaIp MCU 720P作出改变
extern int H264_LEVEL_QCIF; //  22      //1.1 30fps
extern int H264_LEVEL_CIF ; //  36      //1.3 30fps
extern int H264_LEVEL_4CIF; //64      //3   25fps
extern int H264_LEVEL_16CIF ;//85      //4   39fps
extern int H264_LEVEL_720I  ;//71      //3.1 60fps ?
extern int H264_LEVEL_720P  ;//71      //3.1 30fps
extern int H264_LEVEL_1080I ;//78      //3.2 60fps ?
extern int H264_LEVEL_1080P ;//85      //4   30fps
extern int H264_LEVEL_VGA   ;//64      //3   34fps
extern int H264_LEVEL_SVGA  ;//71      //3.1 57fps
extern int H264_LEVEL_XGA   ;//71      //3.1 35fps
extern int H264_LEVEL_SXGA  ;//78      //3.2 42fps

//H264 Max Video Bit Rate 
#define H264_MBPS_QCIF  192        //1.1 30fps   192 kbit/s
#define H264_MBPS_CIF   768        //1.3 30fps   768 kbit/s
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
	int                 nDirection;

	tagDATA_CAPABILITY_ITEM(void)
	{
		DataCodecID=DATA_CODEC_H224;
		nMaxBitrate=480;
		nDirection=0;
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



//用以唯一标识能力项,主要是用于H264分辨率区分
enum eCapNum{
	CAP_INVALID=0,
	CAP_G711A_64K,		//to ->iEntryNumber=7118  G711A_ENTRY_64K
	CAP_G711U_64K,		//G711U_ENTRY_64K 7110
	CAP_G722_64K,		//G722_ENTRY_64K 722
	CAP_G7231,			//G7231_ENTRY 7231
	CAP_G729A,			//G729A_ENTRY 729
	CAP_G7221_24K,      //G7221_ENTRY_24K 7224 24
	CAP_G7221_32K,      //G7221_ENTRY_32K 7225 32
	CAP_G7221C_24K,     //G7221C_ENTRY_24K 17221 annex c 24  
	CAP_G7221C_32K,     //G7221C_ENTRY_32K 17222 annex c 32 
	CAP_G7221C_48K,     //G7221C_ENTRY_48K 17223 annex c 24 

	//notice 20111227 update	
	CAP_G719_32K,
	CAP_G719_48K,
	CAP_G719_64K,
	//notice add 20120202
	CAP_GSM,

	CAP_H261,			//261
	CAP_H263,			//263
	CAP_H263v2,			//2632
	CAP_H263v3,			//2633
	CAP_H264,			//264 QCIF

	CAP_H264_QCIF=CAP_H264, //264 QCIF H264_ENTRY_QCIF
	CAP_H264_CIF,           //264 CIF
	CAP_H264_4CIF,          //264 4CIF
	CAP_H264_16CIF,         //264 4CIF
	CAP_H264_720I,          //264 720I
	CAP_H264_720P,
	CAP_H264_1080I,
	CAP_H264_1080P,
	CAP_H264_VGA,
	CAP_H264_SVGA,
	CAP_H264_XGA,
	CAP_H264_SXGA,         //264 SXGA


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

	CAP_H264_HIG720P,				//H264 720P HighProfile
	CAP_H264EXTEND_HIG720P,			//H264 720P HighProfile
	CAP_H264_HIG1080P,				//H264 1080P  HighProfile
	CAP_H264EXTEND_HIG1080P,		//H264 1080P HighProfile


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
	CH_G7221C_24K,
	CH_G7221C_32K,
	CH_G7221C_48K,

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
	
	//notice 20111227 update
	CH_G719,
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
