#pragma once

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <string>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_unit_filters.h>
#include <sc2api/sc2_map_info.h>
#include "Ramp.h"
#include "Logger.h"

// max distance for a neighboring mineral patch
#define PATCH_NEIGHBOR_DISTANCE 25.0f  // 5^2 = 25

// offsets used when searching for base location in Expansion
#define SEARCH_MIN_OFFSET   -10
#define SEARCH_MAX_OFFSET   10
#define DISTANCE_ERR_MARGIN 10.0f

// used for defining ownership of an Expansion
#define OWNER_NEUTRAL   0
#define OWNER_SELF      1
#define OWNER_ENEMY     2

typedef struct Expansion_s_t {
    Expansion_s_t(): isStartingLocation(false), initialized(false) {}
    sc2::Units mineralLine;
    sc2::Units gasGeysers;
    sc2::Point2D baseLocation;
    sc2::Point3D mineralMidpoint; // used to find base location

    bool isStartingLocation = false;
    bool initialized = false;
    float distanceToStart;
    int numFriendlyRefineries = 0;
    char ownership = OWNER_NEUTRAL;
    MainRamp ramp;

    // used for std::sort
    bool operator < (const Expansion_s_t& e) const {
        return (distanceToStart < e.distanceToStart);
    }

    bool operator == (const Expansion_s_t& e) const {
        if(baseLocation.x == e.baseLocation.x && baseLocation.y == e.baseLocation.y)
            return true;
        else
            return false;
    }


} Expansion;



class Mapper {
    public:
    Mapper();
    void initialize();
    Expansion* getClosestExpansion(sc2::Point3D point);

    // TODO: change this so it returns an Expansion*
    Expansion getStartingExpansion();

    // returns a pointer to the nth closest expansion to the starting location
    Expansion* getNthExpansion(int n);

    // returns a pointer to the newest owned expansion
    Expansion* getCurrentExpansion();

    // returns a pointer to the next expansion to build at
    Expansion* getNextExpansion();

    int numOfExpansions();

    void setExpansionOwnership(Expansion* e, char newOwner);
    void setExpansionOwnership(sc2::Point3D p, char newOwner);


    protected:
    // get expansions functions are from mullemech
    // https://github.com/ludlyl/MulleMech/blob/master/src/core/Map.cpp
    // TODO: find ways to improve performance
    void calculateExpansions();

    // sort by closest expansion to the parameter to farthest from the parameter
    void sortExpansions(sc2::Point2D point);

    // for debugging purposes
    void validateGeysers();

    static std::vector<Expansion> expansions;
    std::vector<Ramp> ramps;
    MainRamp mainRamp;

    Expansion startingExpansion; // TODO: could this be a pointer instead? i.e. yes it can
    Logger logger;
};