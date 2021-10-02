#include "AttackSquad.h"

void AttackSquad::OnStep(){
    for(auto& u : units){
        // TODO: add switch statement here so we use the right manager for the unit
        bm.doStateAction(u.get(), state, target);
        bm.validateState(u.get(), state, target);
    }
    validateState();
}

void AttackSquad::validateState(){
    
}