/**
 * @file WorkerManager.cpp
 * @author Joshua Tenorio
 * 
 * Implementation of the Worker Manager class.
 */
#include "WorkerManager.h"

using namespace sc2;

void WorkerManager::OnStep(){
    DistributeWorkers(0); // TODO: only have this function run every x steps instead of every step
}

void WorkerManager::OnUnitCreated(const Unit* unit_){
    Worker w;
    w.job = JOB_UNEMPLOYED;
    w.scv = unit_;
    workers.emplace_back(w);
    std::cout << workers.size() << "number of workers\n";
}

void WorkerManager::OnUnitDestroyed(const Unit* unit_){
    Tag key = unit_->tag;
    for(auto itr = workers.begin(); itr != workers.end(); ){
        if((*itr).scv->tag == key)
            itr = workers.erase(itr);
        else ++itr;
    }
    std::cout << workers.size() << "number of workers after oopsy\n";
}

void WorkerManager::OnUnitIdle(const sc2::Unit* unit_){
    // just using tutorial code for now, gas not yet taken into consideration
    // if unit idle send to closest mineral
    const Unit* mineralTarget = FindNearestMineralPatch(unit_->pos);
    if(!mineralTarget) return;
    gInterface->actions->UnitCommand(unit_, ABILITY_ID::SMART, mineralTarget);
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

Worker* WorkerManager::getWorker(const Unit* unit_){
    Tag key = unit_->tag;
    for(auto& w : workers){
        if(w.scv->tag == key)
            return &w;
    }
    return nullptr;
}