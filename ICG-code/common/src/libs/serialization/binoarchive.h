#ifndef _BINOARCHIVE_H_
#define _BINOARCHIVE_H_

#include "archive.h"
#include <vmath/streamMethods.h>

class BinaryOArchive : public OArchive {
public:
    BinaryOArchive() { useHashing  = true; }

    static bool isbinary() { return true; }

    virtual void member(const string& name, uint32_t& v) { write(name, v); }
    virtual void member(const string& name, int& v) { write(name, v); }
    virtual void member(const string& name, float& v) { write(name, v); }
    virtual void member(const string& name, double& v) { write(name, v); }
    virtual void member(const string& name, bool& v) { write(name, v); }

    virtual void member(const string& name, string& v) { writeString(name, v); }

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
    bool useHashing;

    virtual void parseErrorInfo(ostream& s) { }

    template<typename T>
    void writeValue(T v) {
		size_t c = sizeof(T);
        os->write((char*)&v, c);
    }

    void writeString(string v) {
        writeValue(static_cast<uint32_t>(v.length()));
        os->write(v.c_str(), v.length());
    }

    template<typename T>
    void write(const string& name, T& v) {
        writeValue(v);
    }

    void writeString(const string& name, string& v) {
        writeString(v);
    }

    template<typename T>
    void writeArray(const string& name, carray<T>& v) {
        writeValue(static_cast<uint32_t>(v.size()));
        for(uint32_t i = 0; i < v.size(); i ++) writeValue(v[i]);
    }

    template<typename T>
    void writeArray2(const string& name, carray2<T>& v) {
        writeValue(static_cast<uint32_t>(v.width()));
        writeValue(static_cast<uint32_t>(v.height()));
        for(uint32_t i = 0; i < v.size(); i ++) writeValue(v[i]);
    }

    virtual void archiveBegin() {
    }

    virtual void archiveEnd() {
    }

    virtual void optionalBegin(const string& name, bool& opt) {
        writeValue(opt);
    }

    virtual void optionalEnd() {
    }

    virtual void objBegin(const string& name, string& type_name, uint32_t chilIdx) {
		if(useHashing) 
		{ 
			uint64_t hash = ArchiveTypeRegistry::to_hash(type_name);
			writeValue(hash); 
		}
		else { writeString(type_name); }
    }

    virtual void objBegin(const string& name, string& type_name, string& id, bool& isref, uint32_t chilIdx) {
        if(useHashing) 
		{
			uint64_t hash = ArchiveTypeRegistry::to_hash(type_name);
			writeValue(hash); 
		}
        else { writeString(type_name); }

        writeValue(isref);

        if(useHashing) { writeValue(Registry()->to_hash(id)); }
        else { writeString(id); }
    }

    virtual void objEnd() {
    }	

    virtual void arrayBegin(const string& name, uint32_t& length) {
        writeValue(length);
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
