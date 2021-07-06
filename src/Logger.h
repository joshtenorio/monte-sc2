#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_data.h>
#include "api.h"

// TODO: implement a way to send output to a file in data/
class Logger {
    public:
    Logger() { topic = "Null"; };
    Logger(std::string topic_) { topic = topic_; };

    // initialize log message
    Logger& infoInit();
    Logger& warningInit();
    Logger& errorInit();

    // append data
    Logger& printStr(std::string str_);
    Logger& printInt(int int_);
    Logger& printFloat(float float_);
    Logger& printPoint(sc2::Point2D point);
    Logger& printPoint(sc2::Point3D point);

    // send output to std::cout
    void send();



    private:
    std::string topic;
    std::string output;
};