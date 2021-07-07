#include "Logger.h"

// including this here to avoid circular dependency
#include "api.h"

Logger& Logger::infoInit(){
    output = "#" + std::to_string(gInterface->observation->GetGameLoop()) + " [INFO] " + topic + ": ";
    return *this;
}

Logger& Logger::warningInit(){
    output = "#" + std::to_string(gInterface->observation->GetGameLoop()) + " [WARNING] " + topic + ": ";
    return *this;
}

Logger& Logger::errorInit(){
    output = "#" + std::to_string(gInterface->observation->GetGameLoop()) + " [ERROR] " + topic + ": ";
    return *this;
}

Logger& Logger::printStr(std::string str_){
    output += str_ + " ";
    return *this;
}

Logger& Logger::printInt(int int_){
    output += std::to_string(int_) + " ";
    return *this;
}

Logger& Logger::printFloat(float float_){
    output += std::to_string(float_) + " ";
    return *this;
}

Logger& Logger::printPoint(sc2::Point2D point){
    output += "(" + std::to_string(point.x) + ", " + std::to_string(point.y) + ") ";
    return *this;
}

Logger& Logger::printPoint(sc2::Point3D point){
    output += "(" + std::to_string(point.x) + ", " + std::to_string(point.y) + ", " + std::to_string(point.z) + ") ";
    return *this;
}

Logger& Logger::printUnit(const sc2::Unit* unit){
    output += sc2::UnitTypeToName(unit->unit_type);
    output += "(" + std::to_string(unit->tag) + ") ";
    return *this;
}

void Logger::send(){
    std::cout << output << std::endl;
}
