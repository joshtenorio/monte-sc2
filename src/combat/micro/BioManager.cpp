#include "BioManager.h"


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