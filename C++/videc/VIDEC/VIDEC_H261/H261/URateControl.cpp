
#include "URateControl.h"

#define RC_BUF_COUNT 3

const int DCIF_Param[][6][3]= {
	{
		{2,6,14},	//1M,25+
		{2,6,14},	//1M,20-25
		{2,6,14},	//1M,15-20
		{2,4,12},	//1M,10-15
		{2,4,10},	//1M,5-10
		{2,4,8}		//1M,1-5
	}, //1M
	{
		{2,8,16},	//25+
		{2,8,16},	//20-25
		{2,6,16},	//15-20
		{2,6,14},	//10-15
		{2,5,12},	//5-10
		{2,5,10}		//1-5
	}, //512k
	{
		{2,10,18},	//25+
		{2,10,18},	//20-25
		{2,8,18},	//15-20
		{2,8,16},	//10-15
		{2,7,14},	//5-10
		{2,7,12}	//1-5
	}, //256k
	{
		{2,12,20},	//25+
		{2,12,20},	//20-25
		{2,10,20},	//15-20
		{2,10,18},	//10-15
		{2,8,16},	//5-10
		{2,8,14}		//1-5
	}, //128k
	{
		{2,14,22},	//25+
		{2,14,22},	//20-25
		{2,12,22},	//15-20
		{2,12,20},	//10-15
		{2,10,18},	//5-10
		{2,10,16}		//1-5
	} //56k
};
const int CIF_Param[][6][3] = {
	{
		{2,6,14},	//1M,25+
		{2,6,14},	//1M,20-25
		{2,6,14},	//1M,15-20
		{2,4,12},	//1M,10-15
		{2,4,10},	//1M,5-10
		{2,4,8}		//1M,1-5
	}, //1M
	{
		{2,8,16},	//25+
		{2,8,16},	//20-25
		{2,6,16},	//15-20
		{2,6,14},	//10-15
		{2,5,12},	//5-10
		{2,5,10}		//1-5
	}, //512k
	{
		{2,10,18},	//25+
		{2,10,18},	//20-25
		{2,8,18},	//15-20
		{2,8,16},	//10-15
		{2,7,14},	//5-10
		{2,7,12}	//1-5
	}, //256k
	{
		{2,12,20},	//25+
		{2,12,20},	//20-25
		// { Dennis 2004/01/08
		//{5,10,20},	//15-20
		//{5,10,18},	//10-15
		{2,8,10},	//15-20
		{2,8,10},	//10-15
		// Dennis 2004/01/08 }
		{2,8,16},	//5-10
		{2,8,14}		//1-5
	}, //128k
	{
		{2,14,22},	//25+
		{2,14,22},	//20-25
		{2,12,22},	//15-20
		{2,12,20},	//10-15
		{2,10,18},	//5-10
		{2,10,16}	//1-5
	} //56k
};
const int QCIF_Param[][6][3] = {
	{
		{2,4,14},	//1M,25+
		{2,4,14},	//1M,20-25
		{2,4,14},	//1M,15-20
		{2,4,12},	//1M,10-15
		{2,4,10},	//1M,5-10
		{2,4,8}		//1M,1-5
	}, //1M
	{
		{2,6,16},	//25+
		{2,6,16},	//20-25
		{2,6,16},	//15-20
		{2,6,14},	//10-15
		{2,4,12},	//5-10
		{2,4,10}		//1-5
	}, //512k
	{
		{2,8,18},	//25+
		{2,8,18},	//20-25
		{2,8,18},	//15-20
		{2,6,16},	//10-15
		{2,6,14},	//5-10
		{2,6,12}		//1-5
	}, //256k
	{
		{2,12,20},	//25+
		{2,10,20},	//20-25
		{2,10,20},	//15-20
		{2,8,18},	//10-15
		{2,6,16},	//5-10
		{2,6,14}	//1-5
	}, //128k
	{
		{2,12,22},	//25+
		{2,12,22},	//20-25
		{2,10,20},	//15-20
		{2,10,20},	//10-15
		{2,8,18},	//5-10
		{2,8,16}	//1-5
	} //56k
};

int _get_BW_Index(int bandwidth)
{
	if (bandwidth <= 128)
		return 4;
	else if (bandwidth <= 384)
		return 3;
	else if (bandwidth <= 500)
		return 2;
	else if (bandwidth <= 700)
		return 1;
	else // above 700
		return 0;
}

int _get_FR_Index(int framerate)
{
	if (framerate >= 25)
		return 0;
	else if (framerate >= 20)
		return 1;
	else if (framerate >= 15)
		return 2;
	else if (framerate >= 10)
		return 3;
	else if (framerate >= 5)
		return 4;
	else // smaller than 5
		return 5;
	
}

URateControl::URateControl()
{
	m_iBitRate = 128000;
	
	m_iSentBW=1024000;

	m_iFrameRate = 10;
	
	m_iMaxFrameRate = 30;
	m_iMinFrameRate = 1;
	
	m_iIFrameInterval = 20;


	RateControlInit(320,240,m_iBitRate/1000,m_iFrameRate,m_iIFrameInterval);
	m_hFrameRateTimer=CreateWaitableTimer(NULL,FALSE,NULL);	

	SetFrameRateTimer(m_iFrameRate);
}

URateControl::~URateControl()
{

	CloseHandle(m_hFrameRateTimer);

}

/*
函数名称：SetFRTimer
功能说明：设置帧率控制事件时钟
参数说明：
	FrameRate:帧率(fps)
*/
void URateControl::SetFrameRateTimer(int iFrameRate)
{
	if(iFrameRate>30)iFrameRate=30;
	if(iFrameRate<1)iFrameRate=1;
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart=0;
	BOOL bResult=SetWaitableTimer(m_hFrameRateTimer,&liDueTime,1000/iFrameRate-2,NULL,NULL,FALSE);
}

/*
	函数名称：DoFrameCheck
	功能说明：根据设定的帧率，检查当前帧是否满足帧率选择
	参数说明：
	dwClock：当前帧的时间戳
	bBitRateControl:是否使用编码器外部的码率控制(divx4.0中使用)
*/
//>>>>>>>>>>>>>>>>>>> diff lixq 2003-12-10
BOOL URateControl::DoFrameRateCheck()
{
	BOOL bResult=TRUE;
	if(m_iFrameRate<m_iMaxFrameRate)
	{
		if(WAIT_OBJECT_0!=WaitForSingleObject(m_hFrameRateTimer,0)) //使用事件控制帧率add by lixq
			bResult=FALSE;
	}
	return bResult;
}
//<<<<<<<<<<<<<<<<<< diff lixq 2003-12-10

BOOL URateControl::CanEncThisFrame(DWORD dwClock,BOOL bBitRateControl)
{
	BOOL bResult=TRUE;
    /*>>>>>>>>>>>>>>>>>>>>>>>add by lixq */
	//diff lixq 2003-12-10  删除帧率控制部分,
	//diff lixq 2003-12-10  由DoFrameRateCheck完成
	__int64 nBufUpSize =(__int64)(((1.0+RC_BURST_RATE)*100)*m_lRCBufSize/100);
	if(m_lRCBufPos>nBufUpSize)
	{
		m_lRCBufPos=nBufUpSize;
		bResult=FALSE;
	}

	return bResult;
	/*<<<<<<<<<<<<<<<<<<<<<<<<add by lixq */
 
}

void URateControl::RateControlInit(int iWidth,int iHeight,int iBitrate, int iFrameRate,int iIFrameInterval)
{
	m_iBitRate = (int)(iBitrate*1000);
	m_iFrameRate = iFrameRate;
	m_iIFrameInterval = iIFrameInterval;
	
		
	if (iWidth*iHeight <= 300*200)
	{
		m_iMaxQuant = QCIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][2];
		m_iRefQuant = QCIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][1];
		m_iMinQuant = QCIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][0];
	}
	else if(iWidth*iHeight <= 352*288)
	{
		m_iMaxQuant = CIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][2];
		m_iRefQuant = CIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][1];
		m_iMinQuant = CIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][0];	
	}
	else
	{
		m_iMaxQuant = DCIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][2];
		m_iRefQuant = DCIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][1];
		m_iMinQuant = DCIF_Param[_get_BW_Index(iBitrate)][_get_FR_Index(iFrameRate)][0];	
	}
	
	m_iCurrFrameType=I_VOP;
	m_iCurrFrameNum=0;



	m_iPreQuant=m_iCurrQuant =m_iRefQuant;
		
	double dTs=(double)m_iIFrameInterval/(double)m_iFrameRate;
	m_lRCBufSize=(int)((double)m_iBitRate*dTs);	
		
	m_lRCBufPos=m_lRCBufSize/2;
	m_iRCTotalFrameNum=m_iIFrameInterval*RC_BUF_COUNT;
	

	memset(&m_Rate_control,0,sizeof(RateControl));
	XvidRateControlInit(&m_Rate_control,m_iBitRate,16,100,100,m_iFrameRate*1000,m_iMaxQuant,m_iMinQuant);

}

void URateControl::RateControlGetParam(int& iQuant,BOOL& bisKey)
{


	if(I_VOP==m_iCurrFrameType)
	{
		bisKey=TRUE;
	}
	else
	{
		bisKey=FALSE;		
	}

	int iFrameType=m_iCurrFrameType;
	

	m_iCurrQuant=XvidRateControlGetQ(&m_Rate_control,bisKey);

	iQuant=m_iCurrQuant;

#if defined(_DEBUG) && 0
	char buf[200]; 
	wsprintf(buf,"RCGetParam FrameType:%d, Quant:%d,PreQuant:%d\n",iFrameType, \
		m_iCurrQuant,m_iPreQuant);
	OutputDebugString(buf);
#endif

}



void URateControl::RateControlUpdate(int ms,URC_DATA encresult)
{


	if(m_iRCTotalFrameNum==m_iIFrameInterval*RC_BUF_COUNT)
	{
		//更新码率控制相关参数
		m_iRCTotalFrameNum+=m_iIFrameInterval;
	}
	
	m_iRCTotalFrameNum--;
	assert(m_iRCTotalFrameNum>0);

	int iRCBufRp=m_iBitRate/m_iFrameRate;
	
	if(encresult.is_key_frame<0)
	{
		
		m_lRCBufPos-=iRCBufRp;
		
		//避免连续丢帧
		if(m_lRCBufPos>(1.0+RC_BURST_RATE)*m_lRCBufSize)
			m_lRCBufPos=(__int64)((1.0+RC_BURST_RATE)*m_lRCBufSize);
		
		if(m_lRCBufPos<0)
			m_lRCBufPos=m_lRCBufSize/4;
	}
	else
	{
		
		m_lRCBufPos+=encresult.total_bits-iRCBufRp;
		
		m_lRCBufPos=max(m_lRCBufPos,m_lRCBufSize/4);
		
		//更新码率控制模型
		BOOL bIsKey=FALSE;
		if(I_VOP==m_iCurrFrameType)
		{
			bIsKey=TRUE;
		}
		XvidRateControlUpdate(&m_Rate_control,encresult.quantizer,
			encresult.total_bits/8,bIsKey);
		m_iPreQuant = m_iCurrQuant;
		//判断下一帧的类型
		m_iCurrFrameNum++;
		if(m_iCurrFrameNum>=m_iIFrameInterval)
		{
			m_iCurrFrameType=I_VOP;
			m_iCurrFrameNum=0;
		}
		else
		{
			m_iCurrFrameType=P_VOP;
		}
		
	}

}


void URateControl::SetSentBW(int iSentBW)
{
	m_iSentBW=iSentBW;

}
