#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include "combat/InfluenceMap.h"
#include "GameObject.h"

enum class SquadOrderType {
    Null,
    Attack,
    Defend,
    Harass,
    Regroup,
    Cleanup,
    Idle
};

enum class SquadStatus {
    Idle,
    Busy,
    Regroup
};

typedef struct SquadOrder_s_t {
    SquadOrder_s_t() { type = SquadOrderType::Null; };
    SquadOrder_s_t(SquadOrderType type_, sc2::Point2D target_, float radius_){
        type = type_; target = target_; radius = radius_;
    }

    SquadOrderType type;
    sc2::Point2D target;
    float radius;
} SquadOrder;

enum class TankState {
    Null = -1,
    Unsieged,
    Sieging,
    Sieged,
    Unsieging
};

typedef struct Tank_s_t : public GameObject {
    Tank_s_t(sc2::Tag tag_) : GameObject(tag_) { state = TankState::Null; };
    TankState state;
} Tank;

enum class ReaperState {
    Null = -1,
    Init,
    Attack,
    Kite,
    Move,
    Bide
};

typedef struct Reaper_s_t : public GameObject {
    Reaper_s_t(sc2::Tag tag_) : GameObject(tag_) { state = ReaperState::Init; };
    ReaperState state;
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

typedef struct Liberator_s_t : public GameObject {
    Liberator_s_t(sc2::Tag tag_) : GameObject(tag_) { state = LiberatorState::Null; };
    LiberatorState state;
    sc2::Point2D target = sc2::Point2D(-1, -1); // mineral midpoint of targeted expansion
    sc2::Point2D intermediateFlightPoint = sc2::Point2D(-1, -1);
    sc2::Point2D targetFlightPoint = sc2::Point2D(-1, -1);
} Liberator;

typedef struct Viking_s_t : public GameObject {
    Viking_s_t(sc2::Tag tag_) : GameObject(tag_) {};
    sc2::Tag target;
} Viking;