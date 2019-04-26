// IDUISound.h: interface for the IDUISound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDUISOUND_H__C8D45841_73AD_4BA4_B230_91205844FC09__INCLUDED_)
#define AFX_IDUISOUND_H__C8D45841_73AD_4BA4_B230_91205844FC09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IDUISound  
{
public:
	virtual ~IDUISound(){};

public:
	virtual void	Play() = 0;
	virtual void	Stop() = 0;

};

#endif // !defined(AFX_IDUISOUND_H__C8D45841_73AD_4BA4_B230_91205844FC09__INCLUDED_)
