#include "Squad.h"

void Squad::absorb(Squad& other){

}

void Squad::addUnit(sc2::Tag tag){

}

void Squad::addUnits(std::vector<sc2::Tag> tags){

}

void Squad::removeUnit(sc2::Tag tag){

}

void Squad::disband(){

}

void Squad::setTarget(sc2::Point2D newTarget, bool attack){
    
}

sc2::Point2D Squad::getTarget(){
    return target;
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
        // get unit* from tag
    }
    center = sc2::Point2D(x/units.size(), y/units.size());
}
