#include "Strategy.h"

void Strategy::pushPriorityStep(sc2::ABILITY_ID ability){
    priorityBuildOrder.push(ability);
}
void Strategy::pushOptionalStep(sc2::ABILITY_ID ability){
    optionalBuildOrder.push(ability);
}

sc2::ABILITY_ID Strategy::getNextPriorityStep(){
    // make sure there is a step in the priority order before returning
    if(!priorityBuildOrder.empty()){
        sc2::ABILITY_ID step = priorityBuildOrder.front();
        priorityBuildOrder.pop();
        return step;
    }
    else{
        // TODO: find a better way to do this
        return (sc2::ABILITY_ID) -1; // i really hope there isn't an ability that uses -1 lol
    }
}

sc2::ABILITY_ID Strategy::getNextOptionalStep(){
    // make sure there is a step in the optional order before returning
    if(!optionalBuildOrder.empty()){
        sc2::ABILITY_ID step = optionalBuildOrder.front();
        optionalBuildOrder.pop();
        return step;
    }
    else{
        // TODO: find a better way to do this
        return (sc2::ABILITY_ID) -1; // i really hope there isn't an ability that uses -1 lol
    }
}

sc2::ABILITY_ID Strategy::peekNextPriorityStep(){
    // make sure there is a step in the priority order before returning
    if(!priorityBuildOrder.empty()){
        sc2::ABILITY_ID step = priorityBuildOrder.front();
        priorityBuildOrder.pop();
        return step;
    }
    else{
        // TODO: find a better way to do this
        return (sc2::ABILITY_ID) -1; // i really hope there isn't an ability that uses -1 lol
    }
}

sc2::ABILITY_ID Strategy::peekNextOptionalStep(){
    // make sure there is a step in the optional order before returning
    if(!optionalBuildOrder.empty()){
        sc2::ABILITY_ID step = optionalBuildOrder.front();
        return step;
    }
    else{
        // TODO: find a better way to do this
        return (sc2::ABILITY_ID) -1; // i really hope there isn't an ability that uses -1 lol
    }
}