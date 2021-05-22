#pragma once

#include <sc2api/sc2_unit.h>

#include "api.h"
#include "Mapper.h"
#include "Manager.h"
#include "BuildingManager.h"
#include "Strategy.h"


using namespace sc2;

class ProductionManager : public Manager {
    public:
    // constructors
    ProductionManager() { bm = BuildingManager(); };
    ProductionManager(Strategy strategy_){
        strategy = strategy_;
    };

    void OnStep();
    void OnGameStart();

    // probably useful for going to the next step if something should be blocking
    void OnBuildingConstructionComplete(const Unit* building_);
    void OnUnitCreated(const Unit* unit_); // pass to building manager
    void OnUnitDestroyed(const sc2::Unit* unit_); // pass to building manager
    void build();

    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();
    bool tryBuildRefinery();

    protected:
    Strategy strategy;
    BuildingManager bm;

};