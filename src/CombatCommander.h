#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"
#include "ScoutManager.h"

class CombatCommander : public Manager {
    public:
    // constructors
    CombatCommander() { sm = ScoutManager(); };
    // TODO: add a constructor with strategy

    void OnGameStart();
    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    void OnUnitEnterVision(const sc2::Unit* unit_);

    void marineOnStep();
    void medivacOnStep();
    void manageStim(const sc2::Unit* unit);
    

    protected:
    ScoutManager sm;

    // used for marine control
    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bio; // filter for GetUnits
    int foo;
};