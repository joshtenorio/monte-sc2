#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <sc2api/sc2_common.h>
#include <sc2api/sc2_data.h>
#include <sc2api/sc2_unit.h>

typedef struct Plot_s_t {
    std::string name;
    std::map<std::string, size_t> columns;
    std::vector<float> currentRow;
} Plot;

class Logger {
    public:
    Logger() { topic = "Null"; };
    Logger(std::string topic_) { topic = topic_; };

    static void setVersionNumber(std::string version);

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

    // write output to std::cout or somewhere else
    void write();
    void write(std::string fileName, bool error = false);
    void chat(std::string str);
    void chat(bool toStdout);
    void tag(std::string str);
    
    // generate a prefix for data output files for the match
    int createOutputPrefix();

    // plot functions
    void initializePlot(std::vector<std::string> columns, std::string plotName);
    void addPlotData(std::string plotName, std::string column, float data);
    void writePlotRow(std::string plotName);

    private:
    static std::string versionNumber;
    std::string path;
    std::string topic;
    std::string output;
    std::map<std::string, Plot> plots;
};