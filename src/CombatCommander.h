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
    CombatCommander() { sm = ScoutManager(); logger = Logger("CombatCommander"); };
    // TODO: add a constructor with strategy

    void OnGameStart();
    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    void OnUnitEnterVision(const sc2::Unit* unit_);

    void medivacOnStep();
    void siegeTankOnStep();
    void ravenOnStep(); // if nearby marine count is low, focus on putting down auto turrets, else use anti armor missles
    

    protected:
    ScoutManager sm;

    // used for marine control
    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bio; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
};