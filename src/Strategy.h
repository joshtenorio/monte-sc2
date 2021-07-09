#pragma once

#include <list>
#include <sc2api/sc2_unit.h>

#define TYPE_NULL           -1
#define TYPE_BUILD          0
#define TYPE_ADDON          1
#define TYPE_TRAIN          2
#define TYPE_BUILDINGCAST   3

#define STEP_BLOCKING       true
#define STEP_NONBLOCKING    false
#define STEP_LOWEST_PRIO    0
#define STEP_HIGHEST_PRIO   10

#define STEP_NULL Step(TYPE_NULL, sc2::ABILITY_ID::BUILD_ASSIMILATOR, false, 0, -1)

typedef struct MetaType_s_t {
    MetaType_s_t(int type_, sc2::ABILITY_ID ability_): type(type_), ability(ability_) {}
    int type;
    sc2::ABILITY_ID ability;
    
    bool operator == (const MetaType_s_t& s) const {
        if(type == s.type && ability == s.ability) return true;
        else return false;
    }

    bool operator = (const MetaType_s_t& s) {
        type = s.type;
        ability = s.ability;
    }
} MetaType;

typedef struct Step_s_t {
    Step_s_t(int mtType, sc2::ABILITY_ID mtAbility, bool blocking_, int priority_, int reqSupply_) :
                container(mtType, mtAbility), blocking(blocking_), priority(priority_), reqSupply(reqSupply_) {}

    // container for type of step
    MetaType container;

    // if true, don't go to next Step in strategy until this step is complete
    bool blocking;

    // priority level; higher level -> higher priority
    int priority;

    // build at a specific supply, if negative ignore it
    int reqSupply;

    bool operator == (const Step_s_t& s) const {
        if(
            container == s.container &&
            reqSupply == s.reqSupply &&
            blocking == s.blocking &&
            priority == s.priority
        ) return true;
        else return false;
    }

    void operator = (const Step_s_t& s) {
        container = s.container;
        reqSupply = s.reqSupply;
        blocking = s.blocking;
        priority = s.priority;
    }

    sc2::ABILITY_ID getAbility(){
        return container.ability;
    }

    int getType(){
        return container.type;
    }
} Step;

class Strategy{
    public:
    Strategy() {};
    
    // add initial steps to build orders
    virtual void initialize(); // TODO: does this need {} here? 

    // for pushing things into build order
    void pushBuildOrderStep(int type, sc2::ABILITY_ID ability_, bool blocking_, int priority, int reqSupply = -1);

    // get the next step in order and pop it
    Step popNextBuildOrderStep();

    // look at the next step but don't remove it yet
    Step peekNextBuildOrderStep();

    // gets the metaType for an ability
    int getType(sc2::ABILITY_ID ability);

    protected:
    std::list<Step> buildOrder;

    // config variables
    int maxWorkers = -1; // most useful for 1 base all ins. if this is negative it should be ignored
    int maxBarracks = 8;
    int maxFactories = 1;
    int maxStarports = 2;
    int maxRefineries = 6;
    int maxEngineeringBays = 2;
    int maxArmories = 1;

    bool libHarass = false;
    bool buildBunker = true;
    bool pullWorkers = true;

    // TODO: add variables that define how much of a unit we should produce
};