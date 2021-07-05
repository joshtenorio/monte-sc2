#include "BuildingManager.h"

void BuildingManager::OnStep(){

    // debug
    if(gInterface->observation->GetGameLoop() % 400 == 0)
        printf("buildingManager: %d in progress buildings\n", inProgressBuildings.size());

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
            if(w != nullptr){ // TODO: not sure if this check is necessary
                if(!w->orders.empty())
                    if(w->orders.front().target_unit_tag == tag){
                        workedOn = true;
                        break;
                    }
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
    int index = -1;
    for(int i = 0; i < inProgressBuildings.size(); i++){
       // the building died, so release the worker and remove Construction
       if(inProgressBuildings[i].first->tag == unit_->tag){
           inProgressBuildings[i].second->job = JOB_UNEMPLOYED;
           index = i;
           break;
       }
       // the worker died so need to assign a new, differnent worker
       // dead worker's object will already be taken care of in workermanager
       else if(inProgressBuildings[i].second->tag == unit_->tag){
            Worker* newWorker = gInterface->wm->getClosestWorker(unit_->pos);
            size_t n = 0;
            while(newWorker->tag == unit_->tag){ // make sure new worker isn't the same one that just died
                newWorker = gInterface->wm->getNthWorker(n);
                n++;
            }
            gInterface->actions->UnitCommand(newWorker->scv, sc2::ABILITY_ID::SMART, inProgressBuildings[i].first); // target the building
            if(inProgressBuildings[i].first->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY || inProgressBuildings[i].first->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
                newWorker->job = JOB_BUILDING_GAS;
            else
                newWorker->job = JOB_BUILDING;
            inProgressBuildings[i].second = newWorker;
            break;
       }
    }
    if(index >=0)
        inProgressBuildings.erase(inProgressBuildings.begin() + index);
}

void BuildingManager::OnBuildingConstructionComplete(const sc2::Unit* building_){
    // remove Construction from list and set worker to unemployed, unless building was a refinery (in which case the new job is gathering gas)
    int index = -1;
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
    if(index >= 0)
        inProgressBuildings.erase(inProgressBuildings.begin() + index);
}

void BuildingManager::OnUnitCreated(const sc2::Unit* building_){
    // if building_'s tag is identical to a building in inProgress, don't do anything
    for(auto& c : inProgressBuildings)
        if(c.first->tag == building_->tag) return;

    // assume the closest worker to the building is assigned to construct it
    Worker* w = gInterface->wm->getClosestWorker(building_->pos);
    if(w == nullptr) return;
    if(building_->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY || building_->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
        w->job = JOB_BUILDING_GAS;
    else 
        w->job = JOB_BUILDING;
    inProgressBuildings.emplace_back(std::make_pair(building_, w));
}

bool BuildingManager::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, int maxConcurrent, sc2::UNIT_TYPEID unit_type){

    const sc2::Unit* unit_to_build = nullptr;
    sc2::Units units = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_SCV));

    // if there is an in progress building, don't immediately build another
    if(checkConstructions(API::abilityToUnitTypeID(ability_type_for_structure))) return false;

    int num = 0;
    for(const auto& unit : units){
        // check how many workers are already building this
        
        for (const auto& order : unit->orders){
            if(order.ability_id == ability_type_for_structure) // checks if structure is already being built
                num++;
        }

        if(num >= maxConcurrent) return false;

        // identify SCV to build structure
        if(unit->unit_type == unit_type && gInterface->wm->isFree(gInterface->wm->getWorker(unit)))
            unit_to_build = unit;
    }
    
    // TODO: this can be refactored into switch statement
    if(unit_to_build == nullptr)
        return false;

    if(ability_type_for_structure != sc2::ABILITY_ID::BUILD_REFINERY){
        sc2::Point2D loc = bp.findLocation(ability_type_for_structure, unit_to_build->pos);
        gInterface->wm->getWorker(unit_to_build)->job = JOB_BUILDING;
        gInterface->actions->UnitCommand(
            unit_to_build,
            ability_type_for_structure,
            loc);
        return true;
    }
    else if (ability_type_for_structure == sc2::ABILITY_ID::BUILD_REFINERY){
        // we are building a refinery!
        // make sure there are geysers
        if(gInterface->map->getStartingExpansion().gasGeysers.size() <= 0)
            return false;
        const sc2::Unit* gas = bp.findUnit(ABILITY_ID::BUILD_REFINERY, &(unit_to_build->pos));
        gInterface->wm->getWorker(unit_to_build)->job = JOB_BUILDING_GAS;
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

