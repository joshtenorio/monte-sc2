#include "ProductionManager.h"

void ProductionManager::OnStep(){
    tryBuildRefinery();
    TryBuildBarracks();
    TryBuildSupplyDepot();

    // TODO: make this into function?
    const Unit* cc = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
    if(gInterface->observation->GetMinerals() >= 50 && cc->orders.size() == 0){
        gInterface->actions->UnitCommand(cc, ABILITY_ID::TRAIN_SCV);
        std::cout << "CC idle & minerals available, training SCV" << std:: endl;
    }
}

void ProductionManager::OnGameStart(){
    //strategy.initialize();
    // do i need to do other stuff here?
}

void ProductionManager::setMapper(Mapper* map_){
    map = map_;
}

bool ProductionManager::TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type){
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
        // get a random location to build building within a 15x15 region where the scv is at a corner
        float rx = GetRandomScalar();
        float ry = GetRandomScalar();
        gInterface->actions->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
        return true;
    }
    else if (ability_type_for_structure == ABILITY_ID::BUILD_REFINERY){
        // we are building a refinery!
        // this needs to get fixed once we have multiple bases,
        // since this implementation will only work for main base
        if(map->getStartingExpansion().gasGeysers.size() <= 0){
            std::cout << "no refinery at main\n";
            return false;
        }
        const Unit* gas = map->getStartingExpansion().gasGeysers.front();
        gInterface->actions->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            gas);
        return true;
    }
    else return false;
}

bool ProductionManager::TryBuildSupplyDepot(){

    // if not supply capped, dont build supply depot
    if(gInterface->observation->GetFoodUsed() <= gInterface->observation->GetFoodCap() - 2)
        return false;
    
    // else, try and build depot using a random scv
    return TryBuildStructure (ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool ProductionManager::TryBuildBarracks() {
    // check for depot and if we have 5 barracks already
    if(API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1 ||
        API::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 8) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

// temporary: only build one refinery
bool ProductionManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || API::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 1 ||
        gInterface->observation->GetMinerals() < 75) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}