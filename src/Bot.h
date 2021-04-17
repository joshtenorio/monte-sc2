#pragma once

#include <sc2api/sc2_agent.h>

#include "WorkerManager.h"
// TODO: add a using namespace sc2 here too
// The main bot class.
class Bot : public sc2::Agent
{
    public:

    Bot();

 private:
    void OnGameStart() final;   
    void OnStep() final;
    void OnBuildingConstructionComplete(const sc2::Unit* building_) final;
    void OnUnitCreated(const sc2::Unit* unit_) final;
    void OnUnitIdle(const sc2::Unit* unit_) final;
    void OnUnitDestroyed(const sc2::Unit* unit_) final;
    void OnUpgradeCompleted(sc2::UpgradeID id_) final;
    void OnError(const std::vector<sc2::ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors = {}) final;

    bool TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure, sc2::UNIT_TYPEID unit_type = sc2::UNIT_TYPEID::TERRAN_SCV);
    bool TryBuildSupplyDepot();


};
