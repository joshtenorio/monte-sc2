#pragma once

#include <map>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"
#include "Strategy.h"
#include "ScoutManager.h"
#include "InfluenceMap.h"
#include "combat/Squad.h"


class CombatCommander : public Manager {
    public:
    // constructors
    CombatCommander();
    // TODO: add a constructor with strategy

    void OnGameStart();
    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    void OnUnitEnterVision(const sc2::Unit* unit_);

    void handleChangelings();

    CombatConfig& getCombatConfig();
    
    bool isUnitInSquad(sc2::Tag tag);

    protected:
    ScoutManager sm;
    CombatConfig config;

    std::map<std::string, Squad> squads;
    Squad mainArmy;

    Monte::InfluenceMap groundMap;
    Monte::InfluenceMap airMap;
};