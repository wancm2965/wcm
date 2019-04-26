//NETEC_MCUTest.h
#ifndef __NETEC_MCUTEST_H__
#define __NETEC_MCUTEST_H__

#include <NETEC/NETEC_Export.h>

class NETEC_API NETEC_MCUTest
{
public:
	NETEC_MCUTest(void){};
	virtual~NETEC_MCUTest(void){};
public:
	/******************************************************************************
	* Start
	*����������MCU����
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int32_t Start(void);

	/******************************************************************************
	* Stop
	*������ֹͣMCU����
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void Stop(void);

	/******************************************************************************
	* AddMCU
	*���������Ҫ���Ե�MCU��IP��ַ�Ͷ˿�
	*���룺	cszMCUIP			-MCU��IP��ַ
			usMCUPort			-MCU�Ķ˿�
	*����� ��
	*����ֵ���ɹ����ز��Ա�ʶ��ʧ�ܷ���0
	*****************************************************************************/
	virtual uint32_t AddMCU(const char*cszMCUIP,unsigned short usMCUPort);
	
	/******************************************************************************
	* OnNETEC_MCUTestBegin
	*������MCU��ʼ���ԵĻص�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestBegin(void)=0;
	
	/******************************************************************************
	* OnNETEC_MCUTestBegin
	*������MCUĳ����ַ��ʼ���ԵĻص�
	*���룺	ulTestID			-���Ա�ʶ
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestBegin(uint32_t ulTestID)=0;
	
	/******************************************************************************
	* OnNETEC_MCUTestTXBitrateChanged
	*������MCUĳ����ַ���д���ı�Ļص�
	*���룺	ulTestID			-���Ա�ʶ
			nBitrate			-���д���kbps��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestTXBitrateChanged(uint32_t ulTestID,int32_t nBitrate)=0;
	
	/******************************************************************************
	* OnNETEC_MCUTestTXBitrateChanged
	*������MCUĳ����ַ���д���ı�Ļص�
	*���룺	ulTestID			-���Ա�ʶ
			nBitrate			-���д���kbps��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestRXBitrateChanged(uint32_t ulTestID,int32_t nBitrate)=0;
	
	/******************************************************************************
	* OnNETEC_MCUTestRTTChanged
	*������MCUĳ����ַ����ʱ��ı�Ļص�
	*���룺	ulTestID			-���Ա�ʶ
			nRTT				-����ʱ�䡾���롿
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestRTTChanged(uint32_t ulTestID,uint32_t nRTT)=0;
	
	/******************************************************************************
	* OnNETEC_MCUTestEnd
	*������MCUĳ����ַ�������ԵĻص�
	*���룺	ulTestID			-���Ա�ʶ
			nMinTXBitrate		-��С���д���kbps��
			nMaxTXBitrate		-������д���kbps��
			nAvgTXBitrate		-ƽ�����д���kbps��
			nMinRXBitrate		-��С���д���kbps��
			nMaxRXBitrate		-������д���kbps��
			nAvgRXBitrate		-ƽ�����д���kbps��
			nMinRTT				-��С����ʱ�䡾���롿
			nMaxRTT				-�������ʱ�䡾���롿
			nAvgRTT				-ƽ������ʱ�䡾���롿
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestEnd(uint32_t ulTestID,int32_t nMinTXBitrate,int32_t nMaxTXBitrate,int32_t nAvgTXBitrate,int32_t nMinRXBitrate,int32_t nMaxRXBitrate,int32_t nAvgRXBitrate,uint32_t nMinRTT,uint32_t nMaxRTT,uint32_t nAvgRTT)=0;
	
	/******************************************************************************
	* OnNETEC_MCUTestEnd
	*������MCU�������ԵĻص�
	*���룺	��
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_MCUTestEnd(void)=0;
};


#endif
