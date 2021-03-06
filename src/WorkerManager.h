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
#define JOB_SCOUTING            7
#define JOB_LONGDISTANCE_MINE   8

typedef struct Worker_s_t {
    int job;
    sc2::Tag tag;
    const sc2::Unit* getUnit();
} Worker;

class WorkerManager : public Manager {
    public:
    WorkerManager(); // empty constructor

    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitIdle(const sc2::Unit* unit_);
    void OnBuildingConstructionComplete(const sc2::Unit* building_);

    void DistributeWorkers(int gasWorkers = 3);
    const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start); // TODO: iirc this is unused, need to remove at some point

    // TODO: make a function that returns a list of Workers
    // return a pointer to the first unemployed worker in list
    Worker* getUnemployedWorker();

    // free worker is defined as a worker who is either unemployed or gathering minerals
    Worker* getFreeWorker();
    Worker* getWorker(const sc2::Unit* unit_); // get a pointer to a Worker object by Unit*
    Worker* getWorker(sc2::Tag tag_);
    Worker* getNthWorker(size_t n);
    // TODO: add implementation for getting closest worker who has a specific job
    Worker* getClosestWorker(sc2::Point2D pos, int jobType = -1);
    int getNumWorkers(int jobType = -1);
    bool isFree(Worker* w);

    protected:
    void printDebug();
    std::vector<Worker> workers;
};