#include "ProductionManager.h"

void ProductionManager::OnStep(){

    // building manager
    bm.OnStep();
    // testing global functionality for worker manager

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


    // make sure production facilities are producing stuff here according to Strategy
}

void ProductionManager::OnGameStart(){
    //strategy.initialize();
    // do i need to do other stuff here?
}

void ProductionManager::OnUnitDestroyed(const sc2::Unit* unit_){
    bm.OnUnitDestroyed(unit_);
}

