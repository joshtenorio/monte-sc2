#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_agent.h>
#include "api.h"
#include "Manager.h"

typedef struct Scout_s_t {
    const sc2::Unit* u;
    sc2::Tag tag;
    sc2::Point2D target;
} Scout;

class ScoutManager : public Manager {
    public:
    ScoutManager() {};
    void OnStep();
    void OnUnitDestroyed(const Unit* unit_);

    bool createScoutingMission();
    bool sendScout();
    protected:
    int foo;
};