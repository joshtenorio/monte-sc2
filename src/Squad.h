#pragma once

#include "api.h"
#include "micro/BioManager.h"
#include "GameObject.h"

class Squad {
    public:

    enum State {
        Null = -1,      // null state
        Init = 0,       // when a squad is created
        Pause = 1,      // squad is ordered to stay idle
        Move = 2,       // squad is ordered to move to a point
        Attack = 3      // squad is ordered to attack move to a point
    };

    enum ManagerType {
        Null = -1,
        Bio = 0,
        Mech = 1,
        Flyer = 2
    };

    Squad() { bm = BioManager(); };
    Squad(short id_) : id(id_) { bm = BioManager(); };

    virtual void OnStep() = 0;

    void absorb(Squad& other);
    void addUnit(sc2::Tag tag);
    void addUnits(std::vector<sc2::Tag> tags);
    void removeUnit(sc2::Tag tag);
    void disband();

    void setTarget(sc2::Point2D newTarget);
    sc2::Point2D getTarget();

    void setAMove(bool attack);
    bool getAMoveStatus();
    
    void setSquadId(short id_);
    short getSquadId();

    void setState(State state_);
    State getState();

    sc2::Point2D getCenter();
    std::vector<Monte::GameObject>& getUnits();
    
    protected:
    ManagerType getManagerType(Monte::GameObject obj);
    void calculateCenter();
    virtual void validateState() = 0;

    sc2::Point2D center;
    sc2::Point2D target;

    State state;
    short id;

    std::vector<Monte::GameObject> units;

    BioManager bm;

};