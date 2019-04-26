#ifndef _AVCON_MTLIB_MULTITOUCH_POINT_H_
#define _AVCON_MTLIB_MULTITOUCH_POINT_H_

#include "MTTime.h"
#include <cmath>
#include "MT_Export.h"

#ifndef PI_MULTITOUCH
#define PI_MULTITOUCH		(3.14159265358979323846)
#define PI2_MULTITOUCH		(PI_MULTITOUCH*2)
#endif

namespace AVCON_SDK_MTTouch {

	class AVCON_CLASS_MT_GESTURE MTPoint
	{
	public:
		///The Constructor.
		MTPoint (float xp, float yp);
		MTPoint (MTTime ttime, float xp, float yp);
		MTPoint (MTPoint *tpoint);

		///The Destructor:doing nothing.
		~MTPoint(){};
		///Update info.
		void update (MTPoint *tpoint);	
		void update (float xp, float yp);
		void update (MTTime ttime, float xp, float yp);

		///Get info.
		float getX() const;
		float getY() const;
		MTTime getMTTime() const;
		MTTime getStartTime() const;

		//MTPoint GetScreenPoint() const;

		///Compute the distance.
		float getDistance(float xp, float yp) const;
		float getDistance(MTPoint *tpoint) const;

		///Compute the angle./Returns the angle
		float getAngle(float xp, float yp) const;
		float getAngle(MTPoint *tpoint) const;
		///Returns the angle in degrees
		float getAngleDegrees(float xp, float yp) const;
		float getAngleDegrees(MTPoint *tpoint) const;


	protected:
		/**
		* X coordinate, in pixel,so far
		*/
		float xpos;
		/**
		* X coordinate, in pixel,so far  
		*/
		float ypos;
		/**
		* The time stamp of the last update represented as MTTime (time since session start)
		*/
		MTTime currentTime;
		/**
		* The creation time of this MTPoint represented as MTTime (time since session start)
		*/
		MTTime startTime;

	};
}
#endif