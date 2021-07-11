#include "MarinePush.h"

void MarinePush::initialize(){
    pushPriorityStep(sc2::ABILITY_ID::BUILD_BARRACKS, STEP_NONBLOCKING, 16);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING, 16);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_REAPER, STEP_NONBLOCKING, STEP_PROD_SINGLE);
    pushPriorityStep(sc2::ABILITY_ID::MORPH_ORBITALCOMMAND, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_COMMANDCENTER, STEP_NONBLOCKING, 22);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_MARINE, STEP_BLOCKING, STEP_PROD_SINGLE);

    pushPriorityStep(sc2::ABILITY_ID::BUILD_FACTORY, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_BUNKER, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS, STEP_NONBLOCKING);
    
    //pushPriorityStep(sc2::ABILITY_ID::MORPH_ORBITALCOMMAND, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_MARINE, STEP_NONBLOCKING, STEP_SET_PROD_ORDER);
    
    pushPriorityStep(sc2::ABILITY_ID::BUILD_REFINERY, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_STARPORT, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::RESEARCH_STIMPACK, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_LIBERATOR, STEP_BLOCKING, STEP_PROD_SINGLE);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::BUILD_BARRACKS, STEP_NONBLOCKING);
    pushPriorityStep(sc2::ABILITY_ID::TRAIN_MEDIVAC, STEP_NONBLOCKING, STEP_SET_PROD_ORDER);
    //pushPriorityStep(sc2::ABILITY_ID::TRAIN_SIEGETANK, STEP_NONBLOCKING, STEP_SET_PROD_ORDER);

    

}