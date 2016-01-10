#ifndef _XMLIARCHIVE_H_
#define _XMLIARCHIVE_H_

#include "archive.h"
#include <fstream>
#include <tinyxml/tinyxml.h>
#include <vmath/streamMethods.h>
#include <stack>

using std::cerr;
using std::ifstream;
using std::stack;

class XmlIArchive : public IArchive {
public:
    XmlIArchive() { valuesInAttributes = true; }

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
    bool valuesInAttributes;

	TiXmlDocument* doc;
	TiXmlNode* curNode;

    virtual void parseErrorInfo(ostream& s) {
        s << endl << "parser stack -----------------------" << endl;
        TiXmlElement* curElement = dynamic_cast<TiXmlElement*>(curNode);
        while(curElement) {
            s << curElement << " <- ";
            curElement = dynamic_cast<TiXmlElement*>(curElement->Parent());
        }
        s << endl << endl;
    }

    string readValueText(const string& name) {
        // check element
		TiXmlElement* e = dynamic_cast<TiXmlElement*>(curNode->FirstChild(name));
        if(e) { return e->GetText(); }

        // check attribute
        if(valuesInAttributes) {
            TiXmlElement* curE = dynamic_cast<TiXmlElement*>(curNode);
            if(curE) {
                const string* str = curE->Attribute(name);
                if(str) return *str;
            }
        }

        // missing value
        parseError(string("missing value ") + name);
        assert(0);
        return "";
    }

	template<typename T>
	void read(const string& name, T& v) {
		stringstream s(readValueText(name));
		s >> v;
	}
	
	template<typename T>
	void readBool(const string& name, T& v) {
		string s(readValueText(name));
		v = s == "true" || s == "TRUE";
	}
	
	template<typename T>
	void readArray(const string& name, carray<T>& v) {
		TiXmlElement* e = dynamic_cast<TiXmlElement*>(curNode->FirstChild(name));
		if(!e) { parseError(string("missing value ") + name); }
		stringstream s(e->GetText());
		vector<T> vec;

		while(!s.eof()) {
			T value;
			s >> value;
			vec.push_back(value);
		}

		v = carray<T>(vec);
	}
	
	template<typename T>
	void readArray2(const string& name, carray2<T>& v) {
		TiXmlElement* e = dynamic_cast<TiXmlElement*>(curNode->FirstChild(name));
		if(!e) { parseError(string("missing value ") + name); }
		stringstream s(e->GetText());
		vector<T> vec;

		while(!s.eof()) {
			T value;
			s >> value;
			vec.push_back(value);
		}

		int width = -1; int height = -1;
		e->Attribute("width", &width);
		e->Attribute("height", &height);
		v.resize(width, height);
		if((int)vec.size() != v.size()) { parseError(string("different size in carray2 ")); }

		int cur = 0;
		for(int j = 0; j < height; j ++) {
			for(int i = 0; i < width; i ++) {
				v.at(i,j) = vec[cur];
				cur ++;
			}
		}
	}
	
	virtual void archiveBegin() {
		doc = new TiXmlDocument();
		TiXmlElement* root = new TiXmlElement("placeholder");
		*is >> *root;
		doc->LinkEndChild(root);
		curNode = doc;

		// handle includes
		loadIncludes(doc->RootElement());
	}

	void loadIncludes(TiXmlElement* element) {
		// find include nodes
		bool done = false;
		while(!done) {
			TiXmlNode* includeNode = element->FirstChild("__include__");
			if(includeNode) {
				string filename = (dynamic_cast<TiXmlElement*>(includeNode))->GetText();
				if(!fileExists(filename)) { parseError(filename + " does not exists"); }
				ifstream ifs(filename.c_str());
				TiXmlElement* includedElement = new TiXmlElement("undefined");
				ifs >> *includedElement;
				TiXmlNode* insertAfterNode = includeNode;
				for(TiXmlNode* includedChild = includedElement->FirstChild(); 
					includedChild; includedChild = includedChild->NextSibling()) {
					insertAfterNode = element->InsertAfterChild(insertAfterNode, *includedChild);
				}
				element->RemoveChild(includeNode);
				delete includedElement;
				done = false;
			} else {
				done = true;
			}
		}

		// recurse
		for(TiXmlNode* child = element->FirstChild(); child; child = child->NextSibling()) {
			if(child->Type() == TiXmlNode::ELEMENT) {
				loadIncludes((TiXmlElement*)child);
			}
		}
	}
	
	virtual void archiveEnd() {
		delete curNode;
	}
	
	virtual void optionalBegin(const string& name, bool& opt) {
        // check member
		TiXmlNode *optChild = curNode->FirstChild(name);
		opt = optChild != 0;
        if(opt) return;

        // check attribute
        if(valuesInAttributes) {
            TiXmlElement* curElement = dynamic_cast<TiXmlElement*>(curNode);
            if(curElement) {
                const string* val = curElement->Attribute(name);
                opt = val != 0;
                if(opt) return;
            }
        }
	}
	
	virtual void optionalEnd() {
	}
	
	virtual void objBegin(const string& name, string& type, uint32_t childIdx) {
		curNode = curNode->FirstChild(name);
		if(!curNode) { parseError(string("missing value ") + name); }
		for(uint32_t i = 0; i < childIdx; i ++) {
			curNode = curNode->NextSibling(name);
			if(!curNode) { cerr << "missing value " << name << endl << flush; assert(0); }
		}

		TiXmlElement* curElement = (TiXmlElement*)curNode;
		if(curElement->Attribute("type")) {
			type = curElement->Attribute("type");
		} else { type = ""; }
	}

	virtual void objBegin(const string& name, string& type, string& id, bool &isref, uint32_t childIdx) {
		curNode = curNode->FirstChild(name);
		if(!curNode) { parseError(string("missing value ") + name); }
		for(uint32_t i = 0; i < childIdx; i ++) {
			curNode = curNode->NextSibling(name);
			if(!curNode) { parseError(string("missing value ") + name); }
		}

		TiXmlElement* curElement = (TiXmlElement*)curNode;
		if(curElement->Attribute("type")) {
			type = curElement->Attribute("type");
		} else { type = ""; }
		id = ""; isref = false;

		// check if ref
		const string* refStr = curElement->Attribute(string("ref"));
		if(refStr) {
			id = *refStr;
			isref = true;
			return;
		}

		// check if id
		const string* idStr = curElement->Attribute(string("id"));
		if(idStr) {
			id = *idStr;
			return;
		}
	}

	virtual void objEnd() {
		curNode = curNode->Parent();
	}
	
	virtual void arrayBegin(const string& name, uint32_t& length) {
		curNode = curNode->FirstChild(name);
		if(!curNode) { parseError(string("missing value ") + name); }

		length = 0;
		for(TiXmlNode* child = curNode->FirstChild(name); child; child = child->NextSibling(name)) {
			length ++;
		}
	}

	virtual void arrayEnd() {
		curNode = curNode->Parent();
	}

	virtual void iterateBegin(const string& name, uint32_t& length) {
		// count the children for iteration
		length = 0;
		for(TiXmlNode* child = curNode->FirstChild(name); child; child = child->NextSibling(name)) {
			length ++;
		}
	}

	virtual void iterateEnd() {
	}

	bool fileExists(const string& filename) {
		FILE* f = fopen(filename.c_str(), "rt");
		if(!f) return false;
		fclose(f);
		return true;
	}
};

#endif
