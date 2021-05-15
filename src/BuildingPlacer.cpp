#include "BuildingPlacer.h"

sc2::Point2D BuildingPlacer::findLocation(sc2::ABILITY_ID building, const sc2::Point3D* around, float freeRadius){
    switch(building){
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            // if barracks count == 0, build at ramp
            // else
            goto useDefault;
            break;
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
            // if depot count < 2, build at ramp
            // else
            goto useDefault;
            break;
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
            return findCommandCenterLocation();
            break;
        default:
        useDefault:
            // TODO: make this behavior better (ie actually utilise freeRadius)
            /**
            // currently, get a random location to build building within a 20x20 region where the scv is at the center
            float rx = sc2::GetRandomScalar();
            float ry = sc2::GetRandomScalar();
            return sc2::Point2D(around->x + rx * 10.0f, around->y + ry * 10.0f);
            */
            // idea: identify a square whose center is around and side length is 2*freeRadius
            //       create a vector from around to the first unpathable point, and change the location of around
            //       WARNING: might result in infinite looping if there is an unpathable at the edge of freeRadius
            //       temp solution: add a minimum distance to move away from the unpathable
            //       don't work with sc2::Point2D during the looping, just use x and y float vars until a suitable location is found
            //       start at a random point within a 20x20 region where the scv is at the center
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