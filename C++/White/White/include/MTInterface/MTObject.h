#ifndef _AVCON_MTLIB_MULTITOUCH_OBJECT_H_
#define _AVCON_MTLIB_MULTITOUCH_OBJECT_H_
#include "MTObjectBase.h"
#include "MT_Export.h"
#include "MTGesture.h"
#include <list>
#include <map>

#pragma warning(disable:4275)

namespace AVCON_SDK_MTTouch
{
	class CGestureProcess;
	class MTCursor;
	class AVCON_CLASS_MT_GESTURE CMTObject : public CMTObjectBase
	{
	public:
		CMTObject(CMTObject *pParent = NULL);
		virtual ~CMTObject();
		typedef struct SNotifyType{
			bool bNotifyChild;//notify child object or not
			bool bNotifyChildFirst;//notify order: child object process first[true]; child object process last[false]
			bool bNotifyChildOnly;//notify object or not: notify the child but itself[true]; both notify [false]
		}SNotifyType;

	public:
		//operation  for mouse updating message
		virtual void addMouseCursor(int xpos, int ypos);
		virtual void removeMouseCursor(int xpos, int ypos);
		virtual void updateMouseCursor(int xpos, int ypos);

	public:
		//operation  for cursor updating message
		virtual void addMTCursor(MTCursor *tcur);
		virtual void removeMTCursor(MTCursor *tcur);
		virtual void updateMTCursor(MTCursor *tcur);

	public:
		//layer operation for touch object in object list(m_childList)
		bool LayerUp(CMTObject *pChildObj);	//up
		bool LayerDown(CMTObject *pChildObj);//down
		bool LayerTop(CMTObject *pChildObj);	//top
		bool LayerBottom(CMTObject *pChildObj);//bottom

		//Get Parent
		CMTObject * GetParentNode()const;

		//Get size of all the child object
		const unsigned int GetChildSize()const;

	protected:
		//Add touch object as it's child
		bool AddChild(CMTObject *pChild);
		//remove a special touch object from it's children
		bool RemoveChild(CMTObject *pChild);

		//destroy a special touch object from it's children
		bool DeleteChild(CMTObject *pChild);
		//Destroy All Children
		void DeleteAllChildren();


	public:
		
		//add a Specific gesture.
		bool AddGesture(const ETouchGestureType type, const void* pConfig = NULL)const;
		//the user-defined gesture,and return an unique ID associated to this gesture.
		bool AddGesture(const std::string &sGestureName)const;

		//remove a Specific gesture
		bool RemoveGesture(const ETouchGestureType type)const;
		//remove a user-defined gesture
		bool RemoveGesture(const std::string &sGestureName)const;


		//config a special gesture type for a class of gestures. for more, 
		//see the development manual.
		bool ConfigGesture(ETouchGestureType type, const void *)const;
		//a feedback mechanism, see the meaning of the struct of SNotifyType.
		void ConfigNotifyType(const SNotifyType &sNotifyType);

		/*------------------------------------------------------------------------------
		a)Suspend current gesture analysis( will auto analysis the next touch fingers when)
			all current fingers are all liftoff )
		b)Reset input data for the MTObject. if you override this member function, 
			call CMTObject::SuspendTemporary() at last.
		------------------------------------------------------------------------------*/
		virtual bool SuspendTemporary();
		//Get the Multi-touch object active state.
		bool BMTObjectActive()const;

	protected:
		//set the active state for the object
		void SetMTObjectActive( bool bActive /*= true*/ );

	private:
		//example: sometimes you touch object lose focus, or you want make 
		//events mutual exclusion, you can overwrite it and return false.
		virtual bool BMTObjectValid() const { return true; }
		//hit the touch object?( should be overrided)
		virtual bool BInRegion(const POINT &pt)const  {return false;}

	private:
		//override it to deal with touch event(just like OnLButtonDown())
		virtual void touchDownHandler(const TouchDown& te){};
		//override it to deal with touch event(just like OnMouseMove())
		virtual void touchMoveHandler(const TouchMove& te){};
		//override it to deal with touch event(just like OnLButtonUp())
		virtual void touchUpHandler(const TouchUp& te){};
	public:
		//override it to deal with Dynamic event(like translation, 
		//amplification(zoom in or zoom out), rotation)
		virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar){};
		//override it to deal with common message
		virtual void OnCallbackGesture(const unsigned int &nMessage, 
			const unsigned int &nAppendMessage = 0){};
		
		//Get All the child object
		const std::list<CMTObject *> *GetChildList()const{return m_pChildList;};
		//Get All the present cursors [just for temp use]
		const std::list<MTCursor *> *GetCursorList()const{return m_pCursorList;};

	private:
		//child object(can be null)
		std::list<CMTObject *> *m_pChildList;
		//container of the current cursors' pointers
		std::list<MTCursor *> *m_pCursorList;

		std::map<DWORD, CMTObject*> *m_pChildObjectMap;
		//parent object: reserved
		CMTObject *m_pParent;
		//gesture process object
		CGestureProcess *m_pGestureProcess;

		//treat mouse cursor to touch cursor.
		MTCursor *m_pMouseCursor;

		//configuration for the Scene's(mt object) the message deliver
		SNotifyType	m_configNotify;

	private:
		//be Active or not; default is  true
		bool m_bActive;
		
		//mutex for MultiThread error
	private:
		CRITICAL_SECTION m_csCursorOperation;
		CRITICAL_SECTION m_csMouseOperation;
		CRITICAL_SECTION m_csChildOperation;
		CRITICAL_SECTION m_csGestureOperation;
	};
};	// end of namespace
#endif
