//IHPDTS.h
#ifndef __IHPDTS_H__
#define __IHPDTS_H__

//========================================================================================
//��Ȩ����: ��Ȩ����(C) 2008���Ϻ���ƽ��Ϣ�����ɷ����޹�˾
//ϵͳ����: AVCON7
//��������: duquansheng
//��������: 2008-3-31
//����˵��: ���ݴ���������ں˽ӿ��ඨ��
//

#ifdef HPDTS_EXPORT
#define HPDTS_API _declspec(dllexport)
#elif HPDTS_DLL
#define HPDTS_API _declspec(dllimport)
#else
#define HPDTS_API
#endif


//---------------------------------------------------------------------------------------
class HPDTS_API IHPDTSNotify
{
public:
	IHPDTSNotify(void){};
	virtual~ IHPDTSNotify(void){};
public:
	virtual void onDTSNetDataNotify(uint32_t ulSessionID,const char* pData,int32_t nLen)=0;  //�ص���������
	virtual void onDTSUIDataNotify(const char* pData,int32_t nLen)=0;							  //�ص���������
};


//---------------------------------------------------------------------------------------
class IHPDTS
{
public:
	IHPDTS(void){};
	virtual~IHPDTS(void){};

	virtual void Init(void) = 0;
	virtual void Release(void) = 0;
public:
	virtual void processUIData(const char* pData,int32_t nLen)=0;								//�����������
	virtual void processDTSData(uint32_t ulSessionID,const char* pData,int32_t nLen)=0;	//������������
	virtual void AddDTSSession(uint32_t ulSessionID,bool bHost/*��ulSessionID�Ƿ�ΪHost Server*/)=0; //����DTS����
	virtual void RemoveDTSSession(uint32_t ulSessionID)=0;								//ɾ��DTS����
};

//---------------------------------------------------------------------------------------
HPDTS_API IHPDTS*CreateIHPDTS(IHPDTSNotify& rIHPDTSNotify,bool bUI/*�Ƿ���UI*/);

HPDTS_API void HPDTSInit(const char* cszTemPath/*�����ļ���ʱĿ¼*/);	
HPDTS_API void HPDTSUnInit(void);
HPDTS_API void HPDTSSetSavePath(const char* cszSavePath);//�ͻ��������ļ�����Ŀ¼,���ظ�����
HPDTS_API const char* GetFileTemPath(void);
HPDTS_API const char* GetFileSavePath(void);

#endif