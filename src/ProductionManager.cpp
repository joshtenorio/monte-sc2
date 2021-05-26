#include "ProductionManager.h"

void ProductionManager::OnStep(){

    // if prod queue is empty, fill it with stuff from 
    if(productionQueue.empty()) fillQueue();

    if(productionQueue.empty() && strategy->peekNextPriorityStep() == STEP_NULL){
        tryBuildRefinery();
        TryBuildBarracks();
    }

    // build a supply depot if needed regardless of whether or not we have a queue
    TryBuildSupplyDepot();

    // act on items in the queue
    parseQueue();

    //std::cout << "prod queue size: " << productionQueue.size() << std::endl;

    // building manager
    bm.OnStep();

    // TODO: make this into function?
    const Unit* cc = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER)).front();
    if(gInterface->observation->GetMinerals() >= 50 && cc->orders.size() == 0)
        gInterface->actions->UnitCommand(cc, ABILITY_ID::TRAIN_SCV);
    
    // train marines
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) > 0){
        sc2::Units barracks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
        for(auto& b : barracks)
            if(gInterface->observation->GetMinerals() >= 50 && b->orders.size() == 0)
                gInterface->actions->UnitCommand(b, sc2::ABILITY_ID::TRAIN_MARINE);
    }
    // FIXME: this is very inconsistent, sometimes a barracks w/o reactor will queue 2 marines, sometimes a barracks w/ reactor will only queue 1 marine
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR) > 0){
        sc2::Units reactors = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR));
        for(auto& r : reactors)
            if(gInterface->observation->GetMinerals() >= 50 && r->orders.size() == 0)
                gInterface->actions->UnitCommand(r, sc2::ABILITY_ID::TRAIN_MARINE);
    }



}

void ProductionManager::OnGameStart(){
    strategy->initialize();
}

void ProductionManager::OnBuildingConstructionComplete(const Unit* building_){
    bm.OnBuildingConstructionComplete(building_);

    // if it is a refinery then increment the refinery count for that expansion
    if(building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERY ||
        building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
        gInterface->map->getClosestExpansion(building_->pos)->numFriendlyRefineries++;

    // loop through production queue to check which Step corresponds to
    // the structure that just finished
    for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
        if(building_->unit_type.ToType() == API::abilityToUnitTypeID((*itr).ability))
            itr = productionQueue.erase(itr);
        else ++itr;
    }

}

void ProductionManager::OnUnitCreated(const sc2::Unit* unit_){
    // only run this after the 50th loop
    // necessary to avoid crashing when the main cc is created
    if(gInterface->observation->GetGameLoop() > 50 && unit_->tag != 0)
        bm.OnUnitCreated(unit_);
    
    // loop through production queue to check which Step corresponds to the unit
    // that just finished (make sure that unit created is a unit not a structure)
    if(API::isStructure(unit_->unit_type.ToType())) return;
    for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
        if(unit_->unit_type.ToType() == API::abilityToUnitTypeID((*itr).ability))
            itr = productionQueue.erase(itr);
        else ++itr;
    }
}

void ProductionManager::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    // TODO: remove relevant thing from production queue
    // requires upgrade to ability function in api.cpp
    
}

void ProductionManager::OnUnitDestroyed(const sc2::Unit* unit_){
    bm.OnUnitDestroyed(unit_);
}

void ProductionManager::fillQueue(){
    // first, make sure nothing in the queue is blocking
    for(auto& s : productionQueue)
        if(s.blocking) return;

    Step s = strategy->peekNextPriorityStep();
    if(!(s == STEP_NULL)) productionQueue.emplace_back(strategy->popNextPriorityStep());
    else return; // technically redundant since if step is null, the while loop won't run anyways

    // accumulate all of the non-blocking steps until a blocking step is accumulated or null step is reached
    while(!s.blocking && !(s == STEP_NULL)){
        s = strategy->peekNextPriorityStep();
        if(!(s == STEP_NULL)) productionQueue.emplace_back(strategy->popNextPriorityStep());
        else break; // if step is null, stop filling the queue
        // TODO: for now this is break just in case i want to do stuff after while loop
    }
}

// TODO: move the switch statement into api.cpp and have that return a char,
//          then depending on the char pass s to buildStructure(), etc.
void ProductionManager::parseQueue(){
    for(auto& s : productionQueue){
        // skip step if we don't have enough supply for step
        // if s.reqSupply is negative, disregard it
        if(gInterface->observation->GetFoodUsed() < s.reqSupply && s.reqSupply > 0) continue;

        if(API::parseStep(s) == ABIL_BUILD) buildStructure(s);
        else if(API::parseStep(s) == ABIL_TRAIN) trainUnit(s);
        else if(API::parseStep(s) == ABIL_RESEARCH) researchUpgrade(s);
        else if(API::parseStep(s) == ABIL_MORPH) morphStructure(s);
    }
}

void ProductionManager::buildStructure(Step s){
    sc2::ABILITY_ID ability = s.ability;
    switch(ability){
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
            TryBuildSupplyDepot();
            break;
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            TryBuildBarracks();
            break;
        case sc2::ABILITY_ID::BUILD_REFINERY:
            tryBuildRefinery();
            break;
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
            tryBuildCommandCenter();
            break;
        default:
            bm.TryBuildStructure(ability);
            break;
    }
}

void ProductionManager::trainUnit(Step s){

}

void ProductionManager::researchUpgrade(Step s){

}

void ProductionManager::morphStructure(Step s){

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

// FIXME: this only builds two refineryies, we need to update this at some point
bool ProductionManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || API::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 2 ||
        gInterface->observation->GetMinerals() < 75) return false;

    return bm.TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}

bool ProductionManager::tryBuildCommandCenter(){
    if(gInterface->observation->GetMinerals() < 400) return false;
    return bm.TryBuildStructure(sc2::ABILITY_ID::BUILD_COMMANDCENTER);
}
