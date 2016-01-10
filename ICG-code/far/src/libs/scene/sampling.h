#ifndef _SAMPLING_H_
#define _SAMPLING_H_

#include "smath.h"
#include <misc/arrays.h>

#define SAMPLINGEPSILON 0.0000001f

class Sampling {
public:
	static void ConcentricDisk(const Vec2f &s, float &x, float &y) {
		float r, theta;
		float sx = 2*s[0] - 1;
		float sy = 2*s[1] - 1;
		if(sx >= -sy){
			if(sx > sy){
				//region 1 (7pi/4,pi/4)
				r = sx;
				if(sy > 0)
					theta = sy/r;
				else
					theta = 8.0f + sy/r;
			} else {
				//region 2 (pi/4,3pi/4)
				r = sy;
				if(sx > 0)
					theta = 2.0f - sx/r; 
				else
					theta = 2.0f - sx/r;
			}
		} else {
			if(sx <= sy){
				//region 3 (3pi/4,5pi/4)
				r = -sx;
				if(sy > 0)
					theta = 4.0f - sy/r; 
				else
					theta = 4.0f - sy/r;
			} else {
				//region 4 (5pi/4,7pi/4)
				r = -sy;
				if(sx > 0)
					theta = 6.0f - sx/r; 
				else
					theta = 6.0f - sx/r;
			}
		}
		theta *= (PIf/4.0f);
		x = r*cosf(theta);
		y = r*sinf(theta);
	}

	static Vec2f ConcentricDisk(const Vec2f &s) {
        Vec2f uv;
        ConcentricDisk(s, uv.x, uv.y);
        return uv;
    }

    static Vec2f UniformTriangle(const Vec2f &s) {
        Vec2f uv;
        float su1 = sqrtf(s.x);
        uv.x = 1.f - su1;
        uv.y = s.y * su1;
    }


    static Vec3f HemisphericalDirection(const Vec2f& s, float* pdf) {
        float z = s[1];
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Vec3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = 1 / (2*PIf);
        return d;
    }

    static Direction3f HemisphericalDirectionD(const Vec2f& s, float* pdf) {
        float z = s[1];
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Direction3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = 1 / (2*PIf);
        return d;
    }

	static float HemisphericalDirectionPdf() {
        return 1 / (2*PIf);
    }

    static Vec3f HemisphericalDirectionCos(const Vec2f& s, float* pdf) {
        float z = sqrtf(s[1]);
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Vec3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = (d[2]+SAMPLINGEPSILON) / PIf; // check for zero probability
        return d;
    }

	static float HemisphericalDirectionCosPdf(const Vec3f& d) {
        return (d[2]+SAMPLINGEPSILON) / PIf;
    }

    static Vec3f HemisphericalDirectionCosPower(float n, const Vec2f& s, float* pdf) {
        float z = sqrtf(powf(s[1],2/(n+1)));
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Vec3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = HemisphericalDirectionCosPowerPdf(n,d);
        return d;
    }

	static float HemisphericalDirectionCosPowerPdf(float n, const Vec3f& d) {
        return (n+1)*pow(d[2]+SAMPLINGEPSILON,n) / (2*PIf);
    }

	static float HemisphericalDirectionCosPowerPdf(float n, float cosine) {
        return (n+1)*pow(cosine+SAMPLINGEPSILON,n) / (2*PIf);
    }

    static Direction3f HemisphericalDirectionCosD(const Vec2f& s, float* pdf) {
        float z = sqrtf(s[1]);
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Direction3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = (d[2]+SAMPLINGEPSILON) / PIf; // check for zero probability
        return d;
    }

	static float HemisphericalDirectionCosPdfD(const Direction3f& d) {
        return (d[2]+SAMPLINGEPSILON) / PIf;
    }

	static float SphericalDirectionPdf() {
        return 1 / (4*PIf);
    }

	static Vec3f SphericalDirection(const Vec2f& s, float* pdf) {
        float z = 1 - 2 * s[1];
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Vec3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = 1 / (4*PIf);
        return d;
    }

	static Direction3f SphericalDirectionD(const Vec2f& s, float* pdf) {
        float z = 1 - 2 * s[1];
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Direction3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = 1 / (4*PIf);
        return d;
    }

	// this should be done outside
    static Vec3f SphericalDirectionCos(const Vec3f &n, const Vec2f &s, float* pdf) {
		float x, y, z;
		ConcentricDisk(s,x,y);
		z = sqrtf( max(0.0f, 1.0f-x*x-y*y) );
		//compute pdf
		Vec3f d(x,y,z);
		*pdf = (fabsf(d[2])+SAMPLINGEPSILON) / (2*PIf);
		//transform direction to world space
		Vec3f v1,v2;
		n.GetNormalized().GetXYFromZ(v1,v2);
		d = Vec3f(	v1.x*d.x + v2.x*d.y + n.x*d.z,
					v1.y*d.x + v2.y*d.y + n.y*d.z,
					v1.z*d.x + v2.z*d.y + n.z*d.z	);
		return d;
    }

    static Vec3f ConicalDirection(const Vec2f& s, float cosThetaMax, float* pdf) {
        float z = 1 - s[1]*(1 - cosThetaMax);
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Vec3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = 1 / (2 * PIf * (1 - cosThetaMax));
        return d;
    }

    static Direction3f ConicalDirectionD(const Vec2f& s, float cosThetaMax, float* pdf) {
        float z = 1 - s[1]*(1 - cosThetaMax);
        float r = sqrtf(1-z*z);
        float phi = 2 * PIf * s[0];

        Direction3f d(r * cosf(phi), r * sinf(phi), z);
        *pdf = 1 / (2 * PIf * (1 - cosThetaMax));
        return d;
    }

	// from pbrt
	static carray<float> ComputeStepCDF(const carray<float>& func, float* integral) {
		carray<float> cdf(func.size()+1);
		cdf[0] = 0;
		for (uint32_t i = 1; i < cdf.size(); i++) {
			cdf[i] = cdf[i-1] + func[i-1] / func.size();
		}
		// Transform step function integral into cdf
		*integral = cdf[cdf.size()-1];
		for (uint32_t i = 1; i < cdf.size(); i ++) {
			cdf[i] /= *integral;
		}
		return cdf;
	}

    static float BalanceHeuristic(int nf, float fPdf, int ng, float gPdf) {
	    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
    }

    static float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
	    float f = nf * fPdf; float g = ng * gPdf;
	    return (f*f) / (f*f + g*g);
    }
};

#endif
