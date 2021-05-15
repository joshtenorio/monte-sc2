#include "BuildingPlacer.h"

void BuildingPlacer::setMapper(Mapper* map_){
    map = map_;
}

Mapper* BuildingPlacer::getMapper(){
    return map;
}