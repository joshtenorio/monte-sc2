#pragma once

#include <sc2api/sc2_unit.h>
#include "api.h"

namespace Monte {

class GameObject {
    public:

    enum State {
        Null = -1,
        Init = 0,
        Move = 1,
        Attack = 2,
        Retreat = 3
    };

    GameObject() {};
    GameObject(sc2::Tag tag_) { tag = tag_; state = State::Init; };

    sc2::Point3D getPos();
    const sc2::Unit* getUnit();

    sc2::Tag getTag();
    State getState();

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
    State state;
};

} // end namespace Monte