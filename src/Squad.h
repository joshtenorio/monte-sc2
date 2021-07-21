#pragma once

#include "api.h"
#include "micro/MicroManager.h"

class Squad {
    public:
    Squad() {};
    Squad(short id_) : id(id_) {};

    void OnStep();

    void absorb(Squad& other); // make sure to disband the other squad when finished
    void addUnit(sc2::Tag tag);
    void addUnits(std::vector<sc2::Tag> tags);
    void removeUnit(sc2::Tag tag);
    void disband(); // just delete/clear vector

    void setTarget(sc2::Point2D, bool attack); // TODO: do we need more parameters?
    sc2::Point2D getTarget();
    
    void setSquadId(short id_);
    short getSquadId();

    sc2::Point2D getCenter();
    std::vector<Monte::Unit> getUnits();
    
    protected:
    void calculateCenter();
    void validateState();

    sc2::Point2D center;
    sc2::Point2D target;

    char state; // TODO: this necessary? possibly so that micromanager knows what type of squad it is and what squad is currently doing
    short id;

    std::vector<Monte::Unit> units;

};