#pragma once

#include <vector>
#include <sc2api/sc2_common.h>
#include "api.h"


namespace Monte {
    typedef struct InfluenceSource_s_t {
        InfluenceSource_s_t(sc2::Point2D center_, float score_, float radius_):
            center(center_), score(score_), maxRadius(radius_) {};
        sc2::Point2D center;
        float score;
        float maxRadius;
    } InfluenceSource;

    class InfluenceMap {
        public:
        InfluenceMap();

        void initialize();

        // set sources to be all ground/air weapons of a specific race
        void setGroundMap();
        void setAirMap();

        // add a new influence source
        void addSource(sc2::Point2D center, float score, float radius);

        // reset list of sources
        void clearSources();

        // resets all scores to zero
        void resetInfluenceScores();

        // updates influence map
        void propagate();

        // given a target, find the safest waypoint that gets us closer to said target
        sc2::Point2D getOptimalWaypoint(sc2::Point2D pos, sc2::Point2D target);

        // given a target, find the safest waypoint
        sc2::Point2D getSafeWaypoint(sc2::Point2D pos);

        // displays influence map
        void debug();

        protected:
        std::vector<InfluenceSource> sources;
        std::vector<std::vector<float>> map;
        int mapWidth, mapHeight;

    };
} // end namespace Monte