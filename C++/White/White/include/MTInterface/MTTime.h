#ifndef _AVCON_MTLIB_MULTITOUCH_TIME_H_
#define _AVCON_MTLIB_MULTITOUCH_TIME_H_

#ifndef WIN32
#include <pthread.h>
#include <sys/time.h>
#else
#include <windows.h>
#include <ctime>
#endif
#include "MT_Export.h"

#define MSEC_SECOND 1000
#define USEC_SECOND 1000000
#define USEC_MILLISECOND 1000

namespace AVCON_SDK_MTTouch {
	
	/**
	 * The MTTime class is a simple structure that is used to reprent the time that has elapsed since the session start.
	 * The time is internally represented as seconds and fractions of microseconds which should be more than sufficient for gesture related timing requirements.
	 * Therefore at the beginning of a typical session the static method initSession() will set the reference time for the session. 
	 * Another important static method getSessionTime will return a MTTime object representing the time elapsed since the session start.
	 * The class also provides various addtional convience method, which allow some simple time arithmetics.
	 */ 
	class  AVCON_CLASS_MT_GESTURE MTTime {
		
	private:
		long seconds;
		long micro_seconds;
		static long start_seconds;
		static long start_micro_seconds;
		
	public:

		/**
		 * The default constructor takes no arguments and sets   
		 * the Seconds and Microseconds attributes of the newly created MTTime both to zero.
		 */
		MTTime ():seconds(0),micro_seconds(0) {};

		/**
		 * The destructor is doing nothing in particular. 
		 */
		~MTTime() {}
		
		/**
		 * This constructor takes the provided time represented in total Milliseconds 
		 * and assigs this value to the newly created MTTime.
		 *
		 * @param  msec  the total time in Millseconds
		 */
		MTTime (long msec);
		
		/**
		 * This constructor takes the provided time represented in Seconds and Microseconds   
		 * and assigs these value to the newly created MTTime.
		 *
		 * @param  sec  the total time in seconds
		 * @param  usec	the microseconds time component
		 */	
		MTTime (long sec, long usec);

		/**
		 * Sums the provided time value represented in total Microseconds to this MTTime.
		 *
		 * @param  us	the total time to add in Microseconds
		 * @return the sum of this MTTime with the provided argument in microseconds
		 */	
		MTTime operator+(long us);
		
		/**
		 * Sums the provided MTTime to the private Seconds and Microseconds attributes.  
		 *
		 * @param  ttime	the MTTime to add
		 * @return the sum of this MTTime with the provided MTTime argument
		 */
		MTTime operator+(MTTime ttime);

		/**
		 * Subtracts the provided time represented in Microseconds from the private Seconds and Microseconds attributes.
		 *
		 * @param  us	the total time to subtract in Microseconds
		 * @return the subtraction result of this MTTime minus the provided time in Microseconds
		 */		
		MTTime operator-(long us);

		/**
		 * Subtracts the provided MTTime from the private Seconds and Microseconds attributes.
		 *
		 * @param  ttime	the MTTime to subtract
		 * @return the subtraction result of this MTTime minus the provided MTTime
		 */	
		MTTime operator-(MTTime ttime);

		
		/**
		 * Assigns the provided MTTime to the private Seconds and Microseconds attributes.
		 *
		 * @param  ttime	the MTTime to assign
		 */	
		void operator=(MTTime ttime);
		
		/**
		 * Takes a MTTime argument and compares the provided MTTime to the private Seconds and Microseconds attributes.
		 *
		 * @param  ttime	the MTTime to compare
		 * @return true if the two MTTime have equal Seconds and Microseconds attributes
		 */	
		bool operator==(MTTime ttime);

		/**
		 * Takes a MTTime argument and compares the provided MTTime to the private Seconds and Microseconds attributes.
		 *
		 * @param  ttime	the MTTime to compare
		 * @return true if the two MTTime have differnt Seconds or Microseconds attributes
		 */	
		bool operator!=(MTTime ttime);
		
		/**
		 * Resets the seconds and micro_seconds attributes to zero.
		 */
		void reset();
		
		/**
		 * Returns the MTTime Seconds component.
		 * @return the MTTime Seconds component
		 */	
		long getSeconds() const;
		
		/**
		 * Returns the MTTime Microseconds component.
		 * @return the MTTime Microseconds component
		 */	
		long getMicroseconds() const;
		
		/**
		 * Returns the total MTTime in Milliseconds.
		 * @return the total MTTime in Milliseconds
		 */	
		long getTotalMilliseconds() const;
		
		/**
		 * This static method globally resets the session time.
		 */		
		static void initSession();
		
		/**
		 * Returns the present MTTime representing the time since session start.
		 * @return the present MTTime representing the time since session start
		 */			
		static MTTime getSessionTime();
		
		/**
		 * Returns the absolut MTTime representing the session start.
		 * @return the absolut MTTime representing the session start
		 */			
		static MTTime getStartTime();
		
		/**
		 * Returns the absolut MTTime representing the current system time.
		 * @return the absolut MTTime representing the current system time
		 */	
		static MTTime getSystemTime();
	};
}
#endif /* _MULTITOUCH_TIME_H_ */
