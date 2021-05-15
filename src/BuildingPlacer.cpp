#include "BuildingPlacer.h"

sc2::Point2D BuildingPlacer::findLocation(sc2::ABILITY_ID building, const sc2::Point3D* around, float freeRadius){
    switch(building){
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            // if barracks count == 0, build at ramp
            // else
            goto useDefault;
            break;
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
            // same logic as build barracks but different numbers obviously
            goto useDefault;
            break;
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
            return findCommandCenterLocation();
            break;
        default:
        useDefault:
            // TODO: make this behavior better (ie actually utilise freeRadius)
            // currently, get a random location to build building within a 20x20 region where the scv is at the center
            float rx = sc2::GetRandomScalar();
            float ry = sc2::GetRandomScalar();
            return Point2D(around->x + rx * 10.0f, around->y + ry * 10.0f);
            break;
    }
}

const sc2::Unit* BuildingPlacer::findUnit(sc2::ABILITY_ID building, const sc2::Point3D* near){
    switch(building){
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
            return nullptr; // for now
            break;
        case sc2::ABILITY_ID::BUILD_REFINERY:{
            Expansion* e = gInterface->map->getClosestExpansion(*near);
            return findRefineryLocation(e);
            break;
        }
        default:
            return nullptr; 
        break;
    }
}

/**
sc2::Point2D BuildingPlacer::findBarracksLocation(){

}

sc2::Point2D BuildingPlacer::findSupplyDepotLocation(){

}
*/
const sc2::Unit* BuildingPlacer::findRefineryLocation(Expansion* e){
    if(e->numFriendlyRefineries == 0)
        return e->gasGeysers.front();
    else if(e->numFriendlyRefineries == 1)
        return e->gasGeysers.back();
    else
        return nullptr;
}

sc2::Point2D BuildingPlacer::findCommandCenterLocation(){
    Expansion* nextExpansion = gInterface->map->getNextExpansion();
    return nextExpansion->baseLocation;
}