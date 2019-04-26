// AVThread.h: interface for the CAVThreadclass.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVTHREAD_H__
#define __AVTHREAD_H__

class HPBC_AFX_EXT_CLASS AVThread
{
public:
	AVThread();
	virtual ~AVThread();
public:
	/*
	�����̣߳��ɹ�����true,ʧ�ܷ���false,ֻ���ڸý��̴������߳�������ϵͳ����ʱ�ŷ���false
	*/
	bool StartThread(void);
	/*
	�ȴ��߳��˳������ô˺���ǰ������÷�������ȷ�������̺߳������˳���������ô˺���������
	*/
	void WaitForStop(void);

	/*
	�˺���ֻ�����ڲ�����
	*/
#ifdef WIN32
	static DWORD WINAPI InitThreadProc(PVOID pObj){
		return	((AVThread*)pObj)->ThreadProc();
	}
#else
	static void* InitThreadProc(void*pObj){
		((AVThread*)pObj)->ThreadProc();
		return 0;
	}
#endif
	/*
	�˺���ֻ�����ڲ�����
	*/
	unsigned long ThreadProc(void);

protected:
	/*
	���̺߳������������ڴ˺�����ʵ���߳�Ҫ������
	*/
	virtual void ThreadProcMain(void)=0;
protected:
#ifdef WIN32
	DWORD	m_dwThreadID;		// �̱߳�ʶ
	HANDLE	m_hThread;			// �߳̾��
	HANDLE	m_evStop;
#else
    pthread_t	m_thread;
    sem_t		m_semWaitForStop;
    bool		m_bThreadStopped;
#endif
};

/*
�߳�����

class TestThread:public AVThread
{
public:
	TestThread()
	{
		m_bWantToStop=false;
	};
	virtual~TestThread()
	{
	};
public:
	bool Connect(void)
	{
		//�����߳�
		return StartThread();
	};

	void ReleaseConnections(void)
	{
		//�����̺߳������˳�
		m_bWantToStop=true;
		//�ȴ��߳��˳�
		WaitForStop();
	};
protected:
	void ThreadProcMain(void)
	{
		//ÿ��һ�����һ�����֣���0��ʼ����
		int i=0;
		//ֻҪû���˳��źžͼ������
		while(!m_bWantToStop)
		{
			Sleep(1000);
		}
	};
protected:
	bool	m_bWantToStop;
};
*/

#endif
