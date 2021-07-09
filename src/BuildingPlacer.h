#pragma once

#include <sc2api/sc2_common.h>
#include "api.h"
#include "Logger.h"

#define POINT_NULL sc2::Point2D(-1, -1)

// purpose is solely to find a suitable location for building something
class BuildingPlacer {
    public:
    BuildingPlacer() { logger = Logger("BuildingPlacer"); };

    // find a location for building near around with open space within freeRadius
    sc2::Point2D findLocation(sc2::ABILITY_ID building, sc2::Point3D around, float freeRadius = 4);

    // build building on a unit (such as gas geyser for refinery, or barracks for addon) nearest to near
    const sc2::Unit* findUnit(sc2::ABILITY_ID building, const sc2::Point3D* near);

    protected:
    sc2::Point2D findBarracksLocation();        // will place barracks at the main ramp if no barracks exist
    const sc2::Unit* findRefineryLocation(Expansion* e);
    sc2::Point2D findSupplyDepotLocation();     // will place supply depots at main ramp if <2 depots exist
    sc2::Point2D findCommandCenterLocation();
    const sc2::Unit* findUnitForAddon(sc2::ABILITY_ID building, const sc2::Point3D* near = nullptr);
    Logger logger;

};