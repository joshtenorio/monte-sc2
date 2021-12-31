#include "api.h"
#include "Ramp.h"

void findRamp(Ramp* r, sc2::Point2D start){
    if(gInterface->observation->GetGameInfo().map_name == "2000 Atmospheres AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(153, 144)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(71, 60))
        ){ // use top right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(153, 144));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(150, 141));
            r->barracksPos = sc2::Point2D(152.5, 141.5);
            r->barracksWithAddonPos = sc2::Point2D(152.5, 141.5);
        }
        else{ // use bottom left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(71, 60));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(74, 63));
            r->barracksPos = sc2::Point2D(71.5, 62.5);
            r->barracksWithAddonPos = sc2::Point2D(69.5, 62.5);
        }
    } // end 2k atmospheres
    else if(gInterface->observation->GetGameInfo().map_name == "Blackburn AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(135, 46)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(49, 46))
        ){ // use bottom right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(135, 46));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(138, 43));
            r->barracksPos = sc2::Point2D(135.5, 43.5);
            r->barracksWithAddonPos = sc2::Point2D(133.5, 43.5);
        }
        else{ // use bot left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(49, 46));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(46, 43));
            r->barracksPos = sc2::Point2D(48.5, 43.5);
            r->barracksWithAddonPos = sc2::Point2D(48.5, 43.5);
        }
    } // end blackburn
    else if(gInterface->observation->GetGameInfo().map_name == "Glittering Ashes AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(155, 152)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(61, 52))
        ){ // use top right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(155, 152));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(152, 149));
            r->barracksPos = sc2::Point2D(154.5, 149.5);
            r->barracksWithAddonPos = sc2::Point2D(154.5, 149.5);
        }
        else{ // use bot left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(61, 52));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(64, 55));
            r->barracksPos = sc2::Point2D(61.5, 54.5);
            r->barracksWithAddonPos = sc2::Point2D(59.5, 54.5);
        }
    } // end glittering ashes
    else if(gInterface->observation->GetGameInfo().map_name == "Lightshade AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(140, 47)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(44, 117))
        ){ // use bot right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(140, 47));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(137, 50));
            r->barracksPos = sc2::Point2D(137.5, 47.5);
            r->barracksWithAddonPos = sc2::Point2D(135.5, 47.5);
        }
        else{ // use top left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(44, 117));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(47, 114));
            r->barracksPos = sc2::Point2D(46.5, 116.5);
            r->barracksWithAddonPos = sc2::Point2D(46.5, 116.5);
        }
    } // end lightshade
    else if(gInterface->observation->GetGameInfo().map_name == "Curious Minds AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(122, 39)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(30, 101))
        ){ // use bot right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(122, 39));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(119, 42));
            r->barracksPos = sc2::Point2D(119.5, 39.5);
            r->barracksWithAddonPos = sc2::Point2D(117.5, 39.5);
        }
        else{ // use top left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(30, 101));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(33, 98));
            r->barracksPos = sc2::Point2D(32.5, 100.5);
            r->barracksWithAddonPos = sc2::Point2D(32.5, 100.5);
        }
    } // end curious minds
    else if(gInterface->observation->GetGameInfo().map_name == "Oxide AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(130, 141)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(59, 60))
        ){ // use top right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(130, 141));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(133, 144));
            r->barracksPos = sc2::Point2D(132.5, 141.5);
            r->barracksWithAddonPos = sc2::Point2D(132.5, 141.5);
        }
        else{ // use bot left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(59, 60));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(62, 63));
            r->barracksPos = sc2::Point2D(59.5, 62.5);
            r->barracksWithAddonPos = sc2::Point2D(57.5, 62.5);
        }
    } // end oxide
}