#ifndef _BINIARCHIVE_H_
#define _BINIARCHIVE_H_

#include "archive.h"
#include <vmath/streamMethods.h>

class BinaryIArchive : public IArchive {
public:
    BinaryIArchive() { useHashing  = true; }

    static bool isbinary() { return true; }

    virtual void member(const string& name, uint32_t& v) { read(name, v); }
    virtual void member(const string& name, int& v) { read(name, v); }
    virtual void member(const string& name, float& v) { read(name, v); }
    virtual void member(const string& name, double& v) { read(name, v); }	
    virtual void member(const string& name, bool& v) { read(name, v); }	

    virtual void member(const string& name, string& v) { readString(name, v); }	

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
    bool useHashing;

    virtual void parseErrorInfo(ostream& s) { }

    template<typename T>
    void readValue(T& v) {
		uint32_t c = sizeof(v);
        is->read((char*)&v, c);
    }

    void readString(string& v) {
        char buf[10000];
        uint32_t length;
        readValue(length);
        is->read(buf, length);
        buf[length] = 0;
        v = string(buf);
    }

    template<typename T>
    void read(const string& name, T& v) {
        readValue(v);
    }

    void readString(const string& name, string& v) {
        readString(v);
    }

    template<typename T>
    void readArray(const string& name, carray<T>& v) {
        uint32_t length;
        readValue(length);

        v.resize(length);
        for(uint32_t i = 0; i < length; i ++) readValue(v[i]);
    }

    template<typename T>
    void readArray2(const string& name, carray2<T>& v) {
		uint32_t width, height;
        readValue(width);
        readValue(height);

        v.resize(width, height);
        for(uint32_t i = 0; i < width*height; i ++) readValue(v[i]);
    }

    virtual void archiveBegin() {
    }

    virtual void archiveEnd() {
    }

    virtual void optionalBegin(const string& name, bool& opt) {
        readValue(opt);
    }

    virtual void optionalEnd() {
    }

    virtual void objBegin(const string& name, string& type, uint32_t childIdx) {
		if(useHashing) { size_t hash; readValue(hash); type = ArchiveTypeRegistry::from_hash(hash); } 
        else { readString(type); }
    }

    virtual void objBegin(const string& name, string& type, string& id, bool &isref, uint32_t childIdx) {
        if(useHashing) 
		{ 
			size_t hash; 
			readValue(hash); 
			type = ArchiveTypeRegistry::from_hash(hash); 
		} 
        else { readString(type); }

        readValue(isref);

        if(useHashing) { 
			size_t hash; 
			readValue(hash); 
			id = Registry()->make_id(hash); 
		}
        else { readString(id); }
    }

    virtual void objEnd() {
    }

    virtual void arrayBegin(const string& name, uint32_t& length) {
        readValue(length);
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
