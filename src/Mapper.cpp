#include "Mapper.h"

#include "api.h" // see WorkerManager.cpp for why this is here

// initialize expansions
std::vector<Expansion> Mapper::expansions;


void Mapper::initialize(){
    calculateExpansions();
    sortExpansions(gInterface->observation->GetStartLocation());

    // get starting location's expansion and define its ramp
    startingExpansion = *getClosestExpansion(gInterface->observation->GetStartLocation());

    ramps = findRamps();
    // find the ramp closest to starting location and call that the main ramp
    Ramp closest;
    for(auto& r : ramps){

    }
    //validateGeysers();

}

Expansion* Mapper::getClosestExpansion(sc2::Point3D point){
    Expansion* tmp = &(expansions.front());
    for (auto& e : expansions){
        if(sc2::DistanceSquared2D(tmp->baseLocation, sc2::Point2D(point)) > sc2::DistanceSquared2D(e.baseLocation, sc2::Point2D(point)))
            tmp = &e;
    }
    return tmp;
}

Expansion Mapper::getStartingExpansion(){
    return startingExpansion;
}

Expansion* Mapper::getNthExpansion(int n){
    return &(expansions[n]);
}

Expansion* Mapper::getCurrentExpansion(){
    int n = 0;
    while(n < expansions.size() - 1){
        // if the next expansion is not owned by myself, return the current expansion
        if(getNthExpansion(n + 1)->ownership != OWNER_SELF) return getNthExpansion(n);
        n++;
    }
    return nullptr;
}

Expansion* Mapper::getNextExpansion(){
    for(auto& e : expansions)
        if(e.ownership == OWNER_NEUTRAL) return &e;
    return nullptr;
}

int Mapper::numOfExpansions(){
    return expansions.size();
}

void Mapper::calculateExpansions(){
    // first step: get all minerals
    // probably inefficient so need to improve this in the future
    sc2::Units mineralPatches = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsMineralPatch());

    // second step: get geysers
    sc2::Units gasGeysers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsGeyser());

    // third: calculate mineral lines and use to create expansions
    expansions.reserve(16);
    while(!mineralPatches.empty()){
        std::queue<const sc2::Unit*> mineralFrontier;
        mineralFrontier.push(mineralPatches.front());
        Expansion e;
        e.mineralLine.reserve(8);
        e.mineralLine.push_back(mineralFrontier.front());
        mineralPatches.erase(mineralPatches.begin());

        while(!mineralPatches.empty() && !mineralFrontier.empty()){
            auto mineral = mineralFrontier.front();
            auto closestPatch = mineralPatches.front();
            float distance = std::numeric_limits<float>::max();

            // get closest patch to mineral frontier.front
            for(const auto& u: mineralPatches){
                if(sc2::DistanceSquared2D(u->pos, mineral->pos) < distance){
                    distance = sc2::DistanceSquared2D(u->pos, mineral->pos);
                    closestPatch = u;
                }
            } // end for loop

            // patch is too far from mineral line so disregard it
            if(distance >= PATCH_NEIGHBOR_DISTANCE){ 
                mineralFrontier.pop();
                continue;
            }

            mineralFrontier.push(closestPatch);
            e.mineralLine.push_back(closestPatch);

            // remove closest patch from mineralPatches
            auto itr = std::find(mineralPatches.begin(), mineralPatches.end(), closestPatch);
            if(itr != mineralPatches.end()) mineralPatches.erase(itr);
            
        }
        // calculate midpoint of expansion's mineral line
        // TODO: this can definitely be improved, maybe put it in the previous loop?
        float x = 0.0, y = 0.0, z = 0.0;
        int n = 0;
        for(auto& m : e.mineralLine){
            x += m->pos.x; y += m->pos.y; z += m->pos.z; n++;
        }
        e.mineralMidpoint.x = x / (float) n;
        e.mineralMidpoint.y = y / (float) n;
        e.mineralMidpoint.z = z / (float) n;

        expansions.push_back(e);
    } // end while !mineralPatches.empty()

    // calculate base locations for each expansion
    if(expansions.empty()){
        logger.errorInit().withStr("No expansions found").write();
        return;
    }

    // manually assign starting location to an expansion
    const sc2::Unit* cc = (gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER))).front();
    if(cc != nullptr){
        for(auto& e : expansions){
            // find the mineral line close to starting location
            if(sc2::Distance2D(sc2::Point2D(e.mineralMidpoint), sc2::Point2D(cc->pos)) < DISTANCE_ERR_MARGIN){
                e.baseLocation = sc2::Point2D(cc->pos);
                e.ownership = OWNER_SELF;
                e.isStartingLocation = true;
                break;
            }
        } // end for e : expansions
    }

    // assign the rest of the expansions a base location
    for(auto& e : expansions){
        if(e.isStartingLocation) continue;
        
        // expansions are at (x+0.5, y+0.5)
        auto center = sc2::Point2D(e.mineralMidpoint);
        center.x = static_cast<int>(center.x) + 0.5f;
        center.y = static_cast<int>(center.y) + 0.5f;

        // Find all possible cc locations for the expansion
        std::vector<sc2::QueryInterface::PlacementQuery> queries;

        queries.reserve((SEARCH_MAX_OFFSET - SEARCH_MIN_OFFSET + 1) * (SEARCH_MAX_OFFSET - SEARCH_MIN_OFFSET + 1));
        for (int xOffset = SEARCH_MIN_OFFSET; xOffset <= SEARCH_MAX_OFFSET; ++xOffset) {
            for (int yOffset = SEARCH_MIN_OFFSET; yOffset <= SEARCH_MAX_OFFSET; ++yOffset) {
                sc2::Point2D pos(center.x + xOffset, center.y + yOffset);
                queries.emplace_back(sc2::ABILITY_ID::BUILD_COMMANDCENTER, pos);
            }
        }
        auto results = gInterface->query->Placement(queries);
        int trueResults = 0;
        for(auto r : results){
            if(r) trueResults++;
        }

        // narrow down results
        for (int xOffset = SEARCH_MIN_OFFSET; xOffset <= SEARCH_MAX_OFFSET; ++xOffset) {
            for (int yOffset = SEARCH_MIN_OFFSET; yOffset <= SEARCH_MAX_OFFSET; ++yOffset) {
                sc2::Point2D pos(center.x + xOffset, center.y + yOffset);

                // is pos buildable?
                int index = (xOffset + 0 - SEARCH_MIN_OFFSET) * (SEARCH_MAX_OFFSET - SEARCH_MIN_OFFSET + 1) + (yOffset + 0 - SEARCH_MIN_OFFSET);
                assert(0 <= index && index < static_cast<int>(results.size()));
                if (!results[static_cast<std::size_t>(index)]) continue;
        
                if(e.initialized){
                    if(sc2::DistanceSquared2D(center, pos) < sc2::DistanceSquared2D(center, e.baseLocation))
                        e.baseLocation = pos;
                }
                else{
                    e.baseLocation = pos;
                    e.initialized = true;
                }
            }
        }
    } // end for e : expansions
    logger.infoInit().withStr("Total number of expansions:").withInt(expansions.size()).write();
    // assign gas geysers to an expansion
    for(auto& g : gasGeysers){
        int c = 0;
        for(int i = 0; i < expansions.size(); i++){
            if(sc2::DistanceSquared2D(g->pos, expansions[i].baseLocation) < sc2::DistanceSquared2D(g->pos, expansions[c].baseLocation)){
                c = i;
            }
        }
        expansions[c].gasGeysers.emplace_back(g);
    }
} // end void Mapper::calculateExpansions()

void Mapper::sortExpansions(sc2::Point2D point){

    // get distances
    for (auto& e : expansions){
        e.distanceToStart = sc2::DistanceSquared2D(point, e.baseLocation);
    }

    // use std::sort
    std::sort(expansions.begin(), expansions.end());
    

}

void Mapper::validateGeysers(){
    std::vector<sc2::QueryInterface::PlacementQuery> queries;
    for(auto& e : expansions){
        for(auto& g : e.gasGeysers)
        queries.emplace_back(sc2::ABILITY_ID::BUILD_REFINERY, g->pos);
    }
    auto results = gInterface->query->Placement(queries);
    int trueResults = 0;
    for(auto r : results){
        if(r) trueResults++;
    }
    //printf("%d of %d valid\n", trueResults, results.size());
}