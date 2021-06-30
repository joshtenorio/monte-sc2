#include "BuildingManager.h"

void BuildingManager::OnStep(){

    // make sure all in-progress buildings are being worked on
    // kinda inefficient method
    bool workedOn = false;
    if(!inProgressBuildings.empty() && gInterface->observation->GetGameLoop() % 20 == 0) // make sure its not empty, and only do this every 20 loops
    for(auto& c : inProgressBuildings){
        sc2::Tag tag = c.first->tag;
        // TODO: this can be optimized if workermanager had a function called getClosestWorkers(pos, distance)
        //       here, pos would be location of the in progress building
        sc2::Units workers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_SCV));
        for(auto& w : workers){
            if(!w->orders.empty())
                if(w->orders.front().target_unit_tag == tag){
                    workedOn = true;
                    break;
                }
        }
        if(!workedOn){ // building is not being worked on, so get a worker to work on it
            Worker* w = gInterface->wm->getClosestWorker(c.first->pos);
            if(w != nullptr){
                gInterface->actions->UnitCommand(w->scv, sc2::ABILITY_ID::SMART, c.first); // target the building
                if(c.first->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERY || c.first->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
                    w->job = JOB_BUILDING_GAS;
                else
                    w->job = JOB_BUILDING;
            }
        } // end if !workedOn
    } // end for c : inProg

}

void BuildingManager::OnUnitDestroyed(const sc2::Unit* unit_){
    // if its an in-prog building that died, release the worker and remove Construction from list
    for(auto itr = inProgressBuildings.begin(); itr != inProgressBuildings.end(); ){
        if((*itr).first->tag == unit_->tag){ // the building is the one who died
            (*itr).second->job = JOB_UNEMPLOYED;
            itr = inProgressBuildings.erase(itr);
            return;
        }
        // TODO: is this sequence unecessary? i already handle it in OnStep
        else if((*itr).second->tag == unit_->tag){
            // worker is already gonna be destroyed when wm.OnUnitDestroyed(unit_) gets called in Bot.cpp
            // so we just need to assign a new, different worker
            Worker* newWorker = gInterface->wm->getClosestWorker(unit_->pos);
            size_t n = 0;
            while(newWorker->tag == unit_->tag){ // make sure new worker isn't the same one that just died
                newWorker = gInterface->wm->getNthWorker(n);
                n++;
            }
            (*itr).second = newWorker;
            gInterface->actions->UnitCommand(newWorker->scv, sc2::ABILITY_ID::SMART, (*itr).first); // target the building
            if((*itr).first->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY || (*itr).first->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
                newWorker->job = JOB_BUILDING_GAS;
            else
                newWorker->job = JOB_BUILDING;
            break;
        }
        else ++itr;
    } // end for loop
}

void BuildingManager::OnBuildingConstructionComplete(const sc2::Unit* building_){
    // remove Construction from list and set worker to unemployed, unless building was a refinery (in which case the new job is gathering gas)
    int index = 0;
    for(int i = 0; i < inProgressBuildings.size(); i++){
        if(inProgressBuildings[i].first->tag == building_->tag){
            index = i;
            if(building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERY ||
                building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH){
                inProgressBuildings[i].second->job = JOB_GATHERING_GAS;
            }
            else{
                inProgressBuildings[i].second->job = JOB_UNEMPLOYED;
            }
            break;
        }
    }
    inProgressBuildings.erase(inProgressBuildings.begin() + index);
}

void BuildingManager::OnUnitCreated(const sc2::Unit* building_){
    // if building_'s tag is identical to a building in inProgress, don't do anything
    for(auto& c : inProgressBuildings)
        if(c.first->tag == building_->tag) return;

    // assume the closest worker to the building is assigned to construct it
    Worker* w = gInterface->wm->getClosestWorker(building_->pos);
    if(building_->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY || building_->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
        w->job = JOB_BUILDING_GAS;
    else 
        w->job = JOB_BUILDING;
    inProgressBuildings.emplace_back(std::make_pair(building_, w));
}

bool BuildingManager::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type){

    // if unit is already building structure of this type, do nothing
    const Unit* unit_to_build = nullptr;
    Units units = gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_SCV));
    // if there is an in progress building, don't immediately build another
    // this one is for if worker dies
    if(checkConstructions(API::abilityToUnitTypeID(ability_type_for_structure))) return false;
    for(const auto& unit : units){
        for (const auto& order : unit->orders){
            if(order.ability_id == ability_type_for_structure) // checks if structure is already being built
                return false;
        }
        // identify SCV to build structure
        if(unit->unit_type == unit_type && gInterface->wm->isFree(gInterface->wm->getWorker(unit)))
            unit_to_build = unit;
    }
    
    // TODO: this can be refactored into switch statement
    if(ability_type_for_structure != ABILITY_ID::BUILD_REFINERY){
        sc2::Point2D loc = bp.findLocation(ability_type_for_structure, &(unit_to_build->pos));
        gInterface->actions->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            loc);
        return true;
    }
    else if (ability_type_for_structure == ABILITY_ID::BUILD_REFINERY){
        // we are building a refinery!
        // make sure there are geysers
        if(gInterface->map->getStartingExpansion().gasGeysers.size() <= 0)
            return false;
        const sc2::Unit* gas = bp.findUnit(ABILITY_ID::BUILD_REFINERY, &(unit_to_build->pos));

        gInterface->actions->UnitCommand(
           unit_to_build,
            ability_type_for_structure,
            gas);
        return true;
    }
    else return false;
}

bool BuildingManager::checkConstructions(sc2::UNIT_TYPEID building){
    for(auto c : inProgressBuildings)
        if(c.first->unit_type.ToType() == building) return true;

    return false;
}

