#include "Mapper.h"

void Mapper::initialize(){

}

Expansion Mapper::getClosestExpansion(sc2::Point2D point){
    Expansion tmp;
    return tmp;
}

void calculateExpansions(){
    // first step: get all minerals
    // probably inefficient so need to improve this in the future
    Units mineralPatches;
    auto neutralUnits = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral);
    for(const auto& unit : neutralUnits){
        if(unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
            unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750 ||
            unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750 ||
            unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750 ||
            unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750 ||
            unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750) mineralPatches.push_back(unit);
    }

    // second step: get geysers
    Units gasGeysers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsGeyser());

    // third: calculate mineral lines and use to create expansions

}

