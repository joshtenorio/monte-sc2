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
    DistributeWorkers(0); // TODO: only have this function run every x steps instead of every step
}

void WorkerManager::OnUnitCreated(const Unit* unit_){
    Worker w;
    w.job = JOB_UNEMPLOYED;
    w.scv = unit_;
    w.tag = unit_->tag;
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
    // just using tutorial code for now, gas not yet taken into consideration
    // if unit idle send to closest mineral
    const Unit* mineralTarget = FindNearestMineralPatch(unit_->pos);
    if(!mineralTarget) return;
    gInterface->actions->UnitCommand(unit_, ABILITY_ID::SMART, mineralTarget);
    getWorker(unit_)->job = JOB_GATHERING_MINERALS;
}

bool WorkerManager::DistributeWorkers(int gasWorkers){
    return true;
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
    for(auto& w : workers){
        if(distance > sc2::DistanceSquared2D(pos, w.scv->pos)){
            distance = sc2::DistanceSquared2D(pos, w.scv->pos);
            closestWorker = &w;
        }
    }
    return closestWorker;
}