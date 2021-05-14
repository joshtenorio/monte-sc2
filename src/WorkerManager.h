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

// for Worker.job
#define GATHERING_MINERALS 0
#define GATHERING_GAS 1
#define BUILDING 2
#define BUILDING_GAS 3
#define FIGHTING 4
#define REPAIRING 5

using namespace sc2;
typedef struct Worker_s_t {
    int job;
    const Unit* scv;
} Worker;

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