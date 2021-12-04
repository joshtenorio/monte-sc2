#pragma once
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_data.h>
#include "api.h"

class GameObject {
    public:
    GameObject(sc2::Tag gTag);

    const sc2::Unit* getUnit();
    sc2::UnitTypeData getData();
    sc2::UnitTypeID getType();

    bool isValid();
    bool isFriendly();
    bool isFlying();
    bool isBuilding();
    bool isVisible();
    bool isLarva();
    bool isEgg();

    bool move(sc2::Point2D target);
    bool attack(sc2::Point2D target);
    bool attack(const sc2::Unit* target);
    bool rightClick(sc2::Point2D target);
    bool rightClick(const sc2::Unit* target);
    bool stop();

    protected:
    sc2::Tag tag;
};