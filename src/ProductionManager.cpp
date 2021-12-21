#include "ProductionManager.h"

ProductionManager::ProductionManager(){
    bm = BuildingManager();
    logger = Logger("ProductionManager");
}

ProductionManager::ProductionManager(Strategy* strategy_){
    strategy = strategy_;
    bm = BuildingManager();
    logger = Logger("ProductionManager");
}

void ProductionManager::OnStep(){

    // clear the busyBuildings vector of tags
    busyBuildings.clear();

    // handle deadlock if it exists // TODO: fix this at some point
    //handleBuildOrderDeadlock();

    //strategy->debugPrintValidSteps();
    
    // build a supply depot if needed
    TryBuildSupplyDepot();

    // handle mules
    callMules();

    // try building missile turrets regardless of build order is finished or not
    tryBuildMissileTurret();

    // build stuff in the build order
    handleBuildOrder();

    // handle building scvs/auto morphing ccs
    handleTownHalls();

    // TODO: temporarily removed while testing handleBuildOrder();
    
    // if queue still empty and strategy is done, just do normal macro stuff
    if(strategy->isEmpty() && strategy->peekNextBuildOrderStep() == STEP_NULL){

        minerals = gInterface->observation->GetMinerals();
        vespene = gInterface->observation->GetVespene();

        tryBuildCommandCenter();
        // if we are prioritising expansion, don't bother doing any other steps
        // since we need to save up money
        if(config.prioritiseExpansion) return;

        TryBuildBarracks();
        tryBuildFactory();
        tryBuildStarport();
        tryBuildRefinery();
        tryBuildArmory();
        tryBuildEngineeringBay();
        tryBuildAddon();

        // handle upgrades
        handleUpgrades();
    } // end if prod queue empty
    
    // handle idle army producers
    handleBarracks();
    handleFactories();
    handleStarports();
    

    // building manager
    bm.OnStep();


    if(gInterface->observation->GetGameLoop() % 400 == 0){
        logger.infoInit().withStr("BuildOrder Size:").withInt(strategy->getBuildOrderSize()).write();
        logger.withStr("BusyBuildings Size:").withInt(busyBuildings.size()).write();
    }

    if(gInterface->observation->GetGameLoop() % 50 == 0){
        logger.addPlotData("income", "game loop", (float) gInterface->observation->GetGameLoop());
        const sc2::ScoreDetails score = gInterface->observation->GetScore().score_details;
        logger.addPlotData("income", "mineral income", (float) score.collection_rate_minerals);
        logger.addPlotData("income", "vespene income", (float) score.collection_rate_vespene);
        int numBases = 0;
        for(int i = 0; i < gInterface->map->numOfExpansions(); i++){
            if(gInterface->map->getNthExpansion(i)->ownership == OWNER_SELF) numBases++;
        }
        logger.addPlotData("income", "num of bases", (float) numBases);
        logger.writePlotRow("income");

        logger.addPlotData("basecount", "game loop", (float) gInterface->observation->GetGameLoop());
        logger.addPlotData("basecount", "bases", (float) numBases);
        logger.writePlotRow("basecount");
    }

}

void ProductionManager::OnGameStart(){
    bm.OnGameStart();
    
    strategy->initialize();
    config = strategy->getConfig();

    logger.initializePlot({"game loop", "mineral income", "vespene income", "num of bases"}, "income");
    logger.initializePlot({"game loop", "bases"}, "basecount");
    minerals = 0;
    vespene = 0;
}

void ProductionManager::OnBuildingConstructionComplete(const sc2::Unit* building_){
    // building manager doesn't handle addons
    if(!API::isAddon(building_->unit_type.ToType()))
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
            gInterface->map->setExpansionOwnership(building_->pos, OWNER_SELF);
            
    }
    
    strategy->removeStep(API::unitTypeIDToAbilityID(building_->unit_type.ToType()));
}

void ProductionManager::OnUnitCreated(const sc2::Unit* unit_){
    // only run this after the 50th loop
    // necessary to avoid crashing when the main cc is created
    if(gInterface->observation->GetGameLoop() > 50 && unit_->tag != 0 && unit_->is_building && !API::isAddon(unit_->unit_type.ToType()))
        bm.OnUnitCreated(unit_);
    
    // set to false since we just expanded
    if(unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER)
        config.prioritiseExpansion = false;

    // loop through production queue to check which Step corresponds to the unit
    // that just finished and make sure that unit created is a unit, not a structure
    if(
        unit_->is_building ||
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV ||
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_MULE) return;
    
    strategy->removeStep(API::unitTypeIDToAbilityID(unit_->unit_type.ToType()));
    
}

void ProductionManager::OnUpgradeCompleted(sc2::UpgradeID upgrade_){
    // remove relevant thing from production queue
    strategy->removeStep(API::upgradeIDToAbilityID(upgrade_));
}

void ProductionManager::OnUnitDestroyed(const sc2::Unit* unit_){
    bm.OnUnitDestroyed(unit_);

    // if unit destroyed was a town hall, update ownership in mapper
    // TODO: move this to mapper
    if(
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER ||
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND ||
        unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS
    )
        gInterface->map->setExpansionOwnership(unit_->pos, OWNER_NEUTRAL);
        

}

void ProductionManager::OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_){
    bm.OnUnitDamaged(unit_, health_, shields_);
}

ProductionConfig& ProductionManager::getProductionConfig(){
    return config;
}

void ProductionManager::handleBuildOrder(){
    // in a loop
    // get the highest priority item and save its priority level
    // then, in the loop only consider steps with the same priority level
    // or we reach a blocking step (we consider the blocking step)
    if(strategy->isEmpty()) return;

    int currentSupply = gInterface->observation->GetFoodUsed();
    int priorityLevel = strategy->getHighestPriorityStep().priority;
    for(int n = 0; n < strategy->getBuildOrderSize(); n++){
        Step s = strategy->getNthBuildOrderStep(n);
        if(s.priority == priorityLevel && s.reqSupply <= currentSupply){
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

// TODO: for handleBarracks, factories, starports etc. try using tryTrainUnit in the function implementation
void ProductionManager::handleBarracks(){
    sc2::Units barracks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
    if(barracks.empty()) return;

    for(auto& b : barracks){
        if(!b->orders.empty() || b->build_progress < 1.0 || isBuildingBusy(b->tag)) continue;
        // don't forget to add busy building tag if we give an order
        // NOTE: if b->addon_tag is zero, that barracks doesn't have an addon

        // no addon
        if(b->add_on_tag == 0 && config.barracksOutput != PRODUCTION_UNUSED){
            gInterface->actions->UnitCommand(b, config.barracksOutput);
            busyBuildings.emplace_back(b->tag);
        }
        // reactor addon
        else if(
            b->add_on_tag != 0 &&
            gInterface->observation->GetUnit(b->add_on_tag)->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR &&
            config.barracksOutput != PRODUCTION_UNUSED
        ){
            gInterface->actions->UnitCommand(b, config.barracksOutput);
            gInterface->actions->UnitCommand(b, config.barracksOutput);
            busyBuildings.emplace_back(b->tag);
        }
        // techlab addon
        else if(
            b->add_on_tag != 0 &&
            gInterface->observation->GetUnit(b->add_on_tag)->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB &&
            config.barracksTechOutput != PRODUCTION_UNUSED
        ){
            gInterface->actions->UnitCommand(b, config.barracksTechOutput);
            busyBuildings.emplace_back(b->tag);
        }
    } // end for b : barracks
}

void ProductionManager::handleFactories(){
    sc2::Units factories = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_FACTORY));
    if(factories.empty()) return;

    for(auto& f : factories){
        if(!f->orders.empty() || f->build_progress < 1.0 || isBuildingBusy(f->tag)) continue;

        // no addon
        if(f->add_on_tag == 0 && config.factoryOutput != PRODUCTION_UNUSED){
            gInterface->actions->UnitCommand(f, config.factoryOutput);
            busyBuildings.emplace_back(f->tag);
        }
        // reactor addon
        else if(
            f->add_on_tag != 0 &&
            gInterface->observation->GetUnit(f->add_on_tag)->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR &&
            config.factoryOutput != PRODUCTION_UNUSED
        ){
            gInterface->actions->UnitCommand(f, config.factoryOutput);
            gInterface->actions->UnitCommand(f, config.factoryOutput);
            busyBuildings.emplace_back(f->tag);
        }
        // techlab addon
        else if(
            f->add_on_tag != 0 &&
            gInterface->observation->GetUnit(f->add_on_tag)->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB &&
            config.factoryTechOutput != PRODUCTION_UNUSED
        ){
            gInterface->actions->UnitCommand(f, config.factoryTechOutput);
            busyBuildings.emplace_back(f->tag);
        }
    } // end for f : factories
}

void ProductionManager::handleStarports(){
    sc2::Units starports = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_STARPORT));
    if(starports.empty()) return;

    for(auto& s : starports){
        if(!s->orders.empty() || s->build_progress < 1.0 || isBuildingBusy(s->tag)) continue;

        // no addon
        if(s->add_on_tag == 0 && config.starportOutput != PRODUCTION_UNUSED){
            gInterface->actions->UnitCommand(s, config.starportOutput);
            busyBuildings.emplace_back(s->tag);
        }
        // reactor addon
        else if(
            s->add_on_tag != 0 &&
            gInterface->observation->GetUnit(s->add_on_tag)->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR &&
            config.starportOutput != PRODUCTION_UNUSED
        ){
            gInterface->actions->UnitCommand(s, config.starportOutput);
            gInterface->actions->UnitCommand(s, config.starportOutput);
            busyBuildings.emplace_back(s->tag);
        }
        // techlab addon
        else if(
            s->add_on_tag != 0 &&
            gInterface->observation->GetUnit(s->add_on_tag)->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB &&
            config.starportTechOutput != PRODUCTION_UNUSED
        ){
            gInterface->actions->UnitCommand(s, config.starportTechOutput);
            busyBuildings.emplace_back(s->tag);
        }
    } // end for s : starports
}

void ProductionManager::handleTownHalls(){
    sc2::Units ccs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsTownHall());
    if(ccs.empty()) return;

    for(auto& cc : ccs){
        // if townhall is busy or is in progress, ignore it
        if(!cc->orders.empty() || cc->build_progress < 1.0) continue;

        // if cc is a command center and we have the relevant tech requirement,
        // morph it to orbital/planetary automatically if config says so
        if(
            config.autoMorphCC &&
            (int) config.maxOrbitals > API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND) &&
            API::countReadyUnits(sc2::UNIT_TYPEID::TERRAN_BARRACKS) >= 1 &&
            cc->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER
            )
            gInterface->actions->UnitCommand(cc, sc2::ABILITY_ID::MORPH_ORBITALCOMMAND);
        else if(
            config.autoMorphCC &&
            (int) config.maxOrbitals <= API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND) &&
            API::countReadyUnits(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY) >= 1 &&
            cc->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER
            )
            gInterface->actions->UnitCommand(cc, sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS);
        // otherwise, build an scv
        else if(
            API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SCV) < (int) config.maxWorkers
            )
            gInterface->actions->UnitCommand(cc, sc2::ABILITY_ID::TRAIN_SCV);
    }
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
        case TYPE_SET_PRODUCTION:
            // 1. determine what produces the unit and if it requires a techlab
            sc2::UNIT_TYPEID buildingType = API::getProducer(s.getAbility());
            bool requiresTechLab = API::requiresTechLab(s.getAbility());

            // 2. set the config to the unit
            switch(buildingType){
                case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
                    if(requiresTechLab) config.barracksTechOutput = s.getAbility();
                    else                config.barracksOutput = s.getAbility();
                    break;
                case sc2::UNIT_TYPEID::TERRAN_FACTORY:
                    if(requiresTechLab) config.factoryTechOutput = s.getAbility();
                    else                config.factoryOutput = s.getAbility();
                    break;
                case sc2::UNIT_TYPEID::TERRAN_STARPORT:
                    if(requiresTechLab) config.starportTechOutput = s.getAbility();
                    else                config.starportOutput = s.getAbility();
                    break;
            }

            // 3. remove the step from buildorder
            strategy->removeStep(s);
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
        case sc2::ABILITY_ID::BUILD_FACTORY:
            tryBuildFactory();
            break;
        case sc2::ABILITY_ID::BUILD_STARPORT:
            tryBuildStarport();
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
        case sc2::ABILITY_ID::BUILD_BUNKER:
            tryBuildBunker();
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

void ProductionManager::castBuildingAbility(Step s){
    // find research building that corresponds to the research ability
    sc2::UNIT_TYPEID structureID = API::abilityToUnitTypeID(s.getAbility());
    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(structureID));
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
        if(b->orders.empty() && canResearch && !isBuildingBusy(b->tag) && canAfford(s.getAbility())){
            logger.infoInit().withStr("casting building ability").withInt((int) s.getAbility()).write();
            balanceBook(s.getAbility());
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
    int numFactories = (int) API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY);
    int cycle = 1 + numTownhalls;
    if(numTownhalls > 2) cycle += (numFactories * 2);
    if(numTownhalls > 1)
        cycle += numBarracks;
    // cycle: how much supply cushion we want, this is numTownhalls (+ numBarracks if we have two expansions)

    // if not supply capped or we are at max supply, dont build supply depot
    if(
        gInterface->observation->GetFoodUsed() < gInterface->observation->GetFoodCap() - cycle ||
        gInterface->observation->GetMinerals() < 100 ||
        gInterface->observation->GetFoodCap() >= 200)
        return false;
    
    // else, try and build a number of depots equal to the number of town halls we have
    return bm.TryBuildStructure(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, numTownhalls);
}

bool ProductionManager::TryBuildBarracks() {
    // check for depot and if we have 8 barracks already
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT) + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) < 1 ||
        API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) >= config.maxBarracks) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_BARRACKS);
}

bool ProductionManager::tryBuildFactory(){
    if(API::countReadyUnits(sc2::UNIT_TYPEID::TERRAN_BARRACKS) < 1 || API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY) >= config.maxFactories) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_FACTORY);
}

bool ProductionManager::tryBuildStarport(){
    if(API::countReadyUnits(sc2::UNIT_TYPEID::TERRAN_FACTORY) < 1 || API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT) >= config.maxStarports) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_STARPORT);
}

bool ProductionManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || gInterface->observation->GetMinerals() < 75 || API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY) >= config.maxRefineries) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_REFINERY);
}

bool ProductionManager::tryBuildCommandCenter(){
    if(gInterface->observation->GetMinerals() < 400) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_COMMANDCENTER);
}

// FIXME: when we make a strategy that actually uses armory for stuff, 
bool ProductionManager::tryBuildArmory(){
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY) >= 1) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_ARMORY);
}

bool ProductionManager::tryBuildEngineeringBay(){
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY) >= 2) return false;
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_ENGINEERINGBAY);
}

bool ProductionManager::tryBuildBunker(){
    return tryBuyStructure(sc2::ABILITY_ID::BUILD_BUNKER);
}

bool ProductionManager::tryBuildAddon(){
    sc2::Units barracks = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS));
    for(auto& b : barracks){
        if(b->add_on_tag != 0 || b->build_progress < 1.0 || !b->orders.empty()) continue;

        gInterface->actions->UnitCommand(b, config.barracksDefaultAddon);
    }

    sc2::Units factories = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_FACTORY));
    for(auto& f : factories){
        if(f->add_on_tag != 0 || f->build_progress < 1.0 || !f->orders.empty()) continue;

        gInterface->actions->UnitCommand(f, config.factoryDefaultAddon);
    }

    sc2::Units starports = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_STARPORT));
    for(auto& s : starports){
        if(s->add_on_tag != 0 || s->build_progress < 1.0 || !s->orders.empty()) continue;
        gInterface->actions->UnitCommand(s, config.starportDefaultAddon);
    }

    return true;
}

bool ProductionManager::tryBuildMissileTurret(){
    // only build missile turret if information manager says so
    if(!config.buildTurrets) return false;

    return tryBuyStructure(sc2::ABILITY_ID::BUILD_MISSILETURRET);
            
}

bool ProductionManager::tryBuyStructure(sc2::ABILITY_ID id){
    // ignore bank for cc
    if(id == sc2::ABILITY_ID::BUILD_COMMANDCENTER && minerals >= 400){
        balanceBook(id);
        return bm.TryBuildStructure(id);
    }
    
    if(!canAfford(id)) return false;

    balanceBook(id);
    return bm.TryBuildStructure(id);
}

// trains at most n units
bool ProductionManager::tryTrainUnit(sc2::ABILITY_ID unitToTrain, int n){
    sc2::UNIT_TYPEID buildingID = API::getProducer(unitToTrain);

    sc2::Units buildings = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(buildingID));
    if(buildings.empty()) return false;

    // TODO: add support for training n units and reactors
    // TODO: check available abilities for this building (like what we do with castBuildingAbility)
    for(auto& b : buildings){
        if(b->build_progress < 1.0) continue;

        if(b->orders.empty() && !isBuildingBusy(b->tag)){
            gInterface->actions->UnitCommand(b, unitToTrain);
            busyBuildings.emplace_back(b->tag);
            return true;
        }
    }
    return false;
}

void ProductionManager::handleUpgrades(){
    // get random infantry unit
    sc2::Units marines = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_MARINE));
    sc2::Units marauders = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_MARAUDER));
    marines.insert(std::end(marines), std::begin(marauders), std::end(marauders));
    // get random vehicle unit
    sc2::Units factoryUnits = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(
        {
            sc2::UNIT_TYPEID::TERRAN_SIEGETANK, sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, sc2::UNIT_TYPEID::TERRAN_CYCLONE, sc2::UNIT_TYPEID::TERRAN_WIDOWMINE,
            sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED, sc2::UNIT_TYPEID::TERRAN_HELLION, sc2::UNIT_TYPEID::TERRAN_HELLIONTANK, sc2::UNIT_TYPEID::TERRAN_THOR,
            sc2::UNIT_TYPEID::TERRAN_THORAP
        }));
    // get random flying unit
    
    // get current upgrade levels
    // FIXME: the combat shields stuff here is temporary
    bool getCombatShields = false;
    int infantryWeapons = 0, infantryArmor = 0;
    if(!marines.empty()){
        infantryWeapons = marines.front()->attack_upgrade_level;
        infantryArmor = marines.front()->armor_upgrade_level;
        if(marines.front()->health_max < 50) getCombatShields = true;
    }

    int vehicleWeapons = 0, vehicleArmor = 0;
    if(!factoryUnits.empty()){
        vehicleWeapons = factoryUnits.front()->attack_upgrade_level;
        vehicleArmor = factoryUnits.front()->armor_upgrade_level;
    }

    if(getCombatShields){
        sc2::Units techLabs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB));
        if(!techLabs.empty()){
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_COMBATSHIELD, false, false, false));
        }
    }

    if(infantryWeapons > infantryArmor){
        upgradeInfantryArmor(infantryArmor);
        upgradeInfantryWeapons(infantryWeapons);
    }
    else{
        upgradeInfantryWeapons(infantryWeapons);
        upgradeInfantryArmor(infantryArmor);
    }

    if(vehicleWeapons > vehicleArmor){
        upgradeVehicleArmor(vehicleArmor);
        upgradeFactoryWeapons(vehicleWeapons);
    }
    else{
        upgradeFactoryWeapons(vehicleWeapons);
        upgradeVehicleArmor(vehicleArmor);
    }
    
}

void ProductionManager::callMules(){
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND) <= 0) return;

    sc2::Units orbitals = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND));

    for(auto& orbital : orbitals){
        if(orbital->energy >= 50){
            // find current expansion
            Expansion* current = gInterface->map->getCurrentExpansion();
            if(current == nullptr) return;

            // get a visible mineral unit closest to the current expansion
            // TODO: in Mapper, update units in Expansion since their visibility status doesnt update automatically
            //          ie i think we should have Mapper extend Manager
            sc2::Units minerals = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, sc2::IsVisibleMineralPatch());
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

bool ProductionManager::canAfford(sc2::ABILITY_ID id){
    Cost cost = API::getCost(id);
    int mCost = cost.first;
    int vCost = cost.second;
    return (
        minerals - config.bank >= mCost && vespene >= vCost
    );
}

bool ProductionManager::balanceBook(sc2::ABILITY_ID id){
    if(!canAfford(id)) return canAfford(id);

    Cost cost = API::getCost(id);
    minerals -= cost.first;
    vespene -= cost.second;
    
    return true;
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

void ProductionManager::upgradeFactoryWeapons(int currLevel){
    switch(currLevel){
        case 0:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1, false, false, false));
            break;
        case 1:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2, false, false, false));
            break;
        case 2:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3, false, false, false));
            break;
    }
}

void ProductionManager::upgradeVehicleArmor(int currLevel){
    switch(currLevel){
        case 0:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1, false, false, false));
            break;
        case 1:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2, false, false, false));
            break;
        case 2:
            castBuildingAbility(Step(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3, false, false, false));
            break;
    }
}

void ProductionManager::upgradeStarshipWeapons(int currLevel){

}
