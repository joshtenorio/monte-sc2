#pragma once

#include <vector>
#include <sc2api/sc2_unit.h>
#include "api.h"
#include "Manager.h"
#include "BuildingPlacer.h"

// first: building
// second: Worker object
typedef std::pair<sc2::Tag, Worker*> Construction;

class BuildingManager : public Manager {
    public:
    BuildingManager();

    void OnGameStart();
    void OnStep();

    // used for if a worker assigned to a job or building under construction is killed
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);

    // used for resetting worker job and for setting ownership of expansion, and for updating num of friendly refineries in an expansion
    void OnBuildingConstructionComplete(const sc2::Unit* building_);
    void OnUnitCreated(const sc2::Unit* building_);

    bool TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, int maxConcurrent = 1, sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);
    bool checkConstructions(sc2::UNIT_TYPEID building);
    


    protected:
    BuildingPlacer bp;
    std::vector<Construction> inProgressBuildings;
    std::vector<sc2::Tag> reservedWorkers;
};