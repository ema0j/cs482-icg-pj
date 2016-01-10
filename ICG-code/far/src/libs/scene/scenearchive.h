#ifndef _SCENEARCHIVE_H_
#define _SCENEARCHIVE_H_

#include "scene.h"
#include <serialization/archivehelper.h>

class SceneArchive {
public:
	// automaticvally chooses based on ext: .xml, .txt, .dta, .bin
	static void save(const string& filename, shared_ptr<Scene> scene);

	// pretty prints to a stream using xml format
	static void prettyprint(shared_ptr<Scene> scene);

	// automaticvally chooses based on ext: .xml, .txt, .dta, .bin
	static shared_ptr<Scene> load(const string& filename);
    static void load(const string& filename, shared_ptr<Scene>& scene);

    template<typename T>
    static void append(std::ofstream &fout, const string &name, shared_ptr<T> obj)
    {
        ArchiveHelper::append(fout, name, obj, registerSceneTypes);
    }
protected:
	static void registerSceneTypes();
};

#endif
