#include "InformationManager.h"

void InformationManager::OnStep(){
    checkForWorkerRush();

    // update expansions well after mapper has initialized
    if(gInterface->observation->GetGameLoop() % 30 == 0 && gInterface->observation->GetGameLoop() >= 3000)
        updateExpoOwnership();
}

ProductionConfig InformationManager::updateProductionConfig(ProductionConfig currentPConfig){
    return currentPConfig; //tmp
}

CombatConfig InformationManager::updateCombatConfig(CombatConfig currentCConfig){
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