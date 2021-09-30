#pragma once

#include <sc2api/sc2_unit.h>
#include "combat/GameObject.h"

class MicroManager {
    public:
    MicroManager() {};

    // TODO: possibly need to pass more info about the squad state
    virtual void doStateAction(Monte::GameObject& unit, Squad::State squadState, sc2::Point2D target) = 0;
    virtual void validateState(Monte::GameObject& unit, Squad::State squadState, sc2::Point2D target) = 0;
};