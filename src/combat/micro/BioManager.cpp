#include "BioManager.h"

void BioManager::doStateAction(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::Pause:
            break;
        case Squad::Move:
            break;
        case Squad::Attack:
            break;
        case Squad::Null:
        case Squad::Init:
        default:
    }
}

void BioManager::validateState(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::Pause:
            break;
        case Squad::Move:
            break;
        case Squad::Attack:
            break;
        case Squad::Null:
        case Squad::Init:
        default:
    }
}

void BioManager::manageStim(Monte::GameObject* unit, Squad::State squadState){

    const sc2::Unit* unitObj = unit->getUnit();
    if(unitObj == nullptr) return;

    std::vector<sc2::BuffID> buffs = unitObj->buffs;
    for(auto& b : buffs)
        if(b.ToType() == sc2::BUFF_ID::STIMPACK || b.ToType() == sc2::BUFF_ID::STIMPACKMARAUDER){
            return;
        }
    
    // if we have a decent amount of health left and there are enemies nearby
    if(
        unitObj->health/unitObj->health_max >= 0.6 &&
        !API::getClosestNUnits(unitObj->pos, 5, 8, sc2::Unit::Alliance::Enemy).empty()){

        switch(unitObj->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_MARINE:
                gInterface->actions->UnitCommand(unitObj, sc2::ABILITY_ID::EFFECT_STIM_MARINE);
                break;
            case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
                gInterface->actions->UnitCommand(unitObj, sc2::ABILITY_ID::EFFECT_STIM_MARAUDER);
                break;
            default:
                return;
        }
    }
}
