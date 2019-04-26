//XImage.h
#ifndef __XIMAGE_H__
#define __XIMAGE_H__

#include <VIDEC/VIDEC.h>

#include "../VIDEC_FRE/VIDECFREAPI.h"

class XImage
	: public VIDEC_Image
{
public:
	XImage(void*pData,int nLen,int nBufOffset = 0, int nAH2KNew = 0);
	virtual~XImage(void);
public:
	virtual void AddRef(void);
	virtual void Release(void);

	virtual int GetWidth(void);
	virtual int GetActualWidth(void);
	virtual void SetActualWidth(int nActualWidth);
	virtual int GetHeight(void);
	virtual int GetActualHeight(void);
	virtual void SetActualHeight(int nActualHeight);
	virtual int GetVirtualWidth(void);
	virtual void SetVirtualWidth(int nVirtualWidth);
	virtual int GetVirtualHeight(void);
	virtual void SetVirtualHeight(int nVirtualHeight);
	virtual bool GetEvenFieldFirst(void);
	virtual void SetEvenFieldFirst(bool bEvenFieldFirst);
	virtual bool GetDoubleField(void);
	virtual void SetDoubleField(bool bDoubleField);
	virtual bool GetEvenField(void);
	virtual void SetEvenField(bool bEvenField);
	virtual bool GetOddField(void);
	virtual void SetOddField(bool bOddField);
	virtual bool GetHardware(void);
	virtual void SetHardware(bool bHardware);
	virtual unsigned long GetTimestamp(void);
	virtual void SetTimestamp(unsigned long ulTimestamp);
	virtual void SetTimestampOffset(unsigned long ulOffset);
	virtual unsigned long GetTimestampOffset(void);

	virtual void SetVideoSignal(bool bVideoSignal);
	virtual bool GetVideoSignal(void);

	virtual int GetFrameRate(void);
	virtual void SetFrameRate(int nFrameRate);

	virtual void*GetData(void);
	virtual int GetDataLen(void);
	virtual void SetDataLen(int nDataLen);

	virtual void SetStride(int nStride);
	virtual int GetStride(void);

	virtual void SetNV12(bool bNV12);
	virtual bool GetNV12(void);
protected:
    volatile LONG	m_cRef;

	int				m_nActualWidth;				//��ʵ��ͼƬ��С
	int				m_nActualHeight;			//��ʵ��ͼƬ��С
	int				m_nVirtualWidth;			//Ҫ��ʾ��ͼƬ��С
	int				m_nVirtualHeight;			//Ҫ��ʾ��ͼƬ��С
	bool			m_bEvenFieldFirst;
	bool			m_bDoubleField;
	bool			m_bEvenField;
	bool			m_bOddField;
	bool			m_bHardware;
	unsigned long	m_ulTimestamp;				//�ɼ�����ʱ��
	unsigned long	m_ulTimestampOffset;				//�ɼ�����ʱ��
	int				m_nFrameRate;
	bool			m_bVideoSignal;

	void*			m_pBuf;
	void*			m_pData;
	int				m_nDataLen;
	int				m_nBufLen;
	int				m_nBufOffset;

	int				m_nStride;
	bool			m_bNV12;

	int				m_nAH2KNew;				//���������ǲ��������ɼ����������� 0:����AH2K�ɼ������ģ�>0��AH2K�ɼ�������
};

class XImageAutoRelease
{
public:
	XImageAutoRelease(VIDEC_Image*pImage)
		: m_pImage(pImage)
	{
	};
	virtual~XImageAutoRelease(void)
	{
		if (m_pImage!=NULL)
		{
			m_pImage->Release();
			m_pImage=NULL;
		}
	}
protected:
	VIDEC_Image*		m_pImage;
};

#endif

