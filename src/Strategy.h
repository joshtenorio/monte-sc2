#pragma once

#include <vector>
#include <sc2api/sc2_unit.h>
#include "api.h"
#include "StrategyTools.h"

#define STRAT_TYPE_STANDARD 0
#define STRAT_TYPE_PROXY    1
#define STRAT_TYPE_ALLIN    2

class Strategy{
    public:
    Strategy() {};

    void debugPrintValidSteps();
    
    // add initial steps to build orders
    virtual void initialize();

    // condition for sending an attack
    // TODO: should return an int, i.e. a score
    virtual GameStatus evaluate();

    // for pushing things into build order
    void pushBuildOrderStep(int type, sc2::ABILITY_ID ability_, bool blocking_, int priority, int reqSupply_ = -1);

    // emergency build order steps are added at the front with priority + 1
    void addEmergencyBuildOrderStep(int type, sc2::ABILITY_ID ability_, bool blocking_, int reqSupply_ = -1);

    // get the next step in order and pop it
    Step popNextBuildOrderStep();

    // look at the next step but don't remove it yet
    Step peekNextBuildOrderStep();

    // get/remove steps
    Step getNthBuildOrderStep(int n);
    Step getHighestPriorityStep();

    void removeNthBuildOrderStep(int n);
    void removeCurrentHighestPriorityStep();
    void removeStep(Step s);
    void removeStep(sc2::ABILITY_ID ability);

    // misc functions
    bool isEmpty();
    int getBuildOrderSize();
    ProductionConfig getProductionConfig(); // TODO: rename this
    CombatConfig getCombatConfig();
    int getType(sc2::ABILITY_ID ability); // gets the metaType for an ability

    protected:
    std::vector<Step> buildOrder;

    // these should be adjusted in initialize() //
    char strategyType;
    ProductionConfig config; // TODO: change this name to prodConfig or pConfig or something
    CombatConfig combatConfig; // cConfig ?
    GameStatus currentStatus = GameStatus::Bide;

};