#pragma once

#include <vector>
#include<sc2api/sc2_unit.h>

namespace Monte {
    // first: tag ; second: state
    typedef std::pair<sc2::Tag, int> Unit;
} // end namespace

class MicroManager {
    public:
    MicroManager() {};
    virtual void doStateAction(Monte::Unit& unit) = 0;
    virtual int validateState(Monte::Unit& unit) = 0;
};