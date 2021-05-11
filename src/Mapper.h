#pragma once

#include <vector>
#include <queue>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_unit_filters.h>
#include "api.h"

// max distance for a neighboring mineral patch
#define PATCH_NEIGHBOR_DISTANCE 5.0

typedef struct Expansion_s_t {
    sc2::Units mineralLine;
    sc2::Units gasGeysers;
    sc2::Point2D baseLocation;
} Expansion;

class Mapper {
    public:
    Mapper() {};
    void initialize();
    Expansion getClosestExpansion(sc2::Point2D point);

    protected:
    // get minerallines/gasgeysers/expansions functions are from mullemech
    // https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp
    // TODO: find ways to improve these functions
    std::vector<sc2::Units> getMineralLines();
    std::vector<sc2::Units> getGasGeysers();
    void calculateExpansions();

    // sort by closest expansion to the parameter to farthest from the parameter
    void sortExpansions(sc2::Point2D point);

    private:
    std::vector<Expansion> expansions;
};