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
#include <sc2api/sc2_gametypes.h>
#include <sc2api/sc2_typeenums.h>
#include <vector>
#include <memory>
#include "VectorMath.h"
#include "WorkerManager.h"
#include "Mapper.h"
#include "Debug.h"
#include "Strategy.h"

namespace API {

    void OnGameStart();

    sc2::Point3D toPoint3D(sc2::Point2D p);
    int countIdleUnits(sc2::UNIT_TYPEID type);
    bool isUnitIdle(const sc2::Unit* unit);
    size_t CountUnitType(sc2::UNIT_TYPEID unitType);
    int countReadyUnits(sc2::UNIT_TYPEID type);
    int countUnitType(sc2::Filter filter);

    // get the closest n units within radius r of loc
    // TODO: this should be sorted
    sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, sc2::UNIT_TYPEID unitType = sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR);
    sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, std::vector<sc2::UNIT_TYPEID> unitTypes);
    sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, sc2::Filter filter);

    // get a building's radius by its ability id
    float getStructureRadiusByAbility(sc2::ABILITY_ID ability);

    // convert a build ABILITY_ID to UNIT_TYPEID, e.g. BUILD SUPPLY DEPOT -> SUPPLY DEPOT
    sc2::UNIT_TYPEID abilityToUnitTypeID(sc2::ABILITY_ID ability); // note: won't return refinery rich if it is a rich geyser
    sc2::ABILITY_ID unitTypeIDToAbilityID(sc2::UNIT_TYPEID unit); // TODO: add more cases
    sc2::ABILITY_ID upgradeIDToAbilityID(sc2::UpgradeID upgrade);
    sc2::UNIT_TYPEID getProducer(sc2::ABILITY_ID unit);
    bool requiresTechLab(sc2::ABILITY_ID unit);

    bool isTownHall(sc2::UNIT_TYPEID unit);
    bool isAddon(sc2::UNIT_TYPEID unit);
    bool isFlying(const sc2::Unit& u);
    bool isWorker(sc2::UNIT_TYPEID unit);

    std::vector<sc2::UNIT_TYPEID> getTechRequirements(sc2::ABILITY_ID ability);

    inline sc2::UnitTypeID ToUnitTypeID(sc2::UNIT_TYPEID id_) {
        return static_cast<sc2::UnitTypeID>(id_);
    }

    inline sc2::UpgradeID ToUpgradeID(sc2::UPGRADE_ID id_) {
        return static_cast<sc2::UpgradeID>(id_);
    }

    inline sc2::AbilityID ToAbilityID(sc2::ABILITY_ID id_) {
        return static_cast<sc2::AbilityID>(id_);
    }
} // end namespace API

class Interface {
    public:
    Interface(const sc2::ObservationInterface* observation_,
        sc2::ActionInterface* actions_,
        sc2::QueryInterface* query_,
        Monte::Debug* debug_,
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
    
    const sc2::ObservationInterface* observation;
    sc2::ActionInterface* actions;
    sc2::QueryInterface* query;
    Monte::Debug* debug;
    WorkerManager* wm;
    Mapper* map;
    int matchID;
};

extern std::unique_ptr<Interface> gInterface;