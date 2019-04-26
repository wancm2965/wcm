#ifndef __IAVSPLAYEREVENT_H__
#define __IAVSPLAYEREVENT_H__

#define		PLAY_EVENT_TIMESTAMP		1
#define		PLAY_EVENT_OVER				2
#define		PLAY_EVENT_START_BUFFER		3
#define		PLAY_EVENT_STOP_BUFFER		4
#define		PLAY_EVENT_START_SEEK		5
#define		PLAY_EVENT_STOP_SEEK		6


class IAVSPlayerEvent
{
public:
	virtual bool Connect(const char*cszFileName)=0;
	virtual void ReleaseConnections(void)=0;
	virtual unsigned long GetDuration(void)=0;
	virtual unsigned long GetPosition(void)=0;
	virtual void Seek(unsigned long ulTimestamp)=0;
	virtual void Pause(bool bPaused)=0;
	virtual bool IsPaused(void)=0;
	virtual bool IsOpen()=0;
	virtual bool IsStop()=0;
	virtual bool IsReadOver()=0;
	virtual void QuitMySystem()=0;	//强行退出系统
};

#endif