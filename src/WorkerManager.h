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
#include <limits>
#include "Manager.h"

// for Worker.job
#define JOB_UNEMPLOYED          0
#define JOB_GATHERING_MINERALS  1
#define JOB_GATHERING_GAS       2
#define JOB_BUILDING            3
#define JOB_BUILDING_GAS        4
#define JOB_FIGHTING            5
#define JOB_REPAIRING           6

using namespace sc2;
typedef struct Worker_s_t {
    int job;
    const Unit* scv;
} Worker;

class WorkerManager : public Manager {
    public:
    WorkerManager() {}; // empty constructor

    void OnStep();
    void OnUnitCreated(const Unit* unit_);
    void OnUnitDestroyed(const Unit* unit_);
    void OnUnitIdle(const Unit* unit_);

    bool DistributeWorkers(int gasWorkers = 3);
    const Unit* FindNearestMineralPatch(const Point3D& start);

    // return a pointer to the first unemployed worker in list
    Worker* getUnemployedWorker();
    Worker* getWorker(const Unit* unit_); // get a pointer to a Worker object by Unit*
    Worker* getNthWorker(size_t n);
    // TODO: add implementation for getting closest worker who has a specific job
    Worker* getClosestWorker(sc2::Point2D pos, int jobType = -1);

    protected:
    std::vector<Worker> workers;
};