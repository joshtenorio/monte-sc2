/**
 * @file api.h
 * @author Joshua Tenorio
 * 
 * Contains the Observation and Action interfaces for use by managers,
 * and mundane helper functions
 */

#pragma once
#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_interfaces.h>
#include <sc2api/sc2_unit_filters.h>
#include <memory>
#include "WorkerManager.h"
#include "Mapper.h"
#include "Strategy.h"

#define ABIL_BUILD      0
#define ABIL_TRAIN      1
#define ABIL_RESEARCH   2
#define ABIL_MORPH      3 // includes building addons
#define ABIL_NULL       -1
using namespace sc2;


namespace API {
    size_t CountUnitType(UNIT_TYPEID unitType);

    // convert a build ABILITY_ID to UNIT_TYPEID, e.g. BUILD SUPPLY DEPOT -> SUPPLY DEPOT
    sc2::UNIT_TYPEID abilityToUnitTypeID(sc2::ABILITY_ID ability); // note: won't return refinery rich if it is a rich geyser
    sc2::ABILITY_ID unitTypeIDToAbilityID(sc2::UNIT_TYPEID unit);
    sc2::ABILITY_ID upgradeIDToAbilityID(sc2::UpgradeID upgrade);

    bool isStructure(sc2::UNIT_TYPEID unit);

    char parseStep(Step s);

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