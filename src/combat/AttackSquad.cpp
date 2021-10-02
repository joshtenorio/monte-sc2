#include "AttackSquad.h"

void AttackSquad::OnStep(){
    for(auto& u : units){
        // TODO: add switch statement here so we use the right manager for the unit
        //       need to code mechmanager and flyermanager first :p
        bm.doStateAction(u.get(), state, target);
        bm.validateState(u.get(), state, target);
    }
    validateState();
}

// state transitions: compare local enemy supply to supply in our squad
void AttackSquad::validateState(){
    int ourSupply = 0;
    int localEnemySupply = 0;
    for(auto& u : units){
        const sc2::Unit* unit = u.get()->getUnit();
        if(unit == nullptr) continue;
        sc2::UnitTypeData data = gInterface->observation->GetUnitTypeData()[unit->unit_type];
        ourSupply += data.food_required;
    }

    // get enemy supply in a circle w/ r=20 and the center at squad center
    sc2::Point2D center = getCenter();
    // if center = (-1, -1) it is likely squad is empty, so don't do anything
    if(center.x == -1){
        setState(Squad::State::Null);
        return;
    }
    else{
        sc2::Units localEnemy = API::getClosestNUnits(center, 200, 20, sc2::Unit::Alliance::Enemy);
        for(auto& e : localEnemy){
            if(e == nullptr) continue;
            sc2::UnitTypeData data = gInterface->observation->GetUnitTypeData()[e->unit_type];
            localEnemySupply += data.food_required;
        }
    }

    // how confident we are that we can win w/ even or slightly less supply
    int confidence = 5;

    // validate states
    switch(state){
        case Squad::State::Attack:
        break;
        case Squad::State::Bide:
        break;
        case Squad::State::Defend:
        break;
        case Squad::State::Move:
        break;
        case Squad::State::Init:
        default:

    }
}