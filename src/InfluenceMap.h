#pragma once

#include <sc2api/sc2_common.h>

/*
ideas for influence map:
a class InfluenceMap will have:
- a center (sc2::Point2DI?)
- a radius
then, influence map will get a list of sc2::Point2DIs that are within radius distance of center


onStep:
0. update local map (ie, what points are in the map)
1. generate InfluenceSource objects from local enemies/allies
2. propagate said InfluenceSources across our map
3. return map to client (ie whoever has a influence map. likely some GameObject will have it)
*/
namespace Monte {
    typedef struct InfluenceSource_s_t {
        sc2::Point2DI center;
        float score;
        float maxRadius;
        virtual float decay(float r) { return 0.0; };
    } InfluenceSource;

    class InfluenceMap {
        public:
        InfluenceMap() {};
    };
} // end namespace Monte