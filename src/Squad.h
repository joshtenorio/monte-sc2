#pragma once

#include "api.h"
#include "micro/BioManager.h"
#include "GameObject.h"

class Squad {
    public:
    Squad() { bm = BioManager(); };
    Squad(short id_) : id(id_) { bm = BioManager(); };

    virtual void OnStep() = 0;

    void absorb(Squad& other); // make sure to disband the other squad when finished
    void addUnit(sc2::Tag tag);
    void addUnits(std::vector<sc2::Tag> tags);
    void removeUnit(sc2::Tag tag);
    void disband(); // just delete/clear vector

    void setTarget(sc2::Point2D newTarget, bool attack); // TODO: do we need more parameters?
    sc2::Point2D getTarget();

    void setAMove(bool attack);
    bool getAMoveStatus();
    
    void setSquadId(short id_);
    short getSquadId();

    void setState(char state_);
    char getState();

    sc2::Point2D getCenter();
    std::vector<Monte::GameObject>& getUnits();
    
    protected:
    void calculateCenter();
    virtual void validateState() = 0;

    sc2::Point2D center;
    sc2::Point2D target;
    
    bool aMoveToTarget;

    char state; // TODO: brrr make a State enum, need to decide if to make it here or in child classes
    short id;

    std::vector<Monte::GameObject> units;

    BioManager bm;

};