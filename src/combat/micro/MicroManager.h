#pragma once

#include <sc2api/sc2_unit.h>
#include "combat/Squad.h"
#include "combat/GameObject.h"

class MicroManager {
    public:
    MicroManager() {};

    // TODO: possibly need to pass more info about the squad state
    void doStateAction(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target);
    void validateState(Monte::GameObject* unit, Squad::State squadState, sc2::Point2D target);

    virtual void doSquadMove(Monte::GameObject* unit, sc2::Point2D target);
    virtual void doSquadAttack(Monte::GameObject* unit, sc2::Point2D target);
    virtual void doSquadDefend(Monte::GameObject* unit, sc2::Point2D target);
    virtual void doSquadNull(Monte::GameObject* unit, sc2::Point2D target);
    virtual void doSquadInit(Monte::GameObject* unit, sc2::Point2D target);

    virtual void validateSquadMove(Monte::GameObject* unit, sc2::Point2D target);
    virtual void validateSquadAttack(Monte::GameObject* unit, sc2::Point2D target);
    virtual void validateSquadDefend(Monte::GameObject* unit, sc2::Point2D target);
    virtual void validateSquadNull(Monte::GameObject* unit, sc2::Point2D target);
    virtual void validateSquadInit(Monte::GameObject* unit, sc2::Point2D target);
};