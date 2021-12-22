#pragma once
#include <sc2api/sc2_common.h>
#include "combat/CombatTools.h"
#include "combat/GameObject.h"
#include "combat/MicroManager.h"
#include "combat/InfluenceMap.h"
#include "Logger.h"


class Squad {
    public:
    Squad(std::string id_, size_t priority_);
    Squad();

    bool onStep(Monte::InfluenceMap& gmap, Monte::InfluenceMap& amap);
    void initialize();

    void setOrder(SquadOrder order_);
    void setOrder(SquadOrderType, sc2::Point2D target, float radius);
    void setPriority(size_t prio);
    
    void addUnit(sc2::Tag tag);
    bool removeUnit(sc2::Tag tag);
    
    std::string getId();
    SquadOrder getOrder();
    SquadStatus getStatus();
    size_t getSize();
    size_t getPriority();
    std::vector<GameObject> getUnits();
    bool containsUnit(sc2::Tag tag);
    bool isEmpty();

    protected:
    // removes invalid units and returns number of units removed
    int validateUnits();
    sc2::Point2D getCenter();
    bool shouldRegroup();
    sc2::Point2D getRegroupPosition();

    Logger logger;
    MicroManager mm;
    size_t priority;
    SquadOrder order;
    SquadStatus status;
    std::string id;
    std::vector<GameObject> units;
    float supplyRemoved; // how much supply we've lost since regrouping
    float supplyInitial; // how much supply we started with since attacking/defending
};