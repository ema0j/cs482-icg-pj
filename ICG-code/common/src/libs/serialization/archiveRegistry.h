#ifndef _ARCHIVEREGISTRY_H_
#define _ARCHIVEREGISTRY_H_

#include <misc/stdcommon.h>

class ISerializable;

struct ArchiveTypeInfo {
	string name;
	const type_info& info;
	ISerializable* (*constructor)();

	ArchiveTypeInfo(const string& n, const type_info& i, ISerializable* (*c)()) : 
		name(n), info(i), constructor(c) {}
};

template<typename T>
ISerializable* _constructor() { return new T(); }

// fix hash function!!!!
class ArchiveTypeRegistry {
public:
	// type has to have a static method called string serialize_typename()
	template<class T>
	static void register_type() {
		register_type<T>((string)T::serialize_typename());
	}
	template<class T>
	static void register_abstract_type() {
		register_abstract_type<T>((string)T::serialize_typename());
	}	
	
	template<class T>
	static void register_abstract_type(const string& name) {
		register_type<T>(name, 0);
	}

	template<class T>
	static void register_type(const string& name) {
		register_type<T>(name, _constructor<T>);
	}

	template<class T>
	static void register_type(const string& name, ISerializable* (*constructor)()) {
		if(name_to_info.find(name) != name_to_info.end()) return;
		
		ArchiveTypeInfo* info = 
		new ArchiveTypeInfo(name,typeid(T),constructor);
		
		name_to_info[info->name] = info;
		typeid_to_name[typeid(T).name()] = info->name;

		size_t h = hashFunc(info->name) + (count++);
		if(hash_to_name.find(h) != hash_to_name.end()) { cerr << "hash function failed!" << endl; assert(0); }

		name_to_hash[info->name] = h;
		hash_to_name[h] = info->name;
	}

	static string null_type_name() { return "null"; }

	template<class T>
	static string type_name(T& t) {
		string typeidname = typeid(t).name();
		return typeid_to_name[typeidname];
	}

	template<class T>
	static string type_name() {
		string typeidname = typeid(T).name();
		return typeid_to_name[typeidname];
	}

	static ArchiveTypeInfo* get_type(const string& type_name) {
		return name_to_info[type_name];
	}

	static size_t to_hash(const string& type_name) {
		return name_to_hash[type_name];
	}

	static string from_hash(size_t type_hash) {
		return hash_to_name[type_hash];
	}

    static void construct(const string& type_name, ISerializable*& v) {
        ArchiveTypeInfo* info = get_type(type_name);
        v = info->constructor();
    }

    static void construct(const string& type_name, shared_ptr<ISerializable>& v) {
        ArchiveTypeInfo* info = get_type(type_name);
		if(!info) cerr << "unable to constuct unknown type " << type_name << endl << flush;
        v = shared_ptr<ISerializable>(info->constructor());
    }
	
protected:
	static map<string, ArchiveTypeInfo*> name_to_info;
	static map<string, string> typeid_to_name;
	static map<string, size_t> name_to_hash;
	static map<size_t, string> hash_to_name;
	static hash<string> hashFunc;
    static size_t count;
};

class ArchivePointerRegistry {
public:
	virtual void clear() {
		name_to_ptr.clear(); 
		name_to_shared.clear();
		ptr_to_name.clear(); 
		shared_to_name.clear();
		name_to_hash.clear();

		obj_count = 0;
		add((ISerializable*)0, "null"); 
		obj_count = 0;
		add(shared_ptr<ISerializable>(), "null");
		obj_count = 1;
	}

	void get(const string& id, ISerializable*& v) {
		v = name_to_ptr[id];
	}

	void get(const string& id, shared_ptr<ISerializable>& v) {
		v = name_to_shared[id];
	}
	
	void add(ISerializable* v, const string& id) {
		name_to_ptr[id] = v;
		ptr_to_name[v] = id;
		name_to_hash[id] = obj_count;
		obj_count ++;
	}

	void add(shared_ptr<ISerializable> v, const string& id) {
		name_to_shared[id] = v;
		shared_to_name[v] = id;
		name_to_hash[id] = obj_count;
		obj_count ++;
	}
	
	bool isref(ISerializable* v) {
		return ptr_to_name.find(v) != ptr_to_name.end();
	}

	bool isref(shared_ptr<ISerializable> v) {
		return shared_to_name.find(v) != shared_to_name.end();
	}
	
	string id(ISerializable* v) {
        if(isref(v)) return ptr_to_name[v];
        else {
            stringstream buf; 
            buf << obj_count;
            return buf.str();
        }
	}

	string id(shared_ptr<ISerializable> v) {
		if(isref(v)) return shared_to_name[v];
		else return make_id(obj_count);
	}

	size_t to_hash(const string& id) {
		return name_to_hash[id];
	}
	
	string make_id(size_t hash) {
		stringstream buf;
		buf << hash;
		return buf.str();
	}
	
protected:
	map<string, ISerializable*> name_to_ptr;
	map<ISerializable*, string> ptr_to_name;
	map<string, shared_ptr<ISerializable> > name_to_shared;
	map<shared_ptr<ISerializable>, string> shared_to_name;
	map<string, size_t> name_to_hash;
	size_t obj_count;
};

#endif
