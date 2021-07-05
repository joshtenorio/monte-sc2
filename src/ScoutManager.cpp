#include "ScoutManager.h"

void ScoutManager::OnStep(){
    if(gInterface->observation->GetGameLoop() % 3000 == 0 && scouts.empty())
        createScoutingMission();

}

void ScoutManager::OnUnitDestroyed(const Unit* unit_){

}

void ScoutManager::OnUnitEnterVision(const sc2::Unit* unit_){
    // 1. if it isn't within any of our scouts' vision, we don't care about it

    // 2. if the unit has a large radius (>= 1.5 perhaps? need to research), isnt flying,
    // and its position is close to any neutral expansion location, assign that expansion location to enemy ownership
}

bool ScoutManager::createScoutingMission(){
    // 1. get a unit that should scout (scv, reaper, or hellion)
    // if we cant find any then just return false

    // 2. iterate through expansions and scouts, and figure out where we should scout next
    // the "iterate through scouts" probably isnt necessary since we only create a scouting mission if scouts.empty()
    return true;
}

bool ScoutManager::sendScout(){
    return true;
}
