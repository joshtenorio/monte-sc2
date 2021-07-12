#include "ProductionManager.h"

void ProductionManager::OnStep(){

    // clear the busyBuildings vector of tags
    busyBuildings.clear();

    
    // build a supply depot if needed
    TryBuildSupplyDepot();

    // handle mules
    callMules();

    // if queue still empty and strategy is done, just do normal macro stuff
    if(strategy->isEmpty() && strategy->peekNextBuildOrderStep() == STEP_NULL){
        TryBuildBarracks();         // max : 8
        tryBuildRefinery();
        tryBuildCommandCenter();
        tryBuildArmory();           // max : 1
        tryBuildEngineeringBay();   // max : 2

        // handle upgrades
        handleUpgrades();
    } // end if prod queue empty


    // building manager
    bm.OnStep();

    // TODO: make this into function?
    Units ccs = gInterface->observation->GetUnits(Unit::Alliance::Self, IsTownHall());
    if(strategy->maxWorkers < API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV))
        for(auto& cc : ccs)
            if(gInterface->observation->GetMinerals() >= 50 && cc->orders.empty())
                gInterface->actions->UnitCommand(cc, ABILITY_ID::TRAIN_SCV);

    if(gInterface->observation->GetGameLoop() % 400 == 0){
        logger.infoInit().withStr("ProdQueue Size:").withInt(strategy->getBuildOrderSize()).write();
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
            break;
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            // remove step from build order
            strategy->removeStep(API::unitTypeIDToAbilityID(building_->unit_type.ToType())); // TODO: is this redundant?
            return;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
            gInterface->map->getClosestExpansion(building_->pos)->ownership = OWNER_SELF;
            break;
    }
    
    strategy->removeStep(API::unitTypeIDToAbilityID(building_->unit_type.ToType()));
}

void ProductionManager::OnUnitCreated(const sc2::Unit* unit_){
    // only run this after the 50th loop
    // necessary to avoid crashing when the main cc is created
    if(gInterface->observation->GetGameLoop() > 50 && unit_->tag != 0 && API::isStructure(unit_->unit_type.ToType()))
        bm.OnUnitCreated(unit_);
    
    // loop through production queue to check which Step corresponds to the unit
    // that just finished and make sure that unit created is a unit, not a structure
    if(API::isStructure(unit_->unit_type.ToType()) || unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV) return;
    strategy->removeStep(API::unitTypeIDToAbilityID(unit_->unit_type.ToType()));
    
}

void ProductionManager::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    // remove relevant thing from production queue
    strategy->removeStep(API::upgradeIDToAbilityID(upgrade_));
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
        

}

void ProductionManager::handleBuildOrder(){
    // in a loop
    // get the highest priority item and save its priority level
    // then, in the loop only consider steps with the same priority level
    // or we reach a blocking step (we consider the blocking step)
    if(strategy->isEmpty()) return;

    int priorityLevel = strategy->getHighestPriorityStep().priority;
    for(int n = 0; n < strategy->getBuildOrderSize(); n++){
        Step s = strategy->getNthBuildOrderStep(n);
        if(s.priority == priorityLevel){
            parseStep(s);
        }

        if(s.blocking) break;
    } // end loop
}

void ProductionManager::handleBuildOrderDeadlock(){
    // if requirements for any of the highest priority level items are not met,
    // we should add requirements in a step with the highest priority level + 1 to the build order
    if(strategy->isEmpty()) return;

    int priorityLevel = strategy->getHighestPriorityStep().priority;
    for(int n = 0; n < strategy->getBuildOrderSize(); n++){
        Step s = strategy->getNthBuildOrderStep(n);
        if(s.priority == priorityLevel){
            std::vector<sc2::UNIT_TYPEID> requirements = API::getTechRequirements(s.getAbility());
            if(requirements.empty()) continue;
            
            // check if we have requirements
            std::vector<bool> requirementsAvailable;
            requirementsAvailable.reserve(requirements.size());
            for(int n = 0; n < requirements.size(); n++){
                sc2::Units requirement = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(requirements[n]));
                if(requirement.empty()) requirementsAvailable[n] = false;
                else requirementsAvailable[n] = true;
            }

            for(int n = 0; n < requirementsAvailable.size(); n++){
                if(!requirementsAvailable[n]){
                    // add tech requirements to build order
                    strategy->addEmergencyBuildOrderStep(TYPE_BUILD, API::unitTypeIDToAbilityID(requirements[n]), false);
                }
            }
        } // end if s.priority level == prioritylevel

        if(s.blocking) break;
    } // end build order loop
}

void ProductionManager::parseStep(Step s){
    switch(s.getType()){
        case TYPE_ADDON:
            buildAddon(s);
            break;
        case TYPE_BUILD:
            buildStructure(s);
            break;
        case TYPE_BUILDINGCAST:
            castBuildingAbility(s);
            break;
        case TYPE_TRAIN:
            trainUnit(s);
            break;
    }
}

void ProductionManager::buildStructure(Step s){
    sc2::ABILITY_ID ability = s.getAbility();
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

void ProductionManager::buildAddon(Step s){
    // TODO: determine if we can/should swap addons
    // for now just pass it to castBuildingAbility
    castBuildingAbility(s);
}

void ProductionManager::trainUnit(Step s){
    // for now just pass it to tryTrainUnit
    tryTrainUnit(s.getAbility(), 1);
}

// TODO: this could probably be combined with morphStructure, into some function called castBuildingAbility or whatever
void ProductionManager::castBuildingAbility(Step s){
    // find research building that corresponds to the research ability
    sc2::UNIT_TYPEID structureID = API::abilityToUnitTypeID(s.getAbility());
    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(structureID));
    for(auto& b : buildings){
        if(b->build_progress < 1.0) continue;
        bool canResearch = false;
        sc2::AvailableAbilities researchs = gInterface->query->GetAbilitiesForUnit(b, false, false);
        for(auto& r : researchs.abilities){
            if(r.ability_id == s.getAbility()){
                canResearch = true;
                break;
            }
        }
        if(b->orders.empty() && canResearch && !isBuildingBusy(b->tag)){
            logger.infoInit().withStr("casting building ability").withInt((int) s.getAbility()).write();
            gInterface->actions->UnitCommand(b, s.getAbility());
            busyBuildings.emplace_back(b->tag);
            return;
        }
    }
}

bool ProductionManager::TryBuildSupplyDepot(){
    int numTownhalls = (int) (API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER) +
                        API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND) +
                        API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS));
    int numBarracks = (int) API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
    int cycle = 1 + numTownhalls;
    if(numTownhalls > 1) cycle += numBarracks;
    // cycle: how much supply cushion we want, this is numTownhalls (+ numBarracks if we have two expansions)

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

// trains at most n units
bool ProductionManager::tryTrainUnit(sc2::ABILITY_ID unitToTrain, int n){
    sc2::UNIT_TYPEID buildingID = API::buildingForUnit(unitToTrain);

    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(buildingID));
    if(buildings.empty()) return false;

    for(auto& b : buildings){
        
    }
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
    int infantryWeaponsLevel = 0;
    int infantryArmorLevel = 0;
    if(!marines.empty()){
        infantryWeaponsLevel = marines.front()->attack_upgrade_level;
        infantryArmorLevel = marines.front()->armor_upgrade_level;
        
        if(gInterface->observation->GetGameLoop() % 400 == 0){
            logger.infoInit().withStr("Infantry Weapons:").withInt(infantryWeaponsLevel);
            logger.withStr("\tInfantry Armor:").withInt(infantryArmorLevel).write();
        }
    }

    if(infantryWeaponsLevel > infantryArmorLevel){
        upgradeInfantryArmor(infantryArmorLevel);
        upgradeInfantryWeapons(infantryWeaponsLevel);
    }
    else{
        upgradeInfantryWeapons(infantryWeaponsLevel);
        upgradeInfantryArmor(infantryArmorLevel);
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
            //          ie i think we should have Mapper extend Manager
            sc2::Units minerals = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsVisibleMineralPatch());
	        if(minerals.empty()) return;
            const sc2::Unit* mineralTarget = minerals.front();
            for(auto& m : minerals)
                if(sc2::DistanceSquared2D(current->baseLocation, m->pos) < sc2::DistanceSquared2D(current->baseLocation, mineralTarget->pos)){
                    mineralTarget = m;
                }

            if(mineralTarget != nullptr)
                gInterface->actions->UnitCommand(orbital, sc2::ABILITY_ID::EFFECT_CALLDOWNMULE, mineralTarget);
        } // end if orbital energy >= 50
    }
}

bool ProductionManager::isBuildingBusy(sc2::Tag bTag){
    for(auto& b : busyBuildings)
        if(b == bTag) return true;
    return false;
}

void ProductionManager::upgradeInfantryWeapons(int currLevel){
    switch(currLevel){
        case 0:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1, false, false, false));
            break;
        case 1:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2, false, false, false));
            break;
        case 2:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3, false, false, false));
            break;
    }
}

void ProductionManager::upgradeInfantryArmor(int currLevel){
    switch(currLevel){
        case 0:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1, false, false, false));
            break;
        case 1:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2, false, false, false));
            break;
        case 2:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3, false, false, false));
            break;
    }
}