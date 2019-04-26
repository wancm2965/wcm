// XDataPool.h: interface for the XDataPool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XDATAPOOL_H__
#define __XDATAPOOL_H__

#include <stdlib.h>

#include "XAutoLock.h"
#include "XListPtr.h"

class XDataPool 
{
public:
	XDataPool(int nMargin=0);
	virtual ~XDataPool(void);
public:
	virtual void PushBack(void*pData,int nLen);
	virtual void LockPool(void);
	virtual void UnlockPool(void);
	virtual bool get_front(void**ppData,int&nLen);
	virtual void PopFront(void);
	virtual int GetDataCount(void);
	virtual void FlushPool(void);
protected:
	struct XDataBuffer
	{
		XDataBuffer(void)
			: pBuffer(NULL)
			, nLen(0)
		{
		}
		virtual~XDataBuffer(void)
		{
			if(pBuffer)
			{
				FREEMSG(pBuffer);
				pBuffer=NULL;
			}
		}

		void*	pBuffer;
		int		nLen;
	};

	XListPtr		m_ListXDataBuffer;
	XCritSec		m_csListXDataBuffer;
	int				m_nMargin;
};

#endif