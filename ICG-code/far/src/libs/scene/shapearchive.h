#ifndef _SHAPEARCHIVE_H_
#define _SHAPEARCHIVE_H_

#include "shape.h"

class ShapeArchive {
public:
	// automaticvally chooses based on ext: .xml, .txt, .dta, .bin
	static void save(const string& filename, shared_ptr<Shape> shape);

	// pretty prints to a stream using xml format
	static void prettyprint(shared_ptr<Shape> shape);

	// automaticvally chooses based on ext: .xml, .txt, .dta, .bin
	static void load(const string& filename, shared_ptr<Shape>& shape);

protected:
	static void registerSceneTypes();
};

#endif
