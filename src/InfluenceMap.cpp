#include "InfluenceMap.h"

namespace Monte {

InfluenceMap::InfluenceMap(sc2::Point2D center_, float maxRadius_){
    center = sc2::Point2DI(center_);
    maxRadius = maxRadius_;
    int xMin = center.x - maxRadius, xMax = center.x + maxRadius;
    int yMin = center.y - maxRadius, yMax = center.y + maxRadius;

    // initialize local region
    for(int x = xMin; x < xMax; x++){
        for(int y = yMin; y < yMax; y++){
            if(sc2::Distance2D(sc2::Point2D(x,y), center_) <= maxRadius)
                localRegion.emplace_back(std::make_pair(sc2::Point2DI(x,y), 0.0));
        }
    }
}

void InfluenceMap::setCenter(sc2::Point2D newCenter){
    center = sc2::Point2DI(newCenter);
    int xMin = center.x - maxRadius, xMax = center.x + maxRadius;
    int yMin = center.y - maxRadius, yMax = center.y + maxRadius;

    // initialize local region
    localRegion.clear();
    for(int x = xMin; x < xMax; x++){
        for(int y = yMin; y < yMax; y++){
            if(sc2::Distance2D(sc2::Point2D(x,y), center) <= maxRadius)
                localRegion.emplace_back(std::make_pair(sc2::Point2DI(x,y), 0.0));
        }
    }
}

void InfluenceMap::setCenter(sc2::Point2D newCenter, float newRadius){
    maxRadius = newRadius;
    setCenter(newCenter);
}

void InfluenceMap::addSource(sc2::Point2D center, float score, float radius){
    sources.emplace_back(InfluenceSource(center, score, radius));
}

void InfluenceMap::addSource(const sc2::Unit* u, float score){
    // get weapon range, see #cpp in sc2ai discord for info from soupcatcher
    addSource(u->pos, score, 6.0); // TODO: the 6 is a placeholder
}

void InfluenceMap::clearSources(){
    sources.clear();
}

void InfluenceMap::resetInfluenceScores(){
    for(auto& t : localRegion)
        t.second = 0.0;
}

void InfluenceMap::propagate(){
    for(auto& t : localRegion){
        t.second = 0.0; // reset influence score
        for(auto& s : sources){
            if(sc2::Distance2D(s.center, t.first) <= s.maxRadius){
                t.second++;
            }
        } // end s : sources
    } // end t: localRegion
}

void InfluenceMap::update(sc2::Point2D newCenter){
    setCenter(newCenter);
    propagate();
}

sc2::Point2D InfluenceMap::getOptimalWaypoint(sc2::Point2D target){
    // 1. get all moore neighbors (should be 8 in total)
    // 2. disregard neighbors, that if we were to move to we would be further from the target
    std::vector<InfluenceTile> neighbors;
    for(auto& t : localRegion){
        // a tile is a neighbor if the squared distance to center is either 1 (in von neumann neighborhood only)
        // or 2 (in moore neighborhood)
        if(sc2::DistanceSquared2D(t.first, center) == 1 || sc2::DistanceSquared2D(t.first, center) == 2){
            // only append if tile is closer to target than center is
            if(sc2::DistanceSquared2D(target, t.first) < sc2::DistanceSquared2D(target, center))
                neighbors.append(t);
        }
    }
    // 3. get the neighbor with lowest score, and return its location
    // this shouldn't be possible, but just in case something goes wrong
    if(neighbors.empty())
        return sc2::Point2D(0,0);
    InfluenceTile waypoint = neighbors.front();
    for(auto& n : neighbors){
        if(n.second < waypoint.second) waypoint = n;
    }
    return waypoint.first;
}

sc2::Point2D InfluenceMap::getSafeWaypoint(sc2::Point2D target){
    return sc2::Point2D(0,0);
}

void InfluenceMap::debug(){
    
    gInterface->debug->sendDebug();
}



} // end namespace Monte