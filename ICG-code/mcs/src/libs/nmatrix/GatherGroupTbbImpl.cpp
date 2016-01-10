#include "GatherGroupTbbImpl.h"

void GatherPointAssign::operator()( const blocked_range<uint32_t> r ) const
{
    for (uint32_t i = r.begin(); i != r.end(); i++)
    {
        float minDist = FLT_MAX;
        uint32_t mIdx = 0;
        const GatherKdItem &item = _items[i];
        for (uint32_t c = 0; c < _seeds.size(); c++)
        {
            float dist = (_seeds[c] - item.p).GetLengthSqr();
            if (dist < minDist)
            {
                minDist = dist;
                mIdx = c;
            }
        }
        _clusters[mIdx].push_back(i);
    }
}

void GatherPointUpdate::operator()( const blocked_range<uint32_t> r ) const
{
    for (uint32_t c = r.begin(); c != r.end(); c++)
    {
        const concurrent_vector<uint32_t> &cluster = _clusters[c];
        if (cluster.size() == 0)
            continue;

        Vec6f P;
        for (uint32_t j = 0; j < cluster.size(); j++)
        {
            const GatherKdItem &item = _items[cluster[j]];
            P += item.p;
        }
        _seeds[c] = P / (float)cluster.size();
    }
}
