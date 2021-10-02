#include "BioManager.h"

void BioManager::doStateAction(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::State::Move:
        break;
        case Squad::State::Attack:
            // kite if our health is low; otherwise attack
        break;
        case Squad::State::Defend:
            // if unit is further than some radius from target (which is defense location) then move back towards defense location
        break;
        case Squad::State::Null:
        case Squad::State::Init:
        default:
    }
}

void BioManager::validateState(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target){
    switch(squadState){
        case Squad::State::Attack:
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
