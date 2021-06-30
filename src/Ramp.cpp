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
    else if(gInterface->observation->GetGameInfo().map_name == "Jagannatha AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(113, 146)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(55, 40))
        ){ // use top right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(113, 146));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(110, 143));
            r->barracksPos = sc2::Point2D(112.5, 143.5);
            r->barracksWithAddonPos = sc2::Point2D(112.5, 143.5);
        }
        else{ // use bot left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(55, 40));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(58, 43));
            r->barracksPos = sc2::Point2D(55.5, 42.5);
            r->barracksWithAddonPos = sc2::Point2D(53.5, 42.5);
        }
    } // end jagannatha
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
    else if(gInterface->observation->GetGameInfo().map_name == "Romanticide AIE"){
        if(
            sc2::DistanceSquared2D(start, sc2::Point2D(154, 53)) <
            sc2::DistanceSquared2D(start, sc2::Point2D(43, 116))
        ){ // use bot right locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(154, 53));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(157, 56));
            r->barracksPos = sc2::Point2D(156.5, 53.5);
            r->barracksWithAddonPos = sc2::Point2D(156.5, 53.5);
        }
        else{ // use top left locations
            r->supplyDepotPoints.emplace_back(sc2::Point2D(43, 116));
            r->supplyDepotPoints.emplace_back(sc2::Point2D(46, 119));
            r->barracksPos = sc2::Point2D(43.5, 118.5);
            r->barracksWithAddonPos = sc2::Point2D(41.5, 118.5);
        }
    } // end romanticide
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