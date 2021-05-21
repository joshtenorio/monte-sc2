#include "BuildingManager.h"

void BuildingManager::OnStep(){
    tryBuildRefinery();
    TryBuildBarracks();
    TryBuildSupplyDepot();

    for(auto c : inProgressBuildings){
        std::cout << "building: " << c.first->tag << "\tworker: " << c.second->tag << "\t";
    }
    std::cout << "\n";

    // FIXME: buildingmanager will still try to build a reactor even if it doesn't fit
    if(API::CountUnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS) >= 1){
        const Unit* barracks = bp.findUnit(sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS, nullptr);
        if(barracks != nullptr){
            gInterface->actions->UnitCommand(barracks, sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS);
        }
    }
}

void BuildingManager::OnUnitDestroyed(const sc2::Unit* unit_){
    // if its an in-prog building that died, release the worker and remove Construction from list
    std::cout << "in prog list size: " << inProgressBuildings.size() << std::endl;
    for(auto itr = inProgressBuildings.begin(); itr != inProgressBuildings.end(); ){
        std::cout << "in loop\t";
        std::cout << "building tag: " << (*itr).first->tag << "\t";
        std::cout << "worker tag: " << (*itr).second->tag << "\n";
        if((*itr).first->tag == unit_->tag){ // the building is the one who died
            (*itr).second->job = JOB_UNEMPLOYED;
            itr = inProgressBuildings.erase(itr);
            std::cout << unit_->unit_type.to_string() << " was removed from inprogress list" << std::endl;
            return;
        }
        else if((*itr).second->tag == unit_->tag){
            // worker is already gonna be destroyed when wm.OnUnitDestroyed(unit_) gets called in Bot.cpp
            // so we just need to assign a new, different worker
            // BUG: crashes when worker dies sometimes
            // BUG: when 3rd worker sent to barracks dies it doesn't send another
            // BUG: the two above (or at least the first) occurs when worker tag is empty
            // thought: worker tag disappears bc maybe the Unit obj for the worker gets deleted already?
            //          i think this can be fixed by making my own id for Workers instead of relying on the Unit tag for this
            Worker* newWorker = gInterface->wm->getClosestWorker(unit_->pos);
            size_t n = 0;
            while(newWorker->tag == unit_->tag){ // make sure new worker isn't the same one that just died
                newWorker = gInterface->wm->getNthWorker(n);
                n++;
            }
            (*itr).second = newWorker;
            std::cout << "dead worker: " << unit_->tag << "\t new worker: " << (*itr).second->tag << "\n";
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
    // remove Construction from list and set worker to unemployed
    int index = 0;
    for(int i = 0; i < inProgressBuildings.size(); i++){
        if(inProgressBuildings[i].first->tag == building_->tag){
            inProgressBuildings[i].second->job = JOB_UNEMPLOYED;
            index = i;
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
    std::cout << "Construction created - worker tag: " << w->scv->tag << "\n";
}

bool BuildingManager::TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type){
    // if unit is already building structure of this type, do nothing
    const Unit* unit_to_build = nullptr;
    Units units = gInterface->observation->GetUnits(Unit::Alliance::Self);
    for(const auto& unit : units){
        for (const auto& order : unit->orders){
            if(order.ability_id == ability_type_for_structure) // checks if structure is already being built
                return false;
        }
        // identify SCV to build structure
        if(unit->unit_type == unit_type)
            unit_to_build = unit;
    }

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
        // this needs to get fixed once we have multiple bases,
        // since this implementation will only work for main base
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

bool BuildingManager::TryBuildSupplyDepot(){

    // if not supply capped, dont build supply depot
    if(gInterface->observation->GetFoodUsed() <= gInterface->observation->GetFoodCap() - 2 || gInterface->observation->GetMinerals() < 100)
        return false;
    
    // else, try and build depot using a random scv
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
}

bool BuildingManager::TryBuildBarracks() {
    // check for depot and if we have 5 barracks already
    if(API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) + API::CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED) < 1 ||
        API::CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >= 8) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
}

// temporary: only build one refinery
bool BuildingManager::tryBuildRefinery(){
    if(gInterface->observation->GetGameLoop() < 100 || API::CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) >= 1 ||
        gInterface->observation->GetMinerals() < 75) return false;
    return TryBuildStructure(ABILITY_ID::BUILD_REFINERY);
}