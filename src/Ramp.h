#pragma once

#include <vector>
#include <queue>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_map_info.h>
#include "VectorMath.h"


typedef struct Ramp_s_t {
    Ramp_s_t() {};
    bool isMainRamp = false;
    std::vector<sc2::Point2D> points; // points that are in the ramp
    Monte::Vector2D direction; // points in the direction of the high ground

} Ramp;

typedef struct MainRamp_s_t : public Ramp {
    MainRamp_s_t() {};
    std::vector<sc2::Point2D> supplyDepotPoints;
    sc2::Point2D barracksPos;
    sc2::Point2D barracksWithAddonPos; // probably the more useful one
} MainRamp;

std::vector<Ramp> findRamps();
MainRamp generateMainRamp(Ramp r);