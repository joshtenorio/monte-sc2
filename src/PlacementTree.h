/**
 * @file PlacementTree.h
 * @author Joshua Tenorio
 * 
 * A way to generate building locations based on a root building's location
 */
#pragma once 

#include <sc2api/sc2_common.h>
#include <vector>
#include <limits>


#define PT_TREE_FULL        sc2::Point2D(-1,-2) // if returned to buildingplacer, buildingplacer should find a new root position
#define PT_NODE_NULL        sc2::Point2D(-1,-1) // location is phsyically invalid, don't search past this node


#define PT_DIR_NULL         'X'
#define PT_DIR_NORTH        'N'
#define PT_DIR_SOUTH        'S'
#define PT_DIR_EAST         'E'
#define PT_DIR_WEST         'W'

/*
when needing to place a building using placementTree,
intake a sc2point2d (which should be a valid placement for building)
and find the closest valid position and return it
if there are no valid positions found, return something
to let buildingplacer know, so that it can generate a new "root" position
note: use recursion!
by doing so we won't have to store anything long term :)))))
*/
namespace Monte {
// TODO: make findPlacement not static and store xMargin, yMargin, isArmyBuilding, perhaps root, etc
// ESPECIALLY IF WE ARE USING MORE THAN ONE PLACEMENT TREES AT A TIME
class PlacementTree {
    public:
    /*  precondition: root is the center of the building (ie, (4,4) or (4.5,4.5) )
        precondition: d>0
        parentDir is the direction of this node's parent
    */
    static sc2::Point2D findPlacement(sc2::Point2D root, std::vector< std::vector< bool >>& reservedTileMap, int depth, char parentDir,
                                int xMargin, int yMargin, bool isArmyBuilding);

    static void clearCache(); // clear cache when tree is full

    private:

    // checks if location is physically valid or is already reserved by something else
    // precondition: location is the center of the building (ie, (4,4) or (4.5,4.5) )
    static bool isValidPlacement(sc2::Point2D location, std::vector< std::vector< bool >>& reservedTileMap, bool isArmyBuilding);
    
    // returns true if location is not in cache
    static bool isFreeLocation(sc2::Point2D location);

    static std::vector<sc2::Point2D> cache;
};

} // end namespace Monte
