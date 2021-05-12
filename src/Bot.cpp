#include "Bot.h"

using namespace sc2;
const Unit* booper = nullptr;
int hehe = 0;
Bot::Bot() {
    wm = WorkerManager();
    gInterface.reset(new Interface(Observation(), Actions(), Query()));
    map = Mapper();

}

void Bot::OnGameStart(){
    std::cout << "boop" << std::endl;

    //Overseer::MapImpl map;

    //map.setBot(this);
    //map.initialize();
    //std::cout << "Overseer initialized" << std::endl;
    map.initialize();
}

void Bot::OnBuildingConstructionComplete(const Unit* building_){
    std::cout << UnitTypeToName(building_->unit_type) <<
        "(" << building_->tag << ") constructed" << std::endl;
}

void Bot::OnStep() {

    if(booper != nullptr){
        Point2D p = Point2D(booper->pos);

        if(Query()->Placement(sc2::ABILITY_ID::BUILD_COMMANDCENTER, p)){
            std::cout << "cc fits at (" << p.x << ", " << p.y <<")\n";
        }
        else {
            std::cout <<"nope\n";
        }

        
    } // end if booper != nullptr


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
            if(Observation()->GetMinerals() >= 50 & hehe == 0){
                Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
                std::cout << "CC idle & minerals available, training SCV" << std:: endl;
                hehe = 1;
            }
            break;
        case UNIT_TYPEID::TERRAN_SCV:
            booper = unit;
            //wm.OnUnitIdle(unit);
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

    float rx = GetRandomScalar();
    float ry = GetRandomScalar();

    Actions()->UnitCommand(
        unit_to_build,
        ability_type_for_structure,
     Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f)
    );
    return true;
}

bool Bot::TryBuildSupplyDepot(){
    //const ObservationInterface* observation = Observation(); // not needed perhaps?

    // if not supply capped, dont build supply depot
    if(Observation()->GetFoodUsed() <= Observation()->GetFoodCap() - 2)
        return false;
    
    // else, try and build depot using a random scv
    return TryBuildStructure (ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool Bot::TryBuildBarracks() {
    // check for depot and if we have 3 barracks already (ie build 3 barracks max)
    if(CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1 ||
        CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) == 3) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

size_t Bot::CountUnitType(UNIT_TYPEID unitType) {
    return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unitType)).size();
}