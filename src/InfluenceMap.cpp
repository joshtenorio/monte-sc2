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
    return sc2::Point2D(0,0);
}

sc2::Point2D InfluenceMap::getSafeWaypoint(sc2::Point2D target){
    return sc2::Point2D(0,0);
}

void InfluenceMap::debug(){
    
    gInterface->debug->sendDebug();
}



} // end namespace Monte