#include "AttackSquad.h"

void AttackSquad::OnStep(){
    for(auto& u : units){
        bm.doStateAction(u, state, target);
        bm.validateState(u, state, target);
    }
    validateState();
}

void AttackSquad::validateState(){
    
}