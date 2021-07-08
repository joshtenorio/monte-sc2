#pragma once

#include <list>
#include <sc2api/sc2_unit.h>

#define STEP_NULL Step(sc2::ABILITY_ID::BUILD_ASSIMILATOR, -1, false, -1)

#define STEP_BLOCKING       true
#define STEP_NONBLOCKING    false
#define STEP_LOWEST_PRIO    0
#define STEP_HIGHEST_PRIO   10

typedef struct Step_s_t {
    Step_s_t(sc2::ABILITY_ID ability_, int reqSupply_, bool blocking_, int priority_) :
                ability(ability_), reqSupply(reqSupply_), blocking(blocking_), priority(priority_) {}

    // ability that corresponds to the one that does the thing
    sc2::ABILITY_ID ability;

    // priority level; higher level -> higher priority
    int priority;

    // build at a specific supply, if negative ignore it
    int reqSupply;

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
        produceSingle = s.produceSingle;
    }
} Step;

class Strategy{
    public:
    Strategy() {};
    
    // add initial steps to build orders
    virtual void initialize(); // TODO: does this need {} here? 

    // for pushing units into build order
    void pushBuildOrderStep(sc2::ABILITY_ID ability_, int reqSupply_, bool blocking_, int priority_ = 0);

    // get the next step in order and pop it
    Step popNextBuildOrderStep();

    // look at the next step but don't remove it yet
    Step peekNextBuildOrderStep();

    protected:
    std::list<Step> buildOrder;
    int maxWorkers = -1; // most useful for 1 base all ins. if this is negative it should be ignored
    bool harass = false;
};