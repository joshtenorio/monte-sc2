#include "Mapper.h"

void Mapper::initialize(){
    calculateExpansions();
}

Expansion Mapper::getClosestExpansion(sc2::Point3D point){
    Expansion tmp;
    return tmp;
}

//////////////////////////////////////////////////////////
float tempDistance = 5.0; /////////////////////////////////
//////////////////////////////////////////////////////////
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
                if(sc2::Distance2D(u->pos, mineralPos) < distance){
                    distance = sc2::Distance2D(u->pos, mineralPos);
                    closestPatch = u;
                }
            } // end for loop

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
        expansions.push_back(e);
    } // end while !mineralPatches.empty()
    expansions.shrink_to_fit();
    std::cout << "number of expansions: " << expansions.size() << std::endl;
}

