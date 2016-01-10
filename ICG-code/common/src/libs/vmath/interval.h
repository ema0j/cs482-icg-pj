#ifndef _INTERVAL_H_
#define _INTERVAL_H_

#include "functions.h"

template<class T>
class Interval {
protected:
    T begin;
    T end;

public:
    Interval() { begin = 0; end = 1; }
    Interval(T t) { begin = t; end = t; }
    Interval(T b, T e) { begin = b; end = e; }

    bool IsValid() const { return begin <= end; }
    bool IsEmpty() const { return begin == end; }
    T Begin() const { return begin; }
    T Middle() const { return (begin + end) / 2; }
    T End() const { return end; }

    // n uniform samples in [begin..end] inclusive
    carray<T> UniformSample(int n) const {
        carray<T> samples(n);
        for(int s = 0; s < n; s ++) {
            samples[s] = Get((float)s / (n-1));
        }
        return samples;
    }

    // n uniform subdivision in [begin..end]
    carray<Interval<T> > UniformSubdivide(int n) const {
        carray<Interval<T> > intervals(n);
        carray<T> samples = UniformSample(n+1);
        for(int s = 0; s < n; s ++) {
            intervals[s] = Interval<T>(samples[s], samples[s+1]);
        }
        return intervals;
    }

    // maps [0..1] -> [begin..end]
    T Get(T t) const {
        return begin + (end-begin) * t;
    }

    void Include(const Interval& i) {
        if(!i.IsValid()) return;

        if(IsValid()) {
            begin = min(begin, i.begin);
            end = max(end, i.end);
        } else {
            begin = i.begin;
            end = i.end;
        }
    }

    static Interval Default() {
        Interval ret;
        ret.begin = 0;
        ret.end = 1;
        return ret;
    }

    static Interval Invalid() {
        Interval ret;
        ret.begin = 1;
        ret.end = -1;
        return ret;
    }
};

typedef Interval<float> Intervalf;
typedef Interval<double> Intervald;

#endif
