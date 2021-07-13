#pragma once

#include <vector>
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
    MetaType_s_t() {};
    MetaType_s_t(int type_, sc2::ABILITY_ID ability_): type(type_), ability(ability_) {}
    int type;
    sc2::ABILITY_ID ability;
    
    bool operator == (const MetaType_s_t& s) const {
        if(type == s.type && ability == s.ability) return true;
        else return false;
    }

    void operator = (const MetaType_s_t& s) {
        type = s.type;
        ability = s.ability;
    }
} MetaType;

typedef struct Step_s_t {
    Step_s_t() { container.type = TYPE_NULL; priority = -1; };
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

typedef struct ProductionConfig_s_t {
    ProductionConfig_s_t() {};

    // defines the maximum amount of a unit we can build
    int maxWorkers = -1; // most useful for 1 base all ins. if this is negative it should be ignored
    int maxBarracks = 8;
    int maxFactories = 1;
    int maxStarports = 2;
    int maxRefineries = 6;
    int maxEngineeringBays = 2;
    int maxArmories = 1;

    // micro config // TODO: integrate strategy into combatcommander
    bool buildBunker = true;
    bool pullWorkers = true;

    // defines what we produce by default; are guaranteed to be true during an opener
    sc2::ABILITY_ID barracksOutput;
    sc2::ABILITY_ID barracksTechOutput;
    sc2::ABILITY_ID factoryOutput;
    sc2::ABILITY_ID factoryTechOutput;
    sc2::ABILITY_ID starportOutput;
    sc2::ABILITY_ID starportTechOutput;

    void operator = (const ProductionConfig_s_t& pc){
        maxWorkers = pc.maxWorkers;
        maxBarracks = pc.maxBarracks;
        maxFactories = pc.maxFactories;
        maxStarports = pc.maxStarports;
        maxRefineries = pc.maxRefineries;
        maxEngineeringBays = pc.maxEngineeringBays;
        maxArmories = pc.maxArmories;
   
        buildBunker = pc.buildBunker;
        pullWorkers = pc.pullWorkers;

        barracksOutput = pc.barracksOutput;
        barracksTechOutput = pc.barracksTechOutput;
        factoryOutput = pc.factoryOutput;
        factoryTechOutput = pc.factoryTechOutput;
        starportOutput = pc.starportOutput;
        starportTechOutput = pc.starportTechOutput;
    }
} ProductionConfig;