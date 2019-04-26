#ifndef __CVCAPMEMMANG_H__
#define __CVCAPMEMMANG_H__

//��Ƶ�ɼ��ڴ�����࣬����ģ���ڴ�صķ�ʽ�����ڴ棬�����ʹ�ö�·�ɼ�����ʱ��Ƶ�������ͷ��ڴ�����ڴ���Ƭ�����·���ʧ��
//��ʵ�����ݵ�ǰ��һ���ֽ�ͨ��0��1���жϸÿ��ڴ��Ƿ��Ѿ����꣬0��ʾ���꣬1��ʾ����ʹ��
//ÿ���ɼ�ͼ�����һ���������ڵײ�ɼ��رպ�localvideowindow�Ỻ��һ֡���ݣ���˸ö��󴴽����ͷ�
class VIDEC_MemMang
{
public:
	VIDEC_MemMang(void){};
	virtual~VIDEC_MemMang(void){};
public:
	virtual unsigned char* GetMemory() = 0;
	virtual void SetMemNotUse(unsigned char* pMem) = 0;


	static VIDEC_MemMang*Create(void);
};

#define FRAME_SIZE_LEN	1920*1080*3						//AH2K�����п��ܲ�ֹ��yuv420���ݣ���˶����Щ
#define FRAME_COUNT		4

class CVCapMemMang
	: public VIDEC_MemMang
{
public:
	CVCapMemMang(int nSize, int nCount);
	virtual ~CVCapMemMang(void);

	virtual unsigned char* GetMemory();
	//void SetMemCanUse(unsigned char* pMem);			//���������ط�ҲҪ���øú��������ͳһ�ŵ�util����
	void SetMemNotUse(unsigned char* pMem);
	int MallocMemory();

protected:
	int m_nCount;							//�����ڴ��ĸ���
	int m_nSize;							//ÿ���ڴ��Ĵ�С
	unsigned char* m_pMem[FRAME_COUNT];		//������ڴ�ָ��
};

#endif