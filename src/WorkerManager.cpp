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
}

bool WorkerManager::DistributeWorkers(int g_workers){
    
}

Unit* FindNearestMineralPatch(const Point2D& start){
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
}