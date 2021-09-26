#include "api.h" // we need access to query
#include "PlacementTree.h"

namespace Monte {

sc2::Point2D PlacementTree::findPlacement(sc2::Point2D root, std::vector< std::vector< bool >>& reservedTileMap,
                                        int maxDepth, int xMargin, int yMargin, bool isArmyBuilding){
    // hehe
}

bool PlacementTree::isValidPlacement(sc2::Point2D location, std::vector< std::vector< bool >>& reservedTileMap, bool isArmyBuilding){
    
}

} // end namespace Monte