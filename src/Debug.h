#pragma once

#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_interfaces.h>
#include "Logger.h"

// namespace Monte to differentiate more from sc2::DebugInterface
namespace Monte {

class Debug {
    public:
    // constructors
    Debug() { logger = Logger("Debug"); };
    Debug(sc2::DebugInterface* debug_): debug(debug_) { logger = Logger("Debug"); };

    // sendDebug is only valid when BUILD_FOR_LADDER is off, see build instructions
    void sendDebug();

    void debugTextOut(const std::string& out, sc2::Color color = sc2::Colors::White);
    void debugTextOut(const std::string& out, const sc2::Point2D& point, sc2::Color color = sc2::Colors::White);
    void debugTextOut(const std::string& out, const sc2::Point3D& point, sc2::Color color = sc2::Colors::White);
    void debugLineOut(const sc2::Point3D& p0, const sc2::Point3D& p1, sc2::Color color = sc2::Colors::White);
    void debugBoxOut(const sc2::Point3D& p_min, const sc2::Point3D& p_max, sc2::Color color = sc2::Colors::White);
    void debugSphereOut(const sc2::Point3D& p, float r, sc2::Color color = sc2::Colors::White);
    void debugDrawTile(const sc2::Point3D& point, const sc2::Color& color);

    protected:
    sc2::DebugInterface* debug;
    Logger logger;
};

} // end namespace Monte
