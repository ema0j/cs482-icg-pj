#ifndef _SPARSECUBEMAP_H_
#define _SPARSECUBEMAP_H_

#include <image/cubemap.h>
#include <image/sparseImage.h>

template<class T>
class SparseCubeMap {
public:
    SparseCubeMap() { }
    SparseCubeMap(const CubeMap<T>& m) { FromCubeMap(m); }

	int Resolution() const { return faces[0].Width(); }

	SparseImage<T>& Face(int f) { return faces[f]; }
	const SparseImage<T>& Face(int f) const { return faces[f]; }

	CubeMap<T> ToCubeMap() const {
        CubeMap<T> cube;
        ToCubeMap(cube);
        return cube;
    }

	void ToCubeMap(CubeMap<T>& cube) const {
        cube.Alloc(Resolution());
		for(int f = 0; f < 6; f ++) {
			faces[f].ToImage(cube.Face(f));
		}
	}

    void FromCubeMap(const CubeMap<T>& cube) {
        for(int i = 0; i < 6; i ++) faces[i].FromImage(cube.Face(i));
    }

protected:
	SparseImage<T> faces[6];
};

#endif
