#pragma once

#include <sc2api/sc2_unit.h>
#include <vector>
#include <memory>
#include "api.h"
#include "Manager.h"
#include "BuildingManager.h"
#include "Strategy.h"

using namespace sc2;

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

    protected:
    void handleBuildOrder();
    void handleBuildOrderDeadlock();

    void handleBarracks();
    void handleFactories();
    void handleStarports();
    void handleTownHalls();

    // identify what building to cast ability/train unit/start upgrade, or if need to get scv to build a structure
    void parseStep(Step s);
    void buildStructure(Step s);
    void buildAddon(Step s);
    void trainUnit(Step s);
    void castBuildingAbility(Step s);

    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();
    bool tryBuildRefinery();
    bool tryBuildCommandCenter();
    bool tryBuildArmory();
    bool tryBuildEngineeringBay();
    bool tryBuildBunker();
    bool tryBuildAddon();

    bool tryTrainUnit(sc2::ABILITY_ID unitToTrain, int n);

    // handle upgrades
    void handleUpgrades();

    // use orbital cc
    void callMules();

    // returns true if the building has been given an order in the current loop
    bool isBuildingBusy(sc2::Tag bTag);

    private:
    Strategy* strategy;
    BuildingManager bm;
    std::vector<sc2::Tag> busyBuildings; // list of buildings that have an order
    ProductionConfig config; // TODO: when we make an InformationManager class this class should be friends with that class

    void upgradeInfantryWeapons(int currLevel);
    void upgradeInfantryArmor(int currLevel);
};