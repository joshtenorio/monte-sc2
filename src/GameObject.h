#pragma once

#include <sc2api/sc2_unit.h>
#include "api.h"

#define UNIT_INIT 0

namespace Monte {

class GameObject {
    public:
    GameObject() {};
    GameObject(sc2::Tag tag_) { tag = tag_; };

    sc2::Point3D getPos();
    sc2::Tag getTag();
    const sc2::Unit* getUnit();
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