#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"
#include "Strategy.h"
#include "ScoutManager.h"
#include "combat/AttackSquad.h"

namespace Monte {
    enum TankState {
        Null = -1,
        Unsieged,
        Sieging,
        Sieged,
        Unsieging
    };

    typedef struct Tank_s_t {
        Tank_s_t(sc2::Tag tag_) { tag = tag_; state = Monte::TankState::Null; };
        sc2::Tag tag;
        Monte::TankState state;
    } Tank;


} // end namespace Monte

class CombatCommander : public Manager {
    public:
    // constructors
    CombatCommander() { sm = ScoutManager(); logger = Logger("CombatCommander"); };
    // TODO: add a constructor with strategy, bc we need CombatConfig from strategy
    CombatCommander(Strategy* strategy_) { config = strategy_->getCombatConfig(); sm = ScoutManager(); logger = Logger("CombatCommander"); };

    void OnGameStart();
    void OnStep();
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    void OnUnitEnterVision(const sc2::Unit* unit_);

    void marineOnStep();
    void medivacOnStep();
    void siegeTankOnStep();
    void ravenOnStep(); // if nearby marine count is low, focus on putting down auto turrets, else use anti armor missles

    void handleChangelings();
    

    protected:
    ScoutManager sm;
    CombatConfig config;

    std::vector<Monte::Tank> tanks;

    // used for marine control
    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bio; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
};