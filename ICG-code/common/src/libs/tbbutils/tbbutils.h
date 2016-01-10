#ifndef _TBB_UTILS_H_
#define _TBB_UTILS_H_

#include <stdint.h>
#include <misc/mtreport.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/spin_mutex.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_while.h>
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_reduce.h>
#include <tbb/queuing_mutex.h>
#include <tbb/recursive_mutex.h>
#include <tbb/tick_count.h>
#ifdef WIN32
#include <tbb/tbbmalloc_proxy.h>
#endif
#include <vmath/vec2.h>

using namespace tbb;

class TbbCounter 
{
public:
    bool pop_if_present( uint32_t& i ) 
    {
        if( _current < _count ) 
        {
            i = _current;
            _current++;
            return true;
        } 
        else 
            return false;
    }
    TbbCounter(uint32_t count) : _count(count), _current(0) {}
private:
    uint32_t        _current;
    uint32_t        _count;
};

class TbbCounter2D 
{
public:
    bool pop_if_present( Vec2i& px ) 
    {
        if( _current.y < _height && _current.x < _width) 
        {
            px = _current;
            _current.x++;
            if (_current.x >= _width)
            {
                _current.x = 0;
                _current.y++;
            }
            return true;
        } 
        else 
            return false;
    }
    TbbCounter2D(uint32_t width, uint32_t height) : _height(height), _width(width), _current() {}
private:
    Vec2i           _current;
    int32_t        _height;
    int32_t        _width;
};


class TbbReportCounter
{
public:
    bool pop_if_present( uint32_t& i ) 
    {
        if( _current < _count ) 
        {
            i = _current;
			if(_report) _report->progress((float)_current / _count, _step);
            _current++;
            return true;
        } 
        else 
            return false;
    }
    TbbReportCounter(uint32_t count, ReportHandler* report, uint32_t step = 1 ) 
        : _count(count), _current(0), _report(report), _step(step) {}
private:
    uint32_t        _current;
    uint32_t        _count;
    ReportHandler   *_report;
    uint32_t        _step;
};


class TbbReportCounter2D
{
public:
    bool pop_if_present( Vec2i& px ) 
    {
        if( _current.y < _height && _current.x < _width) 
        {
            px = _current;
			if(_report) _report->progress((float)(_current.y * _width + _current.x) / (_width * _height), _step);
            _current.x++;
            if (_current.x >= _width)
            {
                _current.x = 0;
                _current.y++;
            }
            return true;
        } 
        else 
            return false;
    }
    TbbReportCounter2D(uint32_t width, uint32_t height, ReportHandler* report, uint32_t step = 1 ) 
        : _height(height), _width(width), _current(), _report(report), _step(step) {}
private:
    Vec2i           _current;
    int32_t         _height;
    int32_t         _width;
    ReportHandler   *_report;
    uint32_t        _step;
};
#endif // _TBB_UTILS_H_