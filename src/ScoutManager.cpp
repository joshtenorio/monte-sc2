#include "ScoutManager.h"


void ScoutManager::OnGameStart(){
    scoutTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SCV);
    scoutTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_REAPER);
}

void ScoutManager::OnStep(){
    if(gInterface->observation->GetGameLoop() % 2000 == 0 && gInterface->observation->GetGameLoop() >= 3000)
        createScoutingMission();

}

void ScoutManager::OnUnitDestroyed(const sc2::Unit* unit_){
    // if unit is scout then remove from scouts
}

void ScoutManager::OnUnitEnterVision(const sc2::Unit* unit_){
    // 1. if it isn't within any of our scouts' vision, we don't care about it
    bool foundByScout = false;
    for(auto& s : scouts){

    }
    if(!foundByScout) return;
    // 2. if the unit has a large radius (>= 1.5 perhaps? need to research), isnt flying,
    // and its position is close to any neutral expansion location, assign that expansion location to enemy ownership
}

bool ScoutManager::createScoutingMission(){
    std::cout << "creating scouting mission\n";
    // 1. get a unit that should scout (scv, reaper, and maybe hellion?)
    // scv is eligible to scout if it is free
    // other scout units are eligible if they are idle
    // if we cant find any then just return false
    const sc2::Unit* scout = nullptr;
    sc2::Units scoutPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(scoutTypes));
    if(scoutPool.empty()) return false;
    std::cout << "spool not empty\n";
    for(auto& s : scoutPool){
        bool scoutFound = false;
        switch(s->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_SCV:
                if(gInterface->wm->isFree(gInterface->wm->getWorker(s))){
                    scout = s;
                    scoutFound = true;
                }
            break;
            default:
                if(s->orders.empty()){
                    scout = s;
                    scoutFound = true;
                }
            break;
        }
        if(scoutFound) break;
    }
    if(scout == nullptr) return false;

    // 2. iterate through expansions and scouts: scout the furthest neutral expansion that isn't being scouted
    // the "iterate through scouts" probably isnt necessary since we only create a scouting mission if scouts.empty()
    sc2::Point2D target;
    for(int n = gInterface->map->numOfExpansions() - 1; n > 0; n--){
        std::cout << "checking expansion " << n << std::endl;
        bool alreadyScouting = false;
        Expansion* e = gInterface->map->getNthExpansion(n);
        if(e == nullptr) continue;
        if(e->ownership == OWNER_ENEMY || e->ownership == OWNER_SELF) continue;

        for(auto& s : scouts)
            if(s.target == e->baseLocation){
                alreadyScouting = true;
                break;
            }
        
        // expansion isn't being scouted so send a scout there
        if(!alreadyScouting){
            target = e->baseLocation;
            break;
        }
    }

    // 3. send scout
    // TODO: should we use move or attack move?
    std::cout << "tag: " << scout->tag << "\ttarget: (" << target.x << ", " << target.y << ")\tscout type: " << scout->unit_type.to_string() << std::endl;
    Scout s;
    s.u = scout;
    s.tag = scout->tag;
    s.target = target;
    if(s.u->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV)
        gInterface->wm->getWorker(s.u)->job = JOB_SCOUTING;
    scouts.emplace_back(s);
    gInterface->actions->UnitCommand(s.u, sc2::ABILITY_ID::MOVE_MOVE, s.target);
    return true;
}

bool ScoutManager::removeScout(sc2::Tag tag){
    return true;
}
