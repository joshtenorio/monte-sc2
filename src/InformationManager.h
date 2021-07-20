#pragma once

#include "api.h"
#include "Manager.h"

class InformationManager : public Manager {
    public:
    InformationManager() { logger = Logger("InformationManager"); };

    // TODO: if any of the instance variables are changed, send a chat message + logger, and perhaps create a tag
    void OnStep();
    // TODO: we should move updateExpoOwnership from scoutingmanager to here
    // TODO: problem: how do we get the current production/combat config so we can update them?
    ProductionConfig updateProduction();

    protected:
    // here, create instance variables that represent important information
    bool requireDetectors = false; // switch starport addon -> techlab, and increase starport count
    bool requireAntiAir = false; // get missile turrets, vikings. increase starport count
    bool requireAntiArmor = false; // switch to marauder production from barracks techlab and get marauder slow
    bool workerRushDetected = false;

    bool enemyHeavyEcon = false; // if this is true, set combatstate to always attacking - test against bluntmacro, miningmachine

};