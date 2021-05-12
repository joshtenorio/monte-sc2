#include "Mapper.h"

void Mapper::initialize(){
    calculateExpansions();
    sortExpansions(gInterface->observation->GetStartLocation());

    // get starting location's expansion
    startingExpansion = getClosestExpansion(gInterface->observation->GetStartLocation());
}

Expansion Mapper::getClosestExpansion(sc2::Point3D point){
    Expansion tmp = expansions.front();
    for (auto e : expansions){
        if(sc2::DistanceSquared2D(tmp.baseLocation, sc2::Point2D(point)) > sc2::DistanceSquared2D(e.baseLocation, sc2::Point2D(point)))
            tmp = e;
    }
    return tmp;
}

Expansion Mapper::getStartingExpansion(){
    return startingExpansion;
}

void Mapper::calculateExpansions(){
    // first step: get all minerals
    // probably inefficient so need to improve this in the future
    Units mineralPatches = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsMineralPatch());

    // second step: get geysers
    Units gasGeysers = gInterface->observation->GetUnits(sc2::Unit::Alliance::Neutral, IsGeyser());

    // third: calculate mineral lines and use to create expansions
    expansions.reserve(16);
    while(!mineralPatches.empty()){
        std::queue<sc2::Point3D> mineralFrontier;
        mineralFrontier.push(mineralPatches.front()->pos);
        Expansion e;
        e.mineralLine.reserve(8);
        e.mineralLine.push_back(mineralFrontier.front());
        mineralPatches.erase(mineralPatches.begin());

        while(!mineralPatches.empty() && !mineralFrontier.empty()){
            auto mineralPos = mineralFrontier.front();
            auto closestPatch = mineralPatches.front();
            float distance = std::numeric_limits<float>::max();

            // get closest patch to mineral frontier.front
            for(const auto& u: mineralPatches){
                if(sc2::DistanceSquared2D(u->pos, mineralPos) < distance){
                    distance = sc2::DistanceSquared2D(u->pos, mineralPos);
                    closestPatch = u;
                }
            } // end for loop

            // patch is too far from mineral line so disregard it
            if(distance >= PATCH_NEIGHBOR_DISTANCE){ 
                mineralFrontier.pop();
                continue;
            }

            mineralFrontier.push(closestPatch->pos);
            e.mineralLine.push_back(closestPatch->pos);

            // remove closest patch from mineralPatches
            auto itr = std::find(mineralPatches.begin(), mineralPatches.end(), closestPatch);
            if(itr != mineralPatches.end()) mineralPatches.erase(itr);
            
        }
        // calculate midpoint of expansion's mineral line
        // TODO: this can definitely be improved, maybe put it in the previous loop?
        float x = 0.0, y = 0.0, z = 0.0;
        int n = 0;
        for(auto& m : e.mineralLine){
            x += m.x; y += m.y; z += m.z; n++;
        }
        e.mineralMidpoint.x = x / (float) n;
        e.mineralMidpoint.y = y / (float) n;
        e.mineralMidpoint.z = z / (float) n;

        gInterface->debug->DebugSphereOut(e.mineralMidpoint, 20.5);
        expansions.push_back(e);
    } // end while !mineralPatches.empty()

    // calculate base locations for each expansion
    if(expansions.empty()){
        std::cout << "no expansions found" << std::endl;
        return;
    }

    // manually assign starting location to an expansion
    const sc2::Unit* cc = (gInterface->observation->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER))).front();
    if(cc != nullptr){
        for(auto e : expansions){
            // find the mineral line close to starting location
            if(sc2::Distance2D(sc2::Point2D(e.mineralMidpoint), sc2::Point2D(cc->pos)) < DISTANCE_ERR_MARGIN){
                e.baseLocation = sc2::Point2D(cc->pos);
                e.isStartingLocation = true;
                break;
            }
        } // end for e : expansions
    }

    // assign the rest of the expansions a base location
    for(auto e : expansions){
        if(e.isStartingLocation) continue;
        
        // expansions are at (x+0.5, y+0.5) // TODO: or perhaps not
        auto center = sc2::Point2D(e.mineralMidpoint);
        center.x = static_cast<int>(center.x) + 0.5f;
        center.y = static_cast<int>(center.y) + 0.5f;
        //e.baseLocation = center; ///////////////////////////////////

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
    std::cout << "expansion at (" << e.baseLocation.x << ", " << e.baseLocation.y << ")\n";
    } // end for e : expansions
    std::cout << "total number of expansions: " << expansions.size() << "\n";
    for(auto e: expansions) std::cout << "base at (" << e.baseLocation.x << ", " << e.baseLocation.y << ")\n";
    // assign gas geysers to an expansion
    for(auto g : gasGeysers){
        std::cout << "gas location: (" << g->pos.x << "," << g->pos.y << ")" << std::endl;
        int c = 0;
        for(int i = 0; i < expansions.size(); i++){
            std::cout << "i: " << i << "\t" << "c: " << c << "\t";
            std::cout << "i location: (" << expansions[i].baseLocation.x << ", " << expansions[i].baseLocation.y << ")\t";
            std::cout << "c location: (" << expansions[c].baseLocation.x << ", " << expansions[c].baseLocation.y << ")\t";
            std::cout << "i distance: " << sc2::DistanceSquared2D(g->pos, expansions[i].baseLocation) << "\t";
            std::cout << "c distance: " << sc2::DistanceSquared2D(g->pos, expansions[c].baseLocation) << std::endl;
            if(sc2::DistanceSquared2D(g->pos, expansions[i].baseLocation) < sc2::DistanceSquared2D(g->pos, expansions[c].baseLocation)){
                c = i;
            }
        }
        expansions[c].gasGeysers.emplace_back(g->pos);
        std::cout << "an expansion has " << expansions[c].gasGeysers.size() << " geysers\n";
    }

    for(auto e : expansions) std::cout << "boop has " << e.gasGeysers.size() << " geysers\n";


} // end void Mapper::calculateExpansions()

void Mapper::sortExpansions(sc2::Point2D point){

}
