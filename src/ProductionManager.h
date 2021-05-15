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
    void setMapper(Mapper* map_); // TODO: remove this when i figure out constructor, this is lazy solution

    protected:
    Strategy strategy;
    BuildingManager bm;

};