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
}

void Bot::OnBuildingConstructionComplete(const Unit* building_){
    std::cout << UnitTypeToName(building_->unit_type) <<
        "(" << building_->tag << ") constructed" << std::endl;
}

void Bot::OnStep() {
    // initialize mapper (find expansions and (soon) ramps)
    if(Observation()->GetGameLoop() == 50)
        map.initialize(); 

    pm.OnStep();

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
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
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
                wm.OnUnitDestroyed(unit_);
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


