#include "InfluenceMap.h"

namespace Monte {

InfluenceMap::InfluenceMap(){

}

void InfluenceMap::initialize(){
    mapWidth = gInterface->observation->GetGameInfo().width;
    mapHeight = gInterface->observation->GetGameInfo().height;
    map.resize(mapWidth);
    for(auto& r : map){
        r.resize(mapHeight);
    }
    
    
}

void InfluenceMap::setGroundMap(){
    clearSources();
    sc2::Units enemies = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy);
    sc2::UnitTypes types = gInterface->observation->GetUnitTypeData();
    for(auto& e : enemies){
        if(!e) continue;
        if(e->is_building) continue;
        std::vector<sc2::Weapon> weapons = types[e->unit_type].weapons;
        for(auto& w : weapons){
            if(w.type == sc2::Weapon::TargetType::Ground || w.type == sc2::Weapon::TargetType::Any)
                addSource(e->pos, w.damage_, w.range);
        }
    }
}
void InfluenceMap::setAirMap(){
    clearSources();
    sc2::Units enemies = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy);
    sc2::UnitTypes types = gInterface->observation->GetUnitTypeData();
    for(auto& e : enemies){
        if(!e) continue;
        // TODO: add if missile turret/spore crawler
        if(e->is_building) continue;
        std::vector<sc2::Weapon> weapons = types[e->unit_type].weapons;
        for(auto& w : weapons){
            if(w.type == sc2::Weapon::TargetType::Air || w.type == sc2::Weapon::TargetType::Any)
                addSource(e->pos, w.damage_, w.range);
        }
    }
}

void InfluenceMap::addSource(sc2::Point2D center, float score, float radius){
    InfluenceSource source(center, score, radius);
    sources.emplace_back(source);
}

void InfluenceMap::clearSources(){
    sources.clear();
}

void InfluenceMap::resetInfluenceScores(){
    for(int i = 0; i < mapWidth; i++){
        for(int j = 0; j < mapHeight; j++){
            map[i][j] = 0.0;
        }
    }
}

void InfluenceMap::propagate(){
    resetInfluenceScores();
    for(auto& s : sources){
        int xMin = s.center.x - s.maxRadius, xMax = s.center.x + s.maxRadius;
        int yMin = s.center.y - s.maxRadius, yMax = s.center.y + s.maxRadius;
        xMin = xMin < 0 ? 0 : xMin;
        xMax = xMax > mapWidth ? mapWidth : xMax;
        yMin = yMin < 0 ? 0 : yMin;
        yMax = yMax > mapHeight ? mapHeight : yMax;
        for(int x = xMin; x < xMax; x++){
            for(int y = yMin; y < yMax; y++){
                if(sc2::DistanceSquared2D(s.center, sc2::Point2D(x,y)) <= s.maxRadius*s.maxRadius){
                    map[x][y] = s.score;
                }
            }
        }
    } // end for s : sources
}

sc2::Point2D InfluenceMap::getOptimalWaypoint(sc2::Point2D pos, sc2::Point2D target){
    sc2::Point2DI center(pos);
    sc2::Point2DI waypoint(center);
    for(int x = center.x-1; x < center.x+1; x++){
        for(int y = center.y-1; y < center.y+1; y++){
            // curr score < waypoint score and current gets us closer to target
            if(map[x][y] < map[waypoint.x][waypoint.y] && sc2::DistanceSquared2D(sc2::Point2D(x,y), target) < sc2::DistanceSquared2D(pos, target))
                waypoint = sc2::Point2DI(x,y);
        }
    }
    return sc2::Point2D(waypoint.x, waypoint.y);
}

sc2::Point2D InfluenceMap::getSafeWaypoint(sc2::Point2D pos){
    sc2::Point2DI center(pos);
    sc2::Point2DI waypoint(center);
    for(int x = center.x-1; x < center.x+1; x++){
        for(int y = center.y-1; y < center.y+1; y++){
            // curr score < waypoint score and current gets us closer to target
            if(map[x][y] < map[waypoint.x][waypoint.y])
                waypoint = sc2::Point2DI(x,y);
        }
    }
    return sc2::Point2D(waypoint.x, waypoint.y);
}

void InfluenceMap::debug(){

}

} // end namespace Monte