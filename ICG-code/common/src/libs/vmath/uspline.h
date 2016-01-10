#ifndef _USPLINE_H_
#define _USPLINE_H_

#include "spline.h"
#include "vec2.h"

#pragma warning (push)
#pragma warning (disable: 4244)

/*
 * Optimized uniform version of splines.
 * Version provided for curves and patches (1D and 2D)
 *
 * Spline types equivalent to previous.
 *
 * CONSTANT
 * Only 1 control point with the value.
 *
 * LINEAR
 * Each 2 control points defines a segment.
 *
 * BEZIER_QUADRATIC
 * Each 3 control points defines a segment.
 *
 * BEZIER_CUBIC
 * Each 4 control points defines a segment.
 */

template<class T>
class UniformSplineCurve {
public:
    enum BasisType {
        CONSTANT = 0,
        LINEAR = 1,
        BEZIER_QUADRATIC = 2,
        BEZIER_CUBIC = 3,
    };

	UniformSplineCurve(const T& v = 0) {
		Set(v);
	}

    UniformSplineCurve(BasisType basisType, const carray<T>& controlPoints) {
		Set(basisType, controlPoints);
    }
	
	void Set(const UniformSplineCurve<T>& s) {
		Set(s.basisType, s.controlPoints);
	}

	void Set(const T& v) {
		basisType = CONSTANT;
		controlPoints.resize(1);
		controlPoints[0] = v;
		_Verify();
	}

	void Set(BasisType basisType, const carray<T>& controlPoints) {
        this->basisType = basisType;
        this->controlPoints = controlPoints;
        _Verify();
	}

	UniformSplineCurve<T>& operator=(const UniformSplineCurve<T>& s) {
		Set(s);
		return *this;
	}

	template<class TT>
    T Eval(TT t) {
        switch(basisType) {
            case CONSTANT: return _EvalConstant(t);
            case LINEAR: return _EvalLinear(t);
            case BEZIER_QUADRATIC: return _EvalBezierQuadratic(t);
            case BEZIER_CUBIC: return _EvalBezierCubic(t);
            default: assert(0); return T();
        }
    }

    BasisType GetBasisType() { return basisType; }
    carray<T>* GetControlPoints() { return &controlPoints; }
    Intervalf GetTimesInterval() {
        if(basisType == CONSTANT) return Intervalf::Invalid();
		else return Intervalf(0,1);
    }

	bool IsConstant() { return basisType == CONSTANT; }

protected:
    BasisType basisType;
    carray<T> controlPoints;

protected:
    void _Verify() {
        switch(basisType) {
            case CONSTANT:
                assert(controlPoints.size() == 1);
                break;
            case LINEAR:
                assert(controlPoints.size() % 2 == 0);
                break;
            case BEZIER_QUADRATIC:
                assert(controlPoints.size() % 3 == 0);
                break;
            case BEZIER_CUBIC:
                assert(controlPoints.size() % 4 == 0);
                break;
        }
    }

	template<class TT>
	bool _Map(TT t, int segs, TT& s, TT& si, int& k, T& outVal) {
        // check if outside of the interal
        if(t <= 0) { outVal = controlPoints.at(0); return false; }
        if(t >= 1) { outVal = controlPoints.at(controlPoints.size()-1); return false; }
		
		k = (int) (t * segs);
		
		s = t * segs - k;
		si = 1-s;
		
		return true;
	}
	
	template<class TT>
    T _EvalConstant(TT t) {
        return controlPoints[0];
    }

	template<class TT>
    T _EvalLinear(TT t) {
        // find the appropriate interval and interpolating value
        int k; TT s, si; T outVal;
		if(!_Map(t, controlPoints.size()/2, s, si, k, outVal)) return outVal;

        // evaluate the spline
        T P0 = controlPoints.at(k+0);
        T P1 = controlPoints.at(k+1);

        TT coeff_P0 = si;
        TT coeff_P1 = s;

        return P0 * coeff_P0 + P1 * coeff_P1;
    }

	template<class TT>
    T _EvalBezierQuadratic(TT t) {
        // find the appropriate interval and interpolating value
        int k; TT s, si; T outVal;
		if(!_Map(t, controlPoints.size()/3, s, si, k, outVal)) return outVal;
		
        // evaluate the spline for segment k at value s
        T P0 = controlPoints.at(k*3+0);
        T P1 = controlPoints.at(k*3+1);
        T P2 = controlPoints.at(k*3+2);

        TT coeff_P0 = si*si;
        TT coeff_P1 = 2*s*si;
        TT coeff_P2 = s*s;

        return P0 * coeff_P0 + P1 * coeff_P1 + P2 * coeff_P2;
    }

	template<class TT>
    T _EvalBezierCubic(TT t) {
        // find the appropriate interval and interpolating value
        int k; TT s, si; T outVal;
		if(!_Map(t, controlPoints.size()/4, s, si, k, outVal)) return outVal;
		
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

template<class T>
class UniformSplinePatch {
public:
    enum BasisType {
        CONSTANT = 0,
        LINEAR = 1,
        BEZIER_QUADRATIC = 2,
        BEZIER_CUBIC = 3,
    };

	UniformSplinePatch(const T& v = 0) {
		Set(v);
	}
	
    UniformSplinePatch(BasisType basisType, const carray2<T>& controlPoints) {
		Set(basisType, controlPoints);
    }
	
	void Set(const UniformSplinePatch<T>& s) {
		Set(s.basisType, s.controlPoints);
	}
	
	void Set(const T& v) {
		basisType = CONSTANT;
		controlPoints.resize(1,1);
		controlPoints[0] = v;
		_Verify();
	}
	
	void Set(BasisType basisType, const carray2<T>& controlPoints) {
        this->basisType = basisType;
        this->controlPoints = controlPoints;
        _Verify();
	}
	
	UniformSplinePatch<T>& operator=(const UniformSplinePatch<T>& s) {
		Set(s);
		return *this;
	}
	
	template<class TT>
    T Eval(const Vec2<TT>& uv) {
        switch(basisType) {
            case CONSTANT: return _EvalConstant(uv);
            case LINEAR: return _EvalLinear(uv);
            case BEZIER_QUADRATIC: return _EvalBezierQuadratic(uv);
            case BEZIER_CUBIC: return _EvalBezierCubic(uv);
            default: assert(0); return T();
        }
    }
	
	template<class TT>
    T Eval(TT u, TT v) {
		return Eval(Vec2f(u,v));
    }
	
    BasisType GetBasisType() { return basisType; }
    carray2<T>* GetControlPoints() { return &controlPoints; }
    Intervalf GetTimesInterval() {
        if(basisType == CONSTANT) return Intervalf::Invalid();
		else return Intervalf(0,1);
    }
	
	bool IsConstant() { return basisType == CONSTANT; }
	
protected:
    BasisType basisType;
    carray2<T> controlPoints;
	
protected:
    void _Verify() {
        switch(basisType) {
            case CONSTANT:
                assert(controlPoints.size() == 1);
                break;
            case LINEAR:
                assert(controlPoints.width() % 2 == 0 && controlPoints.height() % 2 == 0);
                break;
            case BEZIER_QUADRATIC:
                assert(controlPoints.width() % 3 == 0 && controlPoints.height() % 3 == 0);
                break;
            case BEZIER_CUBIC:
                assert(controlPoints.width() % 4 == 0 && controlPoints.height() % 4 == 0);
                break;
        }
    }
	
	template<class TT>
	bool _Map(const Vec2<TT>& t, Vec2<int> segs, Vec2<TT>& s, Vec2<TT>& si, Vec2<int>& k, T& outVal) {
        // check if outside of the interal
        if(t.x <= 0 && t.y <= 0) { outVal = controlPoints.at(0,0); return false; }
        if(t.x <= 0 && t.y >= 1) { outVal = controlPoints.at(0,controlPoints.height()-1); return false; }
        if(t.x >= 1 && t.y <= 0) { outVal = controlPoints.at(controlPoints.width()-1,0); return false; }
        if(t.x >= 1 && t.y >= 1) { outVal = controlPoints.at(controlPoints.width()-1,controlPoints.height()-1); return false; }
		
		// compute all values
		k = Vec2<int>(t * Vec2<TT>(segs));
		
		s = t * Vec2<TT>(segs) - Vec2<TT>(k);
		si = Vec2<TT>(1)-s;
		
		// check if need to clamp some values (the others remain valid given above checks)
		if(t.x <= 0) { k.x = 0; s.x = 0; si.x = 1; }
		if(t.x >= 1) { k.x = segs.x-1; s.x = 1; si.x = 0; }
		if(t.y <= 0) { k.y = 0; s.y = 0; si.y = 1; }
		if(t.y >= 1) { k.y = segs.y-1; s.y = 1; si.y = 0; }
		
		return true;
	}
	
	template<class TT>
    T _EvalConstant(const Vec2<TT>& uv) {
        return controlPoints.at(0,0);
    }
	
	template<class TT>
    T _EvalLinear(const Vec2<TT>& uv) {
        // find the appropriate interval and interpolating value
        Vec2<int> k; Vec2<TT> s, si; T outVal;
		if(!_Map(uv, Vec2<int>(controlPoints.width()/2,controlPoints.height()/2), 
				 s, si, k, outVal)) return outVal;
		
		Vec2<TT> basisEval[2];
        basisEval[0] = si;
        basisEval[1] = s;
		
		outVal = T();
		for(int i = 0; i < 2; i ++) {
			for(int j = 0; j < 2; j ++) {
				outVal += controlPoints.at(2*k.x+i,2*k.y+j) * basisEval[i].x * basisEval[j].y;
			}
		}
		
        return outVal;
    }
	
	template<class TT>
    T _EvalBezierQuadratic(const Vec2<TT>& uv) {
        // find the appropriate interval and interpolating value
        Vec2<int> k; Vec2<TT> s, si; T outVal;
		if(!_Map(uv, Vec2<int>(controlPoints.width()/3,controlPoints.height()/3), 
				 s, si, k, outVal)) return outVal;
		
		Vec2<TT> basisEval[3];
        basisEval[0] = si*si;
        basisEval[1] = s*si*2;
        basisEval[2] = s*s;
		
		outVal = T();
		for(int i = 0; i < 3; i ++) {
			for(int j = 0; j < 3; j ++) {
				outVal += controlPoints.at(3*k.x+i,3*k.y+j) * basisEval[i].x * basisEval[j].y;
			}
		}
		
        return outVal;
    }
	
	template<class TT>
    T _EvalBezierCubic(const Vec2<TT>& uv) {
        // find the appropriate interval and interpolating value
        Vec2<int> k; Vec2<TT> s, si; T outVal;
		if(!_Map(uv, Vec2<int>(controlPoints.width()/4,controlPoints.height()/4), 
				 s, si, k, outVal)) return outVal;
		
		Vec2<TT> basisEval[4];
        basisEval[0] = si*si*si;
        basisEval[1] = si*si*s*3;
        basisEval[2] = si*s*s*3;
        basisEval[3] = s*s*s;
		
		outVal = 0;
		for(int i = 0; i < 4; i ++) {
			for(int j = 0; j < 4; j ++) {
				outVal += controlPoints.at(4*k.x+i,4*k.y+j) * basisEval[i].x * basisEval[j].y;
			}
		}
		
        return outVal;
    }
};

#pragma warning (pop)

#endif
