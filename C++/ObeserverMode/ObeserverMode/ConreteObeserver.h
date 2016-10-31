#pragma once
#include "obeserver.h"

class CConreteObeserver :
	public CObeserver
{
public:
	CConreteObeserver(std::string strname);
	~CConreteObeserver(void);

	virtual void update(CSubject* psub);

private:
	std::string m_strName;
};
