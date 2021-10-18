#include "api.h"
#include "Ramp.h"

std::vector<Ramp> findRamps(){
    // get a list of points that are pathable but not buildable
    std::vector<sc2::Point2D> rampPoints;
    sc2::Point2D playableMin = gInterface->observation->GetGameInfo().playable_min;
    sc2::Point2D playableMax = gInterface->observation->GetGameInfo().playable_max;
    for(int x = playableMin.x; x < playableMax.x; x++){
        for(int y = playableMin.y; y < playableMax.y; y++){
            sc2::Point2D p = sc2::Point2D(x, y);
            if(gInterface->observation->IsPathable(p) && !gInterface->observation->IsPlacable(p)){
                rampPoints.emplace_back(p);
                sc2::Point3D p3d = sc2::Point3D(p.x, p.y, gInterface->observation->TerrainHeight(p));
                gInterface->debug->debugDrawTile(p3d, sc2::Colors::Teal);
            }
        }
    }
    

    // group together nearby ramp points to form a ramp
    std::vector<Ramp> ramps;
    while(!rampPoints.empty()){
        std::queue<sc2::Point2D> rampFrontier;
        rampFrontier.push(rampPoints.front());
        Ramp r;
        r.points.emplace_back(rampPoints.front());
        rampPoints.erase(rampPoints.begin());
        while(!rampPoints.empty() && !rampFrontier.empty()){
            sc2::Point2D r0 = rampFrontier.front();
            sc2::Point2D closestPoint = rampPoints.front();
            float distance = std::numeric_limits<float>::max();

            // get closest ramp point to rampfronter.front
            for(auto p : rampPoints){
                if(sc2::DistanceSquared2D(p, r0) < distance){
                    distance = sc2::DistanceSquared2D(p, r0);
                    closestPoint = p;
                }
            }

            if(distance >= 36){ // if point is too far away from ramp, disregard it
                rampFrontier.pop();
                continue;
            }

            rampFrontier.push(closestPoint);
            r.points.emplace_back(closestPoint);

            auto itr = std::find(rampPoints.begin(), rampPoints.end(), closestPoint);
            if(itr != rampPoints.end()) rampPoints.erase(itr);
        }
        ramps.emplace_back(r);
    } // while we still have ramp points to consider

    for(auto &r: ramps){
        sc2::Point3D p = sc2::Point3D(r.points.front().x, r.points.front().y, gInterface->observation->TerrainHeight(r.points.front()));
        gInterface->debug->debugSphereOut(p, 8.0);
    }
    gInterface->debug->sendDebug();
    return ramps;
}

MainRamp generateMainRamp(Ramp r){
    
}