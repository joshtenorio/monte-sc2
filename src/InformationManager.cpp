#include "InformationManager.h"

InformationManager::InformationManager(){
    logger = Logger("InformationManager");

    // initialize array
    income.resize(1000);
    for(int i = 0; i < 1000; i++){
        income[i] = 0;
    }
}

void InformationManager::OnGameStart(){
    std::vector<sc2::PlayerInfo> players = gInterface->observation->GetGameInfo().player_info;
    for(auto& p : players)
        if(p.player_name != "Monte"){
            enemyRace = p.race_requested;
            break;
        }
}

void InformationManager::OnStep(){

    // only check income after some time
    if(gInterface->observation->GetGameLoop() > 5000){
        checkIncome();
    }

    if(gInterface->observation->GetGameLoop() > 60 && gInterface->observation->GetGameLoop() < 3000)
        checkForWorkerRush(); // only check this before 3 or 4 minutes

    if(gInterface->observation->GetGameLoop() % 30 == 0)
        checkForMassAir();

    // start updating expansions well after mapper has initialized
    if(gInterface->observation->GetGameLoop() % 30 == 0 && gInterface->observation->GetGameLoop() >= 3000)
        updateExpoOwnership();
    
    if(gInterface->observation->GetGameLoop() % 400 == 0 && gInterface->observation->GetGameLoop() > 2){
        int n = 0;
        for(int i = 0; i < gInterface->map->numOfExpansions(); i++)
            if(gInterface->map->getNthExpansion(i)->ownership == OWNER_ENEMY) n++;
        logger.infoInit().withStr("enemy has").withInt(n).withStr("expansions").write();
        logger.infoInit().withStr("delta income:").withFloat(income[0]-income[999]).write();
    }
}

ProductionConfig InformationManager::updateProductionConfig(ProductionConfig& currentPConfig){

    if(requireExpansion){
        currentPConfig.prioritiseExpansion = true;

    }
    
    if(requireAntiAir){
        currentPConfig.buildTurrets = true;
        currentPConfig.starportOutput = sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER;
        currentPConfig.maxStarports = 3;
    }

    float bioRatio = currentPConfig.marineMarauderRatio;
    if(bioRatio != -1){
        int numMarines = API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE);
        int numMarauders = API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER);
        float currRatio = (numMarauders != 0 ? numMarines / numMarauders : numMarines);
        currentPConfig.barracksTechOutput = (currRatio > bioRatio ? sc2::ABILITY_ID::TRAIN_MARAUDER : sc2::ABILITY_ID::TRAIN_MARINE);
    }

    float medicRatio = currentPConfig.marineMedivacRatio;
    if(medicRatio != -1){
        int numMarines = API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MARINE);
        int numMedivacs = API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_MEDIVAC);
        float currRatio = (numMedivacs != 0 ? numMarines / numMedivacs : numMarines);
        
        if(requireAntiAir){
            int numVikings = API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER);
            currRatio = (numVikings != 0 ? numMarines / numVikings : numMarines);
            currentPConfig.starportOutput = (currRatio > medicRatio ? sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER : sc2::ABILITY_ID::TRAIN_MEDIVAC);
        }
        else
            currentPConfig.starportOutput = (currRatio > medicRatio ? sc2::ABILITY_ID::TRAIN_MEDIVAC : sc2::ABILITY_ID::TRAIN_LIBERATOR);

    }


    return currentPConfig; // TODO: since we pass in the reference to the current config, do we need this?
}

CombatConfig InformationManager::updateCombatConfig(CombatConfig& currentCConfig){
    return currentCConfig; //tmp
}

void InformationManager::updateExpoOwnership(){
    sc2::Units enemyTownHalls = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsTownHall());
    for(auto& th : enemyTownHalls){
        gInterface->map->setExpansionOwnership(th->pos, OWNER_ENEMY);
    }
}

void InformationManager::checkForWorkerRush(){
    // if we have 4 or more completed buildings, don't check for worker rush
    int structureCount = API::countUnitType([](const sc2::Unit& u){
                return (u.is_building && u.build_progress == 1.0);
            });
    if(structureCount >= 4) return;

    // pick a location to search for enemy workers from -> ideally a worker closest to nat or the supply depot at the ramp
    sc2::Point2D nat = gInterface->map->getNthExpansion(2)->baseLocation;
    Worker* w = gInterface->wm->getClosestWorker(nat);
    int numEnemies = 0;
    if(w != nullptr)
        if(w->getUnit() != nullptr)
            numEnemies = API::getClosestNUnits(w->getUnit()->pos, 12, 8, sc2::Unit::Alliance::Enemy).size();
    else return;

    // if we count more than 3 enemies, activate worker rush defense code (ie set workerRushDetected to true)
    if(numEnemies > 3 && !workerRushDetected){
        workerRushDetected = true;
        logger.tag("worker rush detected");
    }
    // TODO: should we add an else to make it false?
}

void InformationManager::checkForEnemyCloak(){
    // if we find a dangerous cloaked enemy (e.g. not an observer), set requireDetectors to true
    // also we should send a chat message and a Tag as well

}

void InformationManager::checkForMassAir(){
    // TODO: why is mutaCount a private variable and not a local variable here? same w/ spireExists
    mutaCount = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_MUTALISK)).size();
    
    int bcCount = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER)).size();
    int fusionCoreExists = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_FUSIONCORE)).size();
    int colossusExists = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::PROTOSS_COLOSSUS)).size();
    
    if((bcCount || fusionCoreExists || colossusExists) && !requireAntiAir){
        requireAntiAir = true;
        logger.tag("require_anti_air");
    }

    if(gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_SPIRE)).size() >= 1)
        spireExists = true;
    
    if((mutaCount >= 4 && !requireAntiAir) || (spireExists && !requireAntiAir)){ // TODO: muta threshold probably needs tuning
        requireAntiAir = true;
        logger.tag("require_anti_air");
    }
}

void InformationManager::checkIncome(){
    const sc2::ScoreDetails score = gInterface->observation->GetScore().score_details;
    float currIncome = score.collection_rate_minerals;
    for(int i = income.size()-1; i > 0; i--){
        if(income[i] == income[i-1])
            continue;
        else
            income[i] = income[i-1];
    }
    income[0] = currIncome;

    // if our income has gotten smaller and we have quite a bit of long distance miners and a cc isnt already in production,
    // then we should prioritise expansion
    sc2::Units ccs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER));
    bool ccInProduction = false;
    for(auto& cc : ccs){
        if(cc->build_progress < 1.0){
            ccInProduction = true;
            break;
        }
    }
    if(
        income[0] < income[income.size()-1] &&
        gInterface->wm->getNumWorkers(JOB_LONGDISTANCE_MINE) > 10 &&
        !requireExpansion &&
        !ccInProduction
    ){
        requireExpansion = true;
        logger.infoInit().withStr("income diff from 1000 loops ago:")
            .withFloat(income[0]-income[income.size()-1])
            .withStr("and").withInt(gInterface->wm->getNumWorkers(JOB_LONGDISTANCE_MINE))
            .withStr("long distance miners")
            .chat(true);
    }
    else if(requireExpansion && ccInProduction){
        // reset requireExpansion only when we have a cc in production
        requireExpansion = false;
    }

}