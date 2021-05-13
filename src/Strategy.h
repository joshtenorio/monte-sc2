#pragma once

#include <queue>
#include <sc2api/sc2_unit.h>

class Strategy{
    public:
    Strategy() {};
    
    // add initial steps to build orders
    virtual void initialize() = 0;

    // add more steps if needed
    void pushPriorityStep(sc2::ABILITY_ID ability);
    void pushOptionalStep(sc2::ABILITY_ID ability);

    // get the next step in order and pop it
    sc2::ABILITY_ID getNextPriorityStep();
    sc2::ABILITY_ID getNextOptionalStep();

    // look at the next step but don't remove it yet
    sc2::ABILITY_ID peekNextPriorityStep();
    sc2::ABILITY_ID peekNextOptionalStep();

    protected:
    std::queue<sc2::ABILITY_ID> priorityBuildOrder;
    std::queue<sc2::ABILITY_ID> optionalBuildOrder;
};