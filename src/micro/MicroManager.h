#pragma once

#include <vector>
#include <sc2api/sc2_unit.h>

#define UNIT_STATE_INIT 0

namespace Monte {
    // first: tag ; second: state
    typedef std::pair<sc2::Tag, char> Unit;
} // end namespace

class MicroManager {
    public:
    MicroManager() {};

    // TODO: possibly need to pass more info about the squad state
    virtual void doStateAction(Monte::Unit& unit, char squadState) = 0;
    virtual char validateState(Monte::Unit& unit, char squadState) = 0;
};