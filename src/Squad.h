#pragma once

#include "api.h"

class Squad {
    public:
    Squad();

    void OnStep();

    void absorb(Squad& squad);
    sc2::Point2D getCenter();
    
    // TODO: add parameters
    void addUnit();
    void addUnits();
    void removeUnit();
    void disband();

    void setTarget(bool attack); // TODO: do we need more parameters?
    sc2::Point2D getTarget();
    
    protected:
    void calculateCenter();
    void validateState();

    sc2::Point2D center;
    sc2::Point2D target;

    char state; // TODO: this necessary?

    // TODO: put a list of Monte::Units here that represents the units the squad has control over
};