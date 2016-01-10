#ifndef _TBB_TIMER_H_
#define _TBB_TIMER_H_

#include <tbb/tick_count.h>

class TbbTimer {
private:
    tbb::tick_count _start;
	tbb::tick_count _stop;
    tbb::tick_count::interval_t	_timeElapsed;	// total time spent in all the call start/stop
	int	    _count;			// Number of times stopped

// Public methods ---------------------------------------------------
public:
	TbbTimer();		// Creates an empty timer
    void Reset() {	// Reset the timer
        _start = tbb::tick_count();
        _stop = tbb::tick_count();
		_timeElapsed = tbb::tick_count::interval_t();
        _count = 0;
    }
    void Start() {	// Start the timer
        _start = tbb::tick_count::now();
    }
    void Stop() {	// Stop the timer and add the time to elapsedTime
        _timeElapsed += tbb::tick_count::now() - _start;
        _count ++;
    }
    double GetElapsedTime() { return _timeElapsed.seconds(); }
    int GetCount() { return _count; }
};

// Inline functions ------------------------------------------------------
inline TbbTimer::TbbTimer() {
	Reset();
}


#endif // _TBB_TIMER_H_
