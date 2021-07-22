#include "GameObject.h"

namespace Monte{


sc2::Point3D GameObject::getPos(){
    sc2::Unit unit = getUnit();
    return unit.pos;
}

sc2::Tag GameObject::getTag(){
    return tag;
}

//const sc2::Unit* GameObject::getUnit(){

//}

sc2::Unit GameObject::getUnit(){ // TODO: need to test this
    sc2::Unit unit;
    unit.tag = tag;
    return unit;
}

char GameObject::getState(){
    return state;
}

} // end namespace Monte