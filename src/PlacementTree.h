/**
 * @file PlacementTree.h
 * @author Joshua Tenorio
 * 
 * A way to generate building locations based on a root building's location
 */
#pragma once 

#include <sc2api/sc2_common.h>
#include <vector>

// TODO: since this is used in a Monte namespace, perhaps this should be enum instead
#define PTNODE_FREE      0 // the node's location is open and available
#define PTNODE_TAKEN     1 // the node's location is used
#define PTNODE_TERMINAL  2 // the node's location is invalid

#define PTBRANCH_TERMINAL 0 // the branch has been fully explored
#define PTBRANCH_OPEN     1 // the branch is not yet fully explored

namespace Monte {

typedef struct PlacementTreeNode_s_t {
    PlacementTreeNode_s_t(sc2::Point2D location_) { location = location_; };
    // the building location of the node
    sc2::Point2D location;

    char status = PTNODE_TERMINAL;
    char branchStatus = PTBRANCH_TERMINAL;
    int numChildren = 0;
    short level = 0;

    // pointers to children nodes in the tree
    PTNode* n, *s, *e, *w;

} PTNode;

// TODO: should we refactor so it uses a queue as an underlying data structure?
class PlacementTree {
    public:
    PlacementTree() {};
    PlacementTree(sc2::Point2D loc, size_t maxLevels_, bool isArmyBuilding_, int xMargin_, int yMargin_)
        { root = new PTNode(loc); maxLevels = maxLevels_; isArmyBuilding = isArmyBuilding_; xMargin = xMargin; yMargin = yMargin; };

    // create n newNodes
    // TODO: we need to use Query in addition to checking reserved tiles until we update reservedTiles to make unbuildable destroyables reserved
    // we should reserve locations we find
    void expand(std::vector< std::vector< bool >>& reservedTileMap, int newNodes = 8);

    // if a node's location is invalid, prune the node and its children
    void validate();
    
    // TODO: DFS might be best but BFS could be better for performance/memory reasons
    sc2::Point2D getNextLocation();

    protected:
    // given a PTNode, delete it and its children
    // TODO: do we need to give this function the parent of n instead?
    PTNode* addNode(sc2::Point2D loc);
    void pruneBranch(PTNode* n);

    PTNode* root;
    size_t maxLevels; // maximum height of tree
    bool isArmyBuilding; // if true, then reserve space for addon as well

    // these two define how much space we want between buildings
    // if margin = 0, then there is no space between buildings. if margin = 1, siege tanks cant fit through the gap
    int xMargin;
    int yMargin;
    // TODO: add instance variable here for the radius of the building (or perhaps safe to assume 1.5?)
    float radius = 1.5;
};

} // end namespace Monte
