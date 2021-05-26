#include "Strategy.h"

void Strategy::pushPriorityStep(sc2::ABILITY_ID ability, int supply){
    priorityBuildOrder.emplace_back(Step(ability, supply));
}
void Strategy::pushOptionalStep(sc2::ABILITY_ID ability, int supply){
    optionalBuildOrder.emplace_back(Step(ability, supply));
}

void Strategy::initialize(){
// FIXME: kekek
}

Step Strategy::popNextPriorityStep(){
    // make sure there is a step in the priority order before returning
    if(!priorityBuildOrder.empty()){
        Step step = priorityBuildOrder.front();
        priorityBuildOrder.pop_front();
        return step;
    }
    else{
        return STEP_NULL; 
    }
}

Step Strategy::popNextOptionalStep(){
    // make sure there is a step in the optional order before returning
    if(!optionalBuildOrder.empty()){
        Step step = optionalBuildOrder.front();
        optionalBuildOrder.pop_front();
        return step;
    }
    else{
        return STEP_NULL;
    }
}

Step Strategy::peekNextPriorityStep(){
    // make sure there is a step in the priority order before returning
    if(!priorityBuildOrder.empty()){
        Step step = priorityBuildOrder.front();
        priorityBuildOrder.pop_front();
        return step;
    }
    else{
        return STEP_NULL;
    }
}

Step Strategy::peekNextOptionalStep(){
    // make sure there is a step in the optional order before returning
    if(!optionalBuildOrder.empty()){
        Step step = optionalBuildOrder.front();
        return step;
    }
    else{
        return STEP_NULL;
    }
}