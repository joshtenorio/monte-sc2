/**
 * @file api.h
 * @author Joshua Tenorio
 * 
 * Contains various interfaces for use by managers,
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

using namespace sc2;


namespace API {

    int countIdleUnits(sc2::UNIT_TYPEID type);
    bool isUnitIdle(const sc2::Unit* unit);
    size_t CountUnitType(UNIT_TYPEID unitType);
    
    // get the closest n units within radius r of loc
    sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, sc2::UNIT_TYPEID unitType = sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR);

    // convert a build ABILITY_ID to UNIT_TYPEID, e.g. BUILD SUPPLY DEPOT -> SUPPLY DEPOT
    sc2::UNIT_TYPEID abilityToUnitTypeID(sc2::ABILITY_ID ability); // note: won't return refinery rich if it is a rich geyser
    sc2::ABILITY_ID unitTypeIDToAbilityID(sc2::UNIT_TYPEID unit);
    sc2::ABILITY_ID upgradeIDToAbilityID(sc2::UpgradeID upgrade);
    sc2::UNIT_TYPEID buildingForUnit(sc2::ABILITY_ID unit);

    bool isStructure(sc2::UNIT_TYPEID unit);
    bool isTownHall(sc2::UNIT_TYPEID unit);


} // end namespace API
class Interface {
    public:
    Interface(const ObservationInterface* observation_,
        ActionInterface* actions_,
        QueryInterface* query_,
        DebugInterface* debug_,
        WorkerManager* wm_,
        Mapper* map_,
        int matchID_){
            observation = observation_;
            actions = actions_;
            query = query_;
            debug = debug_;
            wm = wm_;
            map = map_;
            matchID = matchID_;
        };
    
    const ObservationInterface* observation;
    ActionInterface* actions;
    QueryInterface* query;
    DebugInterface* debug;
    WorkerManager* wm;
    Mapper* map;
    int matchID;
};

extern std::unique_ptr<Interface> gInterface;