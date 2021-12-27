#pragma once

#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_interfaces.h>
#include "Logger.h"

namespace Monte {

class Actions {
    public:
    Actions();
    Actions(sc2::ActionInterface* actions_);

    // TODO: wrapper functions for all the UnitCommand overloads
    bool attack(sc2::Point2D target);
    bool attack(const sc2::Unit* target);
    
    protected:
    sc2::ActionInterface* interface;
    Logger logger;
};

}