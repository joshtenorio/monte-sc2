#pragma once

#include <sc2api/sc2_unit.h>
#include "api.h"

// TODO: make these enums instead of defines
// so that we can easily make states for specific units without making a file for them specifically
// shrug

#define GAMEOBJ_NULL    -1
#define GAMEOBJ_INIT    0

namespace Monte {

class GameObject {
    public:
    GameObject() {};
    GameObject(sc2::Tag tag_) { tag = tag_; state = GAMEOBJ_INIT; };

    sc2::Point3D getPos();
    const sc2::Unit* getUnit();

    sc2::Tag getTag();
    char getState();

    bool operator == (const GameObject& go) const{
        if(tag == go.tag) return true;
        else return false;
    }

    bool operator == (const sc2::Unit& unit) const{ // should this be const sc2::Unit* instead?
        if(tag == unit.tag) return true;
        else return false;
    }

    protected:
    sc2::Tag tag;
    char state;
};

} // end namespace Monte