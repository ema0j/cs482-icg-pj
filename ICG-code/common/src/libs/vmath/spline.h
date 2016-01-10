#ifndef _SPLINE_H_
#define _SPLINE_H_

#include <misc/arrays.h>
#include "interval.h"
#include <string>
using std::string;
#include <assert.h>

#pragma warning (push)
#pragma warning (disable: 4244)

/*
 * CONSTANT
 * Only 1 control point with the value
 *
 * LINEAR
 * Each control point defines a point from keyframeTime k
 * There are (keyframeTimes.length-1)*2 control points
 * Each keyframe pair k,k+1 defines a segment controlled by 2 control points k*2, k*2+1
 *
 * BEZIER_CUBIC
 * Each four control points defines a segment from keyframeTime k to keyframeTime k+1
 * There are (keyframeTimes.length-1)*4 control points.
 * Each keyframe pair k,k+1 defines a segment controlled
 * by the 4 control points k*4 ... k*4+3.
 * There are (keyframeTimes.length-1) * 4 control points.
 *
 * BEZIER_QUADRATIC
 * Each three control points defines a segment from keyframeTime k to keyframeTime k+1
 * There are (keyframeTimes.length-1)*3 control points.
 * Each keyframe pair k,k+1 defines a segment controlled
 * by the 3 control points k*3 ... k*3+2.
 * There are (keyframeTimes.length-1) * 3 control points.
 */
template<class T, class TT>
class Spline {
public:
    enum BasisType {
        CONSTANT = 0,
        LINEAR = 1,
        BEZIER_QUADRATIC = 2,
        BEZIER_CUBIC = 3,
    };

	Spline(const T& v = T()) { Set(v); }

    Spline(BasisType basisType, const carray<T>& controlPoints, const carray<TT>& times) {
		Set(basisType, controlPoints, times);
    }

    Spline(BasisType basisType, const carray<T>* controlPoints, const carray<TT>* times) {
        Set(basisType, *controlPoints, (times)?(*times):carray<TT>());
    }

	void Set(const Spline<T,TT>& s) { Set(s.basisType, s.controlPoints, s.times);	}
    void Set(const T& v) { Set(CONSTANT, carray<T>(1,v), carray<TT>()); }
	void Set(BasisType basisType, const carray<T>& controlPoints, const carray<TT>& times) {
        this->basisType = basisType;
        this->controlPoints = controlPoints;
        this->times = times;
        _Verify();
	}

	Spline<T, TT>& operator=(const Spline<T, TT>& s) { Set(s); return *this; }

	bool operator== (const Spline<T, TT>& s) {
		if(basisType != s.basisType) return false;
		if(CONSTANT) return controlPoints[0] == s.controlPoints[0];
		else {
			return controlPoints == s.controlPoints && times == s.times;
		}
	}
	bool operator!= (const Spline<T, TT>& s) { return ! (*this == s); }

    T Eval(TT t) {
        switch(basisType) {
            case CONSTANT: return _EvalConstant(t);
            case LINEAR: return _EvalLinear(t);
            case BEZIER_QUADRATIC: return _EvalBezierQuadratic(t);
            case BEZIER_CUBIC: return _EvalBezierCubic(t);
            default: assert(0); return T();
        }
    }

	static string GetBasisTypeName(BasisType type) {
        switch(type) {
            case CONSTANT: return "constant";
            case LINEAR: return "linear";
            case BEZIER_QUADRATIC: return "bezier_quadratic";
            case BEZIER_CUBIC: return "bezier_cubic";
            default: assert(0); return "";
        }
	}

	static BasisType GetBasisType(string typeName) {
		if(typeName == "constant") return CONSTANT;
		else if(typeName == "linear") return LINEAR;
		else if(typeName == "bezier_quadratic") return BEZIER_QUADRATIC;
		else if(typeName == "bezier_cubic") return BEZIER_CUBIC;
		else { assert(0); return CONSTANT; }
	}

    BasisType GetBasisType() { return basisType; }
    carray<T>* GetControlPoints() { return &controlPoints; }
    carray<TT>* GetTimes() { return &times; }
    Intervalf GetTimesInterval() {
        if(basisType == CONSTANT) return Intervalf::Invalid();
		else if(times.size() == 0) return Intervalf(0,1);
        else return Intervalf(times[0], times[times.size()-1]);
    }
    int NSegments() {
        switch(basisType) {
            case CONSTANT: return 0;
            case LINEAR: return controlPoints.size()/2;
            case BEZIER_QUADRATIC: return controlPoints.size()/3;
            case BEZIER_CUBIC: return controlPoints.size()/4;
            default: assert(0); return -1;
        }
    }

    bool IsUniform() { return times.size() == 0 && basisType != CONSTANT; }
	bool IsConstant() { return basisType == CONSTANT; }

protected:
    BasisType basisType;
    carray<T> controlPoints;
    carray<TT> times;

protected:
    void _Verify() {
        switch(basisType) {
            case CONSTANT:
                assert(controlPoints.size() == 1 && times.size() == 0);
                break;
            case LINEAR:
                assert((NSegments() == times.size()-1 || times.size() == 0) && controlPoints.size() % 2 == 0);
                break;
            case BEZIER_QUADRATIC:
                assert((NSegments() == times.size()-1 || times.size() == 0) && controlPoints.size() % 3 == 0);
                break;
            case BEZIER_CUBIC:
                assert((NSegments() == times.size()-1 || times.size() == 0) && controlPoints.size() % 4 == 0);
                break;
        }
    }

	bool _Map(TT t, TT& s, int& k, T& outVal) {	
        if(IsUniform()) {
            // check if outside of the interal
            if(t <= 0) { outVal = controlPoints.at(0); return false; }
            if(t >= 1) { outVal = controlPoints.at(controlPoints.size()-1); return false; }

		    k = (int) (t * NSegments());
    		s = t * NSegments() - k;

    		return true;
        } else {
            // check if outside of the interal
            if(t <= times.at(0)) { outVal = controlPoints.at(0); return false; }
            if(t >= times.at(times.size()-1)) { outVal = controlPoints.at(controlPoints.size()-1); return false; }

            for(k = 0; k <= NSegments(); k ++) {
                if(t < times.at(k+1)) break;
            }
            s = (t - times.at(k)) / (times.at(k+1) - times.at(k));

    		return true;
        }
	}

    T _EvalConstant(TT t) {
        return controlPoints[0];
    }

    T _EvalLinear(TT t) {
        // find the appropriate interval and interpolating value
        int k; TT s; T outVal;
		if(!_Map(t, s, k, outVal)) return outVal;
		TT si = 1 - s;

        // evaluate the spline
        T P0 = controlPoints.at(k+0);
        T P1 = controlPoints.at(k+1);

        TT coeff_P0 = si;
        TT coeff_P1 = s;

        return P0 * coeff_P0 + P1 * coeff_P1;
    }

    T _EvalBezierQuadratic(TT t) {
        // find the appropriate interval and interpolating value
        int k; TT s; T outVal;
		if(!_Map(t, s, k, outVal)) return outVal;
		TT si = 1 - s;
		
        // evaluate the spline for segment k at value s
        T P0 = controlPoints.at(k*3+0);
        T P1 = controlPoints.at(k*3+1);
        T P2 = controlPoints.at(k*3+2);

        TT coeff_P0 = si*si;
        TT coeff_P1 = 2*s*si;
        TT coeff_P2 = s*s;

        return P0 * coeff_P0 + P1 * coeff_P1 + P2 * coeff_P2;
    }

    T _EvalBezierCubic(TT t) {
        // find the appropriate interval and interpolating value
        int k; TT s; T outVal;
		if(!_Map(t, s, k, outVal)) return outVal;
		TT si = 1 - s;
		
        // evaluate the spline for segment k at value s
        T P0 = controlPoints.at(k*4+0);
        T P1 = controlPoints.at(k*4+1);
        T P2 = controlPoints.at(k*4+2);
        T P3 = controlPoints.at(k*4+3);
		
        TT coeff_P0 = si*si*si;
        TT coeff_P1 = 3*si*si*s;
        TT coeff_P2 = 3*si*s*s;
        TT coeff_P3 = s*s*s;
		
        return P0 * coeff_P0 + P1 * coeff_P1 + P2 * coeff_P2 + P3 * coeff_P3;
    }
};

template<class TT> inline void SplineEvalInterval(const carray<TT>& times, TT t, TT& s, int& k0, int& k1) {
    // check if outside of the interal
    if(t <= times.at(0)) { s = 0; k0 = 0; k1 = 0; return; }
    if(t >= times.at(times.size()-1)) { s = 1; k0 = 0; k1 = 0; return; }

    // find the appropriate interval and interpolating value
    int k;
    for(k = 0; k < times.size()-1; k ++) {
        if(t < times.at(k+1)) break;
    }
    s = (t - times.at(k)) / (times.at(k+1) - times.at(k));
    k0 = k;
    k1 = k+1;
}

#pragma warning (pop)

#endif
