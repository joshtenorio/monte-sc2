/**
 * @file WorkerManager.cpp
 * @author Joshua Tenorio
 * 
 * Implementation of the Worker Manager class.
 */
#include "WorkerManager.h"

#include "api.h" // this breaks our "code style," but it is necessary for now to avoid circular definition


WorkerManager::WorkerManager(){
    logger = Logger("WorkerManager");
    logger.initializePlot({"game loop", "long distance miners"}, "long distance mining");
}
const sc2::Unit* Worker::getUnit(){
    return gInterface->observation->GetUnit(tag);
}
void WorkerManager::OnStep(){

    //printDebug();

    // run distributeWorkers every 15 loops and after everything initializes
    if(gInterface->observation->GetGameLoop() % 15 == 0 && gInterface->observation->GetGameLoop() > 500){
        DistributeWorkers();
    }
}

void WorkerManager::OnUnitCreated(const sc2::Unit* unit_){
    Worker w;
    if(gInterface->observation->GetGameLoop() < 20) w.job = JOB_GATHERING_MINERALS;
    else w.job = JOB_UNEMPLOYED;
    w.tag = (*unit_).tag;
    workers.emplace_back(w);
    //if(gInterface->observation->GetGameLoop() > 100) OnUnitIdle(unit_);
    const sc2::Unit* mineralTarget = FindNearestMineralPatch(unit_->pos);
    if(mineralTarget != nullptr)
        gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::SMART, mineralTarget);
    
}

void WorkerManager::OnUnitDestroyed(const sc2::Unit* unit_){
    sc2::Tag key = unit_->tag;
    int index = 0;
    for(auto itr = workers.begin(); itr != workers.end(); ){
        if((*itr).tag == key){
            itr = workers.erase(itr);
            break;
        }
        else ++itr;
    }
}

void WorkerManager::OnUnitIdle(const sc2::Unit* unit_){
    // send to mine at the current base
    Expansion* e = gInterface->map->getNthExpansion(0);
    if (e != nullptr){
        const sc2::Unit* mineralTarget = FindNearestMineralPatch(e->baseLocation);
        gInterface->actions->UnitCommand(unit_, sc2::ABILITY_ID::SMART, mineralTarget);
        getWorker(unit_)->job = JOB_GATHERING_MINERALS;
    }
    else{
        logger.errorInit().withStr("e is nullptr").write();
    }
}

void WorkerManager::DistributeWorkers(int gasWorkers){
    // 0. sanity check gasWorkers
    if(gasWorkers > 3) gasWorkers = 3;
    else if(gasWorkers < 0) gasWorkers = 0;

    // 1. handle gas workers
    sc2::Units refineries = gInterface->observation->GetUnits(
        sc2::Unit::Alliance::Self,
        [](const sc2::Unit& unit){
            if(unit.build_progress < 1.0) return false;
            return unit.unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY ||
                unit.unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERYRICH;
        }
    );

    for(auto& r : refineries){
        if(r->ideal_harvesters <= 0 || r->vespene_contents <= 0) continue;

        if(r->assigned_harvesters < gasWorkers){
            // add one worker at a time
            Worker* w = getFreeWorker();
            if(w == nullptr || w->getUnit() == nullptr) continue;
            gInterface->actions->UnitCommand(w->getUnit(), sc2::ABILITY_ID::SMART, r);
            w->job = JOB_GATHERING_GAS;
        }
        else if (r->assigned_harvesters > gasWorkers){
            // too much gas workers
            Worker* w = getClosestWorker(r->pos, JOB_GATHERING_GAS);
            const sc2::Unit* mineral = FindNearestMineralPatch(w->getUnit()->pos);
            gInterface->actions->UnitCommand(w->getUnit(), sc2::ABILITY_ID::SMART, mineral);
            w->job = JOB_GATHERING_MINERALS;

        }
    }

    // 2. send to next base if base is overfull
    int numLongDistanceMiners = 0;
    sc2::Units ccs = gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsTownHall());
    for(auto& cc : ccs){
        if(cc->assigned_harvesters > cc->ideal_harvesters){
            // worker to move
            Worker* w = getClosestWorker(cc->pos);

            // grab the next base in general
            Expansion* e = gInterface->map->getClosestExpansion(cc->pos);
            int n = 0;
            Expansion* next = gInterface->map->getNthExpansion(n);
            while(!(e == next)){
                n++;
                next = gInterface->map->getNthExpansion(n);
            }
            // n is the current expansion, so send worker to expansion n + 1
            if(n < gInterface->map->numOfExpansions()){
                e = gInterface->map->getNthExpansion(n+1);
            }
            
            if(e != nullptr){
                numLongDistanceMiners++;
                const sc2::Unit* mineral = FindNearestMineralPatch(e->baseLocation);
                gInterface->actions->UnitCommand(w->getUnit(), sc2::ABILITY_ID::SMART, mineral);
                w->job = JOB_GATHERING_MINERALS;
            }
        } // end if cc->assigned_harvesters > cc->ideal_harvesters
    } // end for cc : ccs

    logger.addPlotData("game loop", (float) gInterface->observation->GetGameLoop());
    logger.addPlotData("long distance miners", (float) numLongDistanceMiners);
    logger.writePlotRow();

    // 3. handle leftover workers that are unemployed/still idle
    for(auto& w : workers){
        if(w.getUnit() != nullptr){
            if(w.getUnit()->orders.empty()) w.job = JOB_UNEMPLOYED;
            if(w.job == JOB_UNEMPLOYED) OnUnitIdle(w.getUnit());
        }

    }
}

const sc2::Unit* WorkerManager::FindNearestMineralPatch(const sc2::Point2D& start){
    sc2::Units units = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, sc2::IsMineralPatch());
    float distance = std::numeric_limits<float>::max();
    const sc2::Unit* target = nullptr;
    for(const auto& u : units){
        if(DistanceSquared2D(u->pos, start) < distance){
            distance = DistanceSquared2D(u->pos, start);
            target = u;
        }
    } // end for loop
    return target;
}

Worker* WorkerManager::getUnemployedWorker(){
    for(auto& w : workers)
        if(w.job == JOB_UNEMPLOYED){
            return &w;
        }
    
    // no unemployed workers found
    logger.errorInit().withStr("getUnemployedWorker is nullptr").write();
    return nullptr;
}

Worker* WorkerManager::getFreeWorker(){
    for(auto& w : workers)
        if(isFree(&w)){
            return &w;
        }
    return nullptr;
}

Worker* WorkerManager::getWorker(const sc2::Unit* unit_){
    sc2::Tag key = unit_->tag;
    for(auto& w : workers){
        if(w.tag == key){
            return &w; 
        }
            
    }
    logger.errorInit().withStr("getWorker is nullptr").write();
    return nullptr;
}

Worker* WorkerManager::getWorker(sc2::Tag tag_){
    for(auto& w : workers){
        if(tag_ == w.tag)
            return &w;
    }
    logger.errorInit().withStr("getWorker is nullptr").write();
    return nullptr;
}

Worker* WorkerManager::getNthWorker(size_t n){
    return &(workers[n]);
}

Worker* WorkerManager::getClosestWorker(sc2::Point2D pos, int jobType){
    Worker* closestWorker = nullptr;
    float distance = std::numeric_limits<float>::max();
    if(jobType == -1){ // default: get any worker
        for(auto& w : workers){
            if(w.getUnit() == nullptr) continue;
            if(distance > sc2::DistanceSquared2D(pos, w.getUnit()->pos)){
                distance = sc2::DistanceSquared2D(pos, w.getUnit()->pos);
                closestWorker = &w;
            }
        }
        return closestWorker;
    }
    else{ // FIXME: if jobType != -1 it crashes here it seems
        for(auto& w : workers){
            if(w.getUnit() == nullptr) continue;
            if(distance > sc2::DistanceSquared2D(pos, w.getUnit()->pos) && w.job == jobType){
                distance = sc2::DistanceSquared2D(pos, w.getUnit()->pos);
                closestWorker = &w;
            }
        }
        return closestWorker;
    }
}

int WorkerManager::getNumWorkers(){
    return workers.size();
}

bool WorkerManager::isFree(Worker* w){
    if(
        w->job == JOB_GATHERING_MINERALS ||
        w->job == JOB_UNEMPLOYED
    ) return true;
    else return false;
}

void WorkerManager::printDebug(){
    int countUnemployed = 0, countMinerals = 0, countGas = 0, countBuilding = 0, countBuildingGas = 0, countScouting = 0;
    // count how many of each worker type we have
    for(auto& w : workers){
        switch(w.job){
            case JOB_UNEMPLOYED:
                countUnemployed++;
                break;
            case JOB_GATHERING_MINERALS:
                countMinerals++;
                break;
            case JOB_GATHERING_GAS:
                countGas++;
                break;
            case JOB_BUILDING:
                countBuilding++;
                break;
            case JOB_BUILDING_GAS:
                countBuildingGas++;
                break;
            case JOB_SCOUTING:
                countScouting++;
                break;
        }
    }
    // print to debug
    gInterface->debug->debugTextOut("unemployed: " + std::to_string(countUnemployed));
    gInterface->debug->debugTextOut("mining minerals: " + std::to_string(countMinerals));
    gInterface->debug->debugTextOut("mining gas: " + std::to_string(countGas));
    gInterface->debug->debugTextOut("building : " + std::to_string(countBuilding));
    gInterface->debug->debugTextOut("building gas: " + std::to_string(countBuildingGas));
    gInterface->debug->debugTextOut("scouting: " + std::to_string(countScouting));

    gInterface->debug->sendDebug();
}