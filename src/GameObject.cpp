#include "GameObject.h"

namespace Monte{


sc2::Point3D GameObject::getPos(){
    return getUnit()->pos;
}

sc2::Tag GameObject::getTag(){
    return tag;
}

const sc2::Unit* GameObject::getUnit(){
    return gInterface->observation->GetUnit(tag);
}

char GameObject::getState(){
    return state;
}

} // end namespace Monte