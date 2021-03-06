#include "BuildingPlacer.h"

BuildingPlacer::BuildingPlacer(){
    logger = Logger("BuildingPlacer");
}

void BuildingPlacer::OnGameStart(){
    int mapHeight = gInterface->observation->GetGameInfo().height;
    int mapWidth = gInterface->observation->GetGameInfo().width;
    logger.infoInit().withStr("map height:").withInt(mapHeight).withStr("max point:").withPoint(gInterface->observation->GetGameInfo().playable_max).write();

    reservedTiles.resize(mapWidth);
    for(auto& r : reservedTiles){
        r.resize(mapHeight);
    }

    for(int i = 0; i < mapWidth; i++){
        for(int j = 0; j < mapHeight; j++){
            if(gInterface->observation->IsPlacable(sc2::Point2D(i, j)))
                reservedTiles[i][j] = false;
            else
                reservedTiles[i][j] = true;
        }
    }

}

void BuildingPlacer::initialize(){
    // reserve expansion location tiles
    // NOTE: this is done in OnStep now
    // TODO: remove this

}

void BuildingPlacer::OnStep(){

    // do tasks that rely on Mapper
    if(gInterface->observation->GetGameLoop() == 60){
        // reserve expansion locations
        for(int n = 0; n < gInterface->map->numOfExpansions(); n++){
            Expansion* e = gInterface->map->getNthExpansion(n);
            if(e == nullptr) continue;

            sc2::Point2D base = e->baseLocation;
            reserveTiles(base, 2.5); // radius of a town hall is 2.5
        }

        // reserve the tiles for bunker
        sc2::Point2D enemyMain = gInterface->observation->GetGameInfo().enemy_start_locations.front();
        if(
            gInterface->observation->GetGameInfo().map_name == "Blackburn AIE" &&
            gInterface->map->getNthExpansion(gInterface->map->numOfExpansions() - 1) != nullptr
            )
            enemyMain = gInterface->map->getNthExpansion(gInterface->map->numOfExpansions() - 1)->baseLocation;
            
        sc2::Point2D natural;
        if(gInterface->map->getNthExpansion(1) != nullptr)
            natural = gInterface->map->getNthExpansion(1)->baseLocation;

        sc2::Point2D bunkerLoc = Monte::getPoint2D(natural, Monte::Vector2D(natural, enemyMain), 5);
        bunkerLoc.x = floorf(bunkerLoc.x) + 0.5;
        bunkerLoc.y = floorf(bunkerLoc.y) + 0.5;

        reserveTiles(bunkerLoc, 1.5);
        
        // generate root for initial army building placement tree
        root = gInterface->map->getStartingExpansion().ramp.barracksWithAddonPos;
        Monte::PlacementTree::addToCache(root);

    } // end if game loop == 60



    // TODO: comment this out when building for ladder
    
    //if(gInterface->observation->GetGameLoop() % 400 == 0)
    //    printDebug();
        
}

sc2::Point2D BuildingPlacer::findLocation(sc2::ABILITY_ID building, sc2::Point3D around, float freeRadius){

    std::vector<sc2::QueryInterface::PlacementQuery> queries;
    std::vector<bool> results;
    float rx = sc2::GetRandomScalar(), ry = sc2::GetRandomScalar();
    sc2::Point2D loc = sc2::Point2D(around.x + rx * 10.0f, around.y + ry * 10.0f);
    switch(building){
        case sc2::ABILITY_ID::BUILD_MISSILETURRET:
            for(int n = 0; n < gInterface->map->numOfExpansions(); n++){
                Expansion* e = gInterface->map->getNthExpansion(n);
                if(e == nullptr) continue;

                if(e->ownership == OWNER_SELF) // build a missile turret at an expansion if we own it and it doesnt have a missile turret already
                    if(API::getClosestNUnits(e->baseLocation, 2, 10, sc2::Unit::Alliance::Self, sc2::UNIT_TYPEID::TERRAN_MISSILETURRET).empty()){
                        Monte::Vector2D direction = Monte::Vector2D(e->baseLocation, e->mineralMidpoint);
                        sc2::Point2D init = Monte::getPoint2D(e->baseLocation, direction, 4);
                        return sc2::Point2D(init.x + rx * 4.0f, init.y + ry * 4.0f);
                    }
            }
            return POINT2D_NULL;
            break;
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            // if barracks count == 0, build at ramp
            if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING) == 0)
                return findBarracksLocation();
        case sc2::ABILITY_ID::BUILD_FACTORY:
        case sc2::ABILITY_ID::BUILD_STARPORT:
            return findArmyBuildingLocation(around);
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

                sc2::Point2D bunkerLoc = Monte::getPoint2D(natural, Monte::Vector2D(natural, enemyMain), 5);
                bunkerLoc.x = floorf(bunkerLoc.x) + 0.5;
                bunkerLoc.y = floorf(bunkerLoc.y) + 0.5;
                return bunkerLoc;
            }
            else goto useDefault;
        default:
        useDefault:
            // TODO: make this behavior better (ie actually utilise freeRadius)

            // if there are no conflicts, get a random location to build building within a 20x20 region where the scv is at the center
            if(!checkConflict(loc, API::getStructureRadiusByAbility(building)))
                return loc;
            else
                return POINT2D_NULL;


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
                }*/
                
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
            break;
        }
        default:
            return nullptr;
        break;
    }
}


void BuildingPlacer::reserveTiles(sc2::Point2D center, float radius){
    int xMin = center.x - radius, xMax = center.x + radius;
    int yMin = center.y - radius, yMax = center.y + radius;

    for(int x = xMin; x < xMax; x++){
        for(int y = yMin; y < yMax; y++){
            reservedTiles[x][y] = true;
        }
    }
}

void BuildingPlacer::freeTiles(sc2::Point2D center, float radius){
    int xMin = center.x - radius, xMax = center.x + radius;
    int yMin = center.y - radius, yMax = center.y + radius;

    for(int x = xMin; x < xMax; x++){
        for(int y = yMin; y < yMax; y++){
            reservedTiles[x][y] = false;
        }
    }
}

bool BuildingPlacer::checkConflict(sc2::Point2D center, float radius){
    int xMin = center.x - radius, xMax = center.x + radius;
    int yMin = center.y - radius, yMax = center.y + radius;

    for(int x = xMin; x < xMax; x++){
        for(int y = yMin; y < yMax; y++){
            if(reservedTiles[x][y]){
                //logger.warningInit().withStr("trying to build a structure with radius").withFloat(radius).withStr("at").withPoint(center);
                //logger.withStr("but space already reserved").write();
                return true;
            }
        }
    }
    return false;
}

void BuildingPlacer::validateTiles(){
    // make sure reserved tiles are correct
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
    /**
    if(e->numFriendlyRefineries == 0)
        return e->gasGeysers.front();
    else if(e->numFriendlyRefineries == 1)
        return e->gasGeysers.back();
    else
        return nullptr;
    */
   // FIXME: update mapper so that we can use the above code instead of doing this below

    sc2::Units geysers = API::getClosestNUnits(e->baseLocation, 2, 11, sc2::Unit::Alliance::Neutral, sc2::IsGeyser());

    if(geysers.empty()) return nullptr;

    if(e->numFriendlyRefineries == 0){
        logger.infoInit().withStr("0 friendly").write();
        gInterface->debug->debugSphereOut(geysers.front()->pos, 3);
        gInterface->debug->sendDebug();
        return geysers.front();
    }
        
    else if(e->numFriendlyRefineries == 1){
        logger.infoInit().withStr("1 friendly").write();
        gInterface->debug->debugSphereOut(geysers.back()->pos, 3);
        gInterface->debug->sendDebug();
        return geysers.back();
    }
        
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

sc2::Point2D BuildingPlacer::findArmyBuildingLocation(sc2::Point3D around){
    sc2::Point2D output = Monte::PlacementTree::findPlacement(root, reservedTiles, 5, PT_DIR_NULL, 2, 2, true);

    // if output is null or the tree is full, find a new root until happy
    // also retry a maximum of 3 times per step
    int iterations = 0;
    while ((output == PT_TREE_FULL || output == PT_NODE_NULL) && iterations < 3){

        // clear cache
        Monte::PlacementTree::clearCache();

        // generate new root
        float rx = sc2::GetRandomScalar(), ry = sc2::GetRandomScalar();
        root = sc2::Point2D(around.x + rx*10.0, around.y + rx*10.0);
        output = Monte::PlacementTree::findPlacement(root, reservedTiles, 2, PT_DIR_NULL, 2, 2, true);
        logger.warningInit().withStr("Placement Tree is full/null, trying again with new root").write();
        iterations++;
    }

    return output;
}

const sc2::Unit* BuildingPlacer::findUnitForAddon(sc2::ABILITY_ID building, const sc2::Point3D* near){
    switch(building){
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        if(near == nullptr){
            sc2::Units barracks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
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
            sc2::Units factories = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_FACTORY));
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
                sc2::Units starports = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_STARPORT));
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

void BuildingPlacer::printDebug(){
    for(int x = 0; x < reservedTiles.size(); x++){
        for(int y = 0; y < reservedTiles[x].size(); y++){
            if(reservedTiles[x][y])
                gInterface->debug->debugDrawTile(sc2::Point3D(x, y, gInterface->observation->TerrainHeight(sc2::Point2D(x,y))), sc2::Colors::Red);
            else
                gInterface->debug->debugDrawTile(sc2::Point3D(x, y, gInterface->observation->TerrainHeight(sc2::Point2D(x,y))), sc2::Colors::Green);
        }
    }
    gInterface->debug->sendDebug();
}