#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "archiveRegistry.h"

#include <misc/arrays.h>
#include <vmath/vec2.h>
#include <vmath/vec3.h>
#include <vmath/vec4.h>
#include <vmath/matrix4.h>
#include <vmath/spline.h>
#include <vmath/uspline.h>

using std::ostream;
using std::istream;
using std::flush;

class Archive;

class ISerializable {
public:
	virtual void serialize(Archive* a) = 0;
};

class Archive {
public:
	virtual bool isreading() = 0;

	virtual void serialize(const string& name, ISerializable& v) {
		Registry()->clear();
		archiveBegin();
		member(name, v);
		archiveEnd();
		Registry()->clear();
	}
	
	virtual void serialize(const string& name, ISerializable*& v) {
		Registry()->clear();
		archiveBegin();
		member(name, v);
		archiveEnd();
		Registry()->clear();
	}
	
	virtual void serialize(const string& name, shared_ptr<ISerializable>& v) {
		Registry()->clear();
		archiveBegin();
		member(name, v);
		archiveEnd();
		Registry()->clear();
	}
	
	template<class T>
	void optional(const string& name, T& v, const T& vopt) {
		bool opt;
		if(isreading()) {
			optionalBegin(name, opt);
			if(opt) member(name, v);
			else v = vopt;
			optionalEnd();
		} else {
			opt = !(v == vopt);
			optionalBegin(name, opt);
			if(opt) member(name, v);
			optionalEnd();
		}
	}
	
	virtual void member(const string& name, ISerializable& v, uint32_t childIdx = 0) {
		string type;
		if(!isreading()) { type = ArchiveTypeRegistry::type_name(v); }
		objBegin(name, type, childIdx);
		(&v)->serialize(this);
		objEnd();
	}
	
	template<typename T>
	void callback(const string& name, T& v, void (*callback)(T&, Archive*), uint32_t childIdx = 0) {
		string type;
		if(!isreading()) { type = ArchiveTypeRegistry::type_name(v); }
		objBegin(name, type, childIdx);
		callback(v, this);
		objEnd();
	}
	
	template<typename T>
	void member(const string& name, T*& v, uint32_t childIdx = 0) {
		ISerializable* is_v = dynamic_cast<ISerializable*>(v);
		memberPtr<ISerializable*, T>(name, is_v, childIdx);
		if(isreading()) {
			v = dynamic_cast<T*>(is_v);			
		}
	}

	template<typename T>
	void member(const string& name, shared_ptr<T>& v, uint32_t childIdx = 0) {
		shared_ptr<ISerializable> is_v = dynamic_pointer_cast<ISerializable>(v);
		memberPtr<shared_ptr<ISerializable>, T>(name, is_v, childIdx);
		if(isreading()) {
			v = dynamic_pointer_cast<T>(is_v);
		}
	}
	
	// T is the ISerializable pointer type (* or shared_ptr)
	// TT is the type of the pointed object before dynamic cast
	template<class T, class TT>
	void memberPtr(const string& name, T& v, uint32_t childIdx = 0) {
		string id; bool isref; string type;
		if(isreading()) {
			objBegin(name, type, id, isref, childIdx);
			if(type == "") type = ArchiveTypeRegistry::type_name<TT>();
			if(isref) {
				Registry()->get(id, v);
			} else {
				if(type == "") parseError(string("underfined type for object ") + name);
                ArchiveTypeRegistry::construct(type, v);
				if(v) v->serialize(this);
				if(id != "") Registry()->add(v, id);
			}
			objEnd();
		} else {
#ifdef WIN32
			if(v == 0) type = ArchiveTypeRegistry::null_type_name();
#else
            if((intptr_t)v == 0) type = ArchiveTypeRegistry::null_type_name();
#endif
			else type = ArchiveTypeRegistry::type_name(*v);
			id = Registry()->id(v);
			isref = Registry()->isref(v);
			if(!isref) Registry()->add(v, Registry()->id(v));

			objBegin(name, type, id, isref, childIdx);
			if(!isref) { v->serialize(this); }
			objEnd();
		}
	}

    virtual void member(const string& name, uint32_t& v) = 0;
	virtual void member(const string& name, int& v) = 0;
	virtual void member(const string& name, float& v) = 0;
	virtual void member(const string& name, double& v) = 0;
	virtual void member(const string& name, bool& v) = 0;

	virtual void member(const string& name, string& v) = 0;

	virtual void member(const string& name, carray<int>& v) = 0;
	virtual void member(const string& name, carray<float>& v) = 0;
	virtual void member(const string& name, carray<double>& v) = 0;

	virtual void member(const string& name, carray2<int>& v) = 0;
	virtual void member(const string& name, carray2<float>& v) = 0;
	virtual void member(const string& name, carray2<double>& v) = 0;

	virtual void member(const string& name, Vec2i& v) = 0;
	virtual void member(const string& name, Vec2f& v) = 0;
	virtual void member(const string& name, Vec2d& v) = 0;

	virtual void member(const string& name, Vec3i& v) = 0;
	virtual void member(const string& name, Vec3f& v) = 0;
	virtual void member(const string& name, Vec3d& v) = 0;

	virtual void member(const string& name, Tuple3f& v) = 0;
	virtual void member(const string& name, Tuple3d& v) = 0;

	virtual void member(const string& name, Vec4i& v) = 0;
	virtual void member(const string& name, Vec4f& v) = 0;
	virtual void member(const string& name, Vec4d& v) = 0;

	virtual void member(const string& name, Matrix4f& v) = 0;
	virtual void member(const string& name, Matrix4d& v) = 0;

	virtual void member(const string& name, carray<Vec2f>& v) = 0;
	virtual void member(const string& name, carray<Vec3f>& v) = 0;
	virtual void member(const string& name, carray<Vec4f>& v) = 0;
	virtual void member(const string& name, carray<Tuple3f>& v) = 0;
	virtual void member(const string& name, carray<Point3f>& v) = 0;
	virtual void member(const string& name, carray<Vector3f>& v) = 0;
	virtual void member(const string& name, carray<Normal3f>& v) = 0;
	virtual void member(const string& name, carray<Direction3f>& v) = 0;

	virtual void member(const string& name, carray<Vec2d>& v) = 0;
	virtual void member(const string& name, carray<Vec3d>& v) = 0;
	virtual void member(const string& name, carray<Vec4d>& v) = 0;
	virtual void member(const string& name, carray<Tuple3d>& v) = 0;
	virtual void member(const string& name, carray<Point3d>& v) = 0;
	virtual void member(const string& name, carray<Vector3d>& v) = 0;
	virtual void member(const string& name, carray<Normal3d>& v) = 0;
	virtual void member(const string& name, carray<Direction3d>& v) = 0;

	virtual void member(const string& name, carray<Vec2i>& v) = 0;
	virtual void member(const string& name, carray<Vec3i>& v) = 0;
	virtual void member(const string& name, carray<Vec4i>& v) = 0;

	virtual void member(const string& name, carray<Matrix4d>& v) = 0;

	virtual void member(const string& name, carray2<Vec2f>& v) = 0;
	virtual void member(const string& name, carray2<Vec3f>& v) = 0;
	virtual void member(const string& name, carray2<Vec4f>& v) = 0;

	virtual void member(const string& name, carray2<Vec2d>& v) = 0;
	virtual void member(const string& name, carray2<Vec3d>& v) = 0;
	virtual void member(const string& name, carray2<Vec4d>& v) = 0;

	virtual void member(const string& name, carray2<Matrix4d>& v) = 0;

	template<typename T, typename TT>
	void member(const string& name, Spline<T,TT>& v, uint32_t childIdx = 0) {
		static bool init = false;
		if(!init) {
			// this is a disgusting hack!
			ArchiveTypeRegistry::register_abstract_type<Spline<T,TT> >("_Spline_");
			init = true;
		}

		callback<Spline<T,TT> >(name, v, serialize_spline<T,TT>, childIdx);
	}

	template<typename T>
	void member(const string& name, UniformSplineCurve<T>& v, uint32_t childIdx = 0) {
		static bool init = false;
		if(!init) {
			// this is a disgusting hack!
			ArchiveTypeRegistry::register_abstract_type<UniformSplineCurve<T> >("_UniformSplineCurve_");
			init = true;
		}
        
		callback<UniformSplineCurve<T> >(name, v, serialize_uspline<T>, childIdx);
	}
    
	template<class T>
	void member(const string& name, carray<T*>& v) { memberArray(name, v); }
	template<class T>
	void member(const string& name, carray<shared_ptr<T> >& v) { memberArray(name, v); }
	template<class T>
	void iterate(const string& name, carray<T*>& v) { iterateArray(name, v); }
	template<class T>
	void iterate(const string& name, carray<shared_ptr<T> >& v) { iterateArray(name, v); }

	template<typename T, typename TT>
	void member(const string& name, carray<Spline<T,TT> >& v) { memberArray<Spline<T,TT> >(name, v); }
	template<typename T, typename TT>
	void iterate(const string& name, carray<Spline<T,TT> >& v) { iterateArray<Spline<T,TT> >(name, v); }

	template<class T>
	void member(const string& name, vector<T>& v) { carray<T> va(v); member(name, va); if(isreading()) { va.tovector(v); } }
	template<class T>
	void iterate(const string& name, vector<T>& v) { carray<T> va(v); iterate(name, va); if(isreading()) { va.tovector(v); } }

	template<class T>
	void memberArray(const string& name, carray<T>& v) {
		uint32_t size;
		if(isreading()) {
			arrayBegin(name, size);
			v.resize(size);
			for(uint32_t i = 0; i < v.size(); i ++) {
				member(name, v[i], i);
			}
			arrayEnd();
		} else {
			size = static_cast<uint32_t>(v.size());
			arrayBegin(name, size);
			for(uint32_t i = 0; i < v.size(); i ++) {
				member(name, v[i], i);
			}
			arrayEnd();
		}
	}

	template<class T>
	void iterateArray(const string& name, carray<T>& v) {
		uint32_t size;
		if(isreading()) {
			iterateBegin(name, size);
			v.resize(size);
			for(uint32_t i = 0; i < v.size(); i ++) {
				member(name, v[i], i);
			}
			iterateEnd();
		} else {
			size = static_cast<uint32_t>(v.size());
			iterateBegin(name, size);
			for(uint32_t i = 0; i < v.size(); i ++) {
				member(name, v[i], i);
			}
			iterateEnd();
		}
	}

protected:
	virtual void archiveBegin() = 0;
	virtual void archiveEnd() = 0;
	virtual void optionalBegin(const string& name, bool& opt) = 0;
	virtual void optionalEnd() = 0;
	virtual void objBegin(const string& name, string& type, uint32_t childIdx) = 0;
	virtual void objBegin(const string& name, string& type, string& id, bool& isref, uint32_t childIdx) = 0;
	virtual void objEnd() = 0;
	virtual void arrayBegin(const string& name, uint32_t& length) = 0;
	virtual void arrayEnd() = 0;
	virtual void iterateBegin(const string& name, uint32_t& length) = 0;
	virtual void iterateEnd() = 0;
    virtual ArchivePointerRegistry* Registry() { return &ptrRegistry; }

    virtual void parseError(const string& msg) {
        cerr << endl << msg << endl << flush;
        parseErrorInfo(cerr);
        assert(0);
    }
    virtual void parseErrorInfo(ostream& s) = 0; 

	template<class T, class TT>
	static void serialize_spline(Spline<T,TT>& v, Archive* a) {
		if(a->isreading()) {
			string type; carray<T> controlPoints; carray<TT> times;
			a->member("basisType", type);
			a->optional("times", times, carray<TT>());
			a->member("controlPoints", controlPoints);
			v.Set(Spline<T,TT>::GetBasisType(type), controlPoints, times);
		} else {
			string type = Spline<T,TT>::GetBasisTypeName(v.GetBasisType());
			a->member("basisType", type);
			a->optional("times", *v.GetTimes(), carray<TT>());
			a->member("controlPoints", *v.GetControlPoints());
		}
	}

	template<class T>
	static void serialize_uspline(UniformSplineCurve<T>& v, Archive* a) {
		if(a->isreading()) {
			string type; carray<T> controlPoints;
			a->member("basisType", type);
			a->member("controlPoints", controlPoints);
			v.Set(UniformSplineCurve<T>::GetBasisType(type), controlPoints);
		} else {
			string type = UniformSplineCurve<T>::GetBasisTypeName(v.GetBasisType());
			a->member("basisType", type);
			a->member("controlPoints", *v.GetControlPoints());
		}
	}
    private:
        ArchivePointerRegistry ptrRegistry;
};

class OArchive : public Archive {
public:
	void setStream(ostream* o) { os = o; }
	
	virtual void serialize(const string& name, ISerializable& v, ostream* o) { setStream(o); Archive::serialize(name, v); }
	virtual void serialize(const string& name, ISerializable*& v, ostream* o) { setStream(o); Archive::serialize(name, v); }
	virtual void serialize(const string& name, shared_ptr<ISerializable>& v, ostream* o) { setStream(o); Archive::serialize(name, v); }
	
	virtual bool isreading() { return false; }	

protected:
	ostream* os;
};

class IArchive : public Archive {
public:
	void setStream(istream* i) { is = i; }
	
	virtual void serialize(const string& name, ISerializable& v, istream* i) { setStream(i); Archive::serialize(name, v); }
	virtual void serialize(const string& name, ISerializable*& v, istream* i) { setStream(i); Archive::serialize(name, v); }
	virtual void serialize(const string& name, shared_ptr<ISerializable>& v, istream* i) { setStream(i); Archive::serialize(name, v); }

	virtual bool isreading() { return true; }

protected:
	istream* is;
};

#endif
