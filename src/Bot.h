#pragma once

#include <iostream>
#include <sc2api/sc2_agent.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_api.h>
#include <sc2api/sc2_unit_filters.h>
#include "api.h"
#include "WorkerManager.h"

using namespace sc2;

class Bot : public Agent{

public:
    Bot();

private:
    WorkerManager wm; //TODO: get a better name for workermanager because wm isn't very identifiable/readable
    //TODO: maybe have a vector of managers?
    void OnGameStart() final;   
    void OnStep() final;
    void OnBuildingConstructionComplete(const Unit* building_) final;
    void OnUnitCreated(const Unit* unit_) final;
    void OnUnitIdle(const Unit* unit_) final;
    void OnUnitDestroyed(const Unit* unit_) final;
    void OnUpgradeCompleted(UpgradeID id_) final;
    void OnError(const std::vector<ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors = {}) final;

    size_t CountUnitType(UNIT_TYPEID unitType); // maybe move this to api.cpp?
    // TODO: these are temp functions, at some point move them to a manager class
    bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();
    bool TryBuildBarracks();


};
