#ifndef _AVCON_MTLIB_MULTITOUCH_CONTAINER_H_
#define _AVCON_MTLIB_MULTITOUCH_CONTAINER_H_

#include <list>
#include "MTPoint.h"
#include "MT_Export.h"

#define POINT_STATE_IDLE 0
#define POINT_STATE_ADDED 1
#define POINT_STATE_ACCELERATING 2
#define POINT_STATE_DECELERATING 3
#define POINT_STATE_ROTATING 4
#define POINT_STATE_STOPPED 5
#define POINT_STATE_REMOVED 6

namespace AVCON_SDK_MTTouch {

	/**
	* The abstract MTContainer class defines common attributes that apply to {@link MTCursor}.
	*/ 
	class AVCON_CLASS_MT_GESTURE MTContainer: public MTPoint 
	{
	public:
		//using MTPoint::update;
		//The Constructor.
		MTContainer (MTTime ttime, long si, float xp, float yp);
		MTContainer (long si, float xp, float yp);
		MTContainer (MTContainer *tcon);

		//The destructor: doing nothing. 
		virtual ~MTContainer();

		//
		virtual void setSource(int src_id, const char *src_name, const char *src_addr);

		//
		virtual const char* getSourceName() const;
		virtual const char* getSourceAddress() const;
		virtual int getSourceID() const;

		//Update
		virtual void update (MTTime ttime, float xp, float yp);
		virtual void stop(MTTime ttime);
		virtual void update (MTTime ttime, float xp, float yp, float xs, float ys, float ma);
		virtual void update (float xp, float yp, float xs, float ys, float ma);
		virtual void update (MTContainer *tcon);

		//
		virtual void remove(MTTime ttime);

		//
		virtual long getSessionID() const;
		virtual void setSessionID(long s_id);

		//
		virtual float getXSpeed() const;
		virtual float getYSpeed() const;
		virtual MTPoint getPosition() const;

		//
		virtual std::list<MTPoint> getPath() const;

		//
		virtual float getMotionSpeed() const;
		virtual float getMotionAccel() const;
		virtual int getState() const;
		virtual bool isMoving() const;

	protected:
		/**
		* The unique session ID number that is assigned to each object or cursor.
		*/ 
		long session_id;
		/**
		* The X-axis velocity value.
		*/ 
		float x_speed;
		/**
		* The Y-axis velocity value.
		*/ 
		float y_speed;
		/**
		* The motion speed value.
		*/ 
		float motion_speed;
		/**
		* The motion acceleration value.
		*/ 
		float motion_accel;
		/**
		* A List of MTPoints containing all the previous positions of the component.
		*/ 
		std::list<MTPoint>* path;
		/**
		* Reflects the current state of the Component
		*/ 
		int state;
		/**
		* The ID of the source
		*/ 
		int source_id;	
		/**
		* The name of the source(valid for TUIO, so far)
		*/ 
		char	source_name[MAX_PATH];
		/**
		* The address of the source(valid for TUIO, so far)
		*/ 
		char	source_addr[MAX_PATH];
	};
}
#endif
