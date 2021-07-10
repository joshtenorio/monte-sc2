#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>

namespace Monte {

class Unit {
    public:
    Unit(sc2::Tag tag_) { tag = tag_; };

    static sc2::Unit toUnit(sc2::Tag tag);
    sc2::Unit toUnit();
    sc2::Point3D getPos();
    sc2::Tag getTag();

    bool operator ==(const Unit& unit_) const;
    bool operator ==(const sc2::Unit& unit_) const;

    protected:
    sc2::Tag tag;
    int state;
};

} // end namespace