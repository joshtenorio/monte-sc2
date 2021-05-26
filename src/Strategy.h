#pragma once

#include <list>
#include <sc2api/sc2_unit.h>

#define STEP_NULL Step(sc2::ABILITY_ID::BUILD_ASSIMILATOR, -1)

typedef struct Step_s_t {
    Step_s_t(sc2::ABILITY_ID ability_, int reqSupply_) : ability(ability_), reqSupply(reqSupply_) {}
    sc2::ABILITY_ID ability;
    int reqSupply = -1; // build at a specific supply, if negative ignore it

    // TODO: maybe add an == operator overload?
} Step;
class Strategy{
    public:
    Strategy() {};
    
    // add initial steps to build orders
    virtual void initialize(); // TODO: does this need {} here? 

    // add more steps if needed
    void pushPriorityStep(sc2::ABILITY_ID ability, int supply = -1);
    void pushOptionalStep(sc2::ABILITY_ID ability, int supply = -1);

    // get the next step in order and pop it
    Step popNextPriorityStep();
    Step popNextOptionalStep();

    // look at the next step but don't remove it yet
    Step peekNextPriorityStep();
    Step peekNextOptionalStep();

    protected:
    std::list<Step> priorityBuildOrder;
    std::list<Step> optionalBuildOrder;
    int maxWorkers = -1; // most useful for 1 base all ins. if this is negative it should be ignored
};