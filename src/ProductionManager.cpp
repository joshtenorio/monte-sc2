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

    switch(building_->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_REFINERY:
        case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
            gInterface->map->getClosestExpansion(building_->pos)->numFriendlyRefineries++;
            break;
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
        case sc2::UNIT_TYPEID::TERRAN_FACTORY:
        case sc2::UNIT_TYPEID::TERRAN_STARPORT:
            ArmyBuilding a;
            a.addon = nullptr;
            a.addonTag = -1; // TODO: put -1 in some define somewhere for unused tag
            a.building = building_;
            a.buildingTag = building_->tag;
            a.order = ARMYBUILDING_UNUSED;
            armyBuildings.emplace_back(a);
            break;
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_REACTOR:
        case sc2::UNIT_TYPEID::TERRAN_TECHLAB:
            // search through armybuildings and see whose
            // addon_tag matches building_
            for (auto& a : armyBuildings){
                if(a.building->add_on_tag == building_->tag){
                    a.addon = building_;
                    a.addonTag = building_->tag;
                    break;
                }
            }
            break;
    }
    
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
    // remove army building or addon from army building if applicable
    for(auto itr = armyBuildings.begin(); itr != armyBuildings.end(); ){
        if(unit_->tag == (*itr).buildingTag){
            itr = armyBuildings.erase(itr);
            break;
        }
        else if(unit_->tag == (*itr).addonTag){
            (*itr).addon = nullptr;
            (*itr).addonTag = -1;
            break;
        }
        else ++ itr;
    }
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

void ProductionManager::swapAddon(ArmyBuilding* b1, ArmyBuilding* b2){
    // lift both buildings

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
    tryTrainUnit(s.ability);
}

void ProductionManager::researchUpgrade(Step s){

}

void ProductionManager::morphStructure(Step s){

}

// TODO: this is an interesting challenge
// if i want to use this for both building units while Strategy is active and after it is done,
// it needs to be able to differentiate between the two, because if the unit to train is part of
// a strategy, it should only build one, but if strategy is already done then it should build as much
// as possible
bool ProductionManager::tryTrainUnit(sc2::ABILITY_ID unitToTrain){
    switch(unitToTrain){
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        case sc2::ABILITY_ID::TRAIN_RAVEN:
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
        break;
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        case sc2::ABILITY_ID::TRAIN_HELLION:
        case sc2::ABILITY_ID::TRAIN_HELLBAT:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_THOR:
        break;
        case sc2::ABILITY_ID::TRAIN_MARINE:
        case sc2::ABILITY_ID::TRAIN_REAPER:
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::TRAIN_GHOST:
        
        break;
        case sc2::ABILITY_ID::TRAIN_SCV:
        default:
        break;
    }
    return true; // placeholder so compiling will work
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

bool ProductionManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || gInterface->observation->GetMinerals() < 75) return false;

    return bm.TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}

bool ProductionManager::tryBuildCommandCenter(){
    if(gInterface->observation->GetMinerals() < 400) return false;
    return bm.TryBuildStructure(sc2::ABILITY_ID::BUILD_COMMANDCENTER);
}