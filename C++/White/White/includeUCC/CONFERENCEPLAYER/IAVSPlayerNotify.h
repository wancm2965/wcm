#pragma once
#include "ConferencePlayer\IAVSPlayerEvent.h"

class IAVSPlayerNotify
{
public:	
	virtual void OnSetPlayerHandle(IAVSPlayerEvent *pPlayer) = 0;
	virtual void OnAVSPlayerNotify_PlayEvent(unsigned long ulParam1, unsigned long ulParam2) = 0;
	virtual void OnReleaseConnections()=0;
};
