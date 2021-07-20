#include "api.h" // we need access to query
#include "PlacementTree.h"

namespace Monte {

// create n newNodes
// TODO: we need to use Query in addition to checking reserved tiles until we update reservedTiles to make unbuildable destroyables reserved
// we should reserve locations we find
// TODO: we should rethink tree implementation bc current structure of expand is pretty janky and not gonna go well i think
//          so we should do some lit review on tree implementations probably. at the very least we need a new structure for expand()
//          we probably require some change in PTNode, maybe it should keep track of its level and how many children it has?
void PlacementTree::expand(std::vector< std::vector< bool >>& reservedTileMap, int newNodes){
    // start at root
    PTNode* itr = root;
    int createdNodes = 0;
    while(createdNodes < newNodes){
        // TODO: should we have a max number of loops to make sure loop has an upper bound?
        // when we create a new node:
        // if it is PTNODE_FREE, we should reserve the tiles and increment createdNodes
        // if it is PTNODE_TERMINAL, we should *not* reserve the tiles and increment createdNodes
        if(itr->n == nullptr){
            // get the north location, double check the math (but pretty sure it works)
            sc2::Point2D north = sc2::Point2D(itr->location.x, itr->location.y + radius * 2 + yMargin);
            itr->n = addNode(north);
            if(itr->n->status == PTNODE_FREE){
                // reserve tiles here
            }
            createdNodes++;
        }
        else if(itr->s == nullptr){

            createdNodes++;
        }
        else if(itr->e == nullptr){

            createdNodes++;
        }
        else if(itr->w == nullptr){

            createdNodes++;
        }
        else{
            // if all 4 child nodes are not nullptr
        }
    }
}

// protected members //

PTNode* PlacementTree::addNode(sc2::Point2D loc){
    PTNode* newNode = new PTNode(loc);
    newNode->n = nullptr;
    newNode->s = nullptr;
    newNode->e = nullptr;
    newNode->w = nullptr;
    newNode->status = PTNODE_FREE;

    // get query for location, make sure it is valid
    std::vector<sc2::QueryInterface::PlacementQuery> queries;
    std::vector<bool> results;
    queries.emplace_back(sc2::ABILITY_ID::BUILD_BARRACKS, loc);
    if(isArmyBuilding)
        queries.emplace_back(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(loc.x + 2.5, loc.y - 0.5));
    
    results = gInterface->query->Placement(queries);
    for(auto r : results)
        if(!r)
            newNode->status = PTNODE_TERMINAL;
    

    
    return newNode;
}

} // end namespace Monte