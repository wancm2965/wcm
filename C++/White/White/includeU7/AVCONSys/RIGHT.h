#ifndef RIGHT_H_H
#define RIGHT_H_H

#include <string.h>

// 权限
typedef enum _MMS_RIGHT
{
	MET_MIN				= 0,
	MET_RIGHT_SPEACK,			// 发言
	MET_RIGHT_RECORD,			// 录像
	MET_RIGHT_TALK,				// 讨论
	MET_RIGHT_CONTROL,			// 控制
	MET_RIGHT_WHITE,			// 白板
	MET_RIGHT_FLOAT,			// 浮动
	MET_RIGHT_MET,				// 锁定
	MET_RIGHT_DESK,				// 桌面
	MET_RIGHT_PAPER,			// 小纸条
	MET_RIGHT_DOC,				// 文档
	MET_RIGHT_MAXWIN,			// 对与会者加上一个权限控制,可以打开或关闭窗口最大化的的权限
	MET_RIGHT_BULL,             // 公告
	MET_RIGHT_TOPIC,            // 标题
	MET_RIGHT_DOCSHARE,			// 文档共享
	MET_RIGHT_SCREENSHARE,		// 屏幕共享
	MET_MAX
};

//权限字符串
static const char* MMS_RIGHT_STRING[] = {
	"",				//缺省，默认从0开始
	"qst",  		//MET_RIGHT_SPEACK	
	"rec",			//MET_RIGHT_RECORD	
	"dis",			//MET_RIGHT_TALK	
	"ctr",			//MET_RIGHT_CONTROL	
	"whb",			//MET_RIGHT_WHITE	
	"flt",			//MET_RIGHT_FLOAT	
	"met",			//MET_RIGHT_MET		
	"dsk",			//MET_RIGHT_DESK	
	"ppe",			//MET_RIGHT_PAPER	
	"doc",			//MET_RIGHT_DOC		
	"max",			//MET_RIGHT_MAXWIN	
	"blt",			//MET_RIGHT_BULL    
	"tpc",			//MET_RIGHT_TOPIC
	"dcs",
	"scs"
};

// 附件
typedef enum _MMS_ANNEX
{
	MET_TOPIC_BULL = 0, // 标题和公告
	MET_TOPIC	= 1,	// 标题
	MET_BULL,			// 公告
};

// 权限控制
// 注意：增删权限，请修改下面的宏
typedef struct tag_MMS_RIGHT
{
	static std::string RIGHT2STR(unsigned int nRight)
	{
		nRight = (nRight <= MET_MIN || nRight >= MET_MAX) ? 0 : nRight;
		return MMS_RIGHT_STRING[nRight];
	};

	static unsigned int STR2RIGHT(const char* str)
	{
		for(int index = MET_RIGHT_SPEACK; index != MET_MAX; index++)
		{
			if(strcmp(MMS_RIGHT_STRING[index], str) == 0)
			{
				return index;
			}
		}
		return 0;
	};

	static std::string RESETRIGHTSTRING(const std::string& strClientStr,const std::string& strRightValue,bool bEnable)
	{
		std::string strNewRightString = strClientStr;
		if(bEnable)
		{
			if(strNewRightString.find(strRightValue) == std::string::npos)
			{
				strNewRightString += strRightValue + ";";
			}
		}
		else
		{
			size_t pos = strNewRightString.find(strRightValue);
			if(pos != std::string::npos)
			{
				size_t epos = strNewRightString.find(";", pos);
				if(epos != std::string::npos)
				{
					strNewRightString.erase(pos, epos-pos+1);
				}
			}
		}

		return strNewRightString;
	}

	static bool GETRIGHTFROMSTRING(const std::string& strRightStr, unsigned int nRight)
	{
		if(strRightStr.find(MMS_RIGHT_STRING[nRight]) != std::string::npos)
		{
			return true;
		}
		return false;
	}

	static unsigned int ANNEX2RIGHT(unsigned short nAnnexType)
	{
		if(nAnnexType == MET_TOPIC)
		{
			return MET_RIGHT_TOPIC;
		}
		else if (nAnnexType == MET_BULL)
		{
			return MET_RIGHT_BULL;
		}
		return 0;
	}

	static unsigned int RIGHT2ANNEX(unsigned int nRight)
	{
		if(nRight == MET_RIGHT_TOPIC)
		{
			return MET_TOPIC;
		}
		else if (nRight == MET_RIGHT_BULL)
		{
			return MET_BULL;
		}
		return MET_TOPIC_BULL;
	}
}MMS_RIGHT, *PMMS_RIGHT;

#endif //RIGHT_H_H
