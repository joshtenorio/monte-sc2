#include "MarinePush.h"

void MarinePush::initialize(){
    pushPriorityStep(sc2::ABILITY_ID::BUILD_BARRACKS, 16);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REFINERY, 16);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_REAPER);
    pushPriorityStep(sc2::ABILITY_ID::MORPH_ORBITALCOMMAND);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_COMMANDCENTER);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_MARINE);

    pushPriorityStep(sc2::ABILITY_ID::BUILD_FACTORY);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REFINERY);
}