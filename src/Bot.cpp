#include "Bot.h"

using namespace sc2;

Bot::Bot() {
    wm = WorkerManager();
    gInterface.reset(new Interface(Observation(), Actions(), Query(), Debug()));
    map = Mapper();

}

void Bot::OnGameStart(){
    std::cout << "boop" << std::endl;

}

void Bot::OnBuildingConstructionComplete(const Unit* building_){
    std::cout << UnitTypeToName(building_->unit_type) <<
        "(" << building_->tag << ") constructed" << std::endl;
}

void Bot::OnStep() {
    if(Observation()->GetGameLoop() == 50){
        map.initialize(); 
    }

    if(Observation()->GetGameLoop() == 60){
        std::cout << "main has" << map.getStartingExpansion().gasGeysers.size() << " geysers\n";
    }

    tryBuildRefinery();
    TryBuildSupplyDepot();
    TryBuildBarracks();

    if(CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 10){
        Units marines = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
        for(const auto& m : marines){
            Actions()->UnitCommand(m, ABILITY_ID::ATTACK_ATTACK,
                Observation()->GetGameInfo().enemy_start_locations.front());
        } // end for loop
    } // end if marine count > 10
}

void Bot::OnUnitCreated(const Unit* unit_){
    std::cout << UnitTypeToName(unit_->unit_type) <<
        "(" << unit_->tag << ") was created" << std::endl;
    
}

void Bot::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()){
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
            if(Observation()->GetMinerals() >= 50){
                Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
                std::cout << "CC idle & minerals available, training SCV" << std:: endl;
            }
            break;
        case UNIT_TYPEID::TERRAN_SCV:
            wm.OnUnitIdle(unit);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKS:
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
        default:
            break;
    }
}

void Bot::OnUnitDestroyed(const Unit* unit_){
    std::cout << UnitTypeToName(unit_->unit_type) <<
         "(" << unit_->tag << ") was destroyed" << std::endl;
}

void Bot::OnUpgradeCompleted (UpgradeID id_){
    std::cout << UpgradeIDToName(id_) << " completed" << std::endl;
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

bool Bot::TryBuildStructure (ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type){
    // if unit is already building structure of this type, do nothing
    const Unit* unit_to_build = nullptr;
 Units units = Observation()->GetUnits(Unit::Alliance::Self);
    for(const auto& unit : units){
        for (const auto& order : unit->orders){
            if(order.ability_id == ability_type_for_structure) // checks if structure is already being built
                return false;
        }
        // get SCV to build structure
        if(unit->unit_type == unit_type)
            unit_to_build = unit;
    }

    if(ability_type_for_structure != ABILITY_ID::BUILD_REFINERY){
        // get a random location to build building within a 15x15 region where the scv is at a corner
        float rx = GetRandomScalar();
        float ry = GetRandomScalar();
        Actions()->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
        Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f)
        );
        return true;
    }
    else if (Observation()->GetGameLoop() > 50 && ability_type_for_structure == ABILITY_ID::BUILD_REFINERY){
        // we are building a refinery!
        // this needs to get fixed once we have multiple bases,
        // since this implementation will only work for main base
        if(map.getStartingExpansion().gasGeysers.size() <= 0){
            return false;
        }
        sc2::Point2D gas = map.getStartingExpansion().gasGeysers.front();
        Actions()->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            gas
        );
        return true;
    }
    else return false;
}

bool Bot::TryBuildSupplyDepot(){

    // if not supply capped, dont build supply depot
    if(Observation()->GetFoodUsed() <= Observation()->GetFoodCap() - 2)
        return false;
    
    // else, try and build depot using a random scv
    return TryBuildStructure (ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool Bot::TryBuildBarracks() {
    // check for depot and if we have 5 barracks already
    if(CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1 ||
        CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) == 5) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

// temporary: only build one refinery
bool Bot::tryBuildRefinery(){
    if(CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) == 1) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}

size_t Bot::CountUnitType(UNIT_TYPEID unitType) {
    return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unitType)).size();
}