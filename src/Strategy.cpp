#include "Strategy.h"


void Strategy::initialize(){}

void Strategy::pushBuildOrderStep(sc2::ABILITY_ID ability_, bool blocking_, int priority_, int reqSupply_){
    buildOrder.emplace_back(Step(ability_, blocking_, priority_, reqSupply_));
}

Step Strategy::popNextBuildOrderStep(){
    // make sure there is a step in the priority order before returning
    if(!buildOrder.empty()){
        Step step = buildOrder.front();
        buildOrder.pop_front();
        return step;
    }
    else{
        return STEP_NULL; 
    }
}

Step Strategy::peekNextBuildOrderStep(){
    // make sure there is a step in the priority order before returning
    if(!buildOrder.empty()){
        //Step step = priorityBuildOrder.front();
        return buildOrder.front();;
    }
    else{
        return STEP_NULL;
    }
}
