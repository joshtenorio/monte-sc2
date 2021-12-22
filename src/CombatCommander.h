#pragma once

#include <map>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"
#include "Strategy.h"
#include "ScoutManager.h"
#include "combat/InfluenceMap.h"
#include "combat/Squad.h"


class CombatCommander : public Manager {
    public:
    // constructors
    CombatCommander();
    CombatCommander(Strategy* strategy_);

    void OnGameStart();
    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    void OnUnitEnterVision(const sc2::Unit* unit_);

    void setLocationTarget(sc2::Point2D loc);
    void setLocationDefense(sc2::Point2D loc);
    void setHarassTarget(sc2::Point2D loc);

    void handleChangelings();

    CombatConfig& getCombatConfig();
    
    bool isUnitInSquad(sc2::Tag tag);

    protected:
    ScoutManager sm;
    CombatConfig config;
    Strategy* strategy;

    //std::map<std::string, Squad> squads;
    Squad mainArmy;
    Squad harassGroup;

    Monte::InfluenceMap groundMap;
    Monte::InfluenceMap airMap;

    sc2::Point2D attackTarget;
    sc2::Point2D defenseTarget;
    sc2::Point2D harassTarget;
};