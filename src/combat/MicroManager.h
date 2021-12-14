#pragma once

#include <vector>
#include "api.h"
#include "Manager.h"
#include "CombatTools.h"



class MicroManager : public Manager {
    public:
    MicroManager();
    void OnStep();
    void initialize();

    void marineOnStep();
    void medivacOnStep();
    void siegeTankOnStep();
    void reaperOnStep();
    void liberatorOnStep();
    // TODO: if nearby marine count is low
    // focus on putting down auto turrets, else use anti armor missles
    void ravenOnStep();

    void addUnit(sc2::Tag tag);
    bool removeUnit(sc2::Tag tag);

    protected:
    std::vector<Tank> tanks;
    std::vector<Reaper> reapers;
    std::vector<Liberator> liberators;

    std::vector<short> harassTable; // TODO: move this elsewhere, its only here for compile purposes

    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bioTypes; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
    Monte::InfluenceMap groundMap;
    Monte::InfluenceMap airMap;
};