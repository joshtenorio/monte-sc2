#include "BioManager.h"

void BioManager::doSquadMove(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::doSquadAttack(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::doSquadDefend(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::doSquadNull(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::doSquadInit(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::validateSquadMove(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::validateSquadAttack(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::validateSquadDefend(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::validateSquadNull(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::validateSquadInit(Monte::GameObject* unit, sc2::Point2D target){

}

void BioManager::manageStim(Monte::GameObject* gameObj, Squad::State squadState){

    const sc2::Unit* unit = gameObj->getUnit();
    if(unit == nullptr) return;

    // copy-pasted old stim code from v0.8.0, pertinent bug: 
    // in order to do so we need to cast gameObj into Bio type
    // TODO: do stim state action
    std::vector<sc2::BuffID> buffs = unit->buffs;
    for(auto& b : buffs)
        if(b.ToType() == sc2::BUFF_ID::STIMPACK || b.ToType() == sc2::BUFF_ID::STIMPACKMARAUDER){
            return;
        }
    
    // if we have a decent amount of health left and there are enemies nearby
    if(
        unit->health/unit->health_max >= 0.6 &&
        !API::getClosestNUnits(unit->pos, 5, 8, sc2::Unit::Alliance::Enemy).empty()){

        switch(unit->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_MARINE:
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::EFFECT_STIM_MARINE);
                break;
            case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
                gInterface->actions->UnitCommand(unit, sc2::ABILITY_ID::EFFECT_STIM_MARAUDER);
                break;
            default:
                return;
        }
    }
    // TODO: validate stim state
    
}
