#include "api.h"

namespace API {

void OnGameStart(){
    // hehe
}

int countIdleUnits(sc2::UNIT_TYPEID type){
    sc2::Units units = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(type));
    int c = 0;
    for(auto& u : units){
        if(isUnitIdle(u)) c++;
    }
    return c;
}

bool isUnitIdle(const sc2::Unit* unit){
    if(unit != nullptr)
        return unit->orders.empty();
    else
        return false;
}

size_t CountUnitType(sc2::UNIT_TYPEID unitType) {
    return gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unitType)).size();
}

int countReadyUnits(sc2::UNIT_TYPEID type){
    sc2::Units units = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(type));
    int c = 0;
    for(auto& u : units)
        if(u->build_progress >= 1.0) c++;
    return c;
}


sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, sc2::UNIT_TYPEID unitType){
    sc2::Units pool;
    if(unitType == sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR) // default, get any 
        pool = gInterface->observation->GetUnits(alliance);
    
    else // TODO: should we just return the getClosestNUnits (filter imp) here?
        pool = gInterface->observation->GetUnits(alliance, IsUnit(unitType));

    sc2::Units output;
    for(auto& u : pool){
        if(sc2::DistanceSquared2D(u->pos, loc) < r*r){
            output.emplace_back(u);
        }
        if(output.size() >= n) break;
    }
    return output;
}

sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, std::vector<sc2::UNIT_TYPEID> unitTypes){
    return getClosestNUnits(loc, n, r, alliance, sc2::IsUnits(unitTypes));
}

sc2::Units getClosestNUnits(sc2::Point2D loc, int n, int r, sc2::Unit::Alliance alliance, sc2::Filter filter){
    sc2::Units pool;
    pool = gInterface->observation->GetUnits(alliance, filter);

    sc2::Units output;
    for(auto& u : pool){
        if(sc2::DistanceSquared2D(u->pos, loc) < r*r){
            output.emplace_back(u);
        }
        if(output.size() >= n) break;
    }
    return output;
}


sc2::ABILITY_ID unitTypeIDToAbilityID(sc2::UNIT_TYPEID unit){
    switch(unit){
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
            return sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS;
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
            return sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS;
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
            return sc2::ABILITY_ID::BUILD_REACTOR_FACTORY;
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
            return sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY;
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
            return sc2::ABILITY_ID::BUILD_REACTOR_STARPORT;
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
            return sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
            return sc2::ABILITY_ID::MORPH_ORBITALCOMMAND;
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            return sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS;

        case sc2::UNIT_TYPEID::TERRAN_ARMORY:
            return sc2::ABILITY_ID::BUILD_ARMORY;
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
            return sc2::ABILITY_ID::BUILD_BARRACKS;
        case sc2::UNIT_TYPEID::TERRAN_BUNKER:
            return sc2::ABILITY_ID::BUILD_BUNKER;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
            return sc2::ABILITY_ID::BUILD_COMMANDCENTER;  
        case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
            return sc2::ABILITY_ID::BUILD_ENGINEERINGBAY; 
        case sc2::UNIT_TYPEID::TERRAN_FACTORY:
            return sc2::ABILITY_ID::BUILD_FACTORY;
        case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
            return sc2::ABILITY_ID::BUILD_FUSIONCORE;
        case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
            return sc2::ABILITY_ID::BUILD_GHOSTACADEMY;
        case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
            return sc2::ABILITY_ID::BUILD_MISSILETURRET;
        case sc2::UNIT_TYPEID::TERRAN_REFINERY:
            return sc2::ABILITY_ID::BUILD_REFINERY;
        case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
            return sc2::ABILITY_ID::BUILD_SENSORTOWER;
        case sc2::UNIT_TYPEID::TERRAN_STARPORT:
            return sc2::ABILITY_ID::BUILD_STARPORT;
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
            return sc2::ABILITY_ID::BUILD_SUPPLYDEPOT;
        
        
        case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
            return sc2::ABILITY_ID::TRAIN_BANSHEE;
        case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
            return sc2::ABILITY_ID::TRAIN_MEDIVAC;
        case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
            return sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER;
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
            return sc2::ABILITY_ID::TRAIN_LIBERATOR;
        case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER:
            return sc2::ABILITY_ID::TRAIN_BATTLECRUISER;
        case sc2::UNIT_TYPEID::TERRAN_RAVEN:
            return sc2::ABILITY_ID::TRAIN_RAVEN;
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
            return sc2::ABILITY_ID::TRAIN_MARAUDER;
        case sc2::UNIT_TYPEID::TERRAN_MARINE:
            return sc2::ABILITY_ID::TRAIN_MARINE;
        case sc2::UNIT_TYPEID::TERRAN_REAPER:
            return sc2::ABILITY_ID::TRAIN_REAPER;
        case sc2::UNIT_TYPEID::TERRAN_GHOST:
            return sc2::ABILITY_ID::TRAIN_GHOST;
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE:
            return sc2::ABILITY_ID::TRAIN_WIDOWMINE;
        case sc2::UNIT_TYPEID::TERRAN_HELLION:
            return sc2::ABILITY_ID::TRAIN_HELLION;
        case sc2::UNIT_TYPEID::TERRAN_HELLIONTANK:
            return sc2::ABILITY_ID::TRAIN_HELLBAT;
        case sc2::UNIT_TYPEID::TERRAN_CYCLONE:
            return sc2::ABILITY_ID::TRAIN_CYCLONE;
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
            return sc2::ABILITY_ID::TRAIN_SIEGETANK;
        case sc2::UNIT_TYPEID::TERRAN_THOR:
            return sc2::ABILITY_ID::TRAIN_THOR;
    }
    std::cout << "API: unitTypeIDToAbilityID returning protoss assimilator" << std::endl;
    return sc2::ABILITY_ID::BUILD_ASSIMILATOR; // placeholder
}

std::vector<sc2::UNIT_TYPEID> getTechRequirements(sc2::ABILITY_ID ability){
    std::vector<sc2::UNIT_TYPEID> requirements;
    switch(ability){
        case sc2::ABILITY_ID::BUILD_BARRACKS:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED);
            break;

        case sc2::ABILITY_ID::TRAIN_HELLBAT:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_ARMORY);
        case sc2::ABILITY_ID::BUILD_STARPORT:
        case sc2::ABILITY_ID::BUILD_ARMORY:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING);
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        case sc2::ABILITY_ID::TRAIN_HELLION:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FACTORY);
            break;

        case sc2::ABILITY_ID::TRAIN_THOR:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_ARMORY);
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FACTORY);
            break;

        case sc2::ABILITY_ID::BUILD_FACTORY:
        case sc2::ABILITY_ID::BUILD_BUNKER:
        case sc2::ABILITY_ID::BUILD_GHOSTACADEMY:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING);
        case sc2::ABILITY_ID::TRAIN_MARINE:
        case sc2::ABILITY_ID::TRAIN_REAPER:
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
            break;

        case sc2::ABILITY_ID::TRAIN_GHOST:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY);
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::RESEARCH_COMBATSHIELD:
        case sc2::ABILITY_ID::RESEARCH_STIMPACK:
        case sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
            break;

        case sc2::ABILITY_ID::BUILD_ENGINEERINGBAY:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS);
            break;
        
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FUSIONCORE);
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_RAVEN:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        case sc2::ABILITY_ID::BUILD_FUSIONCORE:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING);
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_STARPORT);
            break;
        
        case sc2::ABILITY_ID::BUILD_MISSILETURRET:
        case sc2::ABILITY_ID::BUILD_SENSORTOWER:
        case sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS:
        case sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING:
        case sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME: // might be a different ID
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
            break;

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
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_ARMORY);
            break;

        case sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS:
        case sc2::ABILITY_ID::RESEARCH_RAPIDREIGNITIONSYSTEM:
        case sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FUSIONCORE);
            break;

        case sc2::ABILITY_ID::RESEARCH_SMARTSERVOS:
        case sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER:
        case sc2::ABILITY_ID::RESEARCH_CYCLONELOCKONDAMAGE:
        case sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
            break;
        
        case sc2::ABILITY_ID::RESEARCH_ENHANCEDSHOCKWAVES:
        case sc2::ABILITY_ID::BUILD_NUKE:
        case sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY);
            break;

        case sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR:
        case sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS:
        case sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
            break;
        
        case sc2::ABILITY_ID::MORPH_ORBITALCOMMAND:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING);
            break;

        case sc2::ABILITY_ID::BUILD_REACTOR:
        case sc2::ABILITY_ID::BUILD_TECHLAB:
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_FACTORY);
            requirements.emplace_back(sc2::UNIT_TYPEID::TERRAN_STARPORT);
            break;
    }
    return requirements;
}

sc2::ABILITY_ID upgradeIDToAbilityID(sc2::UpgradeID upgrade){
    switch(upgrade.ToType()){
        case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1:
            return sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1;
        case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2:
            return sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2;
        case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL3:
            return sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3;
        case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1:
            return sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1;
        case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2:
            return sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2;
        case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3:
            return sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3;
        case sc2::UPGRADE_ID::TERRANBUILDINGARMOR:
            return sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME; // might be a different ID
        case sc2::UPGRADE_ID::HISECAUTOTRACKING:
            return sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING;
        case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1:
            return sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1;
        case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2:
            return sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2;
        case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL3:
            return sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3;
        case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1:
            return sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1;
        case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2:
            return sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2;
        case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL3:
            return sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3;
        case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1:
            return sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1;
        case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2:
            return sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2;
        case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL3:
            return sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3;
        case sc2::UPGRADE_ID::YAMATOCANNON:
            return sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT;
        case sc2::UPGRADE_ID::LIBERATORAGRANGEUPGRADE:
            return sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS;
        case sc2::UPGRADE_ID::MEDIVACINCREASESPEEDBOOST:
            return sc2::ABILITY_ID::RESEARCH_RAPIDREIGNITIONSYSTEM;
        case sc2::UPGRADE_ID::PERSONALCLOAKING:
            return sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING;
        case sc2::UPGRADE_ID::ENHANCEDSHOCKWAVES:
            return sc2::ABILITY_ID::RESEARCH_ENHANCEDSHOCKWAVES;
        case sc2::UPGRADE_ID::STIMPACK:
            return sc2::ABILITY_ID::RESEARCH_STIMPACK;
        case sc2::UPGRADE_ID::PUNISHERGRENADES:
            return sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS;
        case sc2::UPGRADE_ID::SHIELDWALL:
            return sc2::ABILITY_ID::RESEARCH_COMBATSHIELD;
        case sc2::UPGRADE_ID::BANSHEESPEED:
            return sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS;
        case sc2::UPGRADE_ID::RAVENCORVIDREACTOR:
            return sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR;
        case sc2::UPGRADE_ID::BANSHEECLOAK:
            return sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD;
        case sc2::UPGRADE_ID::SMARTSERVOS:
            return sc2::ABILITY_ID::RESEARCH_SMARTSERVOS;
        case sc2::UPGRADE_ID::DRILLCLAWS:
            return sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS;
        case sc2::UPGRADE_ID::INFERNALPREIGNITERS:
            return sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER;
        case sc2::UPGRADE_ID::CYCLONELOCKONDAMAGEUPGRADE:
            return sc2::ABILITY_ID::RESEARCH_CYCLONELOCKONDAMAGE;

    }
    std::cout << "API: upgradeIDtoAbilityID returning protoss assimilator" << std::endl;
    return sc2::ABILITY_ID::BUILD_ASSIMILATOR; // placeholder
}

sc2::UNIT_TYPEID abilityToUnitTypeID(sc2::ABILITY_ID ability){
    switch(ability){
        // build structures...
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
        
        // train units...
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

        // research...
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS:
        case sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME: // might be a different ID
        case sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING:
            return sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY;
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
            return sc2::UNIT_TYPEID::TERRAN_ARMORY;
        case sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT:
        case sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS:
        case sc2::ABILITY_ID::RESEARCH_RAPIDREIGNITIONSYSTEM:
            return sc2::UNIT_TYPEID::TERRAN_FUSIONCORE;
        case sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING:
        case sc2::ABILITY_ID::RESEARCH_ENHANCEDSHOCKWAVES:
            return sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY;
        case sc2::ABILITY_ID::RESEARCH_STIMPACK:
        case sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS:
        case sc2::ABILITY_ID::RESEARCH_COMBATSHIELD:
            return sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB;
        case sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS:
        case sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR:
        case sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD:
            return sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB;
        case sc2::ABILITY_ID::RESEARCH_SMARTSERVOS:
        case sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS:
        case sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER:
        case sc2::ABILITY_ID::RESEARCH_CYCLONELOCKONDAMAGE:
            return sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB;

        // morphs...
        case sc2::ABILITY_ID::MORPH_ORBITALCOMMAND:
        case sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS:
            return sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER;
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
            return sc2::UNIT_TYPEID::TERRAN_BARRACKS;
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
            return sc2::UNIT_TYPEID::TERRAN_FACTORY;
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
            return sc2::UNIT_TYPEID::TERRAN_STARPORT;
        case sc2::ABILITY_ID::BUILD_REACTOR: // not sure what to do with these, probably just return default
        case sc2::ABILITY_ID::BUILD_TECHLAB:
        default:
            std::cout << "API: abilitytounittypeid returning protoss assimilator" << std::endl;
            return sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR; // placeholder
    }
}

sc2::UNIT_TYPEID getProducer(sc2::ABILITY_ID unit){
    switch(unit){
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        case sc2::ABILITY_ID::TRAIN_RAVEN:
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
            return sc2::UNIT_TYPEID::TERRAN_STARPORT;
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        case sc2::ABILITY_ID::TRAIN_HELLION:
        case sc2::ABILITY_ID::TRAIN_HELLBAT:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_THOR:
            return sc2::UNIT_TYPEID::TERRAN_FACTORY;
        case sc2::ABILITY_ID::TRAIN_MARINE:
        case sc2::ABILITY_ID::TRAIN_REAPER:
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::TRAIN_GHOST:
            return sc2::UNIT_TYPEID::TERRAN_BARRACKS;
        case sc2::ABILITY_ID::TRAIN_SCV:
            // don't know what to do for this one since it is townhall
            break;
        default:
            std::cout << "API: buildingForUnit returning protoss assimilator" << std::endl;
            return sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR; // placeholder
    }
}

bool requiresTechLab(sc2::ABILITY_ID unit){
    switch(unit){
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_RAVEN:
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_THOR:
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::TRAIN_GHOST:
            return true;
        default:
            return false;
    }
}

bool isStructure(sc2::UNIT_TYPEID unit){
    switch(unit){
        case sc2::UNIT_TYPEID::TERRAN_ARMORY:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
        case sc2::UNIT_TYPEID::TERRAN_BUNKER:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case sc2::UNIT_TYPEID::TERRAN_FACTORY:
        case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
        case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
        case sc2::UNIT_TYPEID::TERRAN_REFINERY:
        case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
        case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
        case sc2::UNIT_TYPEID::TERRAN_STARPORT:
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
            return true;
        default:
            return false;
    }
}

bool isTownHall(sc2::UNIT_TYPEID unit){
    switch(unit){
        case sc2::UNIT_TYPEID::PROTOSS_NEXUS:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
        case sc2::UNIT_TYPEID::ZERG_HATCHERY:
        case sc2::UNIT_TYPEID::ZERG_HIVE:
        case sc2::UNIT_TYPEID::ZERG_LAIR:
            return true;
        default:
            return false;
    }
}

bool isAddon(sc2::UNIT_TYPEID unit){
    switch(unit){
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
            return true;
        default:
            return false;
    }
}


} // end namespace
std::unique_ptr<Interface> gInterface;
