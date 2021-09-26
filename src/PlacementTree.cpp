#include "api.h" // we need access to query
#include "PlacementTree.h"

namespace Monte {

sc2::Point2D PlacementTree::findPlacement(sc2::Point2D root, std::vector< std::vector< bool >>& reservedTileMap,
                                        int depth, char parentDir, int xMargin, int yMargin, bool isArmyBuilding){
    
    // stopping condition: d=0 v (!V(r) ^ !C(r)); V(r) root is valid; C(r) root is in cache
    if(depth == 0 || (!isValidPlacement(root, reservedTileMap, isArmyBuilding) && isFreeLocation(root))){
        // if we are in original function call and location is invalid, return tree full
        if(parentDir == PT_DIR_NULL) return PT_TREE_FULL;
        
        // else 
        return (isValidPlacement(root, reservedTileMap, isArmyBuilding) ? root : PT_NODE_NULL);
    }
        
    // generate child locations
    sc2::Point2D nChild = sc2::Point2D(root.x, root.y+yMargin+(2*1.5));
    sc2::Point2D sChild = sc2::Point2D(root.x, root.y-yMargin-(2*1.5));
    sc2::Point2D eChild = sc2::Point2D(root.x+xMargin+(2*1.5)+2, root.y);
    sc2::Point2D wChild = sc2::Point2D(root.x-xMargin-(2*1.5)-2, root.y);
    if(!isArmyBuilding){
        eChild.x -= 2;
        wChild.x += 2;
    }
    
    std::vector<sc2::Point2D> locations; // n_max = 5; root + 3-4 possible children
    locations.emplace_back(root);
    switch(parentDir){
        case PT_DIR_NULL: // get all 4 children
        locations.emplace_back(findPlacement(sChild, reservedTileMap, depth-1, PT_DIR_NORTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(nChild, reservedTileMap, depth-1, PT_DIR_SOUTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(wChild, reservedTileMap, depth-1, PT_DIR_EAST, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(eChild, reservedTileMap, depth-1, PT_DIR_WEST, xMargin, yMargin, isArmyBuilding));
        break;
        case PT_DIR_NORTH: // get 3 children, not the north child
        locations.emplace_back(findPlacement(sChild, reservedTileMap, depth-1, PT_DIR_NORTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(wChild, reservedTileMap, depth-1, PT_DIR_EAST, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(eChild, reservedTileMap, depth-1, PT_DIR_WEST, xMargin, yMargin, isArmyBuilding));
        break;
        case PT_DIR_SOUTH: // get 3 children, not south child
        locations.emplace_back(findPlacement(nChild, reservedTileMap, depth-1, PT_DIR_SOUTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(wChild, reservedTileMap, depth-1, PT_DIR_EAST, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(eChild, reservedTileMap, depth-1, PT_DIR_WEST, xMargin, yMargin, isArmyBuilding));
        break;
        case PT_DIR_EAST: // get 3 children, not east child
        locations.emplace_back(findPlacement(sChild, reservedTileMap, depth-1, PT_DIR_NORTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(nChild, reservedTileMap, depth-1, PT_DIR_SOUTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(wChild, reservedTileMap, depth-1, PT_DIR_EAST, xMargin, yMargin, isArmyBuilding));
        break;
        case PT_DIR_WEST: // get 3 children, not west child
        locations.emplace_back(findPlacement(sChild, reservedTileMap, depth-1, PT_DIR_NORTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(nChild, reservedTileMap, depth-1, PT_DIR_SOUTH, xMargin, yMargin, isArmyBuilding));
        locations.emplace_back(findPlacement(eChild, reservedTileMap, depth-1, PT_DIR_WEST, xMargin, yMargin, isArmyBuilding));
        break;
    }

    // find closest free location
    sc2::Point2D output = PT_NODE_NULL;
    float distToRoot = std::numeric_limits<float>::max();
    for(auto& l : locations){
        if(l == PT_NODE_NULL || l == PT_TREE_FULL) continue;

        float dist = sc2::DistanceSquared2D(root, l);
        if(dist < distToRoot && isFreeLocation(l)) output = l;
    }

    // if node is null or we somehow get pt_tree_full, return tree full
    if(output == PT_NODE_NULL || output == PT_TREE_FULL){
        return PT_TREE_FULL;
    }

    // add location to cache, if in original function call
    if(parentDir == PT_DIR_NULL)
        cache.emplace_back(output);
    return output;
}

void PlacementTree::clearCache(){
    cache.clear();
}

bool PlacementTree::isValidPlacement(sc2::Point2D location, std::vector< std::vector< bool >>& reservedTileMap, bool isArmyBuilding){
    // first use queryinterface to check if building location is valid
    std::vector<sc2::QueryInterface::PlacementQuery> queries;
    std::vector<bool> results;
    queries.emplace_back(sc2::ABILITY_ID::BUILD_BARRACKS, location);
    if(isArmyBuilding)
        queries.emplace_back(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(location.x + 2.5, location.y - 0.5));
    
    results = gInterface->query->Placement(queries);
    for(auto r : results)
        if(!r)
            return false;
        
    // then use reservedtilemap to check if we are reserving anything in that location
    float radius = 1.5;
    int xMin = location.x - radius, xMax = location.x + radius;
    int yMin = location.y - radius, yMax = location.y + radius;

    for(int x = xMin; x < xMax; x++){
        for(int y = yMin; y < yMax; y++){
            if(reservedTileMap[x][y]) return false;
        }
    }
    // if army building, check the addon location too
    if(isArmyBuilding){
        float radius = 1;
        int xMin = location.x+2.5 - radius, xMax = location.x+2.5 + radius;
        int yMin = location.y-0.5 - radius, yMax = location.y-0.5 + radius;
        for(int x = xMin; x < xMax; x++){
            for(int y = yMin; y < yMax; y++){
                if(reservedTileMap[x][y]) return false;
            }
        }
    }
    return true;
}

bool PlacementTree::isFreeLocation(sc2::Point2D location){
    for(auto c : cache)
        if(c == location) return false;
    
    return true;
}

} // end namespace Monte