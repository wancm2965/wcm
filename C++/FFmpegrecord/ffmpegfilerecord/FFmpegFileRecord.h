#pragma once
#include "FFmpegrecord/Iffmpegfilerecord.h"
class CFFmpegFileRecord:public IFFmpegfilerecord
{
public:
	CFFmpegFileRecord(IFFmpegfilerecordNotify& rNotify);
	~CFFmpegFileRecord();

	virtual int CompoundFile(const char*cszFileName, const char*cszVideoFileName, const char*cszAudioFileName);

private:
	IFFmpegfilerecordNotify&m_rNotify;
};

