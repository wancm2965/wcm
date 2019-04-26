#pragma once
#include "XMemIni.h"
#include "AVDataPacket.h"

class IWBDTS
{
public:
	virtual void processWBUIData(AVDataInPacket& in) = 0;			
	virtual void processWBDTSData(AVDataInPacket& in,unsigned long ulSessionID) = 0;	
	virtual void AddWBDTSSession(unsigned long ulSessionID) = 0;
	virtual void RemoveWBDTSSession(unsigned long ulSessionID) = 0;
};