#include "Debug.h"

namespace Monte {

void Debug::sendDebug(){
    #ifdef BUILD_FOR_LADDER
    // don't send debug
    #else
    debug->SendDebug();

    #endif
}

void Debug::debugTextOut(const std::string& out, sc2::Color color){
    debug->DebugTextOut(out, color);
}

void Debug::debugTextOut(const std::string& out, const sc2::Point2D& point, sc2::Color color){
    debug->DebugTextOut(out, point, color);
}

void Debug::debugTextOut(const std::string& out, const sc2::Point3D& point, sc2::Color color){
    debug->DebugTextOut(out, point, color);
}

void Debug::debugLineOut(const sc2::Point3D& p0, const sc2::Point3D& p1, sc2::Color color){
    debug->DebugLineOut(p0, p1, color);
}

void Debug::debugBoxOut(const sc2::Point3D& p_min, const sc2::Point3D& p_max, sc2::Color color){
    debug->DebugBoxOut(p_min, p_max, color);
}

void Debug::debugSphereOut(const sc2::Point3D& p, float r, sc2::Color color){
    debug->DebugSphereOut(p, r, color);
}

void Debug::debugDrawTile(const sc2::Point3D& point, const sc2::Color& color){
    debugBoxOut(
        {point.x + 0.1f, point.y + 0.1f, point.z},
        {point.x + 0.8f, point.y + 0.8f, point.z},
        color);
}


} // end namespace Monte