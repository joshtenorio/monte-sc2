#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"
#include "ScoutManager.h"
#include "InfluenceMap.h"

namespace Monte {
    enum class TankState {
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

    enum class ReaperState {
        Null = -1,
        Init,
        Attack,
        Kite,
        Move,
        Bide
    };

    typedef struct Reaper_s_t{
        Reaper_s_t(sc2::Tag tag_) { tag = tag_; state = Monte::ReaperState::Init; };
        sc2::Tag tag;
        Monte::ReaperState state;
        sc2::Point2D targetLocation = sc2::Point2D(-1, -1);
    } Reaper;

} // end namespace Monte

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

    void marineOnStep();
    void medivacOnStep();
    void siegeTankOnStep();
    void reaperOnStep();
    void ravenOnStep(); // if nearby marine count is low, focus on putting down auto turrets, else use anti armor missles

    void handleChangelings();
    

    protected:
    ScoutManager sm;

    std::vector<Monte::Tank> tanks;
    std::vector<Monte::Reaper> reapers;

    // used for marine control
    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bio; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
};