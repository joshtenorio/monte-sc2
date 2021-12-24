#include "Debug.h"

namespace Monte {

Debug::Debug(){
    logger = Logger("Debug");
}

Debug::Debug(sc2::DebugInterface* debug_): debug(debug_){
    logger = Logger("Debug");
}

void Debug::createTimer(std::string name){
    timers.insert({name, std::chrono::system_clock::now()});
}

long long Debug::getTimer(std::string name){
    auto start = timers[name];
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

void Debug::resetTimer(std::string name){
    timers[name] = std::chrono::system_clock::now();
}

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