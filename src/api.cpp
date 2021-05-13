#include "api.h"

using namespace sc2;

std::unique_ptr<Interface> gInterface;

size_t API::CountUnitType(UNIT_TYPEID unitType) {
    return gInterface->observation->GetUnits(Unit::Alliance::Self, IsUnit(unitType)).size();
}