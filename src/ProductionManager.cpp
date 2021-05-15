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

    // make sure production facilities are producing stuff here according to Strategy
}

void ProductionManager::OnGameStart(){
    //strategy.initialize();
    // do i need to do other stuff here?
}

void ProductionManager::setMapper(Mapper* map_){
    bm.setMapper(map_);
}

