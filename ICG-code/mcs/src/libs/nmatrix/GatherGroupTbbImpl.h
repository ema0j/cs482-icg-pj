#ifndef _GATHER_GROUP_TBB_IMPL_H_
#define _GATHER_GROUP_TBB_IMPL_H_

#include <ray/rayEngine.h>
#include <scene/scene.h>
#include <tbbutils/tbbutils.h>
#include <image/image.h>
#include <gpshoot/GatherPointShooter.h>
#include "vmath/vec6.h"
#include "KnnMatrix.h"

struct GatherPointAssign
{
    GatherPointAssign(const vector<Vec6f> &seeds, const vector<GatherKdItem> &items, vector<concurrent_vector<uint32_t> > &clusters)
        : _seeds(seeds), _items(items), _clusters(clusters) { }
    void operator()( const blocked_range<uint32_t> r ) const;
    const vector<Vec6f>                         &_seeds;
    const vector<GatherKdItem>                  &_items;
    vector<concurrent_vector<uint32_t> >        &_clusters;
};

struct GatherPointUpdate
{
    GatherPointUpdate(vector<Vec6f> &seeds, const vector<GatherKdItem> &items, const vector<concurrent_vector<uint32_t> > &clusters)
        : _seeds(seeds), _items(items), _clusters(clusters) { }
    void operator()( const blocked_range<uint32_t> r ) const;
    vector<Vec6f>                               &_seeds;
    const vector<GatherKdItem>                  &_items;
    const vector<concurrent_vector<uint32_t> >  &_clusters;
};

#endif // _GATHER_GROUP_TBB_IMPL_H_