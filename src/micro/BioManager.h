#pragma once

#include "api.h"
#include "micro/MicroManager.h"
#include "Squad.h"

class BioManager : public MicroManager {
    public:
    BioManager() {};

    // give the GameObject an action based on its and its squad's states
    void doStateAction(Monte::GameObject& unit, Squad::State squadState, sc2::Point2D target);

    // update GameObject state if required
    void validateState(Monte::GameObject& unit, Squad::State squadState);

    protected:
    void manageStim(Monte::GameObject& unit, Squad::State squadState);
};