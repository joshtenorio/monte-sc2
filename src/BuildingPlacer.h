#pragma once

#include <sc2api/sc2_common.h>
#include "api.h"
#include "Mapper.h"

// purpose is solely to find a suitable location for building something
class BuildingPlacer {
    public:
    BuildingPlacer() {};
    void setMapper(Mapper* map_);
    sc2::Point2D findLocation(sc2::UNIT_TYPEID building);
    Mapper* getMapper();

    protected:
    Mapper* map;
};