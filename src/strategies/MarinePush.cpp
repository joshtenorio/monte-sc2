#include "MarinePush.h"

void MarinePush::initialize(){

    config.maxWorkers = 80;
    config.barracksTechOutput = sc2::ABILITY_ID::TRAIN_MARAUDER;
    config.factoryTechOutput = sc2::ABILITY_ID::TRAIN_SIEGETANK;
    config.autoMorphCC = true;
    config.maxRefineries = 6;

    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO, 16);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO, 16);
    pushBuildOrderStep(TYPE_TRAIN, sc2::ABILITY_ID::TRAIN_REAPER, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_COMMANDCENTER, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);
    pushBuildOrderStep(TYPE_TRAIN, sc2::ABILITY_ID::TRAIN_MARINE, STEP_BLOCKING, STEP_HIGHEST_PRIO - 1);

    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_FACTORY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_BUNKER, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_SET_PRODUCTION, sc2::ABILITY_ID::TRAIN_MARINE, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_STARPORT, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);
    pushBuildOrderStep(TYPE_TRAIN, sc2::ABILITY_ID::TRAIN_LIBERATOR, STEP_BLOCKING, STEP_HIGHEST_PRIO - 1);

    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);
    pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);
    //pushBuildOrderStep(TYPE_BUILDINGCAST, sc2::ABILITY_ID::RESEARCH_STIMPACK, STEP_NONBLOCKING, STEP_HIGHEST_PRIO - 1);
    pushBuildOrderStep(TYPE_BUILD, sc2::ABILITY_ID::BUILD_ENGINEERINGBAY, STEP_NONBLOCKING, STEP_HIGHEST_PRIO -1);
    pushBuildOrderStep(TYPE_SET_PRODUCTION, sc2::ABILITY_ID::TRAIN_MEDIVAC, STEP_NONBLOCKING, STEP_HIGHEST_PRIO);

}

GameStatus MarinePush::evaluate(){
    int numPerWave = 5 + API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) * 3;
    if(
        API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARAUDER) >= numPerWave ||
        gInterface->observation->GetFoodUsed() >= 200){
            currentStatus = GameStatus::Attack;
            return GameStatus::Attack;
    }
    else if( // continue attacking if we still have more than half our bio
    // TODO: this could be smarter if we had more information about what we were doing with the bio
    // and not just going off of what we see from gInterface->observation
        currentStatus == GameStatus::Attack &&
        API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARINE) + API::countIdleUnits(sc2::UNIT_TYPEID::TERRAN_MARAUDER) >= numPerWave/2){
            return GameStatus::Attack;
    }
    
    currentStatus = GameStatus::Bide;
    return GameStatus::Bide;
}