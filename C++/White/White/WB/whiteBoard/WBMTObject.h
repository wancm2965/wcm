#ifndef _AVCON_WHITEBOARD_WBMTOBJECT_H_
#define _AVCON_WHITEBOARD_WBMTOBJECT_H_
#include "MTGestureImport.h"
#include <map>

#ifdef SUPPORTMTGES
class CWBMTObject 
#else
class CWBMTObject : public AVCON_SDK_MTTouch::CMTObject
#endif
//class CWBMTObject : public AVCON_SDK_MTTouch::CMTObject

{
protected:
	CWBMTObject();
	~CWBMTObject();

protected:
	//��������λ�ж�
	bool  DoesCarryCounting()const;
public:
	//���һ����ָ����ʱ����������д��[�����һ��������������λ���ж��Ƿ�����������]
	virtual void TouchUpAllCursor(bool bchangID, const AVCON_SDK_MTTouch::TouchUp& te){ m_bCounting = 0;}
protected:
	virtual void OnGestureCombineHandle(const AVCON_SDK_MTTouch::GestureCombinePar &gesPar);

protected:
	//��������
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	virtual void touchDownHandler(const TouchDown& te);
	virtual void touchMoveHandler(const TouchMove& te);

// 	virtual void addMouseCursor(int xpos, int ypos){};
// 	virtual void removeMouseCursor(int xpos, int ypos){};
// 	virtual void updateMouseCursor(int xpos, int ypos){};

protected:
	//ID����������ĵ�ӳ��(����list����)
	std::map<unsigned long, unsigned long> m_map;
	//����
	unsigned short m_bCounting;

	//�жϴ��������Ƿ��иı�
	bool m_bChangID;
};

#endif