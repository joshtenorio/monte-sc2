#pragma once

#include <list>
#include <sc2api/sc2_unit.h>

#define STEP_NULL Step(sc2::ABILITY_ID::BUILD_ASSIMILATOR, -1, false)

typedef struct Step_s_t {
    Step_s_t(sc2::ABILITY_ID ability_, int reqSupply_, bool blocking_) : ability(ability_), reqSupply(reqSupply_), blocking(blocking_) {}
    sc2::ABILITY_ID ability;

    // build at a specific supply, if negative ignore it
    int reqSupply = -1;

    // if true, don't go to next Step in strategy until this step is complete
    bool blocking;

    bool operator == (const Step_s_t& s) const {
        if(
            ability == s.ability &&
            reqSupply == s.reqSupply &&
            blocking == s.blocking
        ) return true;
        else return false;
    }

    void operator = (const Step_s_t& s) {
        ability = s.ability;
        reqSupply = s.reqSupply;
        blocking = s.blocking;
    }
} Step;

class Strategy{
    public:
    Strategy() {};
    
    // add initial steps to build orders
    virtual void initialize(); // TODO: does this need {} here? 

    // add more steps if needed
    void pushPriorityStep(sc2::ABILITY_ID ability, bool blocking = true, int supply = -1);
    void pushOptionalStep(sc2::ABILITY_ID ability, bool blocking = true, int supply = -1);

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