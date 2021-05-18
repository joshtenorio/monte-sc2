#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_unit_filters.h>

// max distance for a neighboring mineral patch
#define PATCH_NEIGHBOR_DISTANCE 25.0  // 5^2 = 25

// offsets used when searching for base location in Expansion
#define SEARCH_MIN_OFFSET   -10
#define SEARCH_MAX_OFFSET   10
#define DISTANCE_ERR_MARGIN 10.0

#define OWNER_NEUTRAL   0
#define OWNER_SELF      1
#define OWNER_ENEMY     2

typedef struct Expansion_s_t {
    Expansion_s_t(): isStartingLocation(false), initialized(false) {}
    std::vector<sc2::Point3D> mineralLine;
    sc2::Units gasGeysers;
    sc2::Point2D baseLocation;
    sc2::Point3D mineralMidpoint; // used to find base location

    bool isStartingLocation = false;
    bool initialized = false;
    float distanceToStart;
    int numFriendlyRefineries = 0;
    char ownership = OWNER_NEUTRAL;

    // used for std::sort
    bool operator < (const Expansion_s_t& e) const {
        return (distanceToStart < e.distanceToStart);
    }
    Ramp ramp;

} Expansion;

typedef struct Ramp_s_t {
    bool isMainRamp = false;
    std::vector<sc2::Point2D> supplyDepotPoints;
    sc2::Point2D barracksPos;
    sc2::Point2D barracksWithAddonPos; // probably the more useful one

} Ramp;

class Mapper {
    public:
    Mapper() {};
    void initialize();
    Expansion* getClosestExpansion(sc2::Point3D point);
    Expansion getStartingExpansion();
    Expansion getNthExpansion(int n);

    // returns a pointer to the next expansion to build at
    Expansion* getNextExpansion();


    protected:
    // get expansions functions are from mullemech
    // https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp
    // TODO: find ways to improve performance
    void calculateExpansions();

    // sort by closest expansion to the parameter to farthest from the parameter
    void sortExpansions(sc2::Point2D point);

    static std::vector<Expansion> expansions;
    static std::vector<Ramp> ramps;

    Expansion startingExpansion; // TODO: could this be a pointer instead?
};