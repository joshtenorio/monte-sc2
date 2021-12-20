#pragma once

#include <vector>
#include "api.h"
#include "Manager.h"
#include "CombatTools.h"



class MicroManager : public Manager {
    public:
    MicroManager();
    void OnStep(); // this necessary?
    void execute(SquadOrder& order, Monte::InfluenceMap& gmap, Monte::InfluenceMap& amap);
    void initialize();

    void addUnit(sc2::Tag tag);
    bool removeUnit(sc2::Tag tag);

    protected:
    void executeMicro(std::vector<GameObject> targets); // TODO: should be virtual when we do subclasses
    void marineOnStep();
    void medivacOnStep();
    void siegeTankOnStep();
    void reaperOnStep();
    void liberatorOnStep();
    // TODO: if nearby marine count is low
    // focus on putting down auto turrets, else use anti armor missles
    void ravenOnStep();


    SquadOrder order;
    // TODO: have one std::vector of GameObject*
    // well, ig in the future we will have different micro managers
    // so it doesnt matter too much rn
    std::vector<Tank> tanks;
    std::vector<Reaper> reapers;
    std::vector<Liberator> liberators;
    std::vector<GameObject> bio;

    std::vector<short> harassTable; // TODO: move this elsewhere, its only here for compile purposes

    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bioTypes; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
    Monte::InfluenceMap groundMap;
    Monte::InfluenceMap airMap;
};