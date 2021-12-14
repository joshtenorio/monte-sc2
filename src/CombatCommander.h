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

    typedef struct Reaper_s_t {
        Reaper_s_t(sc2::Tag tag_) { tag = tag_; state = Monte::ReaperState::Init; };
        sc2::Tag tag;
        Monte::ReaperState state;
        sc2::Point2D targetLocation = sc2::Point2D(-1, -1);
    } Reaper;

    enum class LiberatorState {
        Null = -1,
        Init,
        movingToIntermediate,
        movingToTarget,
        Sieging,
        Sieged,
        Evade
    };

    typedef struct Liberator_s_t {
        Liberator_s_t(sc2::Tag tag_) {tag = tag_; state = Monte::LiberatorState::Null; };
        sc2::Tag tag;
        Monte::LiberatorState state;
        sc2::Point2D target = sc2::Point2D(-1, -1); // mineral midpoint of targeted expansion
        sc2::Point2D intermediateFlightPoint = sc2::Point2D(-1, -1);
        sc2::Point2D targetFlightPoint = sc2::Point2D(-1, -1);
    } Liberator;

} // end namespace Monte

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

    std::vector<Monte::Tank> tanks;
    std::vector<Monte::Reaper> reapers;
    std::vector<Monte::Liberator> liberators;
    std::vector<short> harassTable;

    std::map<std::string, Squad> squads;

    // used for marine control
    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bio; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
    Monte::InfluenceMap groundMap;
    Monte::InfluenceMap airMap;
};