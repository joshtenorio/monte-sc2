#include "InformationManager.h"

InformationManager::InformationManager(){
    logger = Logger("InformationManager");
    
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
    checkForWorkerRush();

    if(gInterface->observation->GetGameLoop() % 30 == 0)
        checkForMassAir();

    // update expansions well after mapper has initialized
    if(gInterface->observation->GetGameLoop() % 30 == 0 && gInterface->observation->GetGameLoop() >= 3000)
        updateExpoOwnership();
}

ProductionConfig InformationManager::updateProductionConfig(ProductionConfig& currentPConfig){
    
    if(requireAntiAir){
        currentPConfig.buildTurrets = true;
    }


    return currentPConfig; // TODO: since we pass in the reference to the current config, do we need this?
}

CombatConfig InformationManager::updateCombatConfig(CombatConfig& currentCConfig){
    return currentCConfig; //tmp
}

void InformationManager::updateExpoOwnership(){
    sc2::Units enemyTownHalls = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsTownHall());
    for(auto& th : enemyTownHalls){
        Expansion* closest = gInterface->map->getClosestExpansion(th->pos);
        if(closest == nullptr) return;
        else
            closest->ownership = OWNER_ENEMY;
    }
}

void InformationManager::checkForWorkerRush(){
    // if we have 4 or more completed buildings, don't check for worker rush

    // pick a location to search for enemy workers from -> ideally a worker closest to nat or the supply depot at the ramp

    // if we count more than 3 workers, activate worker rush defense code (ie set workerRushDetected to true)
    
}

void InformationManager::checkForEnemyCloak(){
    // if we find a dangerous cloaked enemy (e.g. not an observer), set requireDetectors to true
    // also we should send a chat message and temporarily a Tag as well

}

void InformationManager::checkForMassAir(){
    // check for mutacount >= 8, or spire, etc
    // TODO: also check for broodlords, etc
    mutaCount = gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_MUTALISK)).size();

    if(gInterface->observation->GetUnits(sc2::Unit::Alliance::Enemy, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_SPIRE)).size() >= 1)
        spireExists = true;
    
    if((mutaCount >= 8 && !requireAntiAir) || spireExists){ // TODO: muta threshold probably needs tuning
        requireAntiAir = true;
        logger.tag("require_anti_air");
    }
}