#include "Squad.h"

void Squad::absorb(Squad& other){
    std::vector<Monte::GameObject> otherUnits = other.getUnits();
    for(auto& u : otherUnits)
        addUnit(u.getTag());
    
    other.disband();
}

void Squad::addUnit(sc2::Tag tag){
    Monte::GameObject unit = Monte::GameObject(tag);
    units.emplace_back(unit);
}

void Squad::addUnits(std::vector<sc2::Tag> tags){
    for(auto& t : tags)
        addUnit(t);
    
}

void Squad::removeUnit(sc2::Tag tag){
    for(auto itr = units.begin(); itr != units.end(); ){
        if(tag == (*itr).getTag()){
            itr = units.erase(itr);
            break;
        }
        else ++itr;
    }
}

void Squad::disband(){
    units.clear();
}

void Squad::setTarget(sc2::Point2D newTarget, bool attack){
    setAMove(attack);
    target = newTarget;
}

sc2::Point2D Squad::getTarget(){
    return target;
}

void Squad::setAMove(bool attack){
    aMoveToTarget = attack;
}

bool Squad::getAMoveStatus(){
    return aMoveToTarget;
}

void Squad::setSquadId(short id_){
    id = id_;
}

short Squad::getSquadId(){
    return id;
}

void Squad::setState(char state_){
    state = state_;
}

char Squad::getState(){
    return state;
}

sc2::Point2D Squad::getCenter(){
    return center;
}

std::vector<Monte::GameObject>& Squad::getUnits(){
    return units;
}

void Squad::calculateCenter(){
    float x = 0, y = 0;
    for(auto& u : units){
        if(u.getUnit() == nullptr) continue;
        x += u.getPos().x;
        y += u.getPos().y;
    }
    center = sc2::Point2D(x/units.size(), y/units.size());
}
