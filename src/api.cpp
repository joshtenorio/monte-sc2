#include "api.h"

namespace API {

size_t CountUnitType(sc2::UNIT_TYPEID unitType) {
    return gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(unitType)).size();
}
sc2::ABILITY_ID unitTypeIDToAbilityID(sc2::UNIT_TYPEID unit){
    return sc2::ABILITY_ID::BUILD_ASSIMILATOR; // placeholder
}

sc2::ABILITY_ID upgradeIDToAbilityID(sc2::UpgradeID upgrade){
    return sc2::ABILITY_ID::BUILD_ASSIMILATOR; // placeholder
}

sc2::UNIT_TYPEID abilityToUnitTypeID(sc2::ABILITY_ID ability){
    switch(ability){
        case sc2::ABILITY_ID::BUILD_ARMORY:
            return sc2::UNIT_TYPEID::TERRAN_ARMORY;
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            return sc2::UNIT_TYPEID::TERRAN_BARRACKS;
        case sc2::ABILITY_ID::BUILD_BUNKER:
            return sc2::UNIT_TYPEID::TERRAN_BUNKER;
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
            return sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER;
        case sc2::ABILITY_ID::BUILD_ENGINEERINGBAY:
            return sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY;
        case sc2::ABILITY_ID::BUILD_FACTORY:
            return sc2::UNIT_TYPEID::TERRAN_FACTORY;
        case sc2::ABILITY_ID::BUILD_FUSIONCORE:
            return sc2::UNIT_TYPEID::TERRAN_FUSIONCORE;
        case sc2::ABILITY_ID::BUILD_GHOSTACADEMY:
            return sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY;
        case sc2::ABILITY_ID::BUILD_MISSILETURRET:
            return sc2::UNIT_TYPEID::TERRAN_MISSILETURRET;
        case sc2::ABILITY_ID::BUILD_REFINERY:
            return sc2::UNIT_TYPEID::TERRAN_REFINERY;
        case sc2::ABILITY_ID::BUILD_SENSORTOWER:
            return sc2::UNIT_TYPEID::TERRAN_SENSORTOWER;
        case sc2::ABILITY_ID::BUILD_STARPORT:
            return sc2::UNIT_TYPEID::TERRAN_STARPORT;
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
            return sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
            return sc2::UNIT_TYPEID::TERRAN_BANSHEE;
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
            return sc2::UNIT_TYPEID::TERRAN_MEDIVAC;
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
            return sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER;
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
            return sc2::UNIT_TYPEID::TERRAN_LIBERATOR;
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
            return sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER;
        case sc2::ABILITY_ID::TRAIN_RAVEN:
            return sc2::UNIT_TYPEID::TERRAN_RAVEN;
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
            return sc2::UNIT_TYPEID::TERRAN_MARAUDER;
        case sc2::ABILITY_ID::TRAIN_MARINE:
            return sc2::UNIT_TYPEID::TERRAN_MARINE;
        case sc2::ABILITY_ID::TRAIN_REAPER:
            return sc2::UNIT_TYPEID::TERRAN_REAPER;
        case sc2::ABILITY_ID::TRAIN_GHOST:
            return sc2::UNIT_TYPEID::TERRAN_GHOST;
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
            return sc2::UNIT_TYPEID::TERRAN_WIDOWMINE;
        case sc2::ABILITY_ID::TRAIN_HELLION:
            return sc2::UNIT_TYPEID::TERRAN_HELLION;
        case sc2::ABILITY_ID::TRAIN_HELLBAT:
            return sc2::UNIT_TYPEID::TERRAN_HELLIONTANK;
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
            return sc2::UNIT_TYPEID::TERRAN_CYCLONE;
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
            return sc2::UNIT_TYPEID::TERRAN_SIEGETANK;
        case sc2::ABILITY_ID::TRAIN_THOR:
            return sc2::UNIT_TYPEID::TERRAN_THOR;
        default:
            return sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR; // placeholder
        break;
    }
}

bool isStructure(sc2::UNIT_TYPEID unit){
    switch(unit){
        case UNIT_TYPEID::TERRAN_ARMORY:
        case UNIT_TYPEID::TERRAN_BARRACKS:
        case UNIT_TYPEID::TERRAN_BUNKER:
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case UNIT_TYPEID::TERRAN_FACTORY:
        case UNIT_TYPEID::TERRAN_FUSIONCORE:
        case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case UNIT_TYPEID::TERRAN_MISSILETURRET:
        case UNIT_TYPEID::TERRAN_REFINERY:
        case UNIT_TYPEID::TERRAN_REFINERYRICH:
        case UNIT_TYPEID::TERRAN_SENSORTOWER:
        case UNIT_TYPEID::TERRAN_STARPORT:
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
            return true;
        default:
            return false;
    }
}

// TODO: make a unitToAbilityTypeID function which does the inverse of the above

char parseStep(Step s){
    switch(s.ability){
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
            return ABIL_BUILD;
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
            return ABIL_TRAIN;
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
            return ABIL_RESEARCH;
            break;
        case sc2::ABILITY_ID::MORPH_ORBITALCOMMAND:
        case sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS:
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        case sc2::ABILITY_ID::BUILD_REACTOR:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
        case sc2::ABILITY_ID::BUILD_TECHLAB:
            return ABIL_MORPH;
        default:
            return ABIL_NULL;
            break;
    }
}
} // end namespace
std::unique_ptr<Interface> gInterface;



