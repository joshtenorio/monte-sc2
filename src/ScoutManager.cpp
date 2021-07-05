#include "ScoutManager.h"


void ScoutManager::OnGameStart(){
    scoutTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SCV);
    scoutTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_REAPER);
}

void ScoutManager::OnStep(){
    if(gInterface->observation->GetGameLoop() % 3000 == 0 && scouts.empty())
        createScoutingMission();

}

void ScoutManager::OnUnitDestroyed(const sc2::Unit* unit_){

}

void ScoutManager::OnUnitEnterVision(const sc2::Unit* unit_){
    // 1. if it isn't within any of our scouts' vision, we don't care about it

    // 2. if the unit has a large radius (>= 1.5 perhaps? need to research), isnt flying,
    // and its position is close to any neutral expansion location, assign that expansion location to enemy ownership
}

bool ScoutManager::createScoutingMission(){
    std::cout << "creating scouting mission\n";
    // 1. get a unit that should scout (scv, reaper, and maybe hellion?)
    // if we cant find any then just return false
    const sc2::Unit* scout = nullptr;
    sc2::Units scoutPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(scoutTypes));
    if(scoutPool.empty()) return false;
    
    for(auto& s : scoutPool){
        switch(s->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_SCV:
                if(gInterface->wm->isFree(gInterface->wm->getWorker(s)))
                    scout = s;
            break;
            default:
                if(s->orders.empty())
                    scout = s;
            break;
        }
    }
    if(scout == nullptr) return false;

    // 2. iterate through expansions and scouts, and figure out where we should scout next
    // the "iterate through scouts" probably isnt necessary since we only create a scouting mission if scouts.empty()
    return true;
}

bool ScoutManager::sendScout(){
    return true;
}
