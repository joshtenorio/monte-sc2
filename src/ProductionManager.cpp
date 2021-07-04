#include "ProductionManager.h"

void ProductionManager::OnStep(){

    // fill queue with steps
    fillQueue();

    // build a supply depot if needed
    TryBuildSupplyDepot();

    // if queue still empty and strategy is done, just do normal macro stuff
    if(productionQueue.empty() && strategy->peekNextPriorityStep() == STEP_NULL){
        TryBuildBarracks();
        tryBuildRefinery();
        tryBuildCommandCenter();

        // check on army buildings
        for(auto& a : armyBuildings){
            // if army building is unused, give it an order
            if(a.order == ARMYBUILDING_UNUSED){
                switch(a.building->unit_type.ToType()){
                    case sc2::UNIT_TYPEID::TERRAN_BARRACKS: // TODO: if barracks has a tech lab, should it train marauders instead?
                        a.order = sc2::ABILITY_ID::TRAIN_MARINE;
                    break;
                    case sc2::UNIT_TYPEID::TERRAN_FACTORY:
                    break;
                    case sc2::UNIT_TYPEID::TERRAN_STARPORT:
                        a.order = sc2::ABILITY_ID::TRAIN_MEDIVAC;
                    break;
                }
            }

            // if army building is a barrack, then place a reactor if possible
            if(a.building->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKS && a.addon == nullptr){
                if(a.building->orders.empty())
                    gInterface->actions->UnitCommand(a.building, sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS);
            }
            
        } // end for loop
    } // end if prod queue empty

    if(gInterface->observation->GetGameLoop() % 400 == 0)
        std::cout << "prod queue size: " << productionQueue.size() << std::endl;

    // act on items in the queue
    parseQueue();

    // handle ArmyBuildings that have an order set
    handleArmyBuildings();

    // building manager
    bm.OnStep();

    // TODO: make this into function?
    Units ccs = gInterface->observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    for(auto& cc : ccs)
        if(gInterface->observation->GetMinerals() >= 50 && cc->orders.empty())
            gInterface->actions->UnitCommand(cc, ABILITY_ID::TRAIN_SCV);
    
}

void ProductionManager::OnGameStart(){
    strategy->initialize();
}

void ProductionManager::OnBuildingConstructionComplete(const Unit* building_){
    bm.OnBuildingConstructionComplete(building_);

    int index = -1; // for removing items from prod queue
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
            // search through armybuildings and see whose
            // addon_tag matches building_
            for (auto& ab : armyBuildings){
                if(ab.building->add_on_tag == building_->tag){
                    ab.addon = building_;
                    ab.addonTag = building_->tag;
                    break;
                }
            }
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            std::cout << "i am a addon or cc morph and i made it here\n";
            // search through production queue to remove 
            /**
            for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
                if(API::unitTypeIDToAbilityID(building_->unit_type.ToType()) == (*itr).ability)
                    itr = productionQueue.erase(itr);
                else ++itr;
            }*/
            for(int i = 0; i < productionQueue.size(); i++){
                if(productionQueue[i].ability == API::unitTypeIDToAbilityID(building_->unit_type.ToType())){
                    printf("buildingcomplete(morph): removing %d from prod queue\n", productionQueue[i].ability);
                    index = i;
                    break;
                }
            }
            if(index >= 0)
                productionQueue.erase(productionQueue.begin() + index);
            return;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
            gInterface->map->getClosestExpansion(building_->pos)->ownership = OWNER_SELF;
            break;
    }
    
    // loop through production queue to check which Step corresponds to
    // the structure that just finished, doesn't apply to morphs
    /**
    for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
        if(building_->unit_type.ToType() == API::abilityToUnitTypeID((*itr).ability))
            itr = productionQueue.erase(itr);
        else ++itr;
    }
    */
    index = -1;
    for(int i = 0; i < productionQueue.size(); i++){
        if(building_->unit_type.ToType() == API::abilityToUnitTypeID(productionQueue[i].ability)){
            index = i;
            break;
        }
    }
    if(index >= 0)
        productionQueue.erase(productionQueue.begin() + index);
}

void ProductionManager::OnUnitCreated(const sc2::Unit* unit_){
    // only run this after the 50th loop
    // necessary to avoid crashing when the main cc is created
    if(gInterface->observation->GetGameLoop() > 50 && unit_->tag != 0)
        bm.OnUnitCreated(unit_);
    
    // loop through production queue to check which Step corresponds to the unit
    // that just finished and make sure that unit created is a unit, not a structure
    if(API::isStructure(unit_->unit_type.ToType()) || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV) return;
    int index = -1;
    for(int i = 0; i < productionQueue.size(); i++){
        if(unit_->unit_type.ToType() == API::abilityToUnitTypeID(productionQueue[i].ability)){
            index = i;
            printf("unitcreated: removing %d from prod queue\n", productionQueue[i].ability);
            break;
        }
    }
    if(index >= 0)
        productionQueue.erase(productionQueue.begin() + index);
}

void ProductionManager::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    // remove relevant thing from production queue
    // requires upgrade to ability function in api.cpp
    int index = -1;
    for(int i = 0; i < productionQueue.size(); i++){
        if(API::upgradeIDToAbilityID(upgrade_) == productionQueue[i].ability){
            printf("upgradecompleted: removing %d from prod queue\n", productionQueue[i].ability);
            index = i;
            break;
        }
    }
    if(index >= 0)
        productionQueue.erase(productionQueue.begin() + index);
    
}

void ProductionManager::OnUnitDestroyed(const sc2::Unit* unit_){
    bm.OnUnitDestroyed(unit_);
    // remove army building or addon from army building if applicable
    /**
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
    }*/
    int index = -1;
    for(int i = 0; i < armyBuildings.size(); i++){
        if(unit_->tag == armyBuildings[i].buildingTag){
            index = i;
            break;
        }
        else if(unit_->tag == armyBuildings[i].addonTag){
            armyBuildings[i].addon = nullptr;
            armyBuildings[i].addonTag = -1;
            break;
        }
    }
    if(index >= 0)
        armyBuildings.erase(armyBuildings.begin() + index);
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

void ProductionManager::parseQueue(){
    for(auto& s : productionQueue){
        // skip step if we don't have enough supply for step
        int currSupply = gInterface->observation->GetFoodUsed();
        if(currSupply < s.reqSupply) continue;

        // this could probably be switch statement especially if i combine researchUpgrade and morphStructure
        if(API::parseStep(s) == ABIL_BUILD) buildStructure(s);
        else if(API::parseStep(s) == ABIL_TRAIN) trainUnit(s);
        else if(API::parseStep(s) == ABIL_RESEARCH) researchUpgrade(s);
        else if(API::parseStep(s) == ABIL_MORPH) morphStructure(s);
    }
}

void ProductionManager::swapAddon(ArmyBuilding* b1, ArmyBuilding* b2){
    // save positions of each building in local variables
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
    if(s.produceSingle){
        tryTrainUnit(s.ability);
    }
    else{
        setArmyBuildingOrder(nullptr, s.ability);
    }
    
}

// TODO: this could probably be combined with morphStructure, into some function called castBuildingAbility or whatever
void ProductionManager::researchUpgrade(Step s){
    // find research building that corresponds to the research ability
    printf("researchUpgrade: attempting to research %d\n", s.ability);
    sc2::UNIT_TYPEID structureID = API::abilityToUnitTypeID(s.ability);
    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(structureID));
    for(auto& b : buildings){
        if(b->build_progress < 1.0) continue;
        bool canResearch = false;
        sc2::AvailableAbilities researchs = gInterface->query->GetAbilitiesForUnit(b, false, false);
        for(auto& r : researchs.abilities){
            if(r.ability_id == s.ability){
                canResearch = true;
                std::cout << "research possible\n";
                break;
            }
        }
        if(b->orders.empty() && canResearch){
            printf("researchUpgrade: researching %d\n", s.ability);
            gInterface->actions->UnitCommand(b, s.ability);
            return;
        }
    }
}

void ProductionManager::morphStructure(Step s){
    sc2::UNIT_TYPEID structureID = API::abilityToUnitTypeID(s.ability);
    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(structureID));
    for(auto& b : buildings){
        if(b->orders.empty()){
            gInterface->actions->UnitCommand(b, s.ability);
        }
    }
}

bool ProductionManager::TryBuildSupplyDepot(){

    // if not supply capped, dont build supply depot
    if(gInterface->observation->GetFoodUsed() <= gInterface->observation->GetFoodCap() - 2 || gInterface->observation->GetMinerals() < 100)
        return false;
    
    // else, try and build depot using a random scv
    return bm.TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool ProductionManager::TryBuildBarracks() {
    // check for depot and if we have 8 barracks already
    if(API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) + API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) < 1 ||
        API::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 8) return false;
    return bm.TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

bool ProductionManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || gInterface->observation->GetMinerals() < 75 || API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY) >= 6) return false;
    return bm.TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}

bool ProductionManager::tryBuildCommandCenter(){
    if(gInterface->observation->GetMinerals() < 400) return false;
    return bm.TryBuildStructure(sc2::ABILITY_ID::BUILD_COMMANDCENTER);
}

// if armybuilding has an order, manage them
void ProductionManager::handleArmyBuildings(){
    for(auto& a : armyBuildings){
        if(a.order != ARMYBUILDING_UNUSED && a.building->orders.empty()){
            gInterface->actions->UnitCommand(a.building, a.order);
        }
        if(a.order != ARMYBUILDING_UNUSED && a.addon != nullptr && a.building->orders.size() <= 1){
            if(
                a.addon->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR ||
                a.addon->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR ||
                a.addon->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR ||
                a.addon->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REACTOR
            )
                gInterface->actions->UnitCommand(a.building, a.order);
        } // end if order not unused and addon not null
    }
}

void ProductionManager::setArmyBuildingOrder(ArmyBuilding* a, sc2::ABILITY_ID order){
    // if a == nullptr, give all barracks/factories/starports same order
    // if a is an actual ArmyBuilding, then only set order for that one
    if(a == nullptr){
        sc2::UNIT_TYPEID buildingID = API::buildingForUnit(order);
        for(auto& ab : armyBuildings)
            if(ab.building->unit_type.ToType() == buildingID) ab.order = order;
    }
    else{
        a->order = order;
    }
}

// trains a single unit
bool ProductionManager::tryTrainUnit(sc2::ABILITY_ID unitToTrain){
    sc2::UNIT_TYPEID buildingID = API::buildingForUnit(unitToTrain);
    // prioritise a ArmyBuilding that doesn't have an order
    for(auto& a : armyBuildings){
        if(a.order == ARMYBUILDING_UNUSED && a.building->unit_type.ToType() == buildingID && a.building->orders.empty()){
            gInterface->actions->UnitCommand(a.building, unitToTrain);
            return true;
        }
    }

    // if function reaches this point, it is likely there is no armybuilding w/o an order so just pick a valid armybuilding
    for(auto& a : armyBuildings){
        if(a.building->unit_type.ToType() == buildingID && a.building->orders.empty()){
            gInterface->actions->UnitCommand(a.building, unitToTrain);
            return true;
        }
    }
    return false;
}