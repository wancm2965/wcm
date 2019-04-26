#pragma once
#ifdef ENABLE_DAHUA
#include "AVFRecord.h"
#include "AVCritSec.h"
#include "avfplay.h"

class CDHPlay :
	public CAVFPlay
{
public:
	CDHPlay(AVSPlayerNotify * pNotify, HWND hWnd);
	~CDHPlay(void);
	virtual bool Connect(const char*cszFileName);// �����ļ�
	virtual void ReleaseConnections(void);		 // �ر�
	virtual void Seek(unsigned long ulTimestamp);
	virtual void Pause(bool bPaused);
	virtual unsigned long GetDuration();
	virtual DWORD Status(DWORD dwFlag);
	virtual bool CaptureToJPGFile(const char *file);
	void Mode(int nSpeed, BOOL bRev);	//����ģʽ nSpeed=0ʱΪ��֡ģʽ
	virtual int Info(void *buffer, unsigned int dwFlag);
	virtual bool IsSupportMode(int nSpeed, int bReversed){return true;}

protected:
	static void CALLBACK Disconnect(LONG lLoginID, char *pchDVRIP, LONG nDVRPort, DWORD dwUser);
	static void CALLBACK PlayDataCallBack(LONG lPlayHandle, DWORD dwTotalSize, DWORD dwDownLoadSize, DWORD dwUser);
private:
	long m_lFile;
	HWND m_hWnd;
	long m_lUser;
	long m_lNewSeekPos;
	long m_lDuration;
	long m_lDurationTime;
};
#endif
