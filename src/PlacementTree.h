/**
 * @file PlacementTree.h
 * @author Joshua Tenorio
 * 
 * A way to generate building locations based on a root building's location
 */
#pragma once 

#include <sc2api/sc2_common.h>
#include <vector>

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

class PlacementTree {
    public:
    sc2::Point2D findPlacement(sc2::Point2D root, std::vector< std::vector< bool >>& reservedTileMap, int maxDepth,
                                int xMargin, int yMargin, bool isArmyBuilding);

    private:
    bool isValidPlacement(sc2::Point2D location, std::vector< std::vector< bool >>& reservedTileMap, bool isArmyBuilding);
};

} // end namespace Monte
