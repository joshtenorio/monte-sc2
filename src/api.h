/**
 * @file api.h
 * @author Joshua Tenorio
 * 
 * Contains the Observation and Action interfaces for use by managers
 */

#pragma once
#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_interfaces.h>
#include <sc2api/sc2_unit_filters.h>
#include <memory>
#include "WorkerManager.h"
#include "Mapper.h"

using namespace sc2;

//class WorkerManager;

namespace API {
    size_t CountUnitType(UNIT_TYPEID unitType);
} // end namespace API
class Interface {
    public:
    Interface(const ObservationInterface* observation_,
        ActionInterface* actions_,
        QueryInterface* query_,
        DebugInterface* debug_,
        WorkerManager* wm_,
        Mapper* map_){
            observation = observation_;
            actions = actions_;
            query = query_;
            debug = debug_;
            wm = wm_;
            map = map_;
        };
    
    const ObservationInterface* observation;
    ActionInterface* actions;
    QueryInterface* query;
    DebugInterface* debug;
    WorkerManager* wm;
    Mapper* map;
};

extern std::unique_ptr<Interface> gInterface;