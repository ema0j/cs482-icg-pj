#ifndef _BACKGROUND_STANDARD_H_
#define _BACKGROUND_STANDARD_H_

#include "background.h"
#include "light.h"

class LightBackground : public Background {
public:
	LightBackground() { }

	LightBackground(shared_ptr<Light> l) {
		light = l;
	}

	virtual Vec3f SampleBackground(const Vec3f& dir, float time) { return light->SampleBackground(dir, time); }
	
	static string serialize_typename();
	virtual void serialize(Archive * a);

protected:
	shared_ptr<Light> light;
};

#endif
