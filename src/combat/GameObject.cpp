#include "GameObject.h"

GameObject::GameObject(sc2::Tag gTag){
    tag = gTag;
}

const sc2::Unit* GameObject::getUnit(){
    return gInterface->observation->GetUnit(tag);
}

sc2::UnitTypeData GameObject::getData(){
    return gInterface->observation->GetUnitTypeData()[getType()];
}

sc2::UnitTypeID GameObject::getType(){
    if(!isValid()) return isValid();
    return getUnit()->unit_type;
}

bool GameObject::isValid(){
    return getUnit();
}

bool GameObject::isFriendly(){
    if(!isValid()) return isValid();
    return getUnit()->alliance == sc2::Unit::Alliance::Self;
}

bool GameObject::isFlying(){
    if(!isValid()) return isValid();
    return getUnit()->is_flying;
}

bool GameObject::isBuilding(){
    if(!isValid()) return isValid();
    return getUnit()->is_building;
}

bool GameObject::isVisible(){
    if(!isValid()) return isValid();
    return getUnit()->display_type == sc2::Unit::DisplayType::Visible;
}

bool GameObject::isLarva(){
    if(!isValid()) return isValid();
    return getUnit()->unit_type.ToType() == sc2::UNIT_TYPEID::ZERG_LARVA;
}

bool GameObject::isEgg(){
    if(!isValid()) return isValid();
    return getUnit()->unit_type.ToType() == sc2::UNIT_TYPEID::ZERG_EGG;
}

bool GameObject::move(sc2::Point2D target){
    if(!isValid()) return isValid();
    gInterface->actions->UnitCommand(getUnit(), sc2::ABILITY_ID::MOVE_MOVE, target);
    return true;
}

bool GameObject::attack(sc2::Point2D target){
    if(!isValid()) return isValid();
    gInterface->actions->UnitCommand(getUnit(), sc2::ABILITY_ID::ATTACK_ATTACK, target);
    return true;
}

bool GameObject::attack(const sc2::Unit* target){
    if(!isValid()) return isValid();
    gInterface->actions->UnitCommand(getUnit(), sc2::ABILITY_ID::ATTACK_ATTACK, target); // note: sc2::ABILITY_ID::ATTACK also exists
    return true;
}

bool GameObject::rightClick(sc2::Point2D target){
    if(!isValid()) return isValid();
    gInterface->actions->UnitCommand(getUnit(), sc2::ABILITY_ID::SMART, target);
    return true;
}

bool GameObject::rightClick(const sc2::Unit* target){
    if(!isValid()) return isValid();
    gInterface->actions->UnitCommand(getUnit(), sc2::ABILITY_ID::SMART, target);
    return true;
}

bool GameObject::stop(){
    if(!isValid()) return isValid();
    gInterface->actions->UnitCommand(getUnit(), sc2::ABILITY_ID::STOP_STOP); // might be sc2::ABILITY_ID::STOP instead, idk
    return true;
}