#ifndef _TEXTIARCHIVE_H_
#define _TEXTIARCHIVE_H_

#include "archive.h"
#include <vmath/streamMethods.h>

class TextIArchive : public IArchive {
public:
	static bool isbinary() { return false; }
	
    virtual void member(const string& name, uint32_t& v) { read(name, v); }
	virtual void member(const string& name, int& v) { read(name, v); }
	virtual void member(const string& name, float& v) { read(name, v); }
	virtual void member(const string& name, double& v) { read(name, v); }	
	virtual void member(const string& name, bool& v) { readBool(name, v); }	

	virtual void member(const string& name, string& v) { read(name, v); }	

	virtual void member(const string& name, carray<int>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<float>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<double>& v) { readArray(name, v); }

	virtual void member(const string& name, carray2<int>& v) { readArray2(name, v); }
	virtual void member(const string& name, carray2<float>& v) { readArray2(name, v); }
	virtual void member(const string& name, carray2<double>& v) { readArray2(name, v); }

	virtual void member(const string& name, Vec2i& v) { read(name, v); }
	virtual void member(const string& name, Vec2f& v) { read(name, v); }
	virtual void member(const string& name, Vec2d& v) { read(name, v); }

	virtual void member(const string& name, Vec3i& v) { read(name, v); }
	virtual void member(const string& name, Vec3f& v) { read(name, v); }
	virtual void member(const string& name, Vec3d& v) { read(name, v); }

	virtual void member(const string& name, Tuple3f& v) { read(name, v); }
	virtual void member(const string& name, Tuple3d& v) { read(name, v); }

	virtual void member(const string& name, Vec4i& v) { read(name, v); }
	virtual void member(const string& name, Vec4f& v) { read(name, v); }
	virtual void member(const string& name, Vec4d& v) { read(name, v); }

	virtual void member(const string& name, Matrix4f& v) { read(name, v); }
	virtual void member(const string& name, Matrix4d& v) { read(name, v); }

	virtual void member(const string& name, carray<Vec2f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vec3f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vec4f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Tuple3f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Point3f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vector3f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Normal3f>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Direction3f>& v) { readArray(name, v); }

	virtual void member(const string& name, carray<Vec2d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vec3d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vec4d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Tuple3d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Point3d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vector3d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Normal3d>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Direction3d>& v) { readArray(name, v); }

	virtual void member(const string& name, carray<Vec2i>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vec3i>& v) { readArray(name, v); }
	virtual void member(const string& name, carray<Vec4i>& v) { readArray(name, v); }

	virtual void member(const string& name, carray<Matrix4d>& v) { readArray(name, v); }

	virtual void member(const string& name, carray2<Vec2f>& v) { readArray2(name, v); }
	virtual void member(const string& name, carray2<Vec3f>& v) { readArray2(name, v); }
	virtual void member(const string& name, carray2<Vec4f>& v) { readArray2(name, v); }

	virtual void member(const string& name, carray2<Vec2d>& v) { readArray2(name, v); }
	virtual void member(const string& name, carray2<Vec3d>& v) { readArray2(name, v); }
	virtual void member(const string& name, carray2<Vec4d>& v) { readArray2(name, v); }

	virtual void member(const string& name, carray2<Matrix4d>& v) { readArray2(name, v); }

protected:
    virtual void parseErrorInfo(ostream& s) { }

	template<typename T>
	void read(const string& name, T& v) {
		*is >> v;
	}

	void readBool(const string& name, bool& v) {
		string s;
		*is >> s;
		v = s == "true" || s == "TRUE";
	}
	
	template<typename T>
	void readArray(const string& name, carray<T>& v) {
		int length;
		*is >> length;
		
		v.resize(length);
		for(int i = 0; i < length; i ++) {
			*is >> v[i];
		}
	}
	
	template<typename T>
	void readArray2(const string& name, carray2<T>& v) {
		int width, height;
		*is >> width >> height;
		
		v.resize(width, height);
		for(int i = 0; i < width*height; i ++) {
			*is >> v[i];
		}
	}
	
	virtual void archiveBegin() {
	}
	
	virtual void archiveEnd() {
	}
	
	virtual void optionalBegin(const string& name, bool& opt) {
		*is >> opt;
	}
	
	virtual void optionalEnd() {
	}
	
	virtual void objBegin(const string& name, string& type, uint32_t childIdx) {
		string read_name;
		*is >> read_name;
		*is >> type;
	}

	virtual void objBegin(const string& name, string& type, string& id, bool &isref, uint32_t childIdx) {
		string read_name, ref_type;
		*is >> read_name;
		*is >> type;
		
		*is >> ref_type;
		isref = ref_type == "ref";
		*is >> id;
	}

	virtual void objEnd() {
	}
	
	virtual void arrayBegin(const string& name, uint32_t& length) {
		*is >> length;
	}

	virtual void arrayEnd() {
	}

	virtual void iterateBegin(const string& name, uint32_t& length) {
		arrayBegin(name, length);
	}

	virtual void iterateEnd() {
		arrayEnd();
	}
};

#endif
