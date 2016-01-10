#ifndef _TEXTOARCHIVE_H_
#define _TEXTOARCHIVE_H_

#include "archive.h"
#include <vmath/streamMethods.h>

class TextOArchive : public OArchive {
public:
	static bool isbinary() { return false; }
	
    virtual void member(const string& name, uint32_t& v) { write(name, v); }
	virtual void member(const string& name, int& v) { write(name, v); }
	virtual void member(const string& name, float& v) { write(name, v); }
	virtual void member(const string& name, double& v) { write(name, v); }
	virtual void member(const string& name, bool& v) { writeBool(name, v); }

	virtual void member(const string& name, string& v) { write(name, v); }

	virtual void member(const string& name, carray<int>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<float>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<double>& v) { writeArray(name, v); }

	virtual void member(const string& name, carray2<int>& v) { writeArray2(name, v); }
	virtual void member(const string& name, carray2<float>& v) { writeArray2(name, v); }
	virtual void member(const string& name, carray2<double>& v) { writeArray2(name, v); }

	virtual void member(const string& name, Vec2i& v) { write(name, v); }
	virtual void member(const string& name, Vec2f& v) { write(name, v); }
	virtual void member(const string& name, Vec2d& v) { write(name, v); }

	virtual void member(const string& name, Vec3i& v) { write(name, v); }
	virtual void member(const string& name, Vec3f& v) { write(name, v); }
	virtual void member(const string& name, Vec3d& v) { write(name, v); }

	virtual void member(const string& name, Tuple3f& v) { write(name, v); }
	virtual void member(const string& name, Tuple3d& v) { write(name, v); }

	virtual void member(const string& name, Vec4i& v) { write(name, v); }
	virtual void member(const string& name, Vec4f& v) { write(name, v); }
	virtual void member(const string& name, Vec4d& v) { write(name, v); }

	virtual void member(const string& name, Matrix4f& v) { write(name, v); }
	virtual void member(const string& name, Matrix4d& v) { write(name, v); }

	virtual void member(const string& name, carray<Vec2f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vec3f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vec4f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Tuple3f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Point3f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vector3f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Normal3f>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Direction3f>& v) { writeArray(name, v); }

	virtual void member(const string& name, carray<Vec2d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vec3d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vec4d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Tuple3d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Point3d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vector3d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Normal3d>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Direction3d>& v) { writeArray(name, v); }

	virtual void member(const string& name, carray<Vec2i>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vec3i>& v) { writeArray(name, v); }
	virtual void member(const string& name, carray<Vec4i>& v) { writeArray(name, v); }

	virtual void member(const string& name, carray<Matrix4d>& v) { writeArray(name, v); }

	virtual void member(const string& name, carray2<Vec2f>& v) { writeArray2(name, v); }
	virtual void member(const string& name, carray2<Vec3f>& v) { writeArray2(name, v); }
	virtual void member(const string& name, carray2<Vec4f>& v) { writeArray2(name, v); }

	virtual void member(const string& name, carray2<Vec2d>& v) { writeArray2(name, v); }
	virtual void member(const string& name, carray2<Vec3d>& v) { writeArray2(name, v); }
	virtual void member(const string& name, carray2<Vec4d>& v) { writeArray2(name, v); }

	virtual void member(const string& name, carray2<Matrix4d>& v) { writeArray2(name, v); }

protected:
    virtual void parseErrorInfo(ostream& s) { }

	template<typename T>
	void write(const string& name, T& v) {
		*os << v << endl;
	}
	
	void writeBool(const string& name, bool& v) {
		*os << ((v)?"true":"false") << endl;
	}
	
	template<typename T>
	void writeArray(const string& name, carray<T>& v) {
		*os << v.size() << " ";
		for(uint32_t i = 0; i < v.size(); i ++) *os << v[i] << " ";
	}
	
	template<typename T>
	void writeArray2(const string& name, carray2<T>& v) {
		*os << v.width() << " " << v.height() << " ";
		for(uint32_t i = 0; i < v.size(); i ++) *os << v[i] << " ";
	}
	
	virtual void archiveBegin() {
	}
	
	virtual void archiveEnd() {
	}
	
	virtual void optionalBegin(const string& name, bool& opt) {
		*os << opt << " ";
	}
	
	virtual void optionalEnd() {
	}
	
	virtual void objBegin(const string& name, string& type_name, uint32_t chilIdx) {
		*os << name << " " << type_name << endl;
	}
	
	virtual void objBegin(const string& name, string& type_name, string& id, bool& isref, uint32_t chilIdx) {
		*os << name << " " << type_name << " " << ((isref)?"ref":"id") << " " << id << endl;
	}
	
	virtual void objEnd() {
	}	

	virtual void arrayBegin(const string& name, uint32_t& length) {
		*os << length << endl;
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
