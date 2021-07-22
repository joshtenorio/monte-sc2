#pragma once

#include <sc2api/sc2_unit.h>
#include "GameObject.h"

#define UNIT_STATE_INIT 0

class MicroManager {
    public:
    MicroManager() {};

    // TODO: possibly need to pass more info about the squad state
    virtual void doStateAction(Monte::GameObject& unit, char squadState) = 0;
    virtual char validateState(Monte::GameObject& unit, char squadState) = 0;
};