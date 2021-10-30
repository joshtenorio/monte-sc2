#pragma once

#include <vector>
#include <limits>
#include <sc2api/sc2_unit.h>

#define TYPE_NULL           -1
#define TYPE_BUILD          0
#define TYPE_ADDON          1
#define TYPE_TRAIN          2
#define TYPE_BUILDINGCAST   3
#define TYPE_SET_PRODUCTION 4

#define STEP_BLOCKING       true
#define STEP_NONBLOCKING    false
#define STEP_LOWEST_PRIO    0
#define STEP_HIGHEST_PRIO   10

#define STEP_NULL Step(TYPE_NULL, sc2::ABILITY_ID::BUILD_ASSIMILATOR, false, 0, -1)

#define PRODUCTION_UNUSED sc2::ABILITY_ID::TRAIN_ARCHON
#define PRODUCTION_NOMAX -1

#define COMBAT_AGGRESSIVE   0
#define COMBAT_BIDE         1

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
    short maxWorkers = std::numeric_limits<short>::max();
    short maxBarracks = 8;
    short maxFactories = 1;
    short maxStarports = 2;
    short maxRefineries = 6;
    short maxEngineeringBays = 2;
    short maxArmories = 1;
    short maxOrbitals = 3; //std::numeric_limits<short>::max();

    // if we should automatically morph a cc
    bool autoMorphCC = true;

    // micro config // TODO: move to combatconfig
    bool pullWorkers = true;

    // if we need missile turrets
    bool buildTurrets = false;

    // defines what we produce by default
    sc2::ABILITY_ID barracksOutput = PRODUCTION_UNUSED;
    sc2::ABILITY_ID barracksTechOutput = PRODUCTION_UNUSED;
    sc2::ABILITY_ID factoryOutput = PRODUCTION_UNUSED;
    sc2::ABILITY_ID factoryTechOutput = PRODUCTION_UNUSED;
    sc2::ABILITY_ID starportOutput = PRODUCTION_UNUSED;
    sc2::ABILITY_ID starportTechOutput = PRODUCTION_UNUSED;

    sc2::ABILITY_ID barracksDefaultAddon = sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS;
    sc2::ABILITY_ID factoryDefaultAddon = sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY;
    sc2::ABILITY_ID starportDefaultAddon = sc2::ABILITY_ID::BUILD_REACTOR_STARPORT;


    void operator = (const ProductionConfig_s_t& pc){
        maxWorkers = pc.maxWorkers;
        maxBarracks = pc.maxBarracks;
        maxFactories = pc.maxFactories;
        maxStarports = pc.maxStarports;
        maxRefineries = pc.maxRefineries;
        maxEngineeringBays = pc.maxEngineeringBays;
        maxArmories = pc.maxArmories;
        maxOrbitals = pc.maxOrbitals;

        autoMorphCC = pc.autoMorphCC;
   
        pullWorkers = pc.pullWorkers;
        buildTurrets = pc.buildTurrets;

        barracksOutput = pc.barracksOutput;
        barracksTechOutput = pc.barracksTechOutput;
        factoryOutput = pc.factoryOutput;
        factoryTechOutput = pc.factoryTechOutput;
        starportOutput = pc.starportOutput;
        starportTechOutput = pc.starportTechOutput;

        barracksDefaultAddon = pc.barracksDefaultAddon;
        factoryDefaultAddon = pc.factoryDefaultAddon;
        starportDefaultAddon = pc.starportDefaultAddon;
    }
} ProductionConfig;

typedef struct CombatConfig_s_t {
    CombatConfig_s_t() {};

    char combatState = COMBAT_BIDE; // define whether or not we should be always attacking or wait for waves
    int bioMultiplier = 3; // should this be the barracks multiplier instead?
    int minimumWave = 5;

    // TODO: this should be a std::map instead where they key is the unit type and the value is the maximum number
    int maxTanks;
    int medivacMultiplier = 2; // this * numBarracks = max medivac number


    void operator = (const CombatConfig_s_t& cc){
        // hehe
    }
} CombatConfig;