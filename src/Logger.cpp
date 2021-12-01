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

Logger& Logger::withStr(std::string str_){
    output += str_ + " ";
    return *this;
}

Logger& Logger::withInt(int int_){
    output += std::to_string(int_) + " ";
    return *this;
}

Logger& Logger::withFloat(float float_){
    output += std::to_string(float_) + " ";
    return *this;
}

Logger& Logger::withPoint(sc2::Point2D point){
    output += "(" + std::to_string(point.x) + ", " + std::to_string(point.y) + ") ";
    return *this;
}

Logger& Logger::withPoint(sc2::Point3D point){
    output += "(" + std::to_string(point.x) + ", " + std::to_string(point.y) + ", " + std::to_string(point.z) + ") ";
    return *this;
}

Logger& Logger::withUnit(const sc2::Unit* unit){
    output += sc2::UnitTypeToName(unit->unit_type);
    output += "(" + std::to_string(unit->tag) + ") ";
    return *this;
}

void Logger::write(){
    std::cout << output << std::endl;
}

void Logger::write(std::string fileName, bool error){
    std::ofstream file;
    std::string fileWithPath;
    if(!error)
        fileWithPath = "data/" + std::to_string(gInterface->matchID) + fileName;
    else
        fileWithPath = "data/error/" + std::to_string(gInterface->matchID) + fileName;
    file.open(fileWithPath, std::ios_base::app);
    if(file.is_open()){
        file << output << std::endl;
    }
    else{
        errorInit().withStr("failed to open " + fileWithPath).write();
    }
    file.close();
}

void Logger::chat(std::string str){
    gInterface->actions->SendChat(str);
}

void Logger::chat(bool toStdout){
    chat(output);
    if(toStdout)
        write();
}


void Logger::tag(std::string str){
    gInterface->actions->SendChat("Tag: " + str);
}

int Logger::createOutputPrefix(){
    // read match count
    std::ifstream fileReader;
    fileReader.open("data/MatchCount.txt");
    int currentMatchID, prevMatchID = -1;
    if(fileReader.is_open()){
        fileReader >> prevMatchID;
        currentMatchID = ++prevMatchID;
    }
    else{
        errorInit().withStr("couldn't read data/MatchCount.txt").write();
        currentMatchID = 1; // if we couldn't open MatchCount, just overwrite whatever uses prefix 1
    }
    fileReader.close();

    // update match count
    std::ofstream fileWriter;
    fileWriter.open("data/MatchCount.txt");
    if(fileWriter.is_open()){
        fileWriter << currentMatchID << std::endl;
    }
    else{
        errorInit().withStr("failed to update data/MatchCount.txt").write();
    }
    fileWriter.close();

    return currentMatchID;
}

void Logger::initializePlot(std::vector<std::string> columns, std::string plotName){
    plot.name = plotName;
    for(size_t i = 0; i < columns.size(); i++){
        plot.columns.insert({columns[i], i});
    }
    plot.currentRow.resize(columns.size());
    std::ofstream fileWriter;
    std::string file = "data/" + topic + "/" + std::to_string(gInterface->matchID) + plotName + ".csv";
    fileWriter.open(file);
    if(fileWriter.is_open()){
        for(int i = 0; i < columns.size(); i++){
            fileWriter << columns[i];
            if(i != columns.size()-1)
                fileWriter << ", ";
        }
        fileWriter << std::endl;
    }
    fileWriter.close();
}

void Logger::addPlotData(std::string column, float data){
    auto pair = plot.columns.find(column);
    if(pair != plot.columns.end()){
        plot.currentRow[pair->second] = data;
    }
    // TODO: probably should add overloads with diff variable types
}

void Logger::writePlotRow(){
    std::ofstream fileWriter;
    std::string file = "data/" + topic + "/" + std::to_string(gInterface->matchID) + plot.name + ".csv";
    fileWriter.open(file, std::ios_base::app);
    if(fileWriter.is_open()){
        for(int i = 0; i < plot.currentRow.size(); i++){
            fileWriter << plot.currentRow[i];
            if(i != plot.currentRow.size()-1)
                fileWriter << ", ";
        }
        fileWriter << std::endl;
    }
    fileWriter.close();

    // reset currentRow
    for(auto& c : plot.currentRow)
        c = 0;
}

