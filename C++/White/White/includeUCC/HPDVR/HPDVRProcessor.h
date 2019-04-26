/************************************************************************/
/* HPDVRProcessor.h                                                     */
/************************************************************************/
#pragma once

class IHPDVRProcessorCallback
{
public:
	virtual void OnHPDVRProcessorOutputData(char*pData,int nLen) = 0;
};

class IHPDVRDispatcherCallback
{
public:
	virtual void OnHPDVRDispatcherOutputData(char*pData,int nLen) = 0;
};

class IHPDVRProcessor 
	: public KThread
	, public KBufferPool
{
public:
	IHPDVRProcessor() {}
	~IHPDVRProcessor() {}

public:
	virtual bool Connect()
	{
		m_bWantToStop = false;
		return StartThread();
	}

	virtual void Release()
	{
		m_bWantToStop = true;
		WaitForStop();
		Clear();/*Çå¿Õ»º³åÇø*/
	}

	virtual void OnDispatchCommand(char*pData,int nLen)=0;
	
public:
	void PushBufferPool(char* pData,int nLen)
	{
		KBuffer* pBuffer=new KBuffer();
		if(pBuffer)
		{
			if(pBuffer->SetBuffer(pData,nLen))
			{
				PushBack(pBuffer);
			}
		}
	}
protected:
	bool ProcessBufferPool(void)
	{
		if(GetSize()==0)
		{
			return false;
		}

		while(GetSize())
		{
			KBuffer* pBuffer = PopFront();
			if(pBuffer)
			{
				OnDispatchCommand((char*)pBuffer->GetBuffer(), pBuffer->GetSize());
				delete pBuffer;
				pBuffer=NULL;
			}
		}
		return true;
	}

protected:
	virtual void ThreadProcMain(void)
	{
		while(!m_bWantToStop)
		{
			if (!ProcessBufferPool())
			{
				Pause(1);
			}
		}
	}
protected:
	bool m_bWantToStop;
};