#include "AttackSquad.h"

void AttackSquad::OnStep(){
    for(auto& u : units){
        // TODO: add switch statement here so we use the right manager for the unit
        bm.doStateAction(u.get(), state, target);
        bm.validateState(u.get(), state, target);
    }
    validateState();
}

// state transitions: compare local enemy supply to supply in our squad
void AttackSquad::validateState(){
    int ourSupply = 0;
    for(auto& u : units){
        const sc2::Unit* unit = u.get()->getUnit();
        if(unit == nullptr) continue;
        sc2::UnitTypeData data = gInterface->observation->GetUnitTypeData()[unit->unit_type];
        ourSupply += data.food_required;
    }

    // get enemy supply in a circle w/ r=20 and the center at squad center
}