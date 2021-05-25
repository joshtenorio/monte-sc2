#include "ProductionManager.h"

void ProductionManager::OnStep(){

    tryBuildRefinery();
    TryBuildBarracks();
    TryBuildSupplyDepot();

    // building manager
    bm.OnStep();

    // TODO: make this into function?
    const Unit* cc = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
    if(gInterface->observation->GetMinerals() >= 50 && cc->orders.size() == 0){
        gInterface->actions->UnitCommand(cc, ABILITY_ID::TRAIN_SCV);
        std::cout << "CC idle & minerals available, training SCV" << std:: endl;
    }
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 0){
        sc2::Units barracks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
        for(auto& b : barracks)
            if(gInterface->observation->GetMinerals() >= 50 && b->orders.size() == 0)
                gInterface->actions->UnitCommand(b, sc2::ABILITY_ID::TRAIN_MARINE);
    }

    // FIXME: this is very inconsistent, sometimes a barracks w/o reactor will queue 2 marines, sometimes a barracks w/ reactor will only queue 1 marine and sometimes it will queue 2
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR) > 0){
        sc2::Units reactors = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR));
        for(auto& r : reactors)
            if(gInterface->observation->GetMinerals() >= 50 && r->orders.size() == 0)
                gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::TRAIN_MARINE);
    }


    // TODO: make sure production facilities are producing stuff here according to Strategy
}

void ProductionManager::OnGameStart(){
    //strategy.initialize();
    // do i need to do other stuff here?
}

void ProductionManager::OnUnitDestroyed(const sc2::Unit* unit_){
    bm.OnUnitDestroyed(unit_);
}

void ProductionManager::OnUnitCreated(const sc2::Unit* unit_){
    // only run this after the 50th loop
    // necessary to avoid crashing when the main cc is created
    if(gInterface->observation->GetGameLoop() > 50 && unit_->tag != 0)
        bm.OnUnitCreated(unit_);
}

void ProductionManager::OnBuildingConstructionComplete(const Unit* building_){
    bm.OnBuildingConstructionComplete(building_);

    // if it is a refinery then increment the refinery count for that expansion
    if(building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERY ||
        building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
        gInterface->map->getClosestExpansion(building_->pos)->numFriendlyRefineries++;

}

bool ProductionManager::TryBuildSupplyDepot(){

    // if not supply capped, dont build supply depot
    if(gInterface->observation->GetFoodUsed() <= gInterface->observation->GetFoodCap() - 2 || gInterface->observation->GetMinerals() < 100)
        return false;
    
    // else, try and build depot using a random scv
    return bm.TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool ProductionManager::TryBuildBarracks() {
    // check for depot and if we have 5 barracks already
    if(API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) + API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) < 1 ||
        API::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 8) return false;
    return bm.TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

// FIXME: this only builds one refinery, we need to update this at some point
bool ProductionManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || API::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 2 ||
        gInterface->observation->GetMinerals() < 75) return false;

    return bm.TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}
