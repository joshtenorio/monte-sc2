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
#define PTNODE_FREE     0
#define PTNODE_TAKEN    1
#define PTNODE_INVALID  2

namespace Monte {

typedef struct PlacementTreeNode_s_t {
    PlacementTreeNode_s_t(sc2::Point2D location_) { location = location_; };
    // the building location of the node
    sc2::Point2D location;

    char status = PTNODE_INVALID;

    // pointers to children nodes in the tree
    PTNode* n;
    PTNode* s;
    PTNode* e;
    PTNode* w;

} PTNode;

class PlacementTree {
    public:
    PlacementTree() {};
    PlacementTree(sc2::Point2D loc, size_t maxLevels_) { root = new PTNode(loc); maxLevels = maxLevels_; };

    // create more branches
    // TODO: we need to use Query in addition to checking reserved tiles until we update reservedTiles to make unbuildable destroyables reserved
    void expand(std::vector< std::vector< bool >> reservedTileMap, int numLoops);

    // if a node's location is invalid, prune it and its children
    void validate();
    
    // TODO: DFS might be best
    sc2::Point2D getNextLocation();

    protected:
    // given a PTNode, delete it and its children
    // TODO: do we need to give this function the parent of n instead?
    void pruneBranch(PTNode* n);

    PTNode* root;
    size_t maxLevels;
    // TODO: add instance variables for how much spacing we want between buildings
    // TODO: add instance variable here for the radius of the building (perhaps safe to assume 1.5?)
};

} // end namespace Monte
