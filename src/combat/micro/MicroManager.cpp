#include "combat/micro/MicroManager.h"

void MicroManager::doStateAction(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
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

void MicroManager::validateState(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::State::Move:
            validateSquadMove(unit, target);
        break;
        case Squad::State::Attack:
            validateSquadAttack(unit, target);
            // kite if our health is low; otherwise attack
        break;
        case Squad::State::Defend:
            validateSquadDefend(unit, target);
            // if unit is further than some radius from target (which is defense location) then move back towards defense location
        break;
        case Squad::State::Null:
            validateSquadNull(unit, target);
        break;
        case Squad::State::Init:
            validateSquadInit(unit, target);
        break;
        default:
    }
}