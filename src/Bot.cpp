#include "Bot.h"

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_api.h>
#include <iostream>


Bot::Bot()
{}

void Bot::OnGameStart()
{
    std::cout << "boop" << std::endl;
}

void Bot::OnBuildingConstructionComplete(const sc2::Unit* building_)
{
    std::cout << sc2::UnitTypeToName(building_->unit_type) <<
        "(" << building_->tag << ") constructed" << std::endl;
}

void Bot::OnStep() {
    TryBuildSupplyDepot();
}

void Bot::OnUnitCreated(const sc2::Unit* unit_)
{
    std::cout << sc2::UnitTypeToName(unit_->unit_type) <<
        "(" << unit_->tag << ") was created" << std::endl;
}

void Bot::OnUnitIdle(const sc2::Unit* unit) {
    switch (unit->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:{
            if(Observation()->GetMinerals() >= 50){
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
                std::cout << "CC idle & minerals available, training SCV" << std:: endl;
            }
            break;
        }
        case sc2::UNIT_TYPEID::TERRAN_SCV:{
            //worker_manager.FindNearestMineralPatch(Point2D(unit->pos));
            break;
        }
        default: {
            break;
        }
    }
}

void Bot::OnUnitDestroyed(const sc2::Unit* unit_)
{
    std::cout << sc2::UnitTypeToName(unit_->unit_type) <<
         "(" << unit_->tag << ") was destroyed" << std::endl;
}

void Bot::OnUpgradeCompleted(sc2::UpgradeID id_)
{
    std::cout << sc2::UpgradeIDToName(id_) << " completed" << std::endl;
}

void Bot::OnError(const std::vector<sc2::ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors)
{
    for (const auto i : client_errors) {
        std::cerr << "Encountered client error: " <<
            static_cast<int>(i) << std::endl;
    }

    for (const auto& i : protocol_errors)
        std::cerr << "Encountered protocol error: " << i << std::endl;
}

bool Bot::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type){
    // if unit is already building structure of this type, do nothing
    const sc2::Unit* unit_to_build = nullptr;
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    for(const auto& unit : units){
        for (const auto& order : unit->orders){
            if(order.ability_id == ability_type_for_structure) // checks if structure is already being built
                return false;
        }
        // get SCV to build structure
        if(unit->unit_type == unit_type)
            unit_to_build = unit;
    }

    float rx = sc2::GetRandomScalar();
    float ry = sc2::GetRandomScalar();

    Actions()->UnitCommand(
        unit_to_build,
        ability_type_for_structure,
        sc2::Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f)
    );
    return true;
}

bool Bot::TryBuildSupplyDepot(){
    //const sc2::ObservationInterface* observation = Observation(); // not needed perhaps?

    // if not supply capped, dont build supply depot
    if(Observation()->GetFoodUsed() <= Observation()->GetFoodCap() - 2)
        return false;
    
    // else, try and build depot using a random scv
    return TryBuildStructure(sc2::ABILITY_ID::BUILD_SUPPLYDEPOT);
}