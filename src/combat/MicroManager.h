#pragma once

#include <vector>
#include "api.h"
#include "combat/CombatTools.h"
#include "Logger.h"


class MicroManager {
    public:
    MicroManager();
    void execute(SquadOrder& order, Monte::InfluenceMap& gmap, Monte::InfluenceMap& amap);
    void initialize();
    void regroup(sc2::Point2D target);

    void addUnit(sc2::Tag tag);
    bool removeUnit(sc2::Tag tag);

    protected:
    void executeMicro(std::vector<GameObject> targets); // TODO: should be virtual when we do subclasses
    void marineOnStep(SquadOrder& order);
    void medivacOnStep(SquadOrder& order);
    void siegeTankOnStep(SquadOrder& order);
    void reaperOnStep(Monte::InfluenceMap& groundMap);
    void liberatorOnStep();
    // TODO: if nearby marine count is low
    // focus on putting down auto turrets, else use anti armor missles
    void ravenOnStep();


    // TODO: have one std::vector of GameObject*
    // well, ig in the future we will have different micro managers
    // so it doesnt matter too much rn
    // technically it will matter though, because then we won't have to redefien addUnit and removeUnit
    std::vector<Tank> tanks;
    std::vector<Reaper> reapers;
    std::vector<Liberator> liberators;
    std::vector<GameObject> bio;

    std::vector<short> harassTable; // TODO: move this elsewhere, its only here for compile purposes

    bool reachedEnemyMain;
    std::vector<sc2::UNIT_TYPEID> bioTypes; // filter for GetUnits
    std::vector<sc2::UNIT_TYPEID> tankTypes; // filter for GetUnits
    Logger logger;
};