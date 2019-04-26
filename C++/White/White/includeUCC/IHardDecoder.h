// [6/12/2009 lzy]
//-------------------------------------------
//	���뿨���
//-------------------------------------------
#pragma once
//����(����������)
#define	HPNVS_XMC	0/*��ƽ*/
#define HPNVS_HIK	1/*����*/
#define HPNVS_DHD   2/*��*/
#define HPNVS_NZI	3/*����*/

//����һЩ������
#define CARD_SUCCESS			0
#define CARD_FAILED				1
#define CARD_NO_INPORT_USED		2			//û������˿ڿ���
#define CARD_OPEN_CHANN_FAILED	3			//��ͨ��ʧ�� 
#define CARD_UNSUPPORT			4			//���뿨��֧�ִ���

#define TM_FULL		1		//����
#define TM_2x2		4		//4������
#define TM_3x3		9		//9
#define TM_4x4		16		//16


#define HARD_DECODER_API extern "C" __declspec(dllimport)


//Ӳ���뿨�ص��ӿ���
class IHardDecoderNotify 
{
	
public:
// 	virtual void OnDecoderNotifyImageChanged(unsigned short usWidth,unsigned short usHeight,unsigned char usFrameRate)=0;
// 	virtual void OnDecoderNotifyImageData(unsigned char*pData,int nLen)=0;
	virtual void OnNotifyResult(int relOutScreen,int Window,int Result)=0;//֪ͨ���
};

//Ӳ���뿨�ӿ���
class IHardDecoder
{
public:
	IHardDecoder(void){};
	virtual~IHardDecoder(void){};
public:

	//-----------------------------------------------
	//����:��ʼ�����뿨��������صĽ��뿨���ܲ���ʹ��
	//����ֵ:��
	//-----------------------------------------------
	virtual bool InitMDCard(void)=0;
	virtual void Release()=0;

	//-----------------------------------------------
	//����:�õ���ǰϵͳ������˿ڸ���
	//����ֵ:����˿ڸ���
	//-----------------------------------------------
	virtual int GetOutPortCount(void)=0;

	//-----------------------------------------------
	//����:���ý��뿨����˿ڶ�Ӧ��ģ������
	//����:OutNum:�˿ں� 0-15  ϵͳ���16·���
	//	   TemplateType:ģ������,���û��˶����
	//����ֵ:true:�ɹ� false:ʧ��
	//-----------------------------------------------
	virtual bool SetVideoOutMatchTemplate(int OutNum,int TemplateType)=0;

	//-----------------------------------------------
	// ����:�ж�����˿��е�ĳ�������Ƿ��Ѿ����������ʾ
	// ����: OutNum:�˿ں� 0-15  ϵͳ���16·���
	//		 WindowNum:����˿ڶ�Ӧ��ģ���ϵĴ��ں� ��0��ʼ
	// ����ֵ: true:������ʾ false:û����ʾ
	//-----------------------------------------------
	virtual bool IsDisplay(int OutNum,int WindowNum)=0;

	//-----------------------------------------------
	// ����:�û�ȡ��ͨ���ͽ��뿨���ڵĹ�����ʾ
	// ����: OutNum:�˿ں� 0-15 ϵͳ���16·���
	//		 WindowNum:����˿ڶ�Ӧ��ģ���ϵĴ��ں� ��0��ʼ
	// ����ֵ: ��
	//-----------------------------------------------
	virtual void CloseDisplay(int OutNum,int WindowNum)=0;


	//-----------------------------------------------
	// ����:��ʾͨ������Ӧ�Ľ�������,����֮ǰ���жϴ˶�Ӧ�Ĵ����Ƿ���ʾ,����Ѿ���ʾ�ȹرմ˶�Ӧ��ͨ����
	// ����: OutNum:�˿ں� 0-15 һ�����16·���
	//		 WindowNum:����˿ڶ�Ӧ��ģ���ϵĴ��ں� ��0��ʼ
	//		 pData: ͨ������ nLen:���ݳ���
	// ����ֵ: SUCCESS:������ȷ ����:��Ӧ�Ĵ�����
	//-----------------------------------------------
	virtual int  DisplayChannToDecoder(int OutNum,int WindowNum,const char*pData,int nLen)=0;

	//-----------------------------------------------
	// ����:�л�ͬһ������˿��ϵ���������
	// ����: OutNum:�˿ں�
	//		 WindowNum1:����1 һ��Ҫ�Ѿ���
	//		 WindowNum2:����2
	// ����ֵ: true:�л��ɹ� false:�л�ʧ��
	//-----------------------------------------------
	virtual bool SwitchWindow(int OutNum,int WindowNum1,int WindowNum2)=0;

	virtual void SetFilePathName(CString Name)=0;	//Ϊ�˲�����
	virtual void FullScreen(int OutNum,int WindowNum,bool bFull)=0; //ȫ��ָ���Ĵ���
	virtual int GetFullIndex(int OutNum)=0;		//�õ���Ļ��ȫ������

	virtual int  GetMaxWindow(int OutNum)=0;		//�õ�����˿ڶ�Ӧ����󴰿ڸ��� 
	virtual bool IsSupport(const char* pData,DWORD nLen)=0;
	virtual bool IsMaxInput()=0;		//�Ƿ��Ѿ��ﵽ����������
	virtual int GetMaxInput()=0;			//�õ�����������
	

};
// [7/8/2009 lzy]
//��̬����dll

// ����:�õ���ǰӲ����������
// ����ֵ: HPNVS_HIK:����  -1�������� 
HARD_DECODER_API int GetType(void);


//����:�������ʹ���Ӳ���뿨
//����˵��: nMDType:���뿨���� HPNVS_HIK�� 
//			rNotify:֪ͨ�ص��ӿ�
//			nReserved:����
//����ֵ:��Ӧ�Ľ��뿨�ӿڶ���
HARD_DECODER_API IHardDecoder* CreateHardDecoder(int nMDType, IHardDecoderNotify* pNotify,int nReserved);

typedef int (*GETTYPE) (void);
typedef IHardDecoder* (*CREATEDECODER) (int,IHardDecoderNotify*,int);