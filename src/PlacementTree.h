/**
 * @file PlacementTree.h
 * @author Joshua Tenorio
 * 
 * A way to generate building locations based on a root building's location
 */
#pragma once 

#include <sc2api/sc2_common.h>

namespace Monte {

typedef struct PlacementTreeNode_s_t {
    // the building location of the node
    sc2::Point2D location;

    // if true, then all four children nodes are invalid (ie their locations aren't possible)
    bool isTerminal;

    // pointers to children nodes in the tree
    PTNode* n, s, e, w;

} PTNode;

class PlacementTree {
    public:
    PlacementTree() {};
    
    protected:
    PTNode* root;
};

} // end namespace Monte
