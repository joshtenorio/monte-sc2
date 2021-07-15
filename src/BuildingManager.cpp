#include "BuildingManager.h"

void BuildingManager::OnStep(){

    // debug
    if(gInterface->observation->GetGameLoop() % 400 == 0){
        logger.infoInit().withInt(inProgressBuildings.size()).withStr("in progress buildings").write();
    }

    // clear the cache of reserved workers every once in a while
    if(gInterface->observation->GetGameLoop() % 5 == 0)
        reservedWorkers.clear();
    
    // make sure all in-progress buildings are being worked on
    // kinda inefficient method
    bool workedOn = false;
    if(!inProgressBuildings.empty() && gInterface->observation->GetGameLoop() % 20 == 0) // make sure its not empty, and only do this every 20 loops
    for(auto& c : inProgressBuildings){
        // TODO: this can be optimized if workermanager had a function called getClosestWorkers(pos, distance)
        //       here, pos would be location of the in progress building
        sc2::Units workers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_SCV));
        for(auto& w : workers){
            if(w != nullptr){ // TODO: not sure if this check is necessary
                if(!w->orders.empty())
                    if(w->orders.front().target_unit_tag == c.first){
                        workedOn = true;
                        break;
                    }
            }
        }
        if(!workedOn){ // building is not being worked on, so get a worker to work on it
            const sc2::Unit* building = gInterface->observation->GetUnit(c.first);
            if(building == nullptr) continue;

            Worker* w = gInterface->wm->getClosestWorker(building->pos);
            if(w != nullptr){
                gInterface->actions->UnitCommand(w->getUnit(), sc2::ABILITY_ID::SMART, building); // target the building
                if(building->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERY || building->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
                    w->job = JOB_BUILDING_GAS;
                else
                    w->job = JOB_BUILDING;
                c.second = w;
            }
        } // end if !workedOn
    } // end for c : inProg

}

void BuildingManager::OnUnitDestroyed(const sc2::Unit* unit_){
    // if its an in-prog building that died, release the worker and remove Construction from list
    for(auto itr = inProgressBuildings.begin(); itr != inProgressBuildings.end(); ){
       // the building died, so release the worker and remove Construction
       if((*itr).first == unit_->tag){
            (*itr).second->job = JOB_UNEMPLOYED;
            itr = inProgressBuildings.erase(itr);
            break;
       }
       // the worker died so need to assign a new, differnent worker
       // dead worker's object will already be taken care of in workermanager
       else if((*itr).second->tag == unit_->tag){
            Worker* newWorker = gInterface->wm->getClosestWorker(unit_->pos);
            if(newWorker == nullptr) return;
            size_t n = 0;
            while(newWorker->tag == unit_->tag){ // make sure new worker isn't the same one that just died
                newWorker = gInterface->wm->getNthWorker(n);
                if(newWorker == nullptr) return;
                n++;
                if(n >= gInterface->wm->getNumWorkers()) return;
            }

            const sc2::Unit* building = gInterface->observation->GetUnit((*itr).first);
            if(building == nullptr) return;
            
            gInterface->actions->UnitCommand(newWorker->getUnit(), sc2::ABILITY_ID::SMART, building); // target the building
            if(building->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY || building->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
                newWorker->job = JOB_BUILDING_GAS;
            else
                newWorker->job = JOB_BUILDING;
            (*itr).second = newWorker;
            break;
       }
       else ++itr;
    }

}

void BuildingManager::OnBuildingConstructionComplete(const sc2::Unit* building_){
    // remove Construction from list and set worker to unemployed, unless building was a refinery (in which case the new job is gathering gas)
    for(auto itr = inProgressBuildings.begin(); itr != inProgressBuildings.end(); ){
        if((*itr).first == building_->tag){
            if(building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERY ||
                building_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
                (*itr).second->job = JOB_GATHERING_GAS;
            else
                (*itr).second->job = JOB_UNEMPLOYED;
            
            itr = inProgressBuildings.erase(itr);
        }
        else ++itr;
    } // end for
    
}

void BuildingManager::OnUnitCreated(const sc2::Unit* building_){
    // if building_'s tag is identical to a building in inProgress, don't do anything
    for(auto& c : inProgressBuildings)
        if(c.first == building_->tag) return;

    // assume the closest worker to the building is assigned to construct it
    Worker* w = gInterface->wm->getClosestWorker(building_->pos);
    if(w == nullptr) return;
    if(building_->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY || building_->unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH)
        w->job = JOB_BUILDING_GAS;
    else 
        w->job = JOB_BUILDING;
    inProgressBuildings.emplace_back(std::make_pair(building_->tag, w));

    for(auto itr = reservedWorkers.begin(); itr != reservedWorkers.end(); ){
        if((*itr) == w->tag)  itr = reservedWorkers.erase(itr);
        else ++itr;
    }    
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

        bool isReserved = false;
        for(auto& t : reservedWorkers)
            if(t == unit->tag){
                isReserved = true; 
                break;
            }
                
        
        if(isReserved) continue;

        if(num >= maxConcurrent) return false;

        // identify SCV to build structure
        if(unit->unit_type == unit_type && gInterface->wm->isFree(gInterface->wm->getWorker(unit)))
            unit_to_build = unit;
    }
    
    if(unit_to_build == nullptr)
        return false;

    // TODO: this can be refactored into switch statement
    if(ability_type_for_structure != sc2::ABILITY_ID::BUILD_REFINERY){
        sc2::Point2D loc = bp.findLocation(ability_type_for_structure, unit_to_build->pos);
        if(loc.x == POINT2D_NULL.x || loc.y == POINT2D_NULL.y) return false;
        
        reservedWorkers.emplace_back(unit_to_build->tag);
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
        if(gas == nullptr) return false;

        reservedWorkers.emplace_back(unit_to_build->tag);
        gInterface->actions->UnitCommand(
           unit_to_build,
            ability_type_for_structure,
            gas);
        return true;
    }
    else return false;
}

bool BuildingManager::checkConstructions(sc2::UNIT_TYPEID building){
    for(auto c : inProgressBuildings){
        const sc2::Unit* construction = gInterface->observation->GetUnit(c.first);
        if(construction == nullptr) return false;
        if(construction->unit_type.ToType() == building) return true;
    }
        

    return false;
}

