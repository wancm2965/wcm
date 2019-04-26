
#ifndef _AVCON_MTLIB_MULTITOUCH_CURSOR_H_
#define _AVCON_MTLIB_MULTITOUCH_CURSOR_H_

#include "MTContainer.h"
#include "MT_Export.h"

namespace AVCON_SDK_MTTouch {
	
	/**
	 * The MTCursor class encapsulates cursors.
	 */ 
	class AVCON_CLASS_MT_GESTURE MTCursor: public MTContainer {
		 
	protected:
		/**
		 * The individual cursor ID number that is assigned to each MTCursor.
		 */ 
		int cursor_id;
		
	public:
		using MTContainer::update;
		MTCursor (MTTime ttime, long si, int ci, float xp, float yp);
		MTCursor (long si, int ci, float xp, float yp);
		
		MTCursor (const std::list<MTPoint>& lstPoint);
		MTCursor (MTCursor *tcur);
		
		/**
		 * The destructor is doing nothing in particular. 
		 */
		~MTCursor(){};
		
		int getCursorID() const;
	};
}
#endif
