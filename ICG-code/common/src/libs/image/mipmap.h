#ifndef _MIPMAP_H_
#define _MIPMAP_H_

#include "image.h"
#include <vmath/functions.h>

#include <vector>
using std::vector;

template<class T>
class MipMap {
public:
    MipMap(const Image<T>& img) {
        _BuildLevels(img);
    }
    
    void Set(const Image<T>& img) {
        _BuildLevels(img);
    }
    
    int Levels() {
        return levels.size();
    }
    
    Image<T>& Level(int l) { return levels[l]; }
    
protected:
    void _BuildLevels(const Image<T>& img) {
        Image<T> img2;
        if(isPow2(img.Width()) && isPow2(img.Height())) {
            img2 = img;
        } else {
            assert(0);
            printf("no image resampling yet\n");
        }
        
        levels.push_back(img2);
        while(levels.back().Width() > 1 || 
              levels.back().Height() > 1) {
            levels.push_back(levels.back());
            levels.back().Downsample2();
        }
    }
    
protected:
    vector<Image<T> > levels;
};

#endif
