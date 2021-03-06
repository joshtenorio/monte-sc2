#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_score.h>
#include "api.h"
#include "Manager.h"
#include "BuildingManager.h"
#include "Strategy.h"

class ProductionManager : public Manager {
    public:
    // constructors
    ProductionManager();
    ProductionManager(Strategy* strategy_);

    void OnStep();
    void OnGameStart();
    void OnBuildingConstructionComplete(const sc2::Unit* building_);
    void OnUnitCreated(const sc2::Unit* unit_);
    void OnUpgradeCompleted(sc2::UpgradeID upgrade_);

    // pass these to building manager
    void OnUnitDestroyed(const sc2::Unit* unit_);
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_);
    
    ProductionConfig& getProductionConfig();

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
    bool tryBuildFactory();
    bool tryBuildStarport();
    bool tryBuildRefinery();
    bool tryBuildCommandCenter();
    bool tryBuildArmory();
    bool tryBuildEngineeringBay();
    bool tryBuildBunker();
    bool tryBuildAddon();
    bool tryBuildMissileTurret();

    bool tryTrainUnit(sc2::ABILITY_ID unitToTrain, int n);

    // handle upgrades
    void handleUpgrades();
    void upgradeInfantryWeapons(int currLevel);
    void upgradeInfantryArmor(int currLevel);
    void upgradeFactoryWeapons(int currLevel);
    void upgradeVehicleArmor(int currLevel);
    void upgradeStarshipWeapons(int currLevel);

    // use orbital cc
    void callMules();

    // returns true if the building has been given an order in the current loop
    bool isBuildingBusy(sc2::Tag bTag);

    private:
    Strategy* strategy;
    BuildingManager bm;
    std::vector<sc2::Tag> busyBuildings; // list of buildings that have an order
    ProductionConfig config;

};