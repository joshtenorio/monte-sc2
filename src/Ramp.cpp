#include "api.h"
#include "Ramp.h"

void findRamp(Ramp* r, sc2::Point2D start){
    // get a list of points that are pathable but not buildable
    // (reference how we generate mineral lines for this)
    // then get the Ramp closest to start and generate barracks and supply depot positions from that
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
    gInterface->debug->sendDebug();

    // group together 


}