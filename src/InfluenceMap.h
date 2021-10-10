#pragma once

#include <pair>
#include <vector>
#include <sc2api/sc2_common.h>
#include "api.h"

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
3. return map to client? (ie whoever has a influence map. likely some GameObject will have it)

TODO: should maxRadius be int ?
*/
namespace Monte {
    typedef struct InfluenceSource_s_t {
        InfluenceSource_s_t(sc2::Point2D center_, float score_, float radius_):
            center(center_), score(score_), maxRadius(radius_) {};
        sc2::Point2DI center;
        float score;
        float maxRadius;
    } InfluenceSource;

    typedef std::pair<sc2::Point2DI, float> InfluenceTile;

    class InfluenceMap {
        public:
        InfluenceMap(sc2::Point2D center_, float maxRadius_);

        // set a new center/radius
        void setCenter(sc2::Point2D newCenter);
        void setCenter(sc2::Point2D newCenter, float newRadius);

        // add a new influence source
        void addSource(sc2::Point2D center, float score, float radius);

        // preferred way of adding an influence source
        void addSource(const sc2::Unit* u, float score);

        // reset list of sources
        void clearSources();

        void resetInfluenceScores();

        // updates influence map
        void propagate();

        // combines setCenter and propagate
        void update(sc2::Point2D newCenter);

        // given a target, find the safest waypoint that gets us closer to said target
        sc2::Point2D getOptimalWaypoint(sc2::Point2D target);

        // given a target, find the safest waypoint regardless of whether or not it gets us closer to target
        sc2::Point2D getSafeWaypoint(sc2::Point2D target);

        // displays influence map
        // note: might not work if there are multiple units trying to print their own maps at the same time
        void debug();

        protected:
        std::vector<InfluenceSource> sources;
        std::vector<InfluenceTile> localRegion;
        sc2::Point2DI center;
        float maxRadius;
    };
} // end namespace Monte