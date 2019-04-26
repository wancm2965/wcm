// ReSizeMgr.h: interface for the CReSizeMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESIZEMGR_H__B04FC2DE_16FA_497B_860D_252E4C8D63A4__INCLUDED_)
#define AFX_RESIZEMGR_H__B04FC2DE_16FA_497B_860D_252E4C8D63A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectBase.h"

typedef std::deque<CObjectBase*>	DEQUE_OBJECT;

class DIRECTUI_API CReSizeMgr  
{
	enum DirectionType
	{
		HORZ_TYPE = 0,
		VERT_TYPE
	};

public:
	void ReSize( int nSpace );

public:
	void AddObj(CObjectBase* pObj);
	void EraseObj(CObjectBase* pObj);
	BOOL HasObj(CObjectBase* pObj);

	void SetDirection(DirectionType eDirectionType){m_eDirectTionType = eDirectionType;}
	DirectionType GetDirection(){return m_eDirectTionType;}
	BOOL CheckDequeObj();

private:
	void	ModifyObjPos(CObjectBase* pObj, int nSpace_T_L, int nObjLen);
	
private:
	DEQUE_OBJECT m_dequeObjs;
	DirectionType m_eDirectTionType;	
};

#endif // !defined(AFX_RESIZEMGR_H__B04FC2DE_16FA_497B_860D_252E4C8D63A4__INCLUDED_)
