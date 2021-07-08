#pragma once

#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_interfaces.h>
#include "Logger.h"

// namespace Monte to better differentiate from sc2::DebugInterface
namespace Monte {

class Debug {
    public:
    // constructors
    Debug() { logger = Logger("Debug"); };
    Debug(sc2::DebugInterface* debug_): debug(debug_) { logger = Logger("Debug"); };

    void SendDebug();

    protected:
    sc2::DebugInterface* debug;
    Logger logger;
};

} // end namespace Monte
