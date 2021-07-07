#pragma once

#include <sc2api/sc2_unit.h>
#include <vector>
#include <memory>
#include "api.h"
#include "Manager.h"
#include "BuildingManager.h"
#include "Strategy.h"

using namespace sc2;

#define ARMYBUILDING_UNUSED sc2::ABILITY_ID::TRAIN_ARCHON

typedef struct ArmyBuilding_s_t {
    const sc2::Unit* building;
    sc2::ABILITY_ID order; // used for continuous production of units
    const sc2::Unit* addon;

    // tags
    sc2::Tag buildingTag;
    sc2::Tag addonTag;
} ArmyBuilding;

class ProductionManager : public Manager {
    public:
    // constructors
    ProductionManager() { bm = BuildingManager(); logger = Logger("ProductionManager"); };
    ProductionManager(Strategy* strategy_){
        strategy = strategy_;
        bm = BuildingManager();
        logger = Logger("ProductionManager");
    };

    void OnStep();
    void OnGameStart();
    void OnBuildingConstructionComplete(const Unit* building_);
    void OnUnitCreated(const Unit* unit_);
    void OnUpgradeCompleted(sc2::UpgradeID upgrade_);
    void OnUnitDestroyed(const sc2::Unit* unit_); // pass to building manager

    // fill queue with stuff to do
    void fillQueue();

    // swap addons
    void swapAddon(ArmyBuilding* b1, ArmyBuilding* b2);

    // identify what building to morph/train unit/start upgrade, or if need to get scv to build a structure
    void parseQueue();
    void buildStructure(Step s);
    void trainUnit(Step s);
    void researchUpgrade(Step s);
    void morphStructure(Step s); // TODO: could we use researchUpgrade for morphs?

    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();
    bool tryBuildRefinery();
    bool tryBuildCommandCenter();
    bool tryBuildArmory();
    bool tryBuildEngineeringBay();

    // ArmyBuilding related stuff
    void handleArmyBuildings();
    void setArmyBuildingOrder(ArmyBuilding* a, sc2::ABILITY_ID order);
    bool tryTrainUnit(sc2::ABILITY_ID unitToTrain);

    // handle upgrades
    void handleUpgrades();

    // use orbital cc
    void callMules();

    // returns true if the building has been given an order in the current loop
    bool isBuildingBusy(sc2::Tag bTag);

    protected:
    Strategy* strategy;
    BuildingManager bm;
    std::vector<Step> productionQueue; // list of structures/upgrades/units currently being built
    std::vector<ArmyBuilding> armyBuildings; // list of structures that produce army units
    std::vector<sc2::Tag> busyBuildings; // list of buildings that have an order
};