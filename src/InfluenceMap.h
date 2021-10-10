#pragma once

#include <pair>
#include <vector>
#include <sc2api/sc2_common.h>

/*
ideas for influence map:
a class InfluenceMap will have:
- a center (sc2::Point2DI?)
- a radius
then, influence map will get a list of sc2::Point2DIs that are within radius distance of center

questions:
- should gameobjects use InfluenceMap directly, or should we have child classes of InfluenceMap for each unit?
- what if influence map took a pointer to ObservationInterface as a paramter in the constructor?
- should influence map output an "ideal" point to move to st it is closest to target and lowest score?
    or is this the responsibility of the object that has influencemap?


update:
0. update local map (ie, what points are in the map)
2. propagate InfluenceSources across our map
3. return map to client (ie whoever has a influence map. likely some GameObject will have it)
*/
namespace Monte {
    typedef struct InfluenceSource_s_t {
        sc2::Point2DI center;
        float score;
        float maxRadius;
    } InfluenceSource;

    typedef std::pair<sc2::Point2DI, float> InfluenceTile;

    class InfluenceMap {
        public:
        InfluenceMap() {};
        void update(sc2::Point2D center);
        void addSource(sc2::Point2D center_, float score_, float radius_);
        void clearSources();
        sc2::Point2DI getOptimalWaypoint(sc2::Point2D target); // TODO: rename to getSafeWaypoint?

        protected:
        std::vector<InfluenceSource> sources;
        std::vector<InfluenceTile> localRegion;
        sc2::Point2DI center;
        float maxRadius;
    };
} // end namespace Monte