#ifndef DUI_DEBUG_HELPER_H
#define DUI_DEBUG_HELPER_H

#include "ObjectBase.h"

#ifdef _DEBUG

void _PutOutObjPosition(CObjectBase* pObjBase);
void _PutOutObjRect(CObjectBase* pObjBase);
void _PutOutRect(const CRect& rc);

#define PutOutObjPosition(pObjBase) _PutOutObjPosition(pObjBase);
#define PutOutObjRect(pObjBase) _PutOutObjRect(pObjBase);
#define PutOutRect(rect) _PutOutRect(rect);

#else

#define PutOutObjPosition(pObjBase)
#define PutOutObjRect(pObjBase)
#define PutOutRect(rect)

#endif //_DEBUG


#endif //DUI_DEBUG_HELPER_H