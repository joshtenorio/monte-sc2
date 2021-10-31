#include "BioManager.h"

void BioManager::doStateAction(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::State::Move:
            doSquadMove(unit, target);
        break;
        case Squad::State::Attack:
            doSquadAttack(unit, target);
            // kite if our health is low; otherwise attack
        break;
        case Squad::State::Defend:
            doSquadDefend(unit, target);
            // if unit is further than some radius from target (which is defense location) then move back towards defense location
        break;
        case Squad::State::Null:
            doSquadNull(unit, target);
        break;
        case Squad::State::Init:
            doSquadInit(unit, target);
        break;
        default:
    }
}

void BioManager::validateState(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::State::Attack: // TODO: make functions for these
        case Squad::State::Move:
        break;
        case Squad::State::Defend:
        case Squad::State::Null:
        case Squad::State::Init:
        default:
    }
}

void BioManager::manageStim(Monte::GameObject* unit, Squad::State squadState){

    const sc2::Unit* unitObj = unit->getUnit();
    if(unitObj == nullptr) return;

}

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