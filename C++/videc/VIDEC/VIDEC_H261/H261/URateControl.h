#ifndef H261_CONTROL_H
#define H261_CONTROL_H
#include "..\stdafx.h"
#include "ratecontrol.h"


#define I_VOP	0
#define P_VOP	1
#define B_VOP	2

#define RC_BURST_RATE 0.5
#define RC_AVG_RATE 0.6



typedef struct
{
	int is_key_frame;	// the current frame is encoded as a key frame
	int quantizer;		// the quantizer used for this frame
	int texture_bits;	// amount of bits spent on coding DCT coeffs
	int motion_bits;	// amount of bits spend on coding motion
	int total_bits;		// sum of two previous fields
}URC_DATA;



class URateControl
{
public:
	URateControl();
	virtual ~URateControl();
public:
	inline void SetFrameRate(int iFrameRate) 
	{
		if (iFrameRate >= 30)
			m_iFrameRate = 30;
		else
			m_iFrameRate =  iFrameRate;
		SetFrameRateTimer(m_iFrameRate);
	}
	inline int GetFrameRate(){return m_iFrameRate;}
	
	inline void SetBitRate(int iBitRate) {m_iBitRate = iBitRate;}
	inline int GetBitRate() {return m_iBitRate;}

	inline void SetFrameRateRange(int iMaxFrameRate, int iMinFrameRate) 
		{m_iMaxFrameRate=iMaxFrameRate;m_iMinFrameRate=iMinFrameRate;}
	inline void GetFrameRateRange(int& iMaxFrameRate, int& iMinFrameRate) 
		{iMaxFrameRate=m_iMaxFrameRate;iMinFrameRate=m_iMinFrameRate;}

	inline void SetIFrameInterval(int iIFrameInterval) 
		{m_iIFrameInterval = iIFrameInterval;}
	inline void GetIFrameInterval(int& iIFrameInterval) 
		{iIFrameInterval = m_iIFrameInterval;}
public:

	void SetSentBW(int iSentBW);
	int  GetSentBW(void)
	{
		return m_iSentBW;
	}

	BOOL DoFrameRateCheck();

	/*!
	 * @����˵���������趨��֡�ʣ���鵱ǰ֡�Ƿ�����֡��ѡ��
	 * @dwClock[IN]��ǰ֡��ʱ���
	 * @bBitRateControl[IN]�Ƿ�ʹ�ñ������ⲿ�����ʿ���(divx4.0��ʹ��)
	*/
	BOOL CanEncThisFrame(DWORD dwClock,BOOL bBitRateControl=TRUE);

	void RateControlInit(int iWidth,int iHeight,int iBitrate, int iFrameRate,int iIFrameInterval);

	void RateControlUpdate(int ms,URC_DATA encresult);

	void RateControlGetParam(int& iQuant,BOOL& bisKey);

private:  
	void SetFrameRateTimer(int iFrameRate);//����֡�ʿ����¼�ʱ��
	


private:
	
	__int64		m_lRCBufSize;
	__int64		m_lRCBufPos;
	
	int			m_iRCTotalFrameNum;

	int			m_iBitRate;
	int			m_iSentBW;
	// current and reference quant
	int			m_iRefQuant;
	int			m_iCurrQuant;
	int			m_iPreQuant;
	int			m_iCurrFrameType;
	int			m_iCurrFrameNum;
	// max and min available quant
	int			m_iMaxQuant;
	int			m_iMinQuant;

	// --------------------- Frame control section -----------------

	// current and target frate rate
	int			m_iFrameRate;

	// max and min available frame rate
	int			m_iMaxFrameRate;
	int			m_iMinFrameRate;

	int			m_iIFrameInterval;

	HANDLE		m_hFrameRateTimer;

	RateControl m_Rate_control;
};

#endif





















