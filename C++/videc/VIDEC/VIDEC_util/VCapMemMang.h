#ifndef __CVCAPMEMMANG_H__
#define __CVCAPMEMMANG_H__

//视频采集内存管理类，用于模拟内存池的方式分配内存，解决在使用多路采集分配时，频繁分配释放内存造成内存碎片，导致分配失败
//在实际数据的前面一个字节通过0、1来判断该块内存是否已经用完，0表示用完，1表示正在使用
//每个采集图像分配一个对象，由于底层采集关闭后，localvideowindow会缓存一帧数据，因此该对象创建后不释放
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

#define FRAME_SIZE_LEN	1920*1080*3						//AH2K反馈有可能不止是yuv420数据，因此多分配些
#define FRAME_COUNT		4

class CVCapMemMang
	: public VIDEC_MemMang
{
public:
	CVCapMemMang(int nSize, int nCount);
	virtual ~CVCapMemMang(void);

	virtual unsigned char* GetMemory();
	//void SetMemCanUse(unsigned char* pMem);			//由于其他地方也要调用该函数，因此统一放到util里面
	void SetMemNotUse(unsigned char* pMem);
	int MallocMemory();

protected:
	int m_nCount;							//分配内存块的个数
	int m_nSize;							//每个内存块的大小
	unsigned char* m_pMem[FRAME_COUNT];		//分配的内存指针
};

#endif