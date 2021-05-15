#include "BuildingManager.h"

void BuildingManager::OnStep(){
    tryBuildRefinery();
    TryBuildBarracks();
    TryBuildSupplyDepot();
}

void BuildingManager::OnUnitDestroyed(const sc2::Unit* unit_){
    
}

bool BuildingManager::TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type){
    // if unit is already building structure of this type, do nothing
    const Unit* unit_to_build = nullptr;
    Units units = gInterface->observation->GetUnits(Unit::Alliance::Self);
    for(const auto& unit : units){
        for (const auto& order : unit->orders){
            if(order.ability_id == ability_type_for_structure) // checks if structure is already being built
                return false;
        }
        // get SCV to build structure
        if(unit->unit_type == unit_type)
            unit_to_build = unit;
    }

    if(ability_type_for_structure != ABILITY_ID::BUILD_REFINERY){

        sc2::Point2D loc = bp.findLocation(ability_type_for_structure, &(unit_to_build->pos));
        gInterface->actions->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            loc);
        gInterface->wm->getWorker(unit_to_build)->job = JOB_BUILDING;
        return true;
    }
    else if (ability_type_for_structure == ABILITY_ID::BUILD_REFINERY){
        // we are building a refinery!
        // this needs to get fixed once we have multiple bases,
        // since this implementation will only work for main base
        if(gInterface->map->getStartingExpansion().gasGeysers.size() <= 0)
            return false;
        
        const sc2::Unit* gas = bp.findUnit(ABILITY_ID::BUILD_REFINERY, &(unit_to_build->pos));
        gInterface->actions->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            gas);
        gInterface->wm->getWorker(unit_to_build)->job = JOB_BUILDING_GAS;
        return true;
    }
    else return false;
}

bool BuildingManager::TryBuildSupplyDepot(){

    // if not supply capped, dont build supply depot
    if(gInterface->observation->GetFoodUsed() <= gInterface->observation->GetFoodCap() - 2)
        return false;
    
    // else, try and build depot using a random scv
    return TryBuildStructure (ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool BuildingManager::TryBuildBarracks() {
    // check for depot and if we have 5 barracks already
    if(API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1 ||
        API::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 8) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

// temporary: only build one refinery
bool BuildingManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || API::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 1 ||
        gInterface->observation->GetMinerals() < 75) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}