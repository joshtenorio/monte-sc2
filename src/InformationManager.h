#pragma once

#include "api.h"
#include "Manager.h"

class InformationManager : public Manager {
    public:
    InformationManager() { logger = Logger("InformationManager"); };

    void OnStep();
    // TODO: should we move updateExpoOwnership from scoutingmanager to here?
    ProductionConfig updateProduction();
    protected:
    // here, create instance variables that represent important information
    bool requireDetectors = false;

};