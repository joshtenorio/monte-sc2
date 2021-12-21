#pragma once

#include <vector>
#include <sc2api/sc2_score.h>
#include "api.h"
#include "Manager.h"

class InformationManager : public Manager {
    public:
    InformationManager();

    // TODO: if any of the instance variables are changed, send a chat message + logger, and perhaps create a tag
    void OnGameStart();
    void OnStep();

    ProductionConfig updateProductionConfig(ProductionConfig& currentPConfig);
    CombatConfig updateCombatConfig(CombatConfig& currentCConfig);

    sc2::Point2D setLocationTarget();
    sc2::Point2D setLocationDefense();
    sc2::Point2D setHarassTarget(int type); // TODO: define an enum that determines type of harass; ie liberator, mine drop, etc

    protected:

    void updateExpoOwnership();
    void checkForWorkerRush();
    void checkForEnemyCloak();
    void checkForMassAir();
    void checkIncome(); // if income is dropping and we have a lot of long distance miners, then prioritise expansion

    sc2::Race enemyRace;

    // if we are able to detect an all in, we should build a pf instead of orbital at the natural
    // important!!! be completely sure it is an all in and not early harass, since delaying second orbital is pretty oof
    // ideally, we don't want to track opponent ids if they are an all in bot
    bool allInDetected = false; 

    bool requireDetectors = false; // switch starport addon -> techlab, and increase starport count and perhaps add missile turrets at bases
    bool requireAntiAir = false; // get missile turrets, vikings. increase starport count too prolly
    bool requireAntiArmor = false; // switch to marauder production from barracks techlab and get marauder slow

    // check for worker rush before we have 4 completed buildings
    bool workerRushDetected = false;

    bool enemyHeavyEcon = false; // if this is true, set combatstate to aggressive - test against bluntmacro, miningmachine
    bool requireExpansion = false;

    int mutaCount = 0;
    bool spireExists = false;

    std::vector<float> income;


};