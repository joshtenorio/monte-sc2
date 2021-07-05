#pragma once

#include <vector>
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
    void OnUnitEnterVision(const sc2::Unit* unit_);

    bool createScoutingMission();
    bool sendScout();

    protected:
    std::vector<Scout> scouts;
};