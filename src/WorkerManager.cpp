/**
 * @file WorkerManager.cpp
 * @author Joshua Tenorio
 * 
 * Implementation of the Worker Manager class.
 */
#include "WorkerManager.h"

using namespace sc2;

void WorkerManager::OnStep(){
    DistributeWorkers();
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
    Units units = gInterface->observation->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for(const auto& u : units){
        if(u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD)
            if(DistanceSquared3D(u->pos, start) < distance){
                distance = DistanceSquared3D(u->pos, start);
                target = u;
            }
    } // end for loop
    return target;
}

