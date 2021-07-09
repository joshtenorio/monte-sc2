#include "BuildingPlacer.h"

sc2::Point2D BuildingPlacer::findLocation(sc2::ABILITY_ID building, sc2::Point3D around, float freeRadius){

    std::vector<sc2::QueryInterface::PlacementQuery> queries;
    std::vector<bool> results;
    float rx = sc2::GetRandomScalar(), ry = sc2::GetRandomScalar();
    sc2::Point2D loc = sc2::Point2D(around.x + rx * 10.0f, around.y + ry * 10.0f);
    switch(building){
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            // if barracks count == 0, build at ramp
            if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING) == 0)
                return findBarracksLocation();
        case sc2::ABILITY_ID::BUILD_FACTORY:
        case sc2::ABILITY_ID::BUILD_STARPORT:
            queries.emplace_back(sc2::ABILITY_ID::BUILD_BARRACKS, loc);
            queries.emplace_back(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(loc.x + 2.5, loc.y - 0.5));
            results = gInterface->query->Placement(queries);
            for(auto& r : results)
                if(!r){
                    logger.errorInit().withPoint(loc).withStr("not valid location for army building").write();
                    return POINT2D_NULL;
                }
            
            return loc;
            break;
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
            // if depot count < 2, build at ramp
            if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) < 2)
                return findSupplyDepotLocation();
            else goto useDefault;
            break;
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
            return findCommandCenterLocation();
            break;
        case sc2::ABILITY_ID::BUILD_BUNKER:
            if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BUNKER) < 1){
                sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
                if(
                    gInterface->observation->GetGameInfo().map_name == "Blackburn AIE" &&
                    gInterface->map->getNthExpansion(gInterface->map->numOfExpansions() - 1) != nullptr
                    )
                    enemyMain = gInterface->map->getNthExpansion(gInterface->map->numOfExpansions() - 1)->baseLocation;
                    
                sc2::Point2D natural;
                if(gInterface->map->getNthExpansion(1) != nullptr)
                    natural = gInterface->map->getNthExpansion(1)->baseLocation;
                else goto useDefault;
                float dx = enemyMain.x - natural.x, dy = enemyMain.y - natural.y;
                dx /= sqrt(dx*dx + dy*dy);
                dy /= sqrt(dx*dx + dy*dy);
                dx *= 5;
                dy *= 5;
                return sc2::Point2D(natural.x + dx, natural.y + dy);
            }
            else goto useDefault;
        default:
        useDefault:
            // TODO: make this behavior better (ie actually utilise freeRadius)

            // currently, get a random location to build building within a 20x20 region where the scv is at the center
            return sc2::Point2D(around.x + rx * 10.0f, around.y + ry * 10.0f);


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
            return findUnitForAddon(building, near);
            break;
        case sc2::ABILITY_ID::BUILD_REFINERY:{
            /**
            if(gInterface->observation->GetGameLoop() > 70)
                for(int i = 0; i < gInterface->map->numOfExpansions(); i++){
                    Expansion* e = gInterface->map->getNthExpansion(i);
                    if(findRefineryLocation(e) != nullptr){
                        return findRefineryLocation(e);
                    }
                }
            */
            sc2::Units geysers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, sc2::IsGeyser());
            const sc2::Unit* geyserToBuild = nullptr;
            float distSquared = 9000;
            for(auto& g : geysers){
                if(distSquared > sc2::DistanceSquared2D(g->pos, *near) && gInterface->query->Placement(sc2::ABILITY_ID::BUILD_REFINERY, g->pos)){
                    distSquared = sc2::DistanceSquared2D(g->pos, *near);
                    geyserToBuild = g;
                }
            }
            return geyserToBuild;
            //return nullptr;
            break;
        }
        default:
            return nullptr;
        break;
    }
}


sc2::Point2D BuildingPlacer::findBarracksLocation(){
    return gInterface->map->getStartingExpansion().ramp.barracksWithAddonPos;
}

sc2::Point2D BuildingPlacer::findSupplyDepotLocation(){
    size_t numDepots = API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED);
    if(numDepots == 0)
        return gInterface->map->getStartingExpansion().ramp.supplyDepotPoints.front();
    else return gInterface->map->getStartingExpansion().ramp.supplyDepotPoints.back();
}

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
    if(nextExpansion != nullptr)
        return nextExpansion->baseLocation;
    else
        return sc2::Point2D(gInterface->observation->GetStartLocation().x, gInterface->observation->GetStartLocation().y);
}

const sc2::Unit* BuildingPlacer::findUnitForAddon(sc2::ABILITY_ID building, const sc2::Point3D* near){
    switch(building){
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        if(near == nullptr){
            sc2::Units barracks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
            for(auto& b : barracks){
                sc2::AvailableAbilities abilities = gInterface->query->GetAbilitiesForUnit(b, true);
                for(auto a : abilities.abilities){
                    if(
                        a.ability_id == sc2::ABILITY_ID::BUILD_TECHLAB || a.ability_id == sc2::ABILITY_ID::BUILD_REACTOR &&
                        gInterface->query->Placement(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(b->pos.x + 2.5f, b->pos.y - 0.5f))
                    )
                        return b;
                }
            }
            return nullptr;
        }
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        if(near == nullptr){
            sc2::Units factories = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_FACTORY));
            for(auto& b : factories){
                sc2::AvailableAbilities abilities = gInterface->query->GetAbilitiesForUnit(b, true);
                for(auto a : abilities.abilities){
                    if(
                        a.ability_id == sc2::ABILITY_ID::BUILD_TECHLAB || a.ability_id == sc2::ABILITY_ID::BUILD_REACTOR &&
                        gInterface->query->Placement(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(b->pos.x + 2.5f, b->pos.y - 0.5f))
                    )
                        return b;
                }
            }
            return nullptr;
        }
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
            if(near == nullptr){
                sc2::Units starports = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_STARPORT));
                for(auto& b : starports){
                    sc2::AvailableAbilities abilities = gInterface->query->GetAbilitiesForUnit(b, true);
                    for(auto a : abilities.abilities){
                        if(
                            a.ability_id == sc2::ABILITY_ID::BUILD_TECHLAB || a.ability_id == sc2::ABILITY_ID::BUILD_REACTOR &&
                            gInterface->query->Placement(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(b->pos.x + 2.5f, b->pos.y - 0.5f))
                        )
                            return b;
                    }
                }
                return nullptr;
            }
        default:
            return nullptr;
    }
}