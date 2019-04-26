#ifndef _AVCON_MTLIB_MULTITOUCH_OBJECT_BASE_H_
#define _AVCON_MTLIB_MULTITOUCH_OBJECT_BASE_H_
#include "MTCursor.h"
#include "MTGesture.h"

namespace AVCON_SDK_MTTouch
{
	class CMTObjectBase
	{
	public:
		virtual ~CMTObjectBase(){};
	public:
		//operation  for mouse updating message
		virtual void addMouseCursor(int xpos, int ypos) = 0;
		virtual void removeMouseCursor(int xpos, int ypos) = 0;
		virtual void updateMouseCursor(int xpos, int ypos) = 0;

		//operation  for cursor updating message
		virtual void addMTCursor(MTCursor *tcur) = 0;
		virtual void removeMTCursor(MTCursor *tcur) = 0;
		virtual void updateMTCursor(MTCursor *tcur) = 0;

	public:
		//Get Parent
		virtual CMTObjectBase * GetParentNode()const = 0;
		//
		virtual bool BMTObjectActive()const = 0;
		//set the active state for the object
		virtual void SetMTObjectActive( bool bActive /*= true*/ ) = 0;

		//example: sometimes you touch object lose focus, or you want make 
		//events mutual exclusion, you can overwrite it and return false.
		virtual bool BMTObjectValid() const = 0;
		//hit the touch object?( should be overrided)
		virtual bool BInRegion(const POINT &pt)const = 0;

	private:
		//override it to deal with touch event(just like OnLButtonDown())
		virtual void touchDownHandler(const TouchDown& te) = 0;
		//override it to deal with touch event(just like OnMouseMove())
		virtual void touchMoveHandler(const TouchMove& te) = 0;
		//override it to deal with touch event(just like OnLButtonUp())
		virtual void touchUpHandler(const TouchUp& te) = 0;
	private:
		//override it to deal with Dynamic event(like translation, 
		//amplification(zoom in or zoom out), rotation)
		virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar) = 0;
		//override it to deal with common message
		virtual void OnCallbackGesture(const unsigned int &nMessage, 
			const unsigned int &nAppendMessage = 0) = 0;
	};
};	// end of namespace
#endif
