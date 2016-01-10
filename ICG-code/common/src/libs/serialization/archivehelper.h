#ifndef _ARCHIVEHELPER_H_
#define _ARCHIVEHELPER_H_

#include "archive.h"

#include <serialization/xmliarchive.h>
#include <serialization/xmloarchive.h>
#include <serialization/textiarchive.h>
#include <serialization/textoarchive.h>
#include <serialization/biniarchive.h>
#include <serialization/binoarchive.h>

#include <fstream>

using std::ofstream;
using std::ifstream;
using std::ios_base;

// simpler uinterface to archives to manager the main three archive types
// note that type registration needs to be done before
class ArchiveHelper {
public:
    // automaticvally chooses based on ext: .xml, .txt, .dta, .bin
    template<class T>
    static void save(const string& name, const string& filename, shared_ptr<T> v, void (*typeRegisterFunc)()) {
	    string ext = filename.substr(filename.length()-3);
	    if(ext == "xml") { fileout<T, XmlOArchive>(name, filename, v, typeRegisterFunc); }
	    else if(ext == "txt") { fileout<T, TextOArchive>(name, filename, v, typeRegisterFunc); }
	    else if(ext == "bin" || ext == "dta") { fileout<T, BinaryOArchive>(name, filename, v, typeRegisterFunc); }
	    else { cout << "not supported file format" << endl; }
    }

	// pretty prints to a stream using xml format
    template<class T>
    static void prettyprint(const string& name, shared_ptr<T> v, void (*typeRegisterFunc)()) {
        streamout<T, XmlOArchive>(name, cout, v, typeRegisterFunc);
    }

    template<class T>
    static void append(ofstream &fout, const string& name, shared_ptr<T> v, void (*typeRegisterFunc)()) {
        streamout<T, XmlOArchive>(name, fout, v, typeRegisterFunc);
    }

	// automaticvally chooses based on ext: .xml, .txt, .dta, .bin
    template<class T>
	static void load(const string& name, const string& filename, shared_ptr<T>& v, void (*typeRegisterFunc)()) {
		string ext = filename.substr(filename.length() - 3);
		if(ext == "xml") { filein<T, XmlIArchive>(name, filename, v, typeRegisterFunc); }
		else if(ext == "txt") { filein<T, TextIArchive>(name, filename, v, typeRegisterFunc); }
		else if(ext == "bin" || ext == "dta") { filein<T, BinaryIArchive>(name, filename, v, typeRegisterFunc); }
		else { cout << "not supported file format" << endl; }
    }

    template<class T>
    static void registerType(const string& name) { ArchiveTypeRegistry::register_type<T>(name); }
    template<class T>
    static void registerAbstractType(const string& name) { ArchiveTypeRegistry::register_abstract_type<T>(name); }

    template<class T>
    static void registerType() { ArchiveTypeRegistry::register_type<T>(); }
    template<class T>
    static void registerAbstractType() { ArchiveTypeRegistry::register_abstract_type<T>(); }


protected:
    template<class T, class OutArchive>
    static void fileout(const string& name, const string& filename, shared_ptr<T>& v, void (*typeRegisterFunc)()) {
	    ofstream os;
	    if(!OutArchive::isbinary()) { os.open(filename.c_str()); }
	    else { os.open(filename.c_str(), ios_base::out | ios_base::binary); }
	    streamout<T, OutArchive>(name, os, v, typeRegisterFunc);
	    os.close();
    }

    template<class T, class OutArchive>
    static void streamout(const string& name, ostream& os, shared_ptr<T>& v, void (*typeRegisterFunc)()) {
        static bool init = false;
        if(!init && typeRegisterFunc) { typeRegisterFunc(); init = true; }
	    OutArchive oa;
	    shared_ptr<ISerializable> v_is = dynamic_pointer_cast<ISerializable>(v);
	    oa.serialize(name, v_is, &os);
    }

    template<class T, class InArchive>
    static void filein(const string& name, const string& filename, shared_ptr<T>& v, void (*typeRegisterFunc)()) {
		ifstream is;
	    if(!InArchive::isbinary()) { is.open(filename.c_str()); }
	    else { is.open(filename.c_str(), ios_base::in | ios_base::binary); }
	    if(!is.is_open()) 
		{ 
			cerr << "cannot open " << filename << endl << flush; 
			assert(0); 
		}
	    streamin<T, InArchive>(name, is, v, typeRegisterFunc);
	    is.close();
    }

    template<class T, class InArchive>
    static void streamin(const string& name, istream& is, shared_ptr<T>& v, void (*typeRegisterFunc)()) {
        static bool init = false;
        if(!init && typeRegisterFunc) { typeRegisterFunc(); init = true; }
	    InArchive ia;
	    shared_ptr<ISerializable> v_is;
	    ia.serialize(name, v_is, &is);
	    v = dynamic_pointer_cast<T> (v_is);
    }

};

#endif
