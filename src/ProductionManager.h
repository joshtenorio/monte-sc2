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
    void build();

    protected:
    Strategy strategy;
    BuildingManager bm;

};