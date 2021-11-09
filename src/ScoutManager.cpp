#include "ScoutManager.h"

ScoutManager::ScoutManager(){
    logger = Logger("ScoutManager");
}

void ScoutManager::OnGameStart(){
    scoutTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_SCV);

    // removed reaper from scout types because we are micro-ing them now
    //scoutTypes.emplace_back(sc2::UNIT_TYPEID::TERRAN_REAPER);

    // initializing visitTable
    visitTable.reserve(17);
    for(int n = 0; n < 17; n++){ // could we do n < visitTable.max_size() instead?
        visitTable.emplace_back(0);
    }
    
}

void ScoutManager::OnStep(){

    if(gInterface->observation->GetGameLoop() % 900 == 0 && gInterface->observation->GetGameLoop() >= 3000)
        createScoutingMission();
        

}

void ScoutManager::OnUnitDestroyed(const sc2::Unit* unit_){
    // if unit is scout then remove from scouts
    if(unit_->alliance == sc2::Unit::Alliance::Self){
        removeScout(unit_->tag);
    }
    else if(unit_->alliance == sc2::Unit::Alliance::Enemy && API::isTownHall(unit_->unit_type.ToType())){
        gInterface->map->setExpansionOwnership(unit_->pos, OWNER_NEUTRAL);
    }
    
}

void ScoutManager::OnUnitEnterVision(const sc2::Unit* unit_){

}

bool ScoutManager::createScoutingMission(){
    logger.infoInit().withStr("Creating scouting mission").write();
    // 1. get a unit that should scout (scv, reaper, and maybe hellion?)
    // scv is eligible to scout if it is free
    // other scout units are eligible if they are idle
    // if we cant find any then just return false
    const sc2::Unit* scout = nullptr;
    sc2::Units scoutPool = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnits(scoutTypes));
    if(scoutPool.empty()) return false;
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

    // 2. iterate through expansions and pick the one we've been to the least, starting from the furthest
    // the "iterate through scouts" probably isnt necessary since we only create a scouting mission if scouts.empty()
    int expNumber = gInterface->map->numOfExpansions() - 1;
    sc2::Point2D target = gInterface->map->getNthExpansion(expNumber)->baseLocation;
    for(int n = expNumber; n >= 0; n--){
        Expansion* e = gInterface->map->getNthExpansion(n);
        if(e == nullptr) continue;
        if(e->ownership == OWNER_SELF) continue;
        
        if(visitTable[n] < visitTable[expNumber]){
            logger.infoInit().withStr("found suitable scout target at expo").withInt(n).withPoint(e->baseLocation).write();
            expNumber = n;
            target = e->baseLocation;
        }
    }

    // 3. send scout
    logger.infoInit().withStr("Scout:").withUnit(scout).withStr("\tTarget:").withPoint(target).write();
    Scout s;
    s.u = scout;
    s.tag = scout->tag;
    s.target = target;
    if(s.u->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV)
        gInterface->wm->getWorker(s.u)->job = JOB_SCOUTING;
    scouts.emplace_back(s);
    visitTable[expNumber]++;
    logger.infoInit().withStr("we are visitng expansion number").withInt(expNumber).withStr("for the").withInt(visitTable[expNumber]).withStr("time").write();
    gInterface->actions->UnitCommand(s.u, sc2::ABILITY_ID::MOVE_MOVE, s.target);
    return true;
}

bool ScoutManager::removeScout(sc2::Tag tag){
    for(auto itr = scouts.begin(); itr != scouts.end(); ){
        if(tag == (*itr).tag){
            if((*itr).u != nullptr){
                logger.infoInit().withStr("Removing scout").withUnit((*itr).u).write();
                if((*itr).u->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_SCV)
                    gInterface->wm->getWorker((*itr).u)->job = JOB_UNEMPLOYED;
            }
            else
                logger.errorInit().withStr("Tried to remove scout but unit* was nullptr").write();
            

            itr = scouts.erase(itr);
            return true;
        } // end if tag == itr tag
        else ++itr;
    }
    return false;
}

