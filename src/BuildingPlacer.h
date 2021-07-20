#pragma once

#include <sc2api/sc2_common.h>
#include "api.h"
#include "Logger.h"

#define POINT2D_NULL sc2::Point2D(-1, -1)
#define POINT3D_NULL sc2::Point3D(-1, -1, -1)

// typedefing integer points to a more friendly name
namespace Monte {
typedef sc2::Point2DI Tile;
} // end namespace Monte

// purpose is solely to find a suitable location for building a structure
// TODO: should this inherit manager?
class BuildingPlacer {
    public:
    BuildingPlacer() { logger = Logger("BuildingPlacer"); };

    // initialize reserved tiles here (set everything to false unless if tile is unbuildable)
    void OnGameStart();

    void OnStep();
    
    // TODO: this runs after Mapper initializes, so we can reserve expansion tiles
    void initialize();

    // find a location for building near around with open space within freeRadius
    sc2::Point2D findLocation(sc2::ABILITY_ID building, sc2::Point3D around, float freeRadius = 4);

    // build building on a unit (such as gas geyser for refinery, or barracks for addon) nearest to near
    const sc2::Unit* findUnit(sc2::ABILITY_ID building, const sc2::Point3D* near);

    void reserveTiles(sc2::Point2D center, float radius);
    void freeTiles(sc2::Point2D center, float radius);

    // returns true if there is a conflicting tile
    bool checkConflict(sc2::Point2D center, float radius);

    protected:
    void validateTiles(); // TODO: run this periodically in OnStep
    sc2::Point2D findBarracksLocation();        // will place barracks at the main ramp if no barracks exist
    const sc2::Unit* findRefineryLocation(Expansion* e);
    sc2::Point2D findSupplyDepotLocation();     // will place supply depots at main ramp if <2 depots exist
    sc2::Point2D findCommandCenterLocation();

    // TODO: remove this, buildlingplacer doesn't deal with addons anymore
    const sc2::Unit* findUnitForAddon(sc2::ABILITY_ID building, const sc2::Point3D* near = nullptr);

    void printDebug();

    Logger logger;

    // this is a 2D vector of bools as opposed to a 1D vector of tiles so that it is easier to reserve/free tiles
    std::vector< std::vector<bool> > reservedTiles;

};