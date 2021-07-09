#include "Strategy.h"


void Strategy::initialize(){}

void Strategy::pushBuildOrderStep(int type, sc2::ABILITY_ID ability_, bool blocking_, int priority_, int reqSupply_){
    buildOrder.emplace_back(Step(type, ability_, blocking_, priority_, reqSupply_));
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

int Strategy::getType(sc2::ABILITY_ID ability){
    switch(ability){
        case sc2::ABILITY_ID::BUILD_ARMORY:
        case sc2::ABILITY_ID::BUILD_BARRACKS:
        case sc2::ABILITY_ID::BUILD_BUNKER:
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
        case sc2::ABILITY_ID::BUILD_ENGINEERINGBAY:
        case sc2::ABILITY_ID::BUILD_FACTORY:
        case sc2::ABILITY_ID::BUILD_FUSIONCORE:
        case sc2::ABILITY_ID::BUILD_GHOSTACADEMY:
        case sc2::ABILITY_ID::BUILD_MISSILETURRET:
        case sc2::ABILITY_ID::BUILD_REFINERY:
        case sc2::ABILITY_ID::BUILD_SENSORTOWER:
        case sc2::ABILITY_ID::BUILD_STARPORT:
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
            return TYPE_BUILD;
            break;
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
        case sc2::ABILITY_ID::TRAIN_RAVEN:
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::TRAIN_MARINE:
        case sc2::ABILITY_ID::TRAIN_REAPER:
        case sc2::ABILITY_ID::TRAIN_GHOST:
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        case sc2::ABILITY_ID::TRAIN_HELLION:
        case sc2::ABILITY_ID::TRAIN_HELLBAT:
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        case sc2::ABILITY_ID::TRAIN_THOR:
            return TYPE_TRAIN;
            break;
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS:
        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS:
        case sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS:
        case sc2::ABILITY_ID::RESEARCH_RAPIDREIGNITIONSYSTEM:
        case sc2::ABILITY_ID::RESEARCH_SMARTSERVOS:
        case sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS:
        case sc2::ABILITY_ID::RESEARCH_ENHANCEDSHOCKWAVES:
        case sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING:
        case sc2::ABILITY_ID::RESEARCH_CYCLONELOCKONDAMAGE:
        case sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD:
        case sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING:
        case sc2::ABILITY_ID::RESEARCH_STIMPACK:
        case sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS:
        case sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT:
        case sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS:
        case sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR:
        case sc2::ABILITY_ID::RESEARCH_COMBATSHIELD:
        case sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER:
        case sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME: // might be a different ID
        case sc2::ABILITY_ID::BUILD_NUKE:
        case sc2::ABILITY_ID::MORPH_ORBITALCOMMAND:
        case sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS:
            return TYPE_BUILDINGCAST;
            break;
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        case sc2::ABILITY_ID::BUILD_REACTOR:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
        case sc2::ABILITY_ID::BUILD_TECHLAB:
            return TYPE_ADDON;
        default:
            return TYPE_NULL;
            break;
    }
}
