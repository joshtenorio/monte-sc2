#include "api.h" // we need access to query
#include "PlacementTree.h"

namespace Monte {

// create n newNodes
// TODO: we need to use Query in addition to checking reserved tiles until we update reservedTiles to make unbuildable destroyables reserved
// TODO: we should rethink tree implementation bc current structure of expand is pretty janky and not gonna go well i think
//          so we should do some lit review on tree implementations probably. at the very least we need a new structure for expand()
void PlacementTree::expand(std::vector< std::vector< bool >>& reservedTileMap, int newNodes){
    // start at root
    PTNode* itr = root;
    int createdNodes = 0;
    while(createdNodes < newNodes){
        if(itr == nullptr) return; // TODO: what should we do?

        if(itr->status != PTNODE_TERMINAL){
            if(itr->n == nullptr){
                // create the north location
                sc2::Point2D north = sc2::Point2D(itr->location.x, itr->location.y + radius * 2 + yMargin);
                itr->n = addNode(north);
                itr->n->level = itr->level + 1;
                itr->numChildren++;
                createdNodes++;
                if(itr->n->status == PTNODE_FREE){
                    // reserve tiles
                    for(int x = north.x - radius; x < north.x + radius; x++)
                        for(int y = north.y - radius; y < north.y + radius; y++)
                            reservedTileMap[x][y] = true;
                        
                    // reserve space for addon
                    if(isArmyBuilding){
                        int xMin = north.x + 1.5, xMax = north.x + 3.5; //  1.5 + 2
                        int yMin = north.y - 1.5, yMax = north.y + 0.5; // -1.5 + 2
                        for(int x = xMin; x < xMax; x++)
                            for(int y = yMin; y < yMax; y++)
                                reservedTileMap[x][y] = true;

                    }
                } // end if itr->n->status == PTNODE_FREE
            }
            else if(itr->s == nullptr){
                itr->numChildren++;
                createdNodes++;
            }
            else if(itr->e == nullptr){
                itr->numChildren++;
                createdNodes++;
            }
            else if(itr->w == nullptr){
                itr->numChildren++;
                createdNodes++;
            }
            else{
                // if all 4 child nodes are not nullptr
                // pick a direction to expand to
                PTNode* newDirection = nullptr;
                int smallest = 99999; // TODO: replace this with actual max int value
                if(itr->n->branchStatus == PTBRANCH_OPEN && itr->n->numChildren < smallest){
                    smallest = itr->n->numChildren;
                    newDirection = itr->n;
                }
                if(itr->s->branchStatus == PTBRANCH_OPEN && itr->s->numChildren < smallest){
                    smallest = itr->s->numChildren;
                    newDirection = itr->s;
                }
                if(itr->e->branchStatus == PTBRANCH_OPEN && itr->e->numChildren < smallest){
                    smallest = itr->e->numChildren;
                    newDirection = itr->e;
                }
                if(itr->w->branchStatus == PTBRANCH_OPEN && itr->w->numChildren < smallest){
                    smallest = itr->w->numChildren;
                    newDirection = itr->w;
                }
                if(newDirection != nullptr)
                    itr = newDirection;
                else return;
            }
        }
        

    } // end while createdNodes < newNodes
}

// protected members //

PTNode* PlacementTree::addNode(sc2::Point2D loc){
    PTNode* newNode = new PTNode(loc);
    newNode->n = nullptr;
    newNode->s = nullptr;
    newNode->e = nullptr;
    newNode->w = nullptr;
    newNode->status = PTNODE_FREE;
    newNode->branchStatus = PTBRANCH_OPEN;

    // get query for location, make sure it is valid
    std::vector<sc2::QueryInterface::PlacementQuery> queries;
    std::vector<bool> results;
    queries.emplace_back(sc2::ABILITY_ID::BUILD_BARRACKS, loc);
    if(isArmyBuilding)
        queries.emplace_back(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, sc2::Point2D(loc.x + 2.5, loc.y - 0.5));
    
    results = gInterface->query->Placement(queries);
    for(auto r : results)
        if(!r){
            newNode->status = PTNODE_TERMINAL;
            newNode->branchStatus = PTBRANCH_TERMINAL;
        }
            
    

    
    return newNode;
}

void PlacementTree::pruneBranch(PTNode* n){
    
}

} // end namespace Monte