/**
 * @file WorkerManager.cpp
 * @author Joshua Tenorio
 * 
 * Implementation of the Worker Manager class.
 */
#include "WorkerManager.h"

#include "api.h" // this breaks our "code style," but it is necessary for now to avoid circular definition
// TODO: is there a way to avoid this workaround ?

using namespace sc2;

void WorkerManager::OnStep(){

    // run distributeWorkers every 15 loops and after everything initializes
    if(gInterface->observation->GetGameLoop() % 15 == 0 && gInterface->observation->GetGameLoop() > 500){
        DistributeWorkers();
    }
        
    
}

void WorkerManager::OnUnitCreated(const Unit* unit_){
    Worker w;
    if(gInterface->observation->GetGameLoop() < 20) w.job = JOB_GATHERING_MINERALS;
    else w.job = JOB_UNEMPLOYED;
    w.scv = unit_;
    w.tag = (*unit_).tag;
    workers.emplace_back(w);
    if(gInterface->observation->GetGameLoop() > 100) OnUnitIdle(unit_);
}

void WorkerManager::OnUnitDestroyed(const Unit* unit_){
    Tag key = unit_->tag;
    int index = 0;
    for(int i = 0; i < workers.size(); i++){
        if(workers[i].scv->tag == key){
            index = i;
            break;
        }
    }
    workers.erase(workers.begin() + index);
}

void WorkerManager::OnUnitIdle(const sc2::Unit* unit_){
    // send to mine at the current base
    //Expansion* e = gInterface->map->getCurrentExpansion();
    Expansion* e = gInterface->map->getNthExpansion(0); // temporary
    if (e != nullptr){
        const sc2::Unit* mineralTarget = e->mineralLine.front();
        gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::SMART, mineralTarget);
        getWorker(unit_)->job = JOB_GATHERING_MINERALS;
    }
    else{
        logger.errorInit().withStr("e is nullptr").write();
    }
}

void WorkerManager::DistributeWorkers(int gasWorkers){
    // 0. sanity check gasWorkers
    if(gasWorkers > 3) gasWorkers = 3;
    else if(gasWorkers < 0) gasWorkers = 0;

    // 1. handle gas workers
    sc2::Units refineries = gInterface->observation->GetUnits(
        sc2::Unit::Alliance::Self,
        [](const sc2::Unit& unit){
            if(unit.build_progress < 1.0) return false;
            return unit.unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY ||
                unit.unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH;
        }
    );

    for(auto& r : refineries){
        if(r->ideal_harvesters <= 0 || r->vespene_contents <= 0) continue;

        if(r->assigned_harvesters < gasWorkers){
            // add one worker at a time
            Worker* w = getFreeWorker();
            if(w == nullptr) continue;
            gInterface->actions->UnitCommand(w->scv, sc2::ABILITY_ID::SMART, r);
            w->job = JOB_GATHERING_GAS;
        }
        else if (r->assigned_harvesters > gasWorkers){
            // too much gas workers
            Worker* w = getClosestWorker(r->pos, JOB_GATHERING_GAS);
            const sc2::Unit* mineral = FindNearestMineralPatch(w->scv->pos);
            gInterface->actions->UnitCommand(w->scv, sc2::ABILITY_ID::SMART, mineral);
            w->job = JOB_GATHERING_MINERALS;

        }
    }

    // 2. send to next base if base is overfull
    sc2::Units ccs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
    for(auto& cc : ccs){
        if(cc->assigned_harvesters > cc->ideal_harvesters){
            // worker to move
            Worker* w = getClosestWorker(cc->pos);

            // grab the next base in general
            Expansion* e = gInterface->map->getClosestExpansion(cc->pos);
            int n = 0;
            Expansion* next = gInterface->map->getNthExpansion(n);
            while(!(e == next)){
                n++;
                next = gInterface->map->getNthExpansion(n);
            }
            // n is the current expansion, so send worker to expansion n + 1
            if(n < gInterface->map->numOfExpansions()){
                e = gInterface->map->getNthExpansion(n+1);
            }
            
            if(e != nullptr){
                const sc2::Unit* mineral = FindNearestMineralPatch(e->baseLocation);
                gInterface->actions->UnitCommand(w->scv, sc2::ABILITY_ID::SMART, mineral);
                w->job = JOB_GATHERING_MINERALS;
            }
        } // end if cc->assigned_harvesters > cc->ideal_harvesters
    } // end for cc : ccs

    // 3. handle leftover workers that are unemployed/still idle
    for(auto& w : workers){
        if(w.scv->orders.empty()) w.job = JOB_UNEMPLOYED;
        if(w.job == JOB_UNEMPLOYED) OnUnitIdle(w.scv);
    }
}

const Unit* WorkerManager::FindNearestMineralPatch(const Point2D& start){
    Units units = gInterface->observation->GetUnits(Unit::Alliance::Neutral, IsMineralPatch());
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for(const auto& u : units){
        if(DistanceSquared2D(u->pos, start) < distance){
            distance = DistanceSquared2D(u->pos, start);
            target = u;
        }
    } // end for loop
    return target;
}

Worker* WorkerManager::getUnemployedWorker(){
    for(auto& w : workers)
        if(w.job == JOB_UNEMPLOYED){
            return &w;
        }
    
    // no unemployed workers found
    logger.errorInit().withStr("getUnemployedWorker is nullptr").write();
    return nullptr;
}

Worker* WorkerManager::getFreeWorker(){
    for(auto& w : workers)
        if(isFree(&w)){
            return &w;
        }
    return nullptr;
}

Worker* WorkerManager::getWorker(const Unit* unit_){
    Tag key = unit_->tag;
    for(auto& w : workers){
        if(w.scv->tag == key){
            return &w; 
        }
            
    }
    logger.errorInit().withStr("getWorker is nullptr").write();
    return nullptr;
}

Worker* WorkerManager::getWorker(sc2::Tag tag_){
    for(auto& w : workers){
        if(tag_ == w.tag)
            return &w;
    }
    logger.errorInit().withStr("getWorker is nullptr").write();
    return nullptr;
}

Worker* WorkerManager::getNthWorker(size_t n){
    return &(workers[n]);
}

Worker* WorkerManager::getClosestWorker(sc2::Point2D pos, int jobType){
    Worker* closestWorker = nullptr;
    float distance = std::numeric_limits<float>::max();
    if(jobType == -1){ // default: get any worker
        for(auto& w : workers){
            if(distance > sc2::DistanceSquared2D(pos, w.scv->pos)){
                distance = sc2::DistanceSquared2D(pos, w.scv->pos);
                closestWorker = &w;
            }
        }
        return closestWorker;
    }
    else{
        for(auto& w : workers){
            if(distance > sc2::DistanceSquared2D(pos, w.scv->pos) && w.job == jobType){
                distance = sc2::DistanceSquared2D(pos, w.scv->pos);
                closestWorker = &w;
            }
        }
        return closestWorker;
    }
}

int WorkerManager::getNumWorkers(){
    return workers.size();
}

bool WorkerManager::isFree(Worker* w){
    if(
        w->job == JOB_GATHERING_MINERALS ||
        w->job == JOB_UNEMPLOYED
    ) return true;
    else return false;
}