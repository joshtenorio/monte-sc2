#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_unit_filters.h>
#include "api.h"

// max distance for a neighboring mineral patch
#define PATCH_NEIGHBOR_DISTANCE 25.0  // 5^2 = 25

// offsets used when searching for base location in Expansion
#define SEARCH_MIN_OFFSET -10
#define SEARCH_MAX_OFFSET 10
#define DISTANCE_ERR_MARGIN 10.0

typedef struct Expansion_s_t {
    Expansion_s_t(): isStartingLocation(false), initialized(false) {}
    std::vector<Point3D> mineralLine;
    std::vector<Point3D> gasGeysers;
    sc2::Point2D baseLocation;
    sc2::Point3D mineralMidpoint; // used to find base location

    bool isStartingLocation = false;
    bool initialized = false;
} Expansion;

class Mapper {
    public:
    Mapper() {};
    void initialize();
    Expansion getClosestExpansion(sc2::Point3D point);

    protected:
    // get expansions functions are from mullemech
    // https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp
    // TODO: find ways to improve performance
    void calculateExpansions();

    // sort by closest expansion to the parameter to farthest from the parameter
    void sortExpansions(sc2::Point2D point);

    private:
    std::vector<Expansion> expansions;
};