#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_data.h>
#include <sc2api/sc2_unit.h>


// TODO: implement a way to send output to a file in data/, probably overload write() (i.e. write(std::string file) or something like that)
class Logger {
    public:
    Logger() { topic = "Null"; };
    Logger(std::string topic_) { topic = topic_; };

    // initialize log message
    Logger& infoInit();
    Logger& warningInit();
    Logger& errorInit();

    // append data
    Logger& withStr(std::string str_);
    Logger& withInt(int int_);
    Logger& withFloat(float float_);
    Logger& withPoint(sc2::Point2D point);
    Logger& withPoint(sc2::Point3D point);
    Logger& withUnit(const sc2::Unit* unit);

    // write output to std::cout
    void write();



    private:
    std::string topic;
    std::string output;
};