#pragma once

#include <sc2api/sc2_unit.h>

#include "api.h"
#include "Mapper.h"
#include "Manager.h"
#include "Strategy.h"


using namespace sc2;

class ProductionManager : public Manager {
    public:
    // constructors
    ProductionManager() {};
        ProductionManager(Mapper* map_){ // this one is probably temporary until i make an actual strategy
        map = map_;
    };
    ProductionManager(Mapper* map_, Strategy strategy_){
        map = map_;
        strategy = strategy_;
    };

    void OnStep();
    void OnGameStart();
    void setMapper(Mapper* map_); // TODO: remove this when i figure out constructor, this is lazy solution
    // owo, will probably go into BuildingManager when that is made
    bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();
    bool tryBuildRefinery();

    protected:
    Mapper* map;
    Strategy strategy;

};