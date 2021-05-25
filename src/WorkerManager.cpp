/**
 * @file WorkerManager.cpp
 * @author Joshua Tenorio
 * 
 * Implementation of the Worker Manager class.
 */
#include "WorkerManager.h"

#include "api.h" // this breaks our "code style," but it is necessary for now to avoid circular definition
// TODO: is there a way to avoid having to break our style?

using namespace sc2;

void WorkerManager::OnStep(){

    // run distributeWorkers every 15 loops
    if(gInterface->observation->GetGameLoop() % 15 == 0)
        DistributeWorkers();
}

void WorkerManager::OnUnitCreated(const Unit* unit_){
    Worker w;
    w.job = JOB_UNEMPLOYED;
    w.scv = unit_;
    w.tag = (*unit_).tag;
    workers.emplace_back(w);
}

void WorkerManager::OnUnitDestroyed(const Unit* unit_){
    Tag key = unit_->tag;
    std::cout << "erasing worker tag" << unit_->tag << std::endl;
    for(auto itr = workers.begin(); itr != workers.end(); ){
        if((*itr).scv->tag == key)
            itr = workers.erase(itr);
        else ++itr;
    }
    
}

void WorkerManager::OnUnitIdle(const sc2::Unit* unit_){
    // send to mine at the closest base
    Expansion* e = gInterface->map->getCurrentExpansion();
    if (e != nullptr){
        const sc2::Unit* mineralTarget = e->mineralLine.front();
        gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::SMART, mineralTarget);
        getWorker(unit_)->job = JOB_GATHERING_MINERALS;
    }
    else{
        std::cout << "e is null ptr !!!\n";
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
        if(r->assigned_harvesters < gasWorkers){
            // add one worker at a time
            Worker* w = getFreeWorker();
            gInterface->actions->UnitCommand(w->scv, sc2::ABILITY_ID::SMART, r);
            w->job = JOB_GATHERING_GAS;
        }
    }

    // 2. send to next base if base is overfull
    sc2::Units ccs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
    for(auto& cc : ccs){
        if(cc->assigned_harvesters > cc->ideal_harvesters){
            Worker* w = getClosestWorker(cc->pos, JOB_GATHERING_MINERALS);
            
            // grab the next base in general
            Expansion* e = gInterface->map->getClosestExpansion(cc->pos);
            int n = 0;
            Expansion* next = gInterface->map->getNthExpansion(n);
            while(!(e == next)){
                n++;
                next = gInterface->map->getNthExpansion(n);
            }
            // n is the current expansion, so send worker to expansion n + 1
            e = gInterface->map->getNthExpansion(n+1);
            
            if(e != nullptr){
                const sc2::Unit* mineral = e->mineralLine.front();
                gInterface->actions->UnitCommand(w->scv, sc2::ABILITY_ID::SMART, mineral);
                w->job = JOB_GATHERING_MINERALS;
            }
        } // end if cc->assigned_harvesters > cc->ideal_harvesters
    } // end for cc : ccs
}

const Unit* WorkerManager::FindNearestMineralPatch(const Point3D& start){
    Units units = gInterface->observation->GetUnits(Unit::Alliance::Neutral, IsMineralPatch());
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for(const auto& u : units){
        if(DistanceSquared3D(u->pos, start) < distance){
            distance = DistanceSquared3D(u->pos, start);
            target = u;
        }
    } // end for loop
    return target;
}

Worker* WorkerManager::getUnemployedWorker(){
    for(auto& w : workers)
        if(w.job == JOB_UNEMPLOYED) return &w;
    
    // no unemployed workers found
    return nullptr;
}

Worker* WorkerManager::getFreeWorker(){
    for(auto& w : workers)
        if(isFree(&w)) return &w;
    
    return nullptr;
}

Worker* WorkerManager::getWorker(const Unit* unit_){
    Tag key = unit_->tag;
    for(auto& w : workers){
        if(w.scv->tag == key)
            return &w;
    }
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