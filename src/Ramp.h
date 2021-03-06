#pragma once

#include <vector>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_map_info.h>



typedef struct Ramp_s_t {
    Ramp_s_t() {}
    bool isMainRamp = false;
    std::vector<sc2::Point2D> supplyDepotPoints;
    sc2::Point2D barracksPos;
    sc2::Point2D barracksWithAddonPos; // probably the more useful one

} Ramp;

void findRamp(Ramp* r, sc2::Point2D start);