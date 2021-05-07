/**
 * @file WorkerManager.h
 * @author Joshua Tenorio
 * 
 * Manages workers across all bases.
 */
#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include <vector>
#include "api.h"
#include "Manager.h"

using namespace sc2;

class WorkerManager : public Manager {
    protected:
    std::vector<Unit> workers;

    public:
    WorkerManager() {}; // empty constructor

    void OnStep();
    void OnUnitDestroyed();
    void OnUnitIdle(const Unit* unit_);

    bool DistributeWorkers(int gasWorkers = 3);
    const Unit* FindNearestMineralPatch(const Point3D& start);
};