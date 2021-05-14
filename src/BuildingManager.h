#pragma once

#include <sc2api/sc2_unit.h>
#include "api.h"
#include "Manager.h"

class BuildingManager : public Manager {
    public:
    BuildingManager() {};
    void OnStep();
    // used for if a worker assigned to a job or building under construction is killed
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void build(sc2::ABILITY_ID);
    protected:
    private:
    // put functions here to help determine what building to build from (for upgrades) or if an scv needs to be called
    // idea: assume that if it is not of a barracks/factory/starport/enggbay/armory/academy/etc then an scv is required
};