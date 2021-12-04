#pragma once
#include <sc2api/sc2_common.h>
#include "GameObject.h"

/*
minimum functionality:
- add/remove units
- set target and order

required classes/structs/enums:
- Squad class
- SquadOrder struct
*/
enum SquadOrderType {
    Null,
    Attack,
    Defend,
    Harass,
    Regroup,
    Idle
};

enum SquadStatus {
    Idle,
    Busy
};

typedef struct SquadOrder_s_t {
    SquadOrder_s_t() { type = SquadOrderType::Null; };
    SquadOrder_s_t(SquadOrderType type_, sc2::Point2D target_, float radius_){
        type = type_; target = target_; radius = radius_;
    }

    SquadOrderType type;
    sc2::Point2D target;
    float radius;
} SquadOrder;

class Squad {
    public:
    Squad(std::string id_, size_t priority_);
    void onStep();

    void setOrder(SquadOrder order_);
    void setOrder(SquadOrderType, sc2::Point2D target, float radius);
    void setPriority(size_t prio);
    
    void addUnit(sc2::Tag tag);
    bool removeUnit(sc2::Tag tag);
    
    std::string getId();
    SquadOrder getOrder();
    size_t getSize();
    size_t getPriority();
    std::vector<GameObject> getUnits();
    bool containsUnit(sc2::Tag tag);
    bool isEmpty();

    protected:
    // removes invalid units and returns number of units removed
    int validateUnits();
    sc2::Point2D getCenter();

    size_t priority;
    SquadOrder order;
    SquadStatus status;
    std::string id;
    std::vector<GameObject> units;
};