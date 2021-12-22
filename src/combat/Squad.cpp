#include "Squad.h"

Squad::Squad(std::string id_, size_t priority_){
    id = id_;
    priority = priority_;
    status = SquadStatus::Idle;
    logger = Logger(id);
    mm = MicroManager();
    order = SquadOrder();
}

Squad::Squad(){
    id = "brr";
    priority = 10;
    status = SquadStatus::Idle;
    logger = Logger(id);
    mm = MicroManager();
    order = SquadOrder();
}

void Squad::initialize(){
    mm.initialize();
    supplyInitial = 0;
    supplyRemoved = 0;
}

SquadStatus Squad::onStep(Monte::InfluenceMap& gmap, Monte::InfluenceMap& amap){
    if(shouldRegroup()){
        mm.regroup(getRegroupPosition());
        status = SquadStatus::Regroup;
        return status;
    }
    else{
        mm.execute(order, gmap, amap);
        status = SquadStatus::Busy;
        return status;
    }
}

void Squad::setOrder(SquadOrder order_){
    order = order_;
}

void Squad::setOrder(SquadOrderType type, sc2::Point2D target, float radius){
    order = SquadOrder(type, target, radius);
    switch(type){
        case SquadOrderType::Attack:
        case SquadOrderType::Defend:
        supplyInitial = 0;
        for(auto& u : units){
            if(!u.isValid()) continue;
            supplyInitial += u.food;
        }
    }
}

void Squad::setPriority(size_t prio){
    priority = prio;
}

void Squad::addUnit(sc2::Tag tag){
    units.emplace_back(GameObject(tag));
    mm.addUnit(tag);
}

bool Squad::removeUnit(sc2::Tag tag){
    for(auto itr = units.begin(); itr != units.end(); ){
        if((*itr).getTag() == tag){
            mm.removeUnit(tag);
            supplyRemoved += (*itr).food;
            itr = units.erase(itr);
        }
        else ++itr;
    }
    return true;
}

std::string Squad::getId(){
    return id;
}

SquadOrder Squad::getOrder(){
    return order;
}

SquadStatus Squad::getStatus(){
    return status;
}

size_t Squad::getSize(){
    return units.size();
}

size_t Squad::getPriority(){
    return priority;
}

std::vector<GameObject> Squad::getUnits(){
    return units;
}

bool Squad::containsUnit(sc2::Tag tag){
    for(auto& u : units){
        if(u.getTag() == tag) return true;
    }
    return false;
}

bool Squad::isEmpty(){
    return units.empty();
}

int Squad::validateUnits(){
    int numRemoved = 0;
    for(auto& u : units){
        if(!u.isValid()){
            removeUnit(u.getTag());
            numRemoved++;
        }
    }
    return numRemoved;
}

sc2::Point2D Squad::getCenter(){
    if(isEmpty()) return sc2::Point2D(0,0);
    float x = 0.0, y = 0.0;
    float n = 0.0;
    for(auto& u : units){
        x += u.getPos().x;
        y += u.getPos().y;
        if(u.isValid()) n++;
    }
    sc2::Point2D avg = (n == 0.0 ? sc2::Point2D(0,0) : sc2::Point2D(x/n, y/n));
    return avg;
}

bool Squad::shouldRegroup(){
    // regroup if we've lost half or more our initial supply
    if(supplyInitial - supplyRemoved >= supplyInitial/2)
        return true;
    
    return false;
}

sc2::Point2D Squad::getRegroupPosition(){
    // TODO: temporary
    return getCenter();
}