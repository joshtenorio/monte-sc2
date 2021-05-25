#include "Bot.h"

using namespace sc2;

Bot::Bot(){
    wm = WorkerManager();
    map = Mapper();
    pm = ProductionManager();

    gInterface.reset(new Interface(Observation(), Actions(), Query(), Debug(), &wm, &map));
}

void Bot::OnGameStart(){
    std::cout << "boop" << std::endl;
    pm.OnGameStart();
    std::cout << "map name: " << Observation()->GetGameInfo().map_name << "\n";

}

void Bot::OnBuildingConstructionComplete(const Unit* building_){
    std::cout << UnitTypeToName(building_->unit_type) <<
        "(" << building_->tag << ") constructed" << std::endl;

    // if it is a supply depot, lower it
    if(building_->unit_type == sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT)
        Actions()->UnitCommand(building_, sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER);
    
    switch(building_->unit_type.ToType()){
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
            pm.OnBuildingConstructionComplete(building_);
            break;
        default:
            break;
    }
}

void Bot::OnStep() {
    // initialize mapper (find expansions and (soon) ramps)
    if(Observation()->GetGameLoop() == 50)
        map.initialize(); 
    pm.OnStep();
    wm.OnStep();


    if(API::CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 10){
        Units marines = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
        for(const auto& m : marines){
            Actions()->UnitCommand(
                m,
                ABILITY_ID::ATTACK_ATTACK,
                Observation()->GetGameInfo().enemy_start_locations.front());
        } // end for loop
    } // end if marine count > 10
}

void Bot::OnUnitCreated(const Unit* unit_){
    std::cout << UnitTypeToName(unit_->unit_type) <<
        "(" << unit_->tag << ") was created" << std::endl;
    
    switch(unit_->unit_type.ToType()){
        case UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitCreated(unit_);
            break;
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
            pm.OnUnitCreated(unit_);
            break;
        default:
            break;
    }
}

void Bot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()){
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
            break;
        case UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitIdle(unit);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKS:
            break;
        case UNIT_TYPEID::TERRAN_REFINERY:
        case UNIT_TYPEID::TERRAN_REFINERYRICH:
            std::cout << "bowo refinery is idle\n";
            break;
        default:
            break;
    }
}

void Bot::OnUnitDestroyed(const Unit* unit_){
    std::cout << UnitTypeToName(unit_->unit_type) <<
         "(" << unit_->tag << ") was destroyed" << std::endl;
    
    // we only care if one of our units dies (for now, perhaps)
    if(unit_->alliance == Unit::Alliance::Self)
        switch(unit_->unit_type.ToType()){
            case UNIT_TYPEID::TERRAN_SCV:
                // the pm gets called first, bc we need to remove worker pointer from 
                // a Construction if it is building it
                pm.OnUnitDestroyed(unit_);
                wm.OnUnitDestroyed(unit_);
                break;
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
                pm.OnUnitDestroyed(unit_);
                break;
            default:
                break;
        }

}

void Bot::OnError(const std::vector<ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors){
    for (const auto i : client_errors) {
        std::cerr << "Encountered client error: " <<
            static_cast<int>(i) << std::endl;
    }

    for (const auto& i : protocol_errors)
        std::cerr << "Encountered protocol error: " << i << std::endl;
}


