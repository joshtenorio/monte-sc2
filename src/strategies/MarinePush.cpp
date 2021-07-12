#include "MarinePush.h"

void MarinePush::initialize(){
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO, 16);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO, 16);
    pushBuildOrderStep(TYPE_TRAIN, sc2::ABILITY_ID::TRAIN_REAPER, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::MORPH_ORBITALCOMMAND, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_COMMANDCENTER, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);
    pushBuildOrderStep(TYPE_TRAIN, sc2::ABILITY_ID::TRAIN_MARINE, STEP_BLOCKING, STEP_HIGHEST_PRIO - 1);

    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_FACTORY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_BUNKER, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    //pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);

    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_STARPORT, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);
    pushBuildOrderStep(TYPE_TRAIN, sc2::ABILITY_ID::TRAIN_LIBERATOR, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);



}