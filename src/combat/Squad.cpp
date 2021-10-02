#include "Squad.h"

void Squad::absorb(Squad& other){
    std::vector<std::unique_ptr<Monte::GameObject>> otherUnits = other.getUnits();
    for(auto& u : otherUnits)
        addUnit(u.get()->getTag());
    
    other.disband();
}

void Squad::addUnit(sc2::Tag tag){
    units.push_back(std::make_unique<Monte::GameObject>(tag));
}

void Squad::addUnits(std::vector<sc2::Tag> tags){
    for(auto& t : tags)
        addUnit(t);
    
}

void Squad::removeUnit(sc2::Tag tag){
    for(auto itr = units.begin(); itr != units.end(); ){
        if(tag == (*itr)->getTag()){
            itr = units.erase(itr);
            break;
        }
        else ++itr;
    }
}

void Squad::disband(){
    units.clear();
}

void Squad::setTarget(sc2::Point2D newTarget){
    target = newTarget;
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

void Squad::setState(State state_){
    state = state_;
}

Squad::State Squad::getState(){
    return state;
}

sc2::Point2D Squad::getCenter(){
    calculateCenter();
    return center;
}

std::vector<std::unique_ptr<Monte::GameObject>>& Squad::getUnits(){
    return units;
}

Squad::ManagerType Squad::getManagerType(Monte::GameObject obj){
    const sc2::Unit* unit = obj.getUnit();
    if(unit == nullptr) return ManagerType::Null;

    switch(unit->unit_type.ToType()){
        case sc2::UNIT_TYPEID::TERRAN_MARINE:
        case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
        case sc2::UNIT_TYPEID::TERRAN_REAPER:
        case sc2::UNIT_TYPEID::TERRAN_GHOST:
            return ManagerType::Bio;
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE:
        case sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:
        case sc2::UNIT_TYPEID::TERRAN_CYCLONE:
        case sc2::UNIT_TYPEID::TERRAN_HELLION:
        case sc2::UNIT_TYPEID::TERRAN_HELLIONTANK:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
        case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
        case sc2::UNIT_TYPEID::TERRAN_THOR:
        case sc2::UNIT_TYPEID::TERRAN_THORAP:
            return ManagerType::Mech;
        case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
        case sc2::UNIT_TYPEID::TERRAN_VIKINGASSAULT:
        case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
        case sc2::UNIT_TYPEID::TERRAN_RAVEN:
        case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
        case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER:
            return ManagerType::Flyer;
        default:
            return ManagerType::Null;
    }
}

void Squad::calculateCenter(){
    float x = 0, y = 0;
    for(auto& u : units){
        if(u->getUnit() == nullptr) continue;
        x += u->getPos().x;
        y += u->getPos().y;
    }
    if(!units.empty())
        center = sc2::Point2D(x/units.size(), y/units.size());
    else center = sc2::Point2D(-1, -1); // since units is empty, set center to (-1,-1) to avoid division by zero
}
