#pragma once
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>

class GameObject {
    public:
    GameObject(sc2::Tag gTag);

    const sc2::Unit* getUnit();
    bool isValid();

    protected:
    sc2::Tag tag;
};