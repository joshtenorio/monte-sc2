#include "api.h"

using namespace sc2;

std::unique_ptr<Interface> gInterface;

size_t API::CountUnitType(UNIT_TYPEID unitType) {
    return gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnit(unitType)).size();
}

sc2::UNIT_TYPEID API::abilityToUnitTypeID(sc2::ABILITY_ID ability){
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
        default:
        break;
    }
}