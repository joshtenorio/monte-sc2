#pragma once

#include <iostream>
#include <memory>
#include <sc2api/sc2_agent.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_api.h>
#include <sc2api/sc2_unit_filters.h>
#include "api.h"
#include "WorkerManager.h"
#include "ProductionManager.h"
#include "CombatCommander.h"
#include "InformationManager.h"
#include "Mapper.h"
#include "Logger.h"
#include "Debug.h"

// include more strategies here
#include "strategies/MarinePush.h"


class Bot : public sc2::Agent {

public:
    Bot();

private:
    Mapper map;
    WorkerManager wm; //TODO: get a better name for workermanager because wm isn't very identifiable/readable
    ProductionManager pm;
    CombatCommander cc;
    InformationManager im;

    Monte::Debug debug;
    Logger logger;
    
    //TODO: maybe have a vector of managers?
    void OnGameStart() final;   
    void OnStep() final;
    void OnBuildingConstructionComplete(const sc2::Unit* building_) final;
    void OnUpgradeCompleted(sc2::UpgradeID upgrade_) final;
    void OnUnitCreated(const sc2::Unit* unit_) final;
    void OnUnitIdle(const sc2::Unit* unit_) final;
    void OnUnitDestroyed(const sc2::Unit* unit_) final;
    void OnUnitDamaged(const sc2::Unit* unit_, float health_, float shields_) final;
    void OnUnitEnterVision(const sc2::Unit* unit_) final;
    void OnError(const std::vector<sc2::ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors = {}) final;
    void OnGameEnd() final;


};
