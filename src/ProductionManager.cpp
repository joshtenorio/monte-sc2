#include "ProductionManager.h"

void ProductionManager::OnStep(){

    // clear the busyBuildings vector of tags
    busyBuildings.clear();

    // fill queue with steps
    fillQueue();

    // build a supply depot if needed
    TryBuildSupplyDepot();

    // handle mules
    callMules();

    // if queue still empty and strategy is done, just do normal macro stuff
    if(productionQueue.empty() && strategy->peekNextPriorityStep() == STEP_NULL){
        TryBuildBarracks();         // max : 8
        tryBuildRefinery();
        tryBuildCommandCenter();
        tryBuildArmory();           // max : 1
        tryBuildEngineeringBay();   // max : 2

        // check on army buildings
        for(auto& a : armyBuildings){

            // if army building is a barrack, then place a reactor if possible
            if(
                a.building->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKS &&
                a.addon == nullptr &&
                gInterface->query->Placement(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(a.building->pos.x + 2.5, a.building->pos.y - 0.5))
                ){
                if(a.building->orders.empty())
                    gInterface->actions->UnitCommand(a.building, sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS);
            }

        } // end for loop

        // handle upgrades
        handleUpgrades();
    } // end if prod queue empty



    for(auto& a : armyBuildings){
        // if army building is unused, give it an order
        if(a.order == ARMYBUILDING_UNUSED){
            switch(a.building->unit_type.ToType()){
                case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
                    a.order = sc2::ABILITY_ID::TRAIN_MARINE;
                break;
                case sc2::UNIT_TYPEID::TERRAN_FACTORY:
                break;
                case sc2::UNIT_TYPEID::TERRAN_STARPORT:
                    a.order = sc2::ABILITY_ID::TRAIN_MEDIVAC;
                break;
            }
        }
    }
        

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

    if(gInterface->observation->GetGameLoop() % 400 == 0){
        logger.infoInit().withStr("ProdQueue Size:").withInt(productionQueue.size()).write();
        logger.withStr("BusyBuildings Size:").withInt(busyBuildings.size()).write();
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
            // search through production queue to remove 
            for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
                if(API::unitTypeIDToAbilityID(building_->unit_type.ToType()) == (*itr).ability)
                    itr = productionQueue.erase(itr);
                else ++itr;
            }
            return;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
            gInterface->map->getClosestExpansion(building_->pos)->ownership = OWNER_SELF;
            break;
    }
    
    // loop through production queue to check which Step corresponds to
    // the structure that just finished, doesn't apply to morphs
    for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
        if(building_->unit_type.ToType() == API::abilityToUnitTypeID((*itr).ability))
            itr = productionQueue.erase(itr);
        else ++itr;
    }
}

void ProductionManager::OnUnitCreated(const sc2::Unit* unit_){
    // only run this after the 50th loop
    // necessary to avoid crashing when the main cc is created
    if(gInterface->observation->GetGameLoop() > 50 && unit_->tag != 0 && API::isStructure(unit_->unit_type.ToType()))
        bm.OnUnitCreated(unit_);
    
    // loop through production queue to check which Step corresponds to the unit
    // that just finished and make sure that unit created is a unit, not a structure
    if(API::isStructure(unit_->unit_type.ToType()) || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV) return;
    for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
        if(unit_->unit_type.ToType() == API::abilityToUnitTypeID((*itr).ability)){   
            itr = productionQueue.erase(itr);
        }
            
        else ++itr;
   }
}

void ProductionManager::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    // remove relevant thing from production queue
    // requires upgrade to ability function in api.cpp
    for(auto itr = productionQueue.begin(); itr != productionQueue.end(); ){
        if(API::upgradeIDToAbilityID(upgrade_) == (*itr).ability){
            itr = productionQueue.erase(itr);
        }
        else ++itr;
   }
    
}

void ProductionManager::OnUnitDestroyed(const sc2::Unit* unit_){
    bm.OnUnitDestroyed(unit_);

    // if unit destroyed was a town hall, update ownership in mapper
    if(
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER ||
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND ||
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS
    )
        gInterface->map->getClosestExpansion(unit_->pos)->ownership = OWNER_NEUTRAL;
        
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
        case sc2::ABILITY_ID::BUILD_ARMORY:
            tryBuildArmory();
            break;
        case sc2::ABILITY_ID::BUILD_ENGINEERINGBAY:
            tryBuildEngineeringBay();
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
    sc2::UNIT_TYPEID structureID = API::abilityToUnitTypeID(s.ability);
    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(structureID));
    for(auto& b : buildings){
        if(b->build_progress < 1.0) continue;
        bool canResearch = false;
        sc2::AvailableAbilities researchs = gInterface->query->GetAbilitiesForUnit(b, false, false);
        for(auto& r : researchs.abilities){
            if(r.ability_id == s.ability){
                canResearch = true;
                break;
            }
        }
        if(b->orders.empty() && canResearch && !isBuildingBusy(b->tag)){
            logger.infoInit().withStr("researching").withInt((int) s.ability).write();
            gInterface->actions->UnitCommand(b, s.ability);
            busyBuildings.emplace_back(b->tag);
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
    int numTownhalls = (int) (API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) +
                        API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND) +
                        API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS));
    int cycle = 1 + numTownhalls;
    // cycle: how much supply cushion we want, this is numTownhalls + numBarracks

    
    // if not supply capped or we are at max supply, dont build supply depot
    if(
        gInterface->observation->GetFoodUsed() < gInterface->observation->GetFoodCap() - cycle ||
        gInterface->observation->GetMinerals() < 100 ||
        gInterface->observation->GetFoodCap() >= 200)
        return false;
    
    // else, try and build a number of depots equal to the number of town halls we have
    return bm.TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, numTownhalls);
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

// FIXME: when we make a strategy that actually uses armory for stuff, 
bool ProductionManager::tryBuildArmory(){
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY) >= 1) return false;
    return bm.TryBuildStructure(sc2::ABILITY_ID::BUILD_ARMORY);
}

bool ProductionManager::tryBuildEngineeringBay(){
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY) >= 2) return false;
    return bm.TryBuildStructure(sc2::ABILITY_ID::BUILD_ENGINEERINGBAY);
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

void ProductionManager::handleUpgrades(){
    // get random infantry unit
    sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE));
    sc2::Units marauders = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_MARAUDER));
    marines.insert(std::end(marines), std::begin(marauders), std::end(marauders));
    // get random vehicle unit
    // get random flying unit
    // FIXME: implement getting random vehicle/flying units upgrade levels
    
    // get current upgrade levels
    int infantryWeapons = 0;
    int infantryArmor = 0;
    if(!marines.empty()){
        infantryWeapons = marines.front()->attack_upgrade_level;
        infantryArmor = marines.front()->armor_upgrade_level;
        
        if(gInterface->observation->GetGameLoop() % 400 == 0){
            logger.infoInit().withStr("Infantry Weapons:").withInt(infantryWeapons);
            logger.withStr("\tInfantry Armor:").withInt(infantryArmor).write();
        }
    }

    // FIXME: see if theres a better way to do this - based on what is higher, queue one upgrade then the other
    // based on those upgrade levels, select the next upgrade to prioritise
    if(infantryWeapons > infantryArmor){
        switch(infantryArmor){
            case 0:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1, -1, false, false));
                break;
            case 1:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2, -1, false, false));
                break;
            case 2:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3, -1, false, false));
                break;
        }
        switch(infantryWeapons){
            case 0:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1, -1, false, false));
                break;
            case 1:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2, -1, false, false));
                break;
            case 2:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3, -1, false, false));
                break;
        }
    }
    else{
        switch(infantryWeapons){
            case 0:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1, -1, false, false));
                break;
            case 1:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2, -1, false, false));
                break;
            case 2:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3, -1, false, false));
                break;
        }
        switch(infantryArmor){
            case 0:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1, -1, false, false));
                break;
            case 1:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2, -1, false, false));
                break;
            case 2:
                researchUpgrade(Step(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3, -1, false, false));
                break;
        }
    }
}

void ProductionManager::callMules(){
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND) <= 0) return;

    sc2::Units orbitals = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND));

    for(auto& orbital : orbitals){
        if(orbital->energy >= 50){
            // find current expansion
            Expansion* current = gInterface->map->getCurrentExpansion();
            if(current == nullptr) return;

            // get a visible mineral unit closest to the current expansion
            // TODO: in Mapper, update units in Expansion since their visibility status doesnt update automatically
            sc2::Units minerals = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsVisibleMineralPatch());
	    if(minerals.empty()) return;
            const sc2::Unit* mineralTarget = minerals.front();
            for(auto& m : minerals)
                if(sc2::DistanceSquared2D(current->baseLocation, m->pos) < sc2::DistanceSquared2D(current->baseLocation, mineralTarget->pos)){
                    mineralTarget = m;
                }

            
            if(mineralTarget != nullptr){
                gInterface->actions->UnitCommand(orbital, sc2::ABILITY_ID::EFFECT_CALLDOWNMULE, mineralTarget);
            }
                
        } // end if orbital energy >= 50
            
    }
}

bool ProductionManager::isBuildingBusy(sc2::Tag bTag){
    for(auto& b : busyBuildings)
        if(b == bTag) return true;
    return false;
}
