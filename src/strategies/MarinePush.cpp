#include "MarinePush.h"

void MarinePush::initialize(){
    pushPriorityStep(sc2::ABILITY_ID::BUILD_BARRACKS, false, 16);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REFINERY, false, 16);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_REAPER, false);
    pushPriorityStep(sc2::ABILITY_ID::MORPH_ORBITALCOMMAND);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_COMMANDCENTER);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_MARINE);

    pushPriorityStep(sc2::ABILITY_ID::BUILD_FACTORY);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REFINERY, false);
}