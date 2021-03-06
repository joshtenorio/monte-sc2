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

    void initialize();
    SquadStatus onStep(Monte::InfluenceMap& gmap, Monte::InfluenceMap& amap);

    // checks if we are done with current order
    bool isDone();

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
    sc2::Point2D getCenter();
    sc2::Point2D getCenter(sc2::UNIT_TYPEID type);

    protected:
    // removes invalid units and returns number of units removed
    int validateUnits();
    
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